#include <obs-module.h>
#include <util/platform.h>
#include <util/base.h>
#include <util/dstr.h>
#include <stdio.h>
#include <string.h>
#include "url-handler.h"

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#define snprintf _snprintf
#endif

// 规范化路径，确保路径分隔符一致性
// 返回一个新的字符串，使用者需要调用bfree()释放内存
char *normalize_path(const char *base_path, const char *filename)
{
    struct dstr result = {0};
    dstr_init(&result);
    dstr_copy(&result, base_path);
    
    // 确保基本路径不以分隔符结尾
    size_t len = result.len;
    if (len > 0) {
        char last_char = result.array[len - 1];
        if (last_char == '/' || last_char == '\\') {
            dstr_resize(&result, len - 1);
        }
    }
    
    // 添加适合平台的分隔符和文件名
    #ifdef _WIN32
    dstr_cat(&result, "\\");
    #else
    dstr_cat(&result, "/");
    #endif
    
    dstr_cat(&result, filename);
    
    return result.array;
}

bool open_url(const char *url)
{
    blog(LOG_INFO, "Opening file: %s", url);

#ifdef _WIN32
    // Windows环境使用ShellExecute打开URL
    HINSTANCE result = ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
    if ((size_t)result <= 32) {
        blog(LOG_ERROR, "ShellExecute failed with code: %d", (int)(size_t)result);
        return false;
    }
    return true;
#else
    // 其他系统使用系统默认命令
    static const char *const cmd =
#ifdef __APPLE__
        "open";
#else
        "xdg-open";
#endif

    struct dstr command = {0};
    dstr_init(&command);
    dstr_printf(&command, "%s \"%s\"", cmd, url);
    int result = system(command.array);
    dstr_free(&command);
    return result == 0;
#endif
}

// 检查给定盘符上是否存在OBS安装
static bool check_obs_installation(char drive, char *path_buffer, size_t buffer_size)
{
    #ifdef _WIN32
    // 构建可能的OBS安装路径
    snprintf(path_buffer, buffer_size, "%c:\\Program Files\\obs-studio\\data\\obs-plugins\\obs-zoom-filter", drive);
    if (os_file_exists(path_buffer)) {
        return true;
    }
    
    // 尝试Program Files (x86)路径
    snprintf(path_buffer, buffer_size, "%c:\\Program Files (x86)\\obs-studio\\data\\obs-plugins\\obs-zoom-filter", drive);
    if (os_file_exists(path_buffer)) {
        return true;
    }
    #endif
    
    return false;
}

// 从插件二进制路径推导OBS安装目录
static bool get_path_from_module(char *path_buffer, size_t buffer_size)
{
    bool success = false;
    char *bin_path = obs_module_file("");
    
    if (bin_path) {
        blog(LOG_INFO, "插件二进制路径: %s", bin_path);
        
        struct dstr path = {0};
        dstr_init(&path);
        dstr_copy(&path, bin_path);
        
        // 查找插件目录标识
        char *plugin_pos = strstr(path.array, "obs-plugins");
        if (plugin_pos) {
            // 从二进制目录推导到数据目录
            size_t prefix_len = plugin_pos - path.array;
            dstr_resize(&path, prefix_len);
            
            // 检查路径中是否已包含data目录，避免重复添加
            if (strstr(bin_path, "data/obs-plugins") || strstr(bin_path, "data\\obs-plugins")) {
                // 路径已经包含data部分，不需要重复添加
                #ifdef _WIN32
                dstr_cat(&path, "obs-plugins\\obs-zoom-filter");
                #else
                dstr_cat(&path, "obs-plugins/obs-zoom-filter");
                #endif
            } else {
                // 路径不包含data，需要添加
                #ifdef _WIN32
                dstr_cat(&path, "data\\obs-plugins\\obs-zoom-filter");
                #else
                dstr_cat(&path, "data/obs-plugins/obs-zoom-filter");
                #endif
            }
            
            if (path.len < buffer_size) {
                strcpy(path_buffer, path.array);
                blog(LOG_INFO, "从二进制路径推导的数据目录: %s", path_buffer);
                success = true;
            }
        }
        
        dstr_free(&path);
        bfree(bin_path);
    }
    
    return success;
}

// 获取OBS插件数据目录的路径
const char *get_plugin_data_path(void)
{
    // 使用静态缓冲区，提高效率
    static char path_buffer[512] = {0};
    
    if (path_buffer[0] == '\0') {
        // 方法1：从二进制位置推导
        if (get_path_from_module(path_buffer, sizeof(path_buffer))) {
            // 如果推导成功并且目录存在
            if (os_file_exists(path_buffer)) {
                blog(LOG_INFO, "使用从插件位置推导的OBS数据目录: %s", path_buffer);
                return path_buffer;
            }
        }
        
        // 方法2：检查所有可能的盘符上的标准安装路径
        #ifdef _WIN32
        // 从C盘开始检查到Z盘
        for (char drive = 'C'; drive <= 'Z'; drive++) {
            if (check_obs_installation(drive, path_buffer, sizeof(path_buffer))) {
                blog(LOG_INFO, "在盘符 %c: 上找到OBS安装", drive);
                return path_buffer;
            }
        }
        #else
        // 对于Linux/Mac，使用标准安装位置
        #ifdef __APPLE__
        strcpy(path_buffer, "/Applications/OBS.app/Contents/Resources/data/obs-plugins/obs-zoom-filter");
        #else // Linux
        strcpy(path_buffer, "/usr/share/obs/obs-plugins/obs-zoom-filter");
        #endif
        
        if (os_file_exists(path_buffer)) {
            blog(LOG_INFO, "使用OBS标准安装路径: %s", path_buffer);
            return path_buffer;
        }
        #endif
        
        // 最后的备用选项 - 使用相对路径
        #ifdef _WIN32
        strcpy(path_buffer, "../../data/obs-plugins/obs-zoom-filter");
        #else
        strcpy(path_buffer, "../data/obs-plugins/obs-zoom-filter");
        #endif
        blog(LOG_WARNING, "无法找到OBS安装目录，使用备用相对路径: %s", path_buffer);
    }
    
    blog(LOG_INFO, "Found plugin data path: %s", path_buffer);
    return path_buffer;
}
