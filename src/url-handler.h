#pragma once

#include <stdbool.h>

// 打开URL（支持本地文件）
bool open_url(const char *url);

// 获取插件数据目录
const char *get_plugin_data_path(void);

// 规范化路径，确保路径分隔符一致性
// 返回一个新的字符串，使用者需要调用bfree()释放内存
char *normalize_path(const char *base_path, const char *filename);
