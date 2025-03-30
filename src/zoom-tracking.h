#ifndef ZOOM_TRACKING_H
#define ZOOM_TRACKING_H

#include <stdbool.h>
#include <obs-module.h>

// 跟踪模式
#define TRACKING_MODE_DISABLED 0    // 无跟踪
#define TRACKING_MODE_REALTIME 1    // 实时跟踪
#define TRACKING_MODE_ZOOMING 2     // 缩放变化时跟踪

// 鼠标跟踪数据结构
struct tracking_data {
    int mode;              // 跟踪模式
    float mouse_x;         // 当前鼠标X位置（0-1范围）
    float mouse_y;         // 当前鼠标Y位置（0-1范围）
    bool smooth_enabled;   // 是否启用位置平滑
    float smoothness;      // 位置平滑系数（0.1-1.0）
    uint64_t last_update;  // 上次更新时间
};

// 初始化跟踪数据
void tracking_init(struct tracking_data *tracking);

// 更新鼠标位置
void tracking_update_mouse(struct tracking_data *tracking, 
                         float width, float height, 
                         float scale, float last_scale,
                         uint64_t current_time);

// 获取跟踪位置（单位：像素）
void tracking_get_center(struct tracking_data *tracking,
                        float width, float height,
                        float *center_x, float *center_y);

#endif // ZOOM_TRACKING_H
