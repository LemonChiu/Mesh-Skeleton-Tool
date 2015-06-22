#include "box_facet.h"

BoxFacet::BoxFacet(const std::vector<Vertex*> &vertices, Box3D* parent_box)
: Facet(vertices)
, parent_box_(parent_box)
{
    init_position();
}

BoxFacet::BoxFacet(const std::vector<Vertex*> &vertices, const Vector3d &n, Box3D* parent_box)
: Facet(vertices, n)
, parent_box_(parent_box)
{
    init_position();
}

BoxFacet::~BoxFacet()
{
}

void BoxFacet::init_position() {
    Vector3d pos(0.0f, 0.0f, 0.0f);
    for (unsigned int i=0; i<vertices_.size(); ++i) {
        pos = pos + (vertices_[i]->point() - CGAL::ORIGIN);
    }

    pos = pos / static_cast<double>(vertices_.size());
    position_ = CGAL::ORIGIN + pos;
}

void BoxFacet::adjust_to(const Point3d &pos) {
    //const Plane3d &sup_plane = supporting_plane();
    //Point3d proj = supporting_plane().projection(pos);

    ////////////////////////////////////////////////////////////////////////////
    //Vector3d offset = pos - proj;
    //for (unsigned int i=0; i<vertices_.size(); ++i) {
    //  Vertex* v = vertices_[i];
    //  v->set_point(v->point() + offset);
    //}

    //position_ = position_ + offset;

    Line3d line(position_, normal_);
    Point3d old_pos = position_;
    position_ = line.projection(pos);

    Vector3d offset = position_ - old_pos;
    for (unsigned int i=0; i<vertices_.size(); ++i) {
        Vertex* v = vertices_[i];
        v->set_point(v->point() + offset);
    }
}

double BoxFacet::squared_area() const {
    double squared_len_01 = CGAL::squared_distance(vertices_[0]->point(), vertices_[1]->point());
    double squared_len_12 = CGAL::squared_distance(vertices_[1]->point(), vertices_[2]->point());

    return (squared_len_01 * squared_len_12);
}

std::vector<BoxFacet*> BoxFacet::collect_box_faces() const {
    std::vector<BoxFacet*> result;
    result.push_back(const_cast<BoxFacet*>(this));
    return result;
}

std::vector<Segment3d> BoxFacet::collect_edges() const {
    std::vector<Segment3d> edges;

    for (unsigned int i=0; i<vertices_.size() - 1; ++i) {
        Segment3d e(vertices_[i]->point(), vertices_[i+1]->point());
        edges.push_back(e);
    }

    edges.push_back(Segment3d(vertices_[3]->point(), vertices_[0]->point()));
    return edges;
}

std::vector<Facet*> BoxFacet::collect_faces() const {
    std::vector<Facet*> result;
    BoxFacet* f = const_cast<BoxFacet*>(this);
    result.push_back(dynamic_cast<Facet*>(f));
    return result;
}
