#ifndef XDG_GLOBAL_SHORTCUTS_HEADER_FILE
#define XDG_GLOBAL_SHORTCUTS_HEADER_FILE

#include <glib.h>

#define GLOBAL_SHORTCUTS_INTERFACE "org.freedesktop.portal.GlobalShortcuts"

bool shortcuts_load(void);
void shortcuts_unload(void);

#endif // XDG_GLOBAL_SHORTCUTS_HEADER_FILE
