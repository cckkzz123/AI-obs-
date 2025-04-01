#ifndef ZOOM_FILTER_H
#define ZOOM_FILTER_H

#include <obs-module.h>
#include "zoom-filter-ui.h"
#include "zoom-tracking.h"
#include "zoom-smoothing.h"
#include "zoom-rendering.h"

#define S_ZOOM_IN "zoom_in"
#define S_ZOOM_OUT "zoom_out" 
#define S_ZOOM_RESET "zoom_reset"

// 设置项
#define S_TRACKING_MODE "tracking_mode"
#define S_SCALE_FACTOR "scale_factor"
#define S_SINGLE_STEP "single_click_step"
#define S_CONT_STEP "continuous_step"
#define S_TRACKING_SMOOTH_ENABLED "tracking_smooth_enabled"
#define S_TRACKING_SMOOTHNESS "tracking_smoothness"
#define S_SMOOTH_ENABLED "smooth_enabled"
#define S_SMOOTHNESS "smoothness"
#define S_SMOOTH_MODE "smoothing_mode"
#define S_RESPONSE_TIME "response_time"
#define S_ANIM_TIME "animation_time"
#define S_AUTO_RESET "auto_reset_time"
#define S_START_SPEED "start_speed"
#define S_END_DECEL "end_deceleration"
#define S_OVERSHOOT "overshoot"

// 主过滤器结构体
struct zoom_filter {
    obs_source_t *context;    // OBS上下文
    
    // 模块数据
    struct tracking_data tracking;     // 鼠标跟踪模块
    struct smoothing_data smoothing;   // 平滑效果模块
    struct rendering_data rendering;   // 渲染控制模块
    
    // 热键
    obs_hotkey_id zoom_in_hotkey;
    obs_hotkey_id zoom_out_hotkey;
    obs_hotkey_id zoom_reset_hotkey;
    
    // 长按支持
    bool zoom_in_pressed;
    bool zoom_out_pressed;
    obs_hotkey_t *zoom_in_key;
    obs_hotkey_t *zoom_out_key;
    uint64_t last_zoom_time;
<<<<<<< HEAD
    
    // 缩放步长控制
=======

    // Zoom control parameters
>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225
    float single_click_step;  // 单击步长
    float continuous_step;    // 持续步长
    uint64_t response_time;   // 响应间隔(ns)
    uint64_t auto_reset_time; // 自动复位时间(ns)
};

<<<<<<< HEAD
=======
// Smoothing modes
#define SMOOTH_MODE_LINEAR  0
#define SMOOTH_MODE_EXPONENTIAL 1
#define SMOOTH_MODE_LOGARITHMIC 2

>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225
extern struct obs_source_info zoom_filter;

void zoom_in(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed);
void zoom_out(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed);
void zoom_reset(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed);

#endif
