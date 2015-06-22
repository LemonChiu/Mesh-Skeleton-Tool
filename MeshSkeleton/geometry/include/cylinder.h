#pragma once

#include "color.h"
#include "geometry_types.h"

class Prism;
class Facet;

class CylinderMy
{
public:
    CylinderMy() ;
    CylinderMy(const CylinderMy &cld);
    CylinderMy(double radius, const Point3d &base_center, const Point3d &top_center);
    CylinderMy(double radius, const Point3d &base_center, double height, const Vector3d &direction);
    ~CylinderMy();

    CylinderMy &operator = (const CylinderMy &cld);

    double radius() const;
    double height() const;
    void set_radius(double r);
    void set_color(const Colorf &color);
    void set_base_center(const Point3d &c);
    void set_top_center(const Point3d &c);
    void translate(const Vector3d &d);
    void reverse_direction();
    void draw(bool wireframe = false) const;
    void draw_with_name(unsigned int i) const ;
    const Colorf &facet_color() const;
    const Point3d &base_center() const;
    Point3d top_center() const;
    Vector3d direction() const;
    Bbox3d bbox() const ;
    Prism* to_prism() const ;

    // for cylinder, only need top and bottom
    std::vector<Facet*> collect_faces() const ;

protected:
    double radius_;
    Point3d base_center_;
    Point3d top_center_;
    Colorf facet_color_;
    mutable Prism *internal_rep_;
};


