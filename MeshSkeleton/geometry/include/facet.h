#ifndef _FACET_H_
#define _FACET_H_

#include "geometry_types.h"
#include "vertex.h"

class FacetEdge;

class  Facet
{
public:
    Facet(const std::vector<Point3d> &points) ;
    Facet(const std::vector<Point3d> &points, const Vector3d &n) ;
    Facet(const std::vector<Vertex*> &vertices) ;
    Facet(const std::vector<Vertex*> &vertices, const Vector3d &n);
    ~Facet();

    Plane3d supporting_plane() const ;

    std::vector<Vertex*> & vertices() { return vertices_; }
    const std::vector<Vertex*> & vertices() const { return vertices_; }

    std::vector<FacetEdge*> &edges() { return edges_; }
    const std::vector<FacetEdge*> &edges() const { return edges_; }

    unsigned int size() const { return vertices_.size(); }

    Vector3d &normal() { return normal_; }
    const Vector3d &normal() const { return normal_; }

    void set_normal(const Vector3d &n) { normal_ = n; }
    void reverse_normal() { normal_ = -normal_; }

    virtual Bbox3d bbox() const ;

    virtual std::vector<Facet*> collect_faces() const
    {
        std::vector<Facet*> faces;
        faces.push_back(const_cast<Facet*>(this));
        return faces;
    }

    double min_squared_edge_length() const ;

private:
    void create_edges_from_vertices();

protected:
    std::vector<Vertex*> vertices_;
    std::vector<FacetEdge*> edges_;
    Vector3d normal_;
};

#endif
