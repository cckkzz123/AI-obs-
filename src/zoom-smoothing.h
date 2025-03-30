#ifndef ZOOM_SMOOTHING_H
#define ZOOM_SMOOTHING_H

#include <stdbool.h>
#include <stdint.h>

// 平滑模式
#define SMOOTH_MODE_LINEAR      0   // 线性
#define SMOOTH_MODE_EXPONENTIAL 1   // 指数
#define SMOOTH_MODE_LOGARITHMIC 2   // 对数

// 缩放平滑数据结构
struct smoothing_data {
    bool enabled;           // 是否启用平滑过渡
    float smoothness;       // 平滑系数（0.1-1.0）
    int mode;               // 平滑模式
    float current_scale;    // 当前缩放值
    float target_scale;     // 目标缩放值
    uint64_t transition_start; // 过渡开始时间
    uint64_t animation_time;   // 动画时长(ns)
    
    // 增强的平滑控制参数
    float start_speed;      // 起始速度系数(0.1-2.0)
    float end_deceleration; // 结束减速系数(0.1-2.0)
    float overshoot;        // 超调系数(0.0-0.5)
};

// 初始化平滑数据
void smoothing_init(struct smoothing_data *smoothing);

// 设置新的目标缩放值
void smoothing_set_target(struct smoothing_data *smoothing, 
                        float target_scale, 
                        uint64_t current_time);

// 更新并获取当前缩放值
float smoothing_update(struct smoothing_data *smoothing, 
                      uint64_t current_time);

// 检查平滑过渡是否完成
bool smoothing_is_finished(struct smoothing_data *smoothing, 
                         uint64_t current_time);

#endif // ZOOM_SMOOTHING_H
