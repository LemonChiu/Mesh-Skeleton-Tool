#pragma once

#include "geometry_types.h"
#include "geometry_global.h"

class Vertex;

class  FacetEdge
{
public:
    FacetEdge(Vertex* s, Vertex* t)
        : source_(s)
        , target_(t)
    {
        update_position();
    }

    ~FacetEdge(void) {}

    virtual const Vector3d &normal() const { return normal_; }
    virtual void set_normal(const Vector3d &n) { normal_ = n; }

    Point3d mid_point() const { return source_->point() + (target_->point() - source_->point()) * 0.5f; }
    const Vertex* source() const { return source_; }
    const Vertex* target() const { return target_; }
    Line3d supporting_line() const { return Line3d(source_->point(), target_->point()); }

    double squared_length() const { return CGAL::squared_distance(source_->point(), target_->point()); }
    double squared_area() const { return squared_length(); }

    Segment3d to_segment() const { return Segment3d(source_->point(), target_->point()); }
    Vector3d to_vector() const { Vector3d v = to_segment().to_vector(); global::normalize(v); return v; }

    void adjust_to(const Point3d &pos)
    {
        Line3d line(position_, normal_);
        const Point3d &proj = line.projection(pos);
        Vector3d offset = proj - position_;
        translate(offset);
    }

    void translate(const Vector3d &v)
    {
        source_->set_point(source_->point() + v);
        target_->set_point(target_->point() + v);

        update_position();
    }

    virtual void update_position()
    {
        const Point3d &p = source_->point();
        const Point3d &q = target_->point();
        position_ = Point3d((p.x() + q.x()) * 0.5f, (p.y() + q.y()) * 0.5f, (p.z() + q.z()) * 0.5f);
    }

private:
    Vertex *source_;
    Vertex *target_;
    Point3d position_;
    Vector3d normal_;
};
