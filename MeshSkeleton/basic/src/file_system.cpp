#include "file_system.h"
#include "line_stream.h"

#include <iostream>
#include <fstream>

// Transfer "\\" to "/"
static void flip_slashes(std::string &s)
{
    for (unsigned int i=0; i<s.length(); i++) {
        if(s[i] == '\\') {
            s[i] = '/' ;
        }
    }
}

// Rename fuction(old, new)
bool FileSystem::rename_file(const std::string &old_name,const std::string &new_name)
{
    return (::rename(old_name.c_str(), new_name.c_str())!=-1) ;
}

// Get suffix
std::string FileSystem::extension(const std::string &path)
{
    int len = int(path.length()) ;
    int point_pos = path.rfind(".");
    if (point_pos == -1) {
        return "" ;
    }
    std::string result = path.substr(point_pos + 1, len - point_pos - 1) ;
    for (unsigned int i = 0; i < result.length(); i++) {
        result[i] = tolower(result[i]) ;
    }

    return result ;
}

// Get file name without suffix
std::string FileSystem::base_name(const std::string &path) {
    int len = int(path.length()) ;
    int point_pos  = -1 ;
    int base_start = -1 ;
    for (int i=len-1; i>=0; i--) {
        if(point_pos == -1 && base_start == -1 && path[i] == '.') {
            point_pos = i ;
        }
        if(base_start == -1 && (path[i] == '/' || path[i] == '\\')) {
            base_start = i ;
        }
    }
    if (point_pos == -1) {
        point_pos = len ;
    }

    return path.substr(base_start + 1, point_pos - base_start - 1) ;
}

// Get path fuction
std::string FileSystem::dir_name(const std::string &path) {
    int len = int(path.length()) ;
    int base_start = -1 ;
    for (int i = len-1; i >= 0; i--) {
        if (base_start == -1 && (path[i] == '/' || path[i] == '\\')) {
            base_start = i ;
        }
    }
    if (base_start == -1) {
        return "." ;
    }
    std::string result = path.substr(0, base_start + 1) ;
    if (result[result.length() - 1] == '/') {
        result = result.substr(0, result.length() - 1) ;
    }

    return result;
}

// Copy file fuction
bool FileSystem::copy_file(const std::string &original, const std::string &copy)
{
    std::cerr << "Copy " << original << " to " << copy << std::endl;
    std::ifstream in(original.c_str());
    if (!in) {
        return false;
    }
    std::ofstream out(copy.c_str());
    LineInputStream lis(in);
    while (!lis.eof()) {
        lis.get_line();
        out << lis.current_line() << std::endl ;
    }

    return true;
}
