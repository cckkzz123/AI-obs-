#ifndef ZOOM_RENDERING_H
#define ZOOM_RENDERING_H

#include <obs-module.h>
#include "zoom-tracking.h"
#include "zoom-smoothing.h"

// 渲染状态结构体
struct rendering_data {
    struct tracking_data *tracking;     // 跟踪数据引用
    struct smoothing_data *smoothing;   // 平滑数据引用
};

// 初始化渲染数据
void rendering_init(struct rendering_data *rendering,
                  struct tracking_data *tracking,
                  struct smoothing_data *smoothing);

// 执行渲染
void rendering_render(struct rendering_data *rendering,
                    obs_source_t *target,
                    gs_effect_t *effect);

#endif // ZOOM_RENDERING_H
