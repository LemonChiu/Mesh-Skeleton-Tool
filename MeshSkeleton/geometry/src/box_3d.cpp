#include "box_3d.h"
#include "object.h"
#include "box_facet.h"

Box3D::Box3D(const std::vector<Point3d> &points)
{
    set_vertices(points);
}

Box3D::~Box3D(void)
{
    for (unsigned int i=0; i<vertices_.size(); ++i)
        delete (vertices_[i]);
    vertices_.clear() ;

    for (unsigned int i=0; i<faces_.size(); ++i)
        delete (faces_[i]);
    faces_.clear() ;
}

void Box3D::add_vertex(const Point3d &p) {
    Vertex* v = new Vertex(p, vertices_.size(),Vector3d(0,0,0),Colorf(1.0f, 1.0f, 1.0f));
    vertices_.push_back(v);
}

void Box3D::add_facet(const std::vector<unsigned int> &indices) {
    if (indices.size() >= 3) {
        std::vector<Vertex*> vts;
        for (unsigned int i=0; i<indices.size(); ++i) {
            unsigned int index = indices[i];
            vts.push_back(vertices_[index]);
        }

        BoxFacet* f = new BoxFacet(vts, this);
        add_facet(f);
    }
}

void Box3D::add_facet(BoxFacet* f) {
    faces_.push_back(f);
}

double Box3D::min_squared_edge_length() {
    Point3d p0 = vertices_[0]->point();

    Point3d p2 = vertices_[2]->point();
    Point3d p6 = vertices_[6]->point();
    Point3d p1 = vertices_[1]->point();

    double s_len02 = CGAL::squared_distance(p0, p2);
    double s_len06 = CGAL::squared_distance(p0, p6);
    double s_len01 = CGAL::squared_distance(p0, p1);

    // filter degenerate boxes (0 lenthe edge)
    if (s_len02 < 1e-10)
        s_len02 = FLT_MAX;
    if (s_len06 < 1e-10)
        s_len06 = FLT_MAX;
    if (s_len01 < 1e-10)
        s_len01 = FLT_MAX;

    double s_len = std::min(s_len02, std::min(s_len06, s_len01));
    return /*std::sqrt*/(s_len);
}

double Box3D::max_squared_edge_length() {
    Point3d p0 = vertices_[0]->point();

    Point3d p2 = vertices_[2]->point();
    Point3d p6 = vertices_[6]->point();
    Point3d p1 = vertices_[1]->point();

    double s_len02 = CGAL::squared_distance(p0, p2);
    double s_len06 = CGAL::squared_distance(p0, p6);
    double s_len01 = CGAL::squared_distance(p0, p1);

    double s_len = std::max(s_len02, std::max(s_len06, s_len01));
    return /*std::sqrt*/(s_len);
}


//
//              p1 ____________ p7
//               /|           /|
//              / |          / |
//          p3 /__|_______p5/  |
//             |  |        |   |
//             |  p0_ _ _ _|_ _| p6
//             |  /        |  /
//             | /         | /
//             |/__________|/
//          p2            p4
//
//
//
//
void Box3D::set_vertices(const std::vector<Point3d> &points) {
    for (unsigned int i=0; i<points.size(); ++i) {
        add_vertex(points[i]);
    }

    std::vector<unsigned int> indices;
    indices.push_back(0); indices.push_back(2); indices.push_back(3); indices.push_back(1);
    add_facet(indices);

    indices.clear();
    indices.push_back(2); indices.push_back(4); indices.push_back(5); indices.push_back(3); // front facet
    add_facet(indices);

    indices.clear();
    indices.push_back(4); indices.push_back(6); indices.push_back(7); indices.push_back(5);
    add_facet(indices);

    indices.clear();
    indices.push_back(0); indices.push_back(1); indices.push_back(7); indices.push_back(6); // back facet
    add_facet(indices);

    indices.clear();
    indices.push_back(1); indices.push_back(3); indices.push_back(5); indices.push_back(7);
    add_facet(indices);

    indices.clear();
    indices.push_back(0); indices.push_back(6); indices.push_back(4); indices.push_back(2);
    add_facet(indices);
}

Bbox3d Box3D::bbox() const {
    return Object::bbox_of(vertices_);
}

std::vector<Facet*> Box3D::collect_faces() const {
    std::vector<Facet*> result;
    for (unsigned int i=0; i<faces_.size(); ++i)
        result.push_back(faces_[i]);

    return result;
}

std::vector<BoxFacet*> Box3D::side_faces() const {
    std::vector<BoxFacet*> result;
    for (unsigned int i=0; i<faces_.size(); ++i) {
        if (i==1 || i==3)
            continue;

        result.push_back(faces_[i]);
    }

    return result;
}
