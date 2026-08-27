#ifndef XDG_GLOBAL_SHORTCUTS_HEADER_FILE
#define XDG_GLOBAL_SHORTCUTS_HEADER_FILE

#include <glib.h>
#define PROJECT_PREFIX "global-shortcuts"
#define APP_ID "com.obsproject.Studio"

#define BUS_NAME "org.freedesktop.portal.Desktop"
#define OBJECT_PATH "/org/freedesktop/portal/desktop"

#define GLOBAL_SHORTCUTS_INTERFACE "org.freedesktop.portal.GlobalShortcuts"
#define REGISTRY_INTERFACE "org.freedesktop.host.portal.Registry"
#define REQUEST_INTERFACE "org.freedesktop.portal.Request"

#define REQUEST_PREFIX "/org/freedesktop/portal/desktop/request"
#define SESSION_PREFIX "/org/freedesktop/portal/desktop/session"

typedef struct shortcuts_call {
  char *sender;
  char *request_handle;
  char *session_handle;
  guint signal_response_id;
  guint signal_shortcuts_id;
} shortcuts_call_t;

bool shortcuts_load(void);
void shortcuts_unload(void);

#endif // XDG_GLOBAL_SHORTCUTS_HEADER_FILE
