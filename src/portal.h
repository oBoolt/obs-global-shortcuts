#ifndef PORTAL_HEADER_FILE
#define PORTAL_HEADER_FILE

#include <gio/gio.h>

#define PROJECT_PREFIX "global-shortcuts"
#define APP_ID "com.obsproject.Studio"

#define BUS_NAME "org.freedesktop.portal.Desktop"
#define OBJECT_PATH "/org/freedesktop/portal/desktop"

#define REGISTRY_INTERFACE "org.freedesktop.host.portal.Registry"
#define REQUEST_INTERFACE "org.freedesktop.portal.Request"
#define SESSION_INTERFACE "org.freedesktop.portal.Session"

#define REQUEST_PREFIX "/org/freedesktop/portal/desktop/request"
#define SESSION_PREFIX "/org/freedesktop/portal/desktop/session"

GDBusConnection *get_connection();
// Free after use with bfree()
char *get_sender();
// Free path and token after use with bfree()
void portal_handle_new(char **path, char **token);
// Free result after use with bfree()
char *portal_handle_get_path(char *prefix, char *token);

bool portal_load();
void portal_unload();

#endif // PORTAL_HEADER_FILE
