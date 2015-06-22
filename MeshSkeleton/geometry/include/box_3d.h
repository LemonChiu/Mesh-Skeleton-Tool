#pragma once

#include "facet.h"
#include "box_facet.h"

//
//              p7 ____________ p5
//               /|           /|
//              / |          / |
//          p1 /__|_______p3/  |
//             |  |        |   |
//             |  p6_ _ _ _|_ _| p4
//             |  /        |  /
//             | /         | /
//             |/__________|/
//          p0            p2
//

class Box3D
{
public:
    Box3D(const std::vector<Point3d> &points);
    ~Box3D(void);

    void set_vertices(const std::vector<Point3d> &points);

    void add_vertex(const Point3d &p) ;

    void add_facet(BoxFacet* f) ;
    void add_facet(const std::vector<unsigned int> &indices) ;

    unsigned int num_vertices() const { return vertices_.size(); }
    unsigned int num_facets() const { return faces_.size(); }

    double min_squared_edge_length() ;
    double max_squared_edge_length() ;

    std::vector<Vertex*> vertices() { return vertices_; }
    const std::vector<Vertex*> vertices() const { return vertices_; }

    //////////////////////////////////////////////////////////////////////////

    std::vector<BoxFacet*> &faces() { return faces_; }
    const std::vector<BoxFacet*> &faces() const { return faces_; }
    std::vector<Facet*> collect_faces() const;

    BoxFacet* front_facet() { return faces_[1]; }
    const BoxFacet* front_facet() const { return faces_[1]; }

    BoxFacet* back_facet() { return faces_[3]; }
    const BoxFacet* back_facet() const { return faces_[3]; }

    std::vector<BoxFacet*> side_faces() const ;

    Bbox3d bbox() const ;

private:
    std::vector<Vertex*> vertices_;
    std::vector<BoxFacet*> faces_;
};
