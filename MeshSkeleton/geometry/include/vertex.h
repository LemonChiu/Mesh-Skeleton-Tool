#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "color.h"
#include "geometry_types.h"
#include "GL/glut.h"

class Vertex
{
public:
    Vertex(const Point3d &p, unsigned int idx,
           const Vector3d &n = Vector3d(0.0f, 0.0f, 0.0f),
           const Colorf &c = Colorf(0.0f, 0.0f, 0.0f))
        : point_(p)
        , normal_(n)
        ,color_(c)
        , is_highlighted_(false)
    {
        index_ = idx;
    }

    Point3d &point()
    {
        return point_ ;
    }
    const Point3d &point() const
    {
        return point_ ;
    }

    Colorf color() const
    {
        if (is_highlighted_) {
            return Colorf(1.0f,0.0f,0.0f,color_.a());
        } else {
            return color_;
        }
    }

    Vector3d &normal()
    {
        return normal_ ;
    }
    const Vector3d &normal() const
    {
        return  normal_;
    }

    void set_point(const Point3d &p)
    {
        point_ = p ;
    }

    void set_color(const Colorf &c)
    {
        color_ = c;
    }

    void set_normal(const Vector3d &n)
    {
        normal_ = n;
    }

    void translate(const Vector3d &d)
    {
        point_ = point_ + d;
    }

    bool is_highlighted() const
    {
        return is_highlighted_;
    }

    void set_highlighted(bool b)
    {
        is_highlighted_ = b;
    }

    unsigned int idx() const
    {
        return index_;
    }

    inline void draw(bool wireframe = false) const
    {
        glColor4fv(color().data());
        glVertex3d(point_.x(), point_.y(), point_.z());
    }
    inline void draw(Colorf &c, bool wireframe = false) const
    {
        glColor4fv(c.data());
        glVertex3d(point_.x(), point_.y(), point_.z());
    }
    inline void draw_with_name(unsigned int i) const {
        glPushName(i);
        glRasterPos3d(point_.x(), point_.y(), point_.z());
        glPopName();
    }

private:
    Point3d point_ ;
    Vector3d normal_;
    Colorf color_;
    unsigned int index_;
    bool is_highlighted_;
};

#endif
