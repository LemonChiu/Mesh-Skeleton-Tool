#pragma once

/**
* purpose:normalizer of object consist of vertices
*/

#include "geometry_types.h"
#include "stop_watch.h"

class Skeleton;
class Object;

class  Normalizer
{
public:
    Normalizer()
        : orig_radius_(0.0f)
        , orig_center_(0.0f, 0.0f, 0.0f)
        , normalized_radius_(1.0f)
    {
    }

    ~Normalizer() {}

    static std::string title()
    {
        return "[Normalizer]: ";
    }

    void apply(Object *point_set, float r = 1.0f);
    void apply_by_old_parameter(Skeleton *skeleton);
    void apply_by_old_parameter(Object *point_set);

    inline Point3d restore(const Point3d &p)
    {
        Vector3d v = p - CGAL::ORIGIN;
        v = (orig_radius_ / normalized_radius_) * v;
        return (orig_center_ + v);
    }

private:
    float orig_radius_;
    Point3d orig_center_;
    float normalized_radius_;
} ;
