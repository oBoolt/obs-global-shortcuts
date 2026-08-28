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

void bind_split_recording_file() {
  if (!obs_frontend_recording_active() || obs_frontend_recording_paused())
    return;

  if (obs_frontend_recording_split_file())
    blog(LOG_DEBUG, "[%s] request to split recording file", PROJECT_PREFIX);
  else
    blog(LOG_WARNING, "[%s] file splitting is disabled", PROJECT_PREFIX);
}

void bind_add_chapter() {
  if (!obs_frontend_recording_active() || obs_frontend_recording_paused())
    return;

  if (obs_frontend_recording_add_chapter(NULL))
    blog(LOG_DEBUG, "[%s] chapter insertion was successful", PROJECT_PREFIX);
  else
    blog(LOG_WARNING,
         "[%s] chapter insertion is not supported by the current output",
         PROJECT_PREFIX);
}

void bind_toggle_replay_buffer() {
  obs_frontend_replay_buffer_active() ? obs_frontend_replay_buffer_stop()
                                      : obs_frontend_replay_buffer_start();

  blog(LOG_DEBUG, "[%s] replay buffer state changed", PROJECT_PREFIX);
}

void bind_save_replay_buffer() {
  if (!obs_frontend_replay_buffer_active())
    return;

  obs_frontend_replay_buffer_save();
  blog(LOG_DEBUG, "[%s] replay buffer saved", PROJECT_PREFIX);
}

void bind_toggle_virtual_camera() {
  obs_frontend_virtualcam_active() ? obs_frontend_stop_virtualcam()
                                   : obs_frontend_start_virtualcam();

  blog(LOG_DEBUG, "[%s] virtual camera state changed", PROJECT_PREFIX);
}
