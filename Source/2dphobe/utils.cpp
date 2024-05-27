#include "utils.h"
#include <windows.h>

std::string utils::get_bin_dir()
{
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::string f(buffer);
    return f.substr(0, f.find_last_of("\\/"));
}

std::string utils::get_app_dir()
{
    std::string path = get_bin_dir();
    return path.substr(0, path.find_last_of("\\")).append("\\");
}