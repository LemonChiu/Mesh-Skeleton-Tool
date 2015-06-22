#ifndef _FILE_IO_H_
#define _FILE_IO_H_

#include "normalizer.h"
#include "object.h"
#include "attribute.h"

#include <iostream>

class Skeleton;
class PointSet;
class Normalizer;
class Mesh;

class  FileIO
{
public:
    static Object *readObject(const std::string &file_name, ObjectType &type);
    static Mesh *readMesh(const std::string &file_name);
    static PointSet *readPointSet(const std::string &file_name , bool fast = false , unsigned int maxnum = 1000);
    // return the skeleton
    static Skeleton *readSkeleton(const std::string &file_name);
    // save the point set to a file. return false if failed.
    static bool saveSkeleton(Skeleton *skl,
        const std::string &file_name,
        Normalizer *normalizer = NULL);

    static std::string title();

protected:

    static void load_obj(Mesh *mesh, std::ifstream &in);
    static void load_ply(Mesh *mesh, std::ifstream &in);
    static void load_ply(PointSet *pointSet,  std::ifstream &in, bool fast = false, unsigned int maxnum = 1000);
    static void load_obj(PointSet *pointSet,  std::ifstream &in, bool fast = false , unsigned int maxnum = 1000);

    static void load_cg(Skeleton *skl, std::ifstream &in);
    static void save_cg(Skeleton *skl, std::ofstream &out, Normalizer* normalizer = NULL);
    static void load_skl(Skeleton *skl, std::ifstream &in);
    static void load_out(Skeleton *skl, std::ifstream &in);
    static void save_out(Skeleton *skl, std::ofstream &out, Normalizer* normalizer = NULL);

private:

};

#endif
