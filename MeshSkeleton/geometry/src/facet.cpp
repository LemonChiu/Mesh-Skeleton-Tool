#include "facet.h"
#include "vertex.h"
#include "object.h"
#include "facet_edge.h"
#include "cylinder.h"
#include "GL/glut.h"

Facet::Facet(const std::vector<Point3d> &points)
{
    for (unsigned int i = 0; i < points.size(); ++i) {
        const Point3d &p = points[i];
        Vertex* v = new Vertex(p, vertices_.size());
        vertices_.push_back(v);
    }

    if (vertices_.size() >= 3) {
        Point3d A = vertices_[0]->point();
        Point3d B = vertices_[1]->point();
        Point3d C = vertices_[2]->point();
        normal_ = CGAL::cross_product(C-B, A-B); // TODO: check the direction, why?
        global::normalize(normal_);
    }

    create_edges_from_vertices();
}

Facet::Facet(const std::vector<Point3d> &points, const Vector3d &n)
{
    for (unsigned int i=0; i<points.size(); ++i) {
        const Point3d &p = points[i];
        Vertex* v = new Vertex(p, vertices_.size());
        vertices_.push_back(v);
    }

    normal_ = n;
    global::normalize(normal_);

    create_edges_from_vertices();
}

Facet::Facet(const std::vector<Vertex*> &vertices)
: vertices_(vertices)
{
    if (vertices.size() >= 3) {
        Point3d A = vertices[0]->point();
        Point3d B = vertices[1]->point();
        Point3d C = vertices[2]->point();
        normal_ = CGAL::cross_product(C-B, A-B);  // TODO: check the direction, why?
        global::normalize(normal_);
    }

    create_edges_from_vertices();
}

Facet::Facet(const std::vector<Vertex*> &vertices, const Vector3d &n)
: vertices_(vertices)
, normal_(n)
{
    create_edges_from_vertices();
}

Facet::~Facet() {
    for (unsigned int i=0; i<edges_.size(); ++i) {
        delete edges_[i];
    }
}

void Facet::create_edges_from_vertices() {
    unsigned int num = vertices_.size();
    for (unsigned int i=0; i<num; ++i) {
        Vertex* s = vertices_[i];
        Vertex* t = vertices_[(i+1)%num];

        FacetEdge* e = new FacetEdge(s, t);
        Vector3d v = t->point() - s->point();
        Vector3d n = CGAL::cross_product(v, normal_);
        global::normalize(n);
        e->set_normal(n);
        edges_.push_back(e);
    }
}

Plane3d Facet::supporting_plane() const {
    Point3d p = vertices_[0]->point();
    return Plane3d(p, normal_);
}

Bbox3d Facet::bbox() const {
    return Object::bbox_of(vertices_);
}

double Facet::min_squared_edge_length() const {
    double min_len = CGAL::squared_distance(vertices_[0]->point(), vertices_[vertices_.size() - 1]->point());

    for (unsigned int i=0; i<vertices_.size() - 1; ++i) {
        const Point3d &p = vertices_[i]->point();
        const Point3d &q = vertices_[i+1]->point();

        double len = CGAL::squared_distance(p, q);
        if (len < min_len)
            min_len = len;
    }

    return min_len;
}
