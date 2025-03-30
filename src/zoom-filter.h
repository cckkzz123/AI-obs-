#ifndef ZOOM_FILTER_H
#define ZOOM_FILTER_H

#include <obs-module.h>
#include "zoom-filter-ui.h"

#define S_ZOOM_IN "zoom_in"
#define S_ZOOM_OUT "zoom_out" 
#define S_ZOOM_RESET "zoom_reset"

struct zoom_filter {
    obs_source_t *context;
    float scale;
    obs_hotkey_id zoom_in_hotkey;
    obs_hotkey_id zoom_out_hotkey;
    obs_hotkey_id zoom_reset_hotkey;
    
    // Long press support
    bool zoom_in_pressed;
    bool zoom_out_pressed;
    obs_hotkey_t *zoom_in_key;
    obs_hotkey_t *zoom_out_key;
    uint64_t last_zoom_time;
};

extern struct obs_source_info zoom_filter;

void zoom_in(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed);
void zoom_out(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed);
void zoom_reset(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed);

#endif
