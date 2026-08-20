#include <obs-module.h>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("xdg-global-shortcuts", "en-US")

const char *obs_module_name() { return "XDG Global Shortcuts"; }
const char *obs_module_description() {
  return "Register Hotkeys as a XDG Global Shortcut";
}

bool obs_module_load(void) {
  blog(LOG_INFO, "Hello world! from xdg-global-shortcuts");
  return true;
}
