
#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

#include <string>
#include <vector>

class  FileSystem
{
public:
    static std::string extension(const std::string &path) ;
    static std::string base_name(const std::string &path) ;
    static std::string dir_name(const std::string &path) ;

    static bool rename_file(const std::string &old_name, const std::string &new_name);
    static bool copy_file(const std::string &original, const std::string &copy);

    static unsigned int get_time_stamp(const std::string &filename) ;
} ;

#endif
