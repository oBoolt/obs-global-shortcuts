#include <gio/gio.h>
#include <glib-object.h>
#include <obs-module.h>

#include "shortcuts.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("xdg-global-shortcuts", "en-US")

const char *obs_module_name() { return "XDG Global Shortcuts"; }
const char *obs_module_description() {
  return "Register Hotkeys as a XDG Global Shortcut";
}

bool obs_module_load(void) {
  bool result = shortcuts_load();

  if (!result) {
    shortcuts_unload();
  }

  return result;
}

void obs_module_unload(void) { shortcuts_unload(); }
