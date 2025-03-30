#include <obs-module.h>
#include "plugin-support.h"
#include "zoom-filter-ui.h"
#include "zoom-filter.h"

#define S_SCALE_FACTOR "scale_factor"
#define S_SINGLE_STEP "single_click_step"
#define S_CONT_STEP "continuous_step"
#define S_SMOOTH_ENABLED "smooth_enabled"
#define S_SMOOTHNESS "smoothness"
#define S_SMOOTH_MODE "smoothing_mode"
#define S_RESPONSE_TIME "response_time"
#define S_ANIM_TIME "animation_time"
#define S_AUTO_RESET "auto_reset_time"

static obs_properties_t *add_zoom_step_group(obs_properties_t *props)
{
    obs_properties_t *zoom_group = obs_properties_create();
    
    obs_properties_add_float_slider(zoom_group, S_SINGLE_STEP,
        obs_module_text("SingleClickStep"), 0.01, 0.5, 0.01);
    obs_properties_add_float_slider(zoom_group, S_CONT_STEP,
        obs_module_text("ContinuousStep"), 0.001, 0.05, 0.001);

    return zoom_group;
}

static obs_properties_t *add_smooth_group(obs_properties_t *props)
{
    obs_properties_t *smooth_group = obs_properties_create();

    obs_properties_add_float_slider(smooth_group, S_SMOOTHNESS,
        obs_module_text("Smoothness"), 0.1, 1.0, 0.1);

    obs_property_t *list = obs_properties_add_list(smooth_group, S_SMOOTH_MODE,
        obs_module_text("SmoothingMode"),
        OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);
    obs_property_list_add_int(list, obs_module_text("Linear"), 
        SMOOTH_MODE_LINEAR);
    obs_property_list_add_int(list, obs_module_text("Exponential"),
        SMOOTH_MODE_EXPONENTIAL);
    obs_property_list_add_int(list, obs_module_text("Logarithmic"),
        SMOOTH_MODE_LOGARITHMIC);

    return smooth_group;
}

static obs_properties_t *add_time_control_group(obs_properties_t *props)
{
    obs_properties_t *time_group = obs_properties_create();

    obs_properties_add_int_slider(time_group, S_RESPONSE_TIME,
        obs_module_text("ResponseTime"), 10, 200, 1);
    obs_properties_add_int_slider(time_group, S_ANIM_TIME,
        obs_module_text("AnimationTime"), 0, 1000, 10);
    obs_properties_add_int_slider(time_group, S_AUTO_RESET,
        obs_module_text("AutoResetTime"), 0, 10000, 100);

    return time_group;
}

obs_properties_t *zoom_filter_get_properties(void *data)
{
    UNUSED_PARAMETER(data);
    obs_properties_t *props = obs_properties_create();

    // 1. 基本设置组
    obs_properties_t *basic_group = obs_properties_create();
    obs_properties_add_float_slider(basic_group, S_SCALE_FACTOR, 
        obs_module_text("ScaleFactor"), 1.0, 5.0, 0.1);
    
    obs_properties_add_group(props, "basic_settings", 
        obs_module_text("BasicSettings"),
        OBS_GROUP_NORMAL, basic_group);

    // 2. 缩放步长组 
    obs_properties_t *step_group = add_zoom_step_group(props);
    obs_properties_add_group(props, "zoom_step_settings", 
        obs_module_text("ZoomStepSettings"),
        OBS_GROUP_NORMAL, step_group);

    // 3. 平滑过渡组 (可勾选)
    obs_properties_t *smooth_group = add_smooth_group(props);
    obs_properties_add_group(props, S_SMOOTH_ENABLED, 
        obs_module_text("SmoothSettings"),
        OBS_GROUP_CHECKABLE, smooth_group);

    // 4. 时间控制组
    obs_properties_t *time_group = add_time_control_group(props);
    obs_properties_add_group(props, "time_control_settings", 
        obs_module_text("TimeControlSettings"),
        OBS_GROUP_NORMAL, time_group);

    return props;
}

void zoom_filter_get_defaults(obs_data_t *settings)
{
    obs_data_set_default_double(settings, S_SCALE_FACTOR, 1.0);
    obs_data_set_default_double(settings, S_SINGLE_STEP, 0.1);
    obs_data_set_default_double(settings, S_CONT_STEP, 0.01);
    obs_data_set_default_bool(settings, S_SMOOTH_ENABLED, true);
    obs_data_set_default_double(settings, S_SMOOTHNESS, 0.3);
    obs_data_set_default_int(settings, S_SMOOTH_MODE, SMOOTH_MODE_LINEAR);
    obs_data_set_default_int(settings, S_RESPONSE_TIME, 50);
    obs_data_set_default_int(settings, S_ANIM_TIME, 200);
    obs_data_set_default_int(settings, S_AUTO_RESET, 0);
}
