#include "zoom-tracking.h"
#include <math.h>
#include <util/platform.h>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
#else
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#endif

// 获取鼠标位置的平台相关实现
static void get_mouse_pos(struct vec2 *pos)
{
#ifdef _WIN32
    POINT mouse_pos;
    GetCursorPos(&mouse_pos);
    pos->x = (float)mouse_pos.x;
    pos->y = (float)mouse_pos.y;
#elif defined(__APPLE__)
    CGEventRef event = CGEventCreate(NULL);
    CGPoint mouse_pos = CGEventGetLocation(event);
    CFRelease(event);
    pos->x = (float)mouse_pos.x;
    pos->y = (float)mouse_pos.y;
#else
    Display *display = XOpenDisplay(NULL);
    if (display) {
        Window root, child;
        int root_x, root_y, win_x, win_y;
        unsigned int mask;
        XQueryPointer(display, DefaultRootWindow(display),
            &root, &child, &root_x, &root_y, &win_x, &win_y, &mask);
        XCloseDisplay(display);
        pos->x = (float)root_x;
        pos->y = (float)root_y;
    } else {
        pos->x = 0;
        pos->y = 0;
    }
#endif
}

// 平滑值计算
static float calculate_smooth_value(float current, float target, float smoothness, float dt)
{
    float t = smoothness * dt;
    // 限制最大变化率以防止过度震荡
    t = t > 1.0f ? 1.0f : t;
    return current + (target - current) * t;
}

// 初始化跟踪数据
void tracking_init(struct tracking_data *tracking)
{
    tracking->mode = TRACKING_MODE_DISABLED;
    tracking->mouse_x = 0.5f;
    tracking->mouse_y = 0.5f;
    tracking->smooth_enabled = true;
    tracking->smoothness = 0.6f;
    tracking->last_update = os_gettime_ns();
}

// 更新鼠标位置
void tracking_update_mouse(struct tracking_data *tracking, 
                         float width, float height, 
                         float scale, float last_scale,
                         uint64_t current_time)
{
    // 根据跟踪模式确定是否更新鼠标位置
    bool should_update = false;
    
    switch (tracking->mode) {
        case TRACKING_MODE_REALTIME:
            // 实时模式始终更新
            should_update = true;
            break;
        case TRACKING_MODE_ZOOMING:
            // 缩放变化时跟踪模式仅在缩放变化时更新
            should_update = fabsf(scale - last_scale) > 0.001f;
            break;
        case TRACKING_MODE_DISABLED:
        default:
            // 禁用模式不更新
            should_update = false;
            break;
    }
    
    if (should_update) {
        // 获取鼠标位置
        struct vec2 mouse_pos;
        get_mouse_pos(&mouse_pos);
        
        // 计算相对位置（0-1范围）
        float target_x = mouse_pos.x / width;
        float target_y = mouse_pos.y / height;
        
        // 限制在0-1范围内
        target_x = (target_x < 0.0f) ? 0.0f : (target_x > 1.0f) ? 1.0f : target_x;
        target_y = (target_y < 0.0f) ? 0.0f : (target_y > 1.0f) ? 1.0f : target_y;
        
        // 计算时间差
        float dt = (float)(current_time - tracking->last_update) / 1000000000.0f; // ns to s
        
        // 应用平滑过渡
        if (tracking->smooth_enabled) {
            tracking->mouse_x = calculate_smooth_value(tracking->mouse_x, target_x, tracking->smoothness * 10.0f, dt);
            tracking->mouse_y = calculate_smooth_value(tracking->mouse_y, target_y, tracking->smoothness * 10.0f, dt);
        } else {
            tracking->mouse_x = target_x;
            tracking->mouse_y = target_y;
        }
    }
    
    tracking->last_update = current_time;
}

// 获取跟踪位置（单位：像素）
void tracking_get_center(struct tracking_data *tracking,
                        float width, float height,
                        float *center_x, float *center_y)
{
    switch (tracking->mode) {
        case TRACKING_MODE_DISABLED:
            // 无跟踪模式：使用中心点
            *center_x = width * 0.5f;
            *center_y = height * 0.5f;
            break;
        case TRACKING_MODE_REALTIME:
        case TRACKING_MODE_ZOOMING:
        default:
            // 其他模式：使用当前鼠标位置
            *center_x = width * tracking->mouse_x;
            *center_y = height * tracking->mouse_y;
            break;
    }
}
