#ifndef XDG_GLOBAL_SHORTCUTS_HEADER_FILE
#define XDG_GLOBAL_SHORTCUTS_HEADER_FILE

#include <glib.h>

#define GLOBAL_SHORTCUTS_INTERFACE "org.freedesktop.portal.GlobalShortcuts"

typedef void exec_t(void);

#define SHORTCUT_NULL {NULL, NULL, NULL}
typedef struct shortcut {
  gchar *id;
  gchar *description;
  exec_t *exec;
} shortcut_t;

bool shortcuts_load(void);
void shortcuts_unload(void);

#endif // XDG_GLOBAL_SHORTCUTS_HEADER_FILE
