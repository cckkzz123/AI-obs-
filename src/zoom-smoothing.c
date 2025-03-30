#include "zoom-smoothing.h"
#include <math.h>
#include <util/platform.h>

// 初始化平滑数据
void smoothing_init(struct smoothing_data *smoothing)
{
    smoothing->enabled = true;
    smoothing->smoothness = 0.6f;
    smoothing->mode = SMOOTH_MODE_EXPONENTIAL;
    smoothing->current_scale = 1.0f;
    smoothing->target_scale = 1.0f;
    smoothing->transition_start = 0;
    smoothing->animation_time = 400 * 1000000; // 400ms默认
    
    // 初始化新增的平滑控制参数
    smoothing->start_speed = 1.0f;    // 默认值1.0表示正常速度
    smoothing->end_deceleration = 1.0f; // 默认值1.0表示正常减速
    smoothing->overshoot = 0.0f;      // 默认值0.0表示无超调
}

// 设置新的目标缩放值
void smoothing_set_target(struct smoothing_data *smoothing, 
                        float target_scale, 
                        uint64_t current_time)
{
    // 只有当目标和当前值不同时才触发过渡
    if (fabsf(target_scale - smoothing->target_scale) > 0.001f) {
        smoothing->target_scale = target_scale;
        
        if (smoothing->enabled) {
            // 启用平滑过渡
            smoothing->transition_start = current_time;
        } else {
            // 禁用平滑过渡，立即设置
            smoothing->current_scale = target_scale;
        }
    }
}

// 根据不同的平滑模式计算当前缩放值
static float calculate_smoothed_scale(struct smoothing_data *smoothing, 
                                    float start_scale,
                                    float end_scale, 
                                    float t)
{
    if (t >= 1.0f) {
        return end_scale;
    }
    
    float diff = end_scale - start_scale;
    
    // 防止震荡
    if (fabsf(diff) < 0.001f) {
        return end_scale;
    }
    
    // 应用起始速度修正值 - 调整进度以影响初始速度
    float adjusted_t = powf(t, 2.0f / smoothing->start_speed);
    
    // 计算基本平滑值
    float smooth_value = 0.0f;
    
    switch (smoothing->mode) {
        case SMOOTH_MODE_LINEAR:
            // 线性平滑
            smooth_value = adjusted_t;
            break;
            
        case SMOOTH_MODE_EXPONENTIAL:
            // 指数平滑：提供更自然的开始和更平缓的结束
            // 应用结束减速系数来调整曲线陡峭度
            smooth_value = 1.0f - expf(-adjusted_t * (2.0f / (smoothing->smoothness * smoothing->end_deceleration)));
            break;
            
        case SMOOTH_MODE_LOGARITHMIC:
            // 对数平滑：提供更快的开始和更平缓的结束
            smooth_value = logf(1.0f + adjusted_t * 9.0f) / logf(10.0f);
            break;
            
        default:
            smooth_value = adjusted_t;
            break;
    }
    
    // 应用超调/弹性效果 - 使用正弦波在接近目标时产生波动
    if (smoothing->overshoot > 0.0f && t > 0.5f) {
        // 仅在动画后半段应用超调
        float overshoot_factor = smoothing->overshoot * sinf((t - 0.5f) * 2.0f * 3.14159f);
        float damping = (1.0f - t) * 2.0f; // 随时间衰减
        
        // 添加阻尼振荡到平滑值
        smooth_value += overshoot_factor * damping;
        
        // 限制在0-1范围内（或稍微超出以产生弹性）
        if (smooth_value > 1.0f + smoothing->overshoot) {
            smooth_value = 1.0f + smoothing->overshoot;
        }
    }
    
    // 应用最终缩放计算
    return start_scale + diff * smooth_value;
}

// 更新并获取当前缩放值
float smoothing_update(struct smoothing_data *smoothing, 
                      uint64_t current_time)
{
    if (!smoothing->enabled || smoothing->transition_start == 0 || 
        smoothing->animation_time == 0) {
        return smoothing->target_scale;
    }
    
    // 计算过渡进度 (0.0 - 1.0)
    float t = (float)(current_time - smoothing->transition_start) / 
              (float)smoothing->animation_time;
    
    // 如果超过动画时间，直接设置为目标值
    if (t >= 1.0f) {
        smoothing->current_scale = smoothing->target_scale;
        smoothing->transition_start = 0; // 标记过渡结束
        return smoothing->current_scale;
    }
    
    // 应用选定的平滑算法
    float start_scale = smoothing->current_scale;
    smoothing->current_scale = calculate_smoothed_scale(
        smoothing, start_scale, smoothing->target_scale, t);
    
    return smoothing->current_scale;
}

// 检查平滑过渡是否完成
bool smoothing_is_finished(struct smoothing_data *smoothing, 
                         uint64_t current_time)
{
    if (!smoothing->enabled || smoothing->transition_start == 0 || 
        smoothing->animation_time == 0) {
        return true;
    }
    
    float t = (float)(current_time - smoothing->transition_start) / 
              (float)smoothing->animation_time;
    
    return (t >= 1.0f);
}
