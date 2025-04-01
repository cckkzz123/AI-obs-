#include <obs-module.h>
#include <util/platform.h>
#include <math.h>
#include "plugin-support.h"
#include "zoom-filter.h"

<<<<<<< HEAD
=======
#define S_SCALE_FACTOR "scale_factor"
#define S_SINGLE_STEP "single_click_step"
#define S_CONT_STEP "continuous_step"
#define S_SMOOTH_ENABLED "smooth_enabled"
#define S_SMOOTHNESS "smoothness"
#define S_SMOOTH_MODE "smoothing_mode"
#define S_RESPONSE_TIME "response_time"
#define S_ANIM_TIME "animation_time"
#define S_AUTO_RESET "auto_reset_time"

>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225
static const char *zoom_filter_get_name(void *unused)
{
    UNUSED_PARAMETER(unused);
    return obs_module_text("ZoomFilter");
}

<<<<<<< HEAD
=======
static float calculate_smooth_scale(struct zoom_filter *filter, uint64_t current_time)
{
    if (!filter->smooth_enabled || filter->animation_time == 0)
        return filter->target_scale;

    float t = (float)(current_time - filter->transition_start) / (float)filter->animation_time;
    if (t >= 1.0f) {
        filter->scale = filter->target_scale;
        return filter->target_scale;
    }

    float start_scale = filter->scale;
    float end_scale = filter->target_scale;
    float diff = end_scale - start_scale;

    // 防止震荡
    if (fabsf(diff) < 0.001f)
        return end_scale;

    switch (filter->smoothing_mode) {
    case SMOOTH_MODE_LINEAR:
        return start_scale + diff * t;
    case SMOOTH_MODE_EXPONENTIAL:
        return start_scale + diff * (1.0f - expf(-t * (1.0f / filter->smoothness)));
    case SMOOTH_MODE_LOGARITHMIC:
        return start_scale + diff * (logf(1.0f + t * 9.0f) / logf(10.0f));
    default:
        return filter->target_scale;
    }
}

>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225
static void *zoom_filter_create(obs_data_t *settings, obs_source_t *source)
{
    struct zoom_filter *filter = bzalloc(sizeof(struct zoom_filter));
    filter->context = source;
    
    // 初始化各个模块
    tracking_init(&filter->tracking);
    smoothing_init(&filter->smoothing);
    rendering_init(&filter->rendering, &filter->tracking, &filter->smoothing);
    
    // 设置初始值
    filter->tracking.mode = (int)obs_data_get_int(settings, S_TRACKING_MODE);
    filter->tracking.smooth_enabled = obs_data_get_bool(settings, S_TRACKING_SMOOTH_ENABLED);
    filter->tracking.smoothness = (float)obs_data_get_double(settings, S_TRACKING_SMOOTHNESS);
    filter->smoothing.current_scale = (float)(double)obs_data_get_double(settings, S_SCALE_FACTOR);
    filter->smoothing.target_scale = filter->smoothing.current_scale;
    
    // 初始化热键
    filter->zoom_in_hotkey = obs_hotkey_register_frontend(
        "zoom_filter.zoom_in.global", obs_module_text("ZoomIn"), zoom_in, filter);
    filter->zoom_out_hotkey = obs_hotkey_register_frontend(
        "zoom_filter.zoom_out.global", obs_module_text("ZoomOut"), zoom_out, filter);
    filter->zoom_reset_hotkey = obs_hotkey_register_frontend(
        "zoom_filter.zoom_reset.global", obs_module_text("ZoomReset"), zoom_reset, filter);
    
    // 初始化长按支持
    filter->zoom_in_pressed = false;
    filter->zoom_out_pressed = false;
    filter->zoom_in_key = NULL;
    filter->zoom_out_key = NULL;
    filter->last_zoom_time = 0;

    // 初始化缩放控制参数
    filter->single_click_step = (float)obs_data_get_double(settings, S_SINGLE_STEP);
    filter->continuous_step = (float)obs_data_get_double(settings, S_CONT_STEP);
    filter->response_time = obs_data_get_int(settings, S_RESPONSE_TIME) * 1000000; // ms to ns
    filter->auto_reset_time = obs_data_get_int(settings, S_AUTO_RESET) * 1000000; // ms to ns
    
    // 平滑相关参数
    filter->smoothing.enabled = obs_data_get_bool(settings, S_SMOOTH_ENABLED);
    filter->smoothing.smoothness = (float)obs_data_get_double(settings, S_SMOOTHNESS);
    filter->smoothing.mode = (int)obs_data_get_int(settings, S_SMOOTH_MODE);
    filter->smoothing.animation_time = obs_data_get_int(settings, S_ANIM_TIME) * 1000000; // ms to ns
    
    // 新增的平滑控制参数
    filter->smoothing.start_speed = (float)obs_data_get_double(settings, S_START_SPEED);
    filter->smoothing.end_deceleration = (float)obs_data_get_double(settings, S_END_DECEL);
    filter->smoothing.overshoot = (float)obs_data_get_double(settings, S_OVERSHOOT);
    
    return filter;
}

