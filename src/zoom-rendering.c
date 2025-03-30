#include "zoom-rendering.h"
#include <graphics/vec4.h>

// 初始化渲染数据
void rendering_init(struct rendering_data *rendering,
                  struct tracking_data *tracking,
                  struct smoothing_data *smoothing)
{
    rendering->tracking = tracking;
    rendering->smoothing = smoothing;
}

// 执行渲染
void rendering_render(struct rendering_data *rendering,
                    obs_source_t *target,
                    gs_effect_t *effect)
{
    if (!rendering->tracking || !rendering->smoothing || !target) {
        return;
    }
    
    // 获取源的尺寸
    uint32_t width = obs_source_get_width(target);
    uint32_t height = obs_source_get_height(target);
    
    if (!width || !height) {
        obs_source_skip_video_filter(target);
        return;
    }
    
    // 准备清空画面
    struct vec4 clear_color;
    vec4_zero(&clear_color);
    gs_clear(GS_CLEAR_COLOR, &clear_color, 0.0f, 0);
    
    // 设置正交投影（2D渲染）
    gs_ortho(0.0f, (float)width, 0.0f, (float)height, -100.0f, 100.0f);
    
    // 获取当前缩放比例
    float scale = rendering->smoothing->current_scale;
    
    // 获取缩放中心点
    float center_x, center_y;
    tracking_get_center(rendering->tracking, (float)width, (float)height, &center_x, &center_y);
    
    // 执行变换
    gs_matrix_push();
    
    // 将中心点移动到原点
    gs_matrix_translate3f(center_x, center_y, 0.0f);
    
    // 缩放
    gs_matrix_scale3f(scale, scale, 1.0f);
    
    // 将原点移回中心点
    gs_matrix_translate3f(-center_x, -center_y, 0.0f);
    
    // 渲染源
    obs_source_video_render(target);
    
    // 恢复矩阵
    gs_matrix_pop();
}
