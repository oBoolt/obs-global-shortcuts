#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <glibconfig.h>
#include <strings.h>
#include <util/base.h>
#include <util/bmem.h>
#include <util/dstr.h>

#include "shortcuts.h"

static GDBusConnection *connection = NULL;
static GDBusProxy *proxy = NULL;
static shortcuts_call_t *call;

static bool connection_init() {
  g_autoptr(GError) error = NULL;
  connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
  if (error != NULL) {
    g_assert(connection == NULL);
    blog(LOG_WARNING, "[%s] failed to start dbus connection: %s",
         PROJECT_PREFIX, error->message);
    return false;
  }

  blog(LOG_DEBUG, "[%s] dbus connection established", PROJECT_PREFIX);

  return true;
}

static bool proxy_init() {
  g_autoptr(GError) error = NULL;
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

static char *get_sender() {
  char *result = bstrdup(g_dbus_connection_get_unique_name(connection) + 1);

  if (result == NULL)
    return NULL;

  for (int i = 0; result[i]; i++)
    if (result[i] == '.')
      result[i] = '_';

  return result;
}

static bool shortcuts_register_app() {
  g_autoptr(GError) error = NULL;
  g_autoptr(GVariant) ret = NULL;

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
  call->sender = get_sender();

  struct dstr str;
  dstr_init(&str);
  dstr_printf(&str, "gs%u", g_random_int());
  call->session_token = bstrdup(str.array);
  dstr_free(&str);
}

static void shortcuts_get_request_handle(char **path, char **token) {
  guint32 t = g_random_int();

  if (path != NULL) {
    struct dstr str;
    dstr_init(&str);
    dstr_printf(&str, "%s/%s/gs%u", REQUEST_PREFIX, call->sender, t);
    *path = bstrdup(str.array);
    dstr_free(&str);
  }

  if (token != NULL) {
    struct dstr str;
    dstr_init(&str);
    dstr_printf(&str, "gs%u", t);
    *token = bstrdup(str.array);
    dstr_free(&str);
  }
}

static bool shortcuts_create_session() {
  g_autoptr(GError) error = NULL;
  g_autoptr(GVariant) ret = NULL;

  char *token;
  shortcuts_get_request_handle(NULL, &token);

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

  return true;
}

static void shortcuts_signal_response_callback(
    GDBusConnection *connection, const gchar *sender_name,
    const gchar *object_path, const gchar *interface_name,
    const gchar *signal_name, GVariant *parameters, gpointer user_data) {

  blog(LOG_DEBUG, "[%s] (SIGNAL{%s::%s}) : %s\n", PROJECT_PREFIX,
       interface_name, signal_name, g_variant_print(parameters, FALSE));
}

static void shortcuts_signal_binds_callback(
    GDBusConnection *connection, const gchar *sender_name,
    const gchar *object_path, const gchar *interface_name,
    const gchar *signal_name, GVariant *parameters, gpointer user_data) {
  blog(LOG_DEBUG, "[%s] (SIGNAL{%s::%s}) : %s\n", PROJECT_PREFIX,
       interface_name, signal_name, g_variant_print(parameters, FALSE));
}

static void shortcuts_signals_subscribe() {
  call->signal_response_id = g_dbus_connection_signal_subscribe(
      connection, BUS_NAME, REQUEST_INTERFACE, "Response", NULL, NULL,
      G_DBUS_SIGNAL_FLAGS_NO_MATCH_RULE, shortcuts_signal_response_callback,
      NULL, NULL);

  call->signal_shortcuts_id = g_dbus_connection_signal_subscribe(
      connection, BUS_NAME, GLOBAL_SHORTCUTS_INTERFACE, NULL, OBJECT_PATH, NULL,
      G_DBUS_SIGNAL_FLAGS_NO_MATCH_RULE, shortcuts_signal_binds_callback, NULL,
      NULL);

  return;
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
  shortcuts_get_request_handle(NULL, &token);

  GVariantBuilder opts;
  g_variant_builder_init(&opts, G_VARIANT_TYPE_VARDICT);
  g_variant_builder_add(&opts, "{sv}", "handle_token",
                        g_variant_new_string(token));

  struct dstr session_handle;
  dstr_init(&session_handle);
  dstr_printf(&session_handle, "%s/%s/%s", SESSION_PREFIX, call->sender,
              call->session_token);

  ret = g_dbus_proxy_call_sync(proxy, "BindShortcuts",
                               g_variant_new("(oa(sa{sv})sa{sv})",
                                             session_handle.array, &shortcuts,
                                             "", &opts),
                               G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);

  bfree(token);
  dstr_free(&session_handle);

  if (error != NULL) {
    g_assert(ret == NULL);
    blog(LOG_WARNING, "[%s] failed to bind shortcuts: %s", PROJECT_PREFIX,
         error->message);
    return false;
  }

  return true;
}

bool shortcuts_load() {
  if (!connection_init())
    return false;

  if (!proxy_init())
    return false;

  if (!shortcuts_register_app())
    return false;

  shortcuts_call_init();

  if (!shortcuts_create_session())
    return false;

  shortcuts_signals_subscribe();

  shortcuts_bind();

  return true;
}

void shortcuts_unload() {

  bfree(call->sender);
  bfree(call->session_token);
  bfree(call->session_handle);

  g_dbus_connection_signal_unsubscribe(connection, call->signal_response_id);
  g_dbus_connection_signal_unsubscribe(connection, call->signal_shortcuts_id);

  g_object_unref(connection);
  g_object_unref(proxy);
  bfree(call);
  return;
}