static void zoom_filter_destroy(void *data)
{
    struct zoom_filter *filter = data;
    
    // 注销热键
    obs_hotkey_unregister(filter->zoom_in_hotkey);
    obs_hotkey_unregister(filter->zoom_out_hotkey);
    obs_hotkey_unregister(filter->zoom_reset_hotkey);
    
    // 释放内存
    bfree(filter);
}

static void zoom_filter_update(void *data, obs_data_t *settings)
{
    struct zoom_filter *filter = data;
<<<<<<< HEAD
    float old_scale = filter->smoothing.current_scale;
=======
    float old_scale = filter->scale;
>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225
    
    // 更新跟踪模式和平滑度
    filter->tracking.mode = (int)obs_data_get_int(settings, S_TRACKING_MODE);
    filter->tracking.smooth_enabled = obs_data_get_bool(settings, S_TRACKING_SMOOTH_ENABLED);
    filter->tracking.smoothness = (float)obs_data_get_double(settings, S_TRACKING_SMOOTHNESS);
    
    // 更新平滑设置
    filter->smoothing.enabled = obs_data_get_bool(settings, S_SMOOTH_ENABLED);
    filter->smoothing.smoothness = (float)obs_data_get_double(settings, S_SMOOTHNESS);
    filter->smoothing.mode = (int)obs_data_get_int(settings, S_SMOOTH_MODE);
    filter->smoothing.animation_time = obs_data_get_int(settings, S_ANIM_TIME) * 1000000;
    
    // 更新新增的平滑控制参数
    filter->smoothing.start_speed = (float)obs_data_get_double(settings, S_START_SPEED);
    filter->smoothing.end_deceleration = (float)obs_data_get_double(settings, S_END_DECEL);
    filter->smoothing.overshoot = (float)obs_data_get_double(settings, S_OVERSHOOT);
    
    // 更新缩放控制
    filter->single_click_step = (float)obs_data_get_double(settings, S_SINGLE_STEP);
    filter->continuous_step = (float)obs_data_get_double(settings, S_CONT_STEP);
    filter->response_time = obs_data_get_int(settings, S_RESPONSE_TIME) * 1000000;
    filter->auto_reset_time = obs_data_get_int(settings, S_AUTO_RESET) * 1000000;
    
    // 检查缩放值是否变化
    float new_scale = (float)(double)obs_data_get_double(settings, S_SCALE_FACTOR);
    if (new_scale != filter->smoothing.target_scale) {
        smoothing_set_target(&filter->smoothing, new_scale, os_gettime_ns());
    }
}

static void save_scale(struct zoom_filter *filter, float scale)
{
    obs_data_t *settings = obs_source_get_settings(filter->context);
    obs_data_set_double(settings, S_SCALE_FACTOR, (double)scale);
    obs_source_update(filter->context, settings);
    obs_data_release(settings);
}

static void apply_zoom(struct zoom_filter *filter, float target)
{
    // 限制缩放值范围
    target = (float)fmax(fmin((double)target, 5.0), 1.0);
    
<<<<<<< HEAD
    // 设置新的目标缩放值
    smoothing_set_target(&filter->smoothing, target, os_gettime_ns());
    
    // 保存到设置
    save_scale(filter, target);
=======
    if (filter->smooth_enabled) {
        if (fabs(target - filter->target_scale) > 0.001f) {
            filter->target_scale = target;
            filter->transition_start = os_gettime_ns();
            save_scale(filter, target);
        }
    } else {
        filter->scale = target;
        filter->target_scale = target;
        save_scale(filter, target);
    }
>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225
}

static void zoom_filter_video_render(void *data, gs_effect_t *effect)
{
    struct zoom_filter *filter = data;
    obs_source_t *target = obs_filter_get_target(filter->context);
    
    if (!target) {
        obs_source_skip_video_filter(filter->context);
        return;
    }

    uint64_t current_time = os_gettime_ns();
<<<<<<< HEAD
    
    // 更新跟踪模块
    uint32_t width = obs_source_get_width(target);
    uint32_t height = obs_source_get_height(target);
    static float last_scale = 1.0f; // 存储上一帧的缩放值
    tracking_update_mouse(&filter->tracking,
                        (float)width, (float)height,
                        filter->smoothing.current_scale,
                        last_scale, // 使用真正的上一帧缩放值
                        current_time);
    // 记录当前缩放值，供下一帧使用
    last_scale = filter->smoothing.current_scale;
    
    // 更新平滑模块
    float scale = smoothing_update(&filter->smoothing, current_time);
    
=======

>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225
    // 处理长按缩放
    if (current_time - filter->last_zoom_time > filter->response_time) {
        if (filter->zoom_in_pressed) {
            float target = filter->smoothing.target_scale + filter->continuous_step;
            apply_zoom(filter, target);
            filter->last_zoom_time = current_time;
        } else if (filter->zoom_out_pressed) {
            float target = filter->smoothing.target_scale - filter->continuous_step;
            apply_zoom(filter, target);
            filter->last_zoom_time = current_time;
        }
    }

    // 处理自动复位
    if (filter->auto_reset_time > 0 && 
        !filter->zoom_in_pressed && !filter->zoom_out_pressed &&
        current_time - filter->last_zoom_time > filter->auto_reset_time) {
        apply_zoom(filter, 1.0f);
        filter->last_zoom_time = current_time;
    }
<<<<<<< HEAD
    
    // 执行渲染
    rendering_render(&filter->rendering, target, effect);
}

