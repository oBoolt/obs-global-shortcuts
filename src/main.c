#include <gio/gio.h>
#include <glib-object.h>
#include <obs-module.h>

#include "portal.h"
#include "shortcuts.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-global-shortcuts", "en-US")

const char *obs_module_name() { return "GlobalShortcuts"; }
const char *obs_module_description() { return obs_module_text("Description"); }

bool obs_module_load(void) {
  if (!portal_load()) {
    portal_unload();

    return false;
  }

  if (!shortcuts_load()) {
    shortcuts_unload();
    portal_unload();

    return false;
  }

  return true;
}

void obs_module_unload(void) {
  shortcuts_unload();
  portal_unload();
}
