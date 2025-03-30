#include <obs-module.h>
#include <util/platform.h>
#include "plugin-support.h"
#include "zoom-filter.h"

#define S_SCALE_FACTOR "scale_factor"

static const char *zoom_filter_get_name(void *unused)
{
    UNUSED_PARAMETER(unused);
    return obs_module_text("ZoomFilter");
}

static void *zoom_filter_create(obs_data_t *settings, obs_source_t *source)
{
    struct zoom_filter *filter = bzalloc(sizeof(struct zoom_filter));
    filter->context = source;
    filter->scale = (float)(double)obs_data_get_double(settings, S_SCALE_FACTOR);
    
    filter->zoom_in_hotkey = obs_hotkey_register_frontend(
        "zoom_filter.zoom_in.global", obs_module_text("ZoomIn"), zoom_in, filter);
    filter->zoom_out_hotkey = obs_hotkey_register_frontend(
        "zoom_filter.zoom_out.global", obs_module_text("ZoomOut"), zoom_out, filter);
    filter->zoom_reset_hotkey = obs_hotkey_register_frontend(
        "zoom_filter.zoom_reset.global", obs_module_text("ZoomReset"), zoom_reset, filter);
    
    filter->zoom_in_pressed = false;
    filter->zoom_out_pressed = false;
    filter->zoom_in_key = NULL;
    filter->zoom_out_key = NULL;
    filter->last_zoom_time = 0;
    
    obs_source_update(source, settings);
    return filter;
}

static void zoom_filter_destroy(void *data)
{
    struct zoom_filter *filter = data;
    
    obs_hotkey_unregister(filter->zoom_in_hotkey);
    obs_hotkey_unregister(filter->zoom_out_hotkey);
    obs_hotkey_unregister(filter->zoom_reset_hotkey);
    
    bfree(filter);
}

static void zoom_filter_update(void *data, obs_data_t *settings)
{
    struct zoom_filter *filter = data;
    filter->scale = (float)(double)obs_data_get_double(settings, S_SCALE_FACTOR);
    obs_source_update(filter->context, NULL);
}

static void zoom_filter_video_render(void *data, gs_effect_t *effect)
{
    struct zoom_filter *filter = data;
    obs_source_t *target = obs_filter_get_target(filter->context);
    uint32_t width = obs_source_get_width(target);
    uint32_t height = obs_source_get_height(target);
    
    if (!target || !width || !height) {
        obs_source_skip_video_filter(filter->context);
        return;
    }

    // Handle long press zoom
    uint64_t current_time = os_gettime_ns();
    if (current_time - filter->last_zoom_time > 50000000) { // 50ms interval
        if (filter->zoom_in_pressed) {
            filter->scale = (float)fmin((double)filter->scale + 0.01, 5.0);
            obs_data_t *settings = obs_source_get_settings(filter->context);
            obs_data_set_double(settings, S_SCALE_FACTOR, (double)filter->scale);
            obs_source_update(filter->context, settings);
            obs_data_release(settings);
        } else if (filter->zoom_out_pressed) {
            filter->scale = (float)fmax((double)filter->scale - 0.01, 1.0);
            obs_data_t *settings = obs_source_get_settings(filter->context);
            obs_data_set_double(settings, S_SCALE_FACTOR, (double)filter->scale);
            obs_source_update(filter->context, settings);
            obs_data_release(settings);
        }
        filter->last_zoom_time = current_time;
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
{
    UNUSED_PARAMETER(id);
    
    struct zoom_filter *filter = data;
    if (!filter || !filter->context) return;
    
    filter->zoom_in_pressed = pressed;
    filter->zoom_in_key = hotkey;
    
    if (pressed) {
        // Initial zoom on press
        obs_data_t *settings = obs_source_get_settings(filter->context);
        filter->scale = (float)fmin((double)filter->scale + 0.1, 5.0);
        obs_data_set_double(settings, S_SCALE_FACTOR, (double)filter->scale);
        obs_source_update(filter->context, settings);
        obs_data_release(settings);
        filter->last_zoom_time = os_gettime_ns();
    }
}

static void zoom_out(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed)
{
    UNUSED_PARAMETER(id);
    
    struct zoom_filter *filter = data;
    if (!filter || !filter->context) return;
    
    filter->zoom_out_pressed = pressed;
    filter->zoom_out_key = hotkey;
    
    if (pressed) {
        // Initial zoom on press
        obs_data_t *settings = obs_source_get_settings(filter->context);
        filter->scale = (float)fmax((double)filter->scale - 0.1, 1.0);
        obs_data_set_double(settings, S_SCALE_FACTOR, (double)filter->scale);
        obs_source_update(filter->context, settings);
        obs_data_release(settings);
        filter->last_zoom_time = os_gettime_ns();
    }
}

static void zoom_reset(void *data, obs_hotkey_id id, obs_hotkey_t *hotkey, bool pressed)
{
    UNUSED_PARAMETER(id);
    UNUSED_PARAMETER(hotkey);
    
    if (!pressed) return;
    
    struct zoom_filter *filter = data;
    if (!filter || !filter->context) return;
    
    obs_data_t *settings = obs_source_get_settings(filter->context);
    filter->scale = 1.0f;
    obs_data_set_double(settings, S_SCALE_FACTOR, 1.0);
    obs_source_update(filter->context, settings);
    obs_data_release(settings);
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
