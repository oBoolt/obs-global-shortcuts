#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <glibconfig.h>
#include <strings.h>
#include <util/base.h>
#include <util/bmem.h>
#include <util/dstr.h>

#include "portal.h"
#include "shortcuts.h"

static GDBusProxy *proxy = NULL;

typedef struct shortcuts_call {
  char *session_token;
  guint signal_shortcuts_id;
} shortcuts_call_t;

static shortcuts_call_t *call;

static bool proxy_init() {
  g_autoptr(GError) error = NULL;

  GDBusConnection *connection = get_connection();
  g_return_val_if_fail(connection != NULL, false);

  proxy = g_dbus_proxy_new_sync(connection, G_DBUS_PROXY_FLAGS_NONE, NULL,
                                BUS_NAME, OBJECT_PATH,
                                GLOBAL_SHORTCUTS_INTERFACE, NULL, &error);
  if (error != NULL) {
    g_assert(proxy == NULL);
    blog(LOG_WARNING, "[%s] failed to create GlobalShortcuts proxy: %s",
         PROJECT_PREFIX, error->message);
    return false;
  }

  blog(LOG_DEBUG, "[%s] proxy for GlobalShortcuts interface created",
       PROJECT_PREFIX);

  return true;
}

static bool shortcuts_register_app() {
  g_autoptr(GError) error = NULL;
  g_autoptr(GVariant) ret = NULL;

  GDBusConnection *connection = get_connection();
  g_return_val_if_fail(connection != NULL, false);

  GVariantBuilder opts;
  g_variant_builder_init(&opts, G_VARIANT_TYPE_VARDICT);

  ret = g_dbus_connection_call_sync(
      connection, BUS_NAME, OBJECT_PATH, REGISTRY_INTERFACE, "Register",
      g_variant_new("(sa{sv})", APP_ID, &opts), NULL, G_DBUS_CALL_FLAGS_NONE,
      -1, NULL, &error);

  if (error != NULL) {
    g_assert(ret == NULL);
    blog(LOG_WARNING, "[%s] failed to register app id: %s", PROJECT_PREFIX,
         error->message);
    return false;
  }

  blog(LOG_DEBUG, "[%s] app %s registered", PROJECT_PREFIX, APP_ID);

  return true;
}

static void shortcuts_call_init() {
  call = bzalloc(sizeof(shortcuts_call_t));

  portal_handle_new(NULL, &call->session_token);
}

static void shortcuts_signal_binds_callback(
    GDBusConnection *connection, const gchar *sender_name,
    const gchar *object_path, const gchar *interface_name,
    const gchar *signal_name, GVariant *parameters, gpointer user_data) {
  blog(LOG_DEBUG, "[%s] (SIGNAL{%s::%s}) : %s\n", PROJECT_PREFIX,
       interface_name, signal_name, g_variant_print(parameters, FALSE));
}

static bool shortcuts_create_session() {
  g_autoptr(GError) error = NULL;
  g_autoptr(GVariant) ret = NULL;

  GDBusConnection *connection = get_connection();
  g_return_val_if_fail(connection != NULL, false);

  char *token;
  portal_handle_new(NULL, &token);

  GVariantBuilder opts;
  g_variant_builder_init(&opts, G_VARIANT_TYPE_VARDICT);
  g_variant_builder_add(&opts, "{sv}", "handle_token",
                        g_variant_new_string(token));
  g_variant_builder_add(&opts, "{sv}", "session_handle_token",
                        g_variant_new_string(call->session_token));

  ret = g_dbus_proxy_call_sync(proxy, "CreateSession",
                               g_variant_new("(a{sv})", &opts),
                               G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

  bfree(token);

  if (error != NULL) {
    g_assert(ret == NULL);
    blog(LOG_WARNING, "[%s] failed to create GlobalShortcuts session: %s",
         PROJECT_PREFIX, error->message);
    return false;
  }

  blog(LOG_DEBUG, "[%s] GlobalShortcuts session created", PROJECT_PREFIX);

  call->signal_shortcuts_id = g_dbus_connection_signal_subscribe(
      connection, BUS_NAME, GLOBAL_SHORTCUTS_INTERFACE, NULL, OBJECT_PATH, NULL,
      G_DBUS_SIGNAL_FLAGS_NO_MATCH_RULE, shortcuts_signal_binds_callback, NULL,
      NULL);

  return true;
}

// TODO: accept Shortcut array
static bool shortcuts_bind() {
  g_autoptr(GError) error = NULL;
  g_autoptr(GVariant) ret = NULL;

  GVariantBuilder shortcuts;
  g_variant_builder_init(&shortcuts, G_VARIANT_TYPE("a(sa{sv})"));

  // TODO: with shortcut array add each one
  {
    GVariantBuilder bind;
    g_variant_builder_init(&bind, G_VARIANT_TYPE_VARDICT);
    g_variant_builder_add(&bind, "{sv}", "description",
                          g_variant_new_string("test description"));

    g_variant_builder_add(&shortcuts, "(sa{sv})", "foo", &bind);
  }

  char *token;
  portal_handle_new(NULL, &token);

  GVariantBuilder opts;
  g_variant_builder_init(&opts, G_VARIANT_TYPE_VARDICT);
  g_variant_builder_add(&opts, "{sv}", "handle_token",
                        g_variant_new_string(token));

  char *session_handle =
      portal_handle_get_path(SESSION_PREFIX, call->session_token);
  blog(LOG_DEBUG, "[%s] %s", PROJECT_PREFIX, session_handle);

  ret =
      g_dbus_proxy_call_sync(proxy, "BindShortcuts",
                             g_variant_new("(oa(sa{sv})sa{sv})", session_handle,
                                           &shortcuts, "", &opts),
                             G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

  bfree(token);
  bfree(session_handle);

  if (error != NULL) {
    g_assert(ret == NULL);
    blog(LOG_WARNING, "[%s] failed to bind shortcuts: %s", PROJECT_PREFIX,
         error->message);
    return false;
  }

  return true;
}

bool shortcuts_load() {
  if (!proxy_init())
    return false;

  if (!shortcuts_register_app())
    return false;

  shortcuts_call_init();

  if (!shortcuts_create_session())
    return false;

  shortcuts_bind();

  return true;
}

void shortcuts_unload() {
  GDBusConnection *connection = get_connection();
  if (connection != NULL) {
    g_autoptr(GError) error = NULL;
    g_autoptr(GVariant) ret = NULL;

    char *session_handle =
        portal_handle_get_path(SESSION_PREFIX, call->session_token);

    ret = g_dbus_connection_call_sync(connection, BUS_NAME, session_handle,
                                      SESSION_INTERFACE, "Close",
                                      g_variant_new("()", NULL), NULL,
                                      G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
    bfree(session_handle);

    if (error != NULL) {
      g_assert(ret == NULL);
      blog(LOG_WARNING, "[%s] failed to close shortcuts session: %s",
           PROJECT_PREFIX, error->message);
    }
  }

  bfree(call->session_token);
  g_dbus_connection_signal_unsubscribe(get_connection(),
                                       call->signal_shortcuts_id);

  g_object_unref(proxy);
  bfree(call);
  return;
}
