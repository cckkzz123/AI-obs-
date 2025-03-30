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

    // Zoom control parameters
    float single_click_step;  // 单击步长
    float continuous_step;    // 持续步长
    bool smooth_enabled;      // 平滑开关
    float smoothness;         // 平滑系数
    int smoothing_mode;       // 平滑模式
    uint64_t response_time;   // 响应间隔(ns)
    uint64_t animation_time;  // 动画时长(ns)
    uint64_t auto_reset_time; // 自动复位时间(ns)
    
    // Smooth transition support
    float target_scale;       // 目标缩放值
    uint64_t transition_start;// 过渡开始时间
};

// Smoothing modes
#define SMOOTH_MODE_LINEAR  0
#define SMOOTH_MODE_EXPONENTIAL 1
#define SMOOTH_MODE_LOGARITHMIC 2

extern struct obs_source_info zoom_filter;

void zoom_in(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed);
void zoom_out(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed);
void zoom_reset(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed);

#endif
