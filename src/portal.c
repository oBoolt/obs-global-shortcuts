#include <gio/gio.h>
#include <glib.h>
#include <util/base.h>
#include <util/bmem.h>
#include <util/dstr.h>

#include "portal.h"

static GDBusConnection *connection = NULL;

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

GDBusConnection *get_connection() {
  if (connection != NULL)
    return connection;

  connection_init();
  return connection;
}

char *get_sender() {
  char *result = bstrdup(g_dbus_connection_get_unique_name(connection) + 1);

  if (result == NULL)
    return NULL;

  for (int i = 0; result[i]; i++)
    if (result[i] == '.')
      result[i] = '_';

  return result;
}

void portal_handle_new(char **path, char **token) {
  guint32 t = g_random_int();

  if (path != NULL) {
    char *sender = get_sender();
    struct dstr str;

    dstr_init(&str);
    dstr_printf(&str, "%s/%s/gs%u", REQUEST_PREFIX, sender, t);

    *path = bstrdup(str.array);

    dstr_free(&str);
    bfree(sender);
  }

  if (token != NULL) {
    struct dstr str;
    dstr_init(&str);
    dstr_printf(&str, "gs%u", t);
    *token = bstrdup(str.array);
    dstr_free(&str);
  }
}

char *portal_handle_get_path(char *prefix, char *token) {
  char *result;
  char *sender = get_sender();
  struct dstr str;

  dstr_init(&str);
  dstr_printf(&str, "%s/%s/%s", prefix, sender, token);

  result = bstrdup(str.array);

  dstr_free(&str);
  bfree(sender);

  return result;
}

void portal_request_callback(GDBusConnection *connection,
                             const char *sender_name, const char *object_path,
                             const char *interface_name,
                             const char *signal_name, GVariant *parameters,
                             gpointer user_data) {
  request_call_t *req = user_data;

  guint32 response;
  g_autoptr(GVariant) result = NULL;

  g_variant_get(parameters, "(u@a{sv})", &response, &result);

  switch (response) {
  case 0:
    blog(LOG_INFO, "[%s(%s)] %s", PROJECT_PREFIX, req->method, req->message);
    break;
  case 1:
    blog(LOG_WARNING, "[%s(%s)] request failed (user)", PROJECT_PREFIX,
         req->method);
    break;
  case 2:
    blog(LOG_WARNING, "[%s(%s)] request failed", PROJECT_PREFIX, req->method);
    break;
  }

  g_dbus_connection_call(connection, BUS_NAME, object_path, REQUEST_INTERFACE,
                         "Close", NULL, NULL, G_DBUS_CALL_FLAGS_NONE, -1, NULL,
                         NULL, NULL);

  bfree(req->method);
  bfree(req->message);
  g_dbus_connection_signal_unsubscribe(connection, req->signal_id);
}

bool portal_load() {
  if (!connection_init())
    return false;

  return true;
}

void portal_unload() { g_object_unref(connection); }
