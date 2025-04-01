#include <obs-module.h>
#include <util/platform.h>
#include <util/base.h>
#include <stdio.h>
#include "plugin-support.h"
#include "zoom-filter-ui.h"
#include "zoom-filter.h"
#include "url-handler.h"

#ifdef _WIN32
#define snprintf _snprintf
#endif

// 使用 zoom-filter.h 中的宏定义，不再重复定义

// 从url-handler.c导入规范化路径函数（确保函数声明匹配）
extern char *normalize_path(const char *base_path, const char *filename);

static bool open_donation_page_clicked(obs_properties_t *props, obs_property_t *property, void *data)
{
    UNUSED_PARAMETER(props);
    UNUSED_PARAMETER(property);
    UNUSED_PARAMETER(data);

    // 获取插件数据目录
    const char *plugin_data_path = get_plugin_data_path();
    if (!plugin_data_path) {
        blog(LOG_ERROR, "Failed to get plugin data path");
        return true;
    }

    blog(LOG_INFO, "Found plugin data path: %s", plugin_data_path);

    // 根据系统语言选择对应的捐赠页面
    const char *locale = obs_get_locale();
    const char *html_name = (locale && strncmp(locale, "zh", 2) == 0) ? "donate.html" : "donate-en.html";

    // 使用规范化路径函数来确保路径分隔符正确
    char *donate_path = normalize_path(plugin_data_path, html_name);
    if (!donate_path) {
        blog(LOG_ERROR, "Failed to create donation page path");
        return true;
    }

    blog(LOG_INFO, "Opening donation page: %s", donate_path);
    bool success = open_url(donate_path);
    
    // 释放由normalize_path分配的内存
    bfree(donate_path);
    
    if (!success) {
        blog(LOG_ERROR, "Failed to open donation page");
    }

    return true;
}

static obs_properties_t *add_zoom_step_group(obs_properties_t *props)
{
    obs_properties_t *zoom_group = obs_properties_create();
    
    obs_properties_add_float_slider(zoom_group, S_SINGLE_STEP,
        obs_module_text("SingleClickStep"), 0.01, 5.0, 0.01);
    obs_properties_add_float_slider(zoom_group, S_CONT_STEP,
        obs_module_text("ContinuousStep"), 0.001, 5.0, 0.001);

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
        
    // 添加高级平滑控制选项
    obs_properties_add_float_slider(smooth_group, S_START_SPEED,
        obs_module_text("StartSpeed"), 0.1, 2.0, 0.1);
    obs_properties_add_float_slider(smooth_group, S_END_DECEL,
        obs_module_text("EndDeceleration"), 0.1, 2.0, 0.1);
    obs_properties_add_float_slider(smooth_group, S_OVERSHOOT,
        obs_module_text("Overshoot"), 0.0, 0.5, 0.01);

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
<<<<<<< HEAD

static obs_properties_t *add_support_group(obs_properties_t *props)
{
    obs_properties_t *support_group = obs_properties_create();

    obs_properties_add_text(support_group, "plugin_description",
        obs_module_text("PluginDescription"),
        OBS_TEXT_INFO);
    
    obs_properties_add_button(support_group, "open_donation",
        obs_module_text("OpenDonationPage"),
        open_donation_page_clicked);

    return support_group;
}
=======
>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225

obs_properties_t *zoom_filter_get_properties(void *data)
{
    UNUSED_PARAMETER(data);
    obs_properties_t *props = obs_properties_create();

    // 1. 基本设置组
    obs_properties_t *basic_group = obs_properties_create();
    obs_properties_add_float_slider(basic_group, S_SCALE_FACTOR, 
        obs_module_text("ScaleFactor"), 1.0, 5.0, 0.1);
    
    obs_property_t *tracking_list = obs_properties_add_list(basic_group, S_TRACKING_MODE,
        obs_module_text("MouseTrackingMode"),
        OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_INT);
    obs_property_list_add_int(tracking_list, obs_module_text("TrackingDisabled"), 
        TRACKING_MODE_DISABLED);
    obs_property_list_add_int(tracking_list, obs_module_text("TrackingRealtime"), 
        TRACKING_MODE_REALTIME);
    obs_property_list_add_int(tracking_list, obs_module_text("TrackingZooming"), 
        TRACKING_MODE_ZOOMING);
    
    // 添加鼠标跟踪平滑度控制
    obs_properties_t *tracking_smooth_group = obs_properties_create();
    obs_properties_add_float_slider(tracking_smooth_group, S_TRACKING_SMOOTHNESS,
        obs_module_text("TrackingSmoothness"), 0.1, 1.0, 0.1);
    
    obs_properties_add_group(basic_group, S_TRACKING_SMOOTH_ENABLED, 
        obs_module_text("TrackingSmoothSettings"),
        OBS_GROUP_CHECKABLE, tracking_smooth_group);
    
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

<<<<<<< HEAD
    // 5. 支持开发者组
    obs_properties_t *support_group = add_support_group(props);
    obs_properties_add_group(props, "support_settings", 
        obs_module_text("SupportDeveloper"),
        OBS_GROUP_NORMAL, support_group);

=======
>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225
    return props;
}

void zoom_filter_get_defaults(obs_data_t *settings)
{
    obs_data_set_default_double(settings, S_SCALE_FACTOR, 1.0);
<<<<<<< HEAD
    obs_data_set_default_int(settings, S_TRACKING_MODE, TRACKING_MODE_REALTIME);
=======
>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225
    obs_data_set_default_double(settings, S_SINGLE_STEP, 0.1);
    obs_data_set_default_double(settings, S_CONT_STEP, 0.01);
    obs_data_set_default_bool(settings, S_SMOOTH_ENABLED, true);
    obs_data_set_default_double(settings, S_SMOOTHNESS, 0.6);
    obs_data_set_default_int(settings, S_SMOOTH_MODE, SMOOTH_MODE_EXPONENTIAL);
    obs_data_set_default_int(settings, S_RESPONSE_TIME, 50);
    obs_data_set_default_int(settings, S_ANIM_TIME, 400);
    obs_data_set_default_int(settings, S_AUTO_RESET, 0);
<<<<<<< HEAD
    
    // 新增平滑控制参数的默认值
    obs_data_set_default_double(settings, S_START_SPEED, 1.0);
    obs_data_set_default_double(settings, S_END_DECEL, 1.0);
    obs_data_set_default_double(settings, S_OVERSHOOT, 0.0);
    
    // 鼠标跟踪平滑度默认值
    obs_data_set_default_bool(settings, S_TRACKING_SMOOTH_ENABLED, true);
    obs_data_set_default_double(settings, S_TRACKING_SMOOTHNESS, 0.6);
=======
>>>>>>> 48bf4d025458e267826309d3286d5f08ba700225
}