void zoom_in(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed)
=======

    // 处理平滑过渡
    if (filter->smooth_enabled && filter->transition_start > 0) {
        filter->scale = calculate_smooth_scale(filter, current_time);
    }

    struct vec4 clear_color;
    vec4_zero(&clear_color);
    gs_clear(GS_CLEAR_COLOR, &clear_color, 0.0f, 0);
    gs_ortho(0.0f, (float)width, 0.0f, (float)height, -100.0f, 100.0f);

    float scale = filter->scale;
    float half_w = width * 0.5f;
    float half_h = height * 0.5f;

    gs_matrix_push();
    gs_matrix_translate3f(half_w, half_h, 0.0f);
    gs_matrix_scale3f(scale, scale, 1.0f);
    gs_matrix_translate3f(-half_w, -half_h, 0.0f);

    obs_source_video_render(target);

    gs_matrix_pop();
}

static void zoom_in(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed)
>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225
{
    UNUSED_PARAMETER(id);
    
    struct zoom_filter *filter = data;
    if (!filter || !filter->context) return;
    
    filter->zoom_in_pressed = pressed;
    filter->zoom_in_key = hotkey;
    
    if (pressed) {
<<<<<<< HEAD
        float target = filter->smoothing.target_scale + filter->single_click_step;
=======
        float target = filter->target_scale + filter->single_click_step;
>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225
        apply_zoom(filter, target);
        filter->last_zoom_time = os_gettime_ns();
    }
}

void zoom_out(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed)
{
    UNUSED_PARAMETER(id);
    
    struct zoom_filter *filter = data;
    if (!filter || !filter->context) return;
    
    filter->zoom_out_pressed = pressed;
    filter->zoom_out_key = hotkey;
    
    if (pressed) {
<<<<<<< HEAD
        float target = filter->smoothing.target_scale - filter->single_click_step;
=======
        float target = filter->target_scale - filter->single_click_step;
>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225
        apply_zoom(filter, target);
        filter->last_zoom_time = os_gettime_ns();
    }
}

void zoom_reset(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed)
{
    UNUSED_PARAMETER(id);
    UNUSED_PARAMETER(hotkey);
    
    if (!pressed) return;
    
    struct zoom_filter *filter = data;
    if (!filter || !filter->context) return;
    
    apply_zoom(filter, 1.0f);
    filter->last_zoom_time = os_gettime_ns();
}

static void zoom_filter_save(void *data, obs_data_t *hotkeys)
{
    struct zoom_filter *filter = data;
    obs_data_array_t *save_array = obs_hotkey_save(filter->zoom_in_hotkey);
    obs_data_set_array(hotkeys, "zoom_in_hotkey", save_array);
    obs_data_array_release(save_array);
    
    save_array = obs_hotkey_save(filter->zoom_out_hotkey);
    obs_data_set_array(hotkeys, "zoom_out_hotkey", save_array);
    obs_data_array_release(save_array);
    
    save_array = obs_hotkey_save(filter->zoom_reset_hotkey);
    obs_data_set_array(hotkeys, "zoom_reset_hotkey", save_array);
    obs_data_array_release(save_array);
}

static void zoom_filter_load(void *data, obs_data_t *hotkeys)
{
    struct zoom_filter *filter = data;
    obs_data_array_t *load_array = obs_data_get_array(hotkeys, "zoom_in_hotkey");
    obs_hotkey_load(filter->zoom_in_hotkey, load_array);
    obs_data_array_release(load_array);
    
    load_array = obs_data_get_array(hotkeys, "zoom_out_hotkey");
    obs_hotkey_load(filter->zoom_out_hotkey, load_array);
    obs_data_array_release(load_array);
    
    load_array = obs_data_get_array(hotkeys, "zoom_reset_hotkey");
    obs_hotkey_load(filter->zoom_reset_hotkey, load_array);
    obs_data_array_release(load_array);
}

struct obs_source_info zoom_filter = {
    .id = "zoom_filter",
    .type = OBS_SOURCE_TYPE_FILTER,
    .output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW,
    .get_name = zoom_filter_get_name,
    .create = zoom_filter_create,
    .destroy = zoom_filter_destroy,
    .update = zoom_filter_update,
    .video_render = zoom_filter_video_render,
    .get_properties = zoom_filter_get_properties,
    .get_defaults = zoom_filter_get_defaults,
    .save = zoom_filter_save,
    .load = zoom_filter_load,
};
