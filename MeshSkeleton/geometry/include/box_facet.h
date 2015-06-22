#pragma once

#include "facet.h"

class Box3D;

class BoxFacet : public Facet
{
public:
    BoxFacet(const std::vector<Vertex*> &vertices, Box3D *parent_box) ;
    BoxFacet(const std::vector<Vertex*> &vertices, const Vector3d &n, Box3D *parent_box);
    ~BoxFacet();

    Box3D *parent_box() { return parent_box_; }
    const Box3D *parent_box() const { return parent_box_; }
    void set_parent_box(Box3D *box) { parent_box_ = box; }

    const Vector3d &normal() const { return Facet::normal(); }
    void set_normal(const Vector3d &n) { Facet::set_normal(n); }

    // move the position to 'pos' along it normal
    void  adjust_to(const Point3d &pos) ;

    double squared_area() const ;

    std::vector<Facet*> collect_faces() const ;
    std::vector<BoxFacet*> collect_box_faces() const ;
    std::vector<Segment3d> collect_edges() const ;

    Plane3d supporting_plane() const { return Facet::supporting_plane(); }

private:
    void init_position();

protected:
    Box3D *parent_box_;
    Point3d position_;
};
