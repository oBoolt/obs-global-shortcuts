#include <obs-frontend-api.h>
#include <util/base.h>

#include "binds.h"
#include "portal.h"

void bind_toggle_streaming() {
  obs_frontend_streaming_active() ? obs_frontend_streaming_stop()
                                  : obs_frontend_streaming_start();

  blog(LOG_DEBUG, "[%s] streaming state changed", PROJECT_PREFIX);
}

void bind_toggle_recording() {
  obs_frontend_recording_active() ? obs_frontend_recording_stop()
                                  : obs_frontend_recording_start();

  blog(LOG_DEBUG, "[%s] recording state changed", PROJECT_PREFIX);
}
void bind_play_pause_recording() {
  if (!obs_frontend_recording_active())
    return;

  obs_frontend_recording_pause(!obs_frontend_recording_paused());
  blog(LOG_DEBUG, "[%s] play/pause recording", PROJECT_PREFIX);
}
