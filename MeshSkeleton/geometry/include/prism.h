#pragma once

#include <vector>
#include "geometry_types.h"

class Facet;
class Vertex;

class  Prism
{
public:
    Prism(void);
    ~Prism() ;

    void clear();

    // NOTE:: VERY IMPORTANT!!!
    // you must call this when you finished the 'construction' of a prism
    void end_construction();

    Prism &operator = (const Prism &prism) ;

    unsigned int size() const ;

    void  set_height(double h, bool bTop = true) ;
    double height() const ;

    const Vector3d &direction() const ;
    void set_direction(const Vector3d &dir);

    Plane3d base_plane() const ;
    Plane3d top_plane() const ;

    Line3d side_edge(unsigned int i) const;

    std::vector<Vertex*> &base_vertices() ;
    std::vector<Vertex*> &top_vertices();

    const std::vector<Vertex*> &base_vertices() const;
    const std::vector<Vertex*> &top_vertices()  const;

    std::vector<Facet*> faces();
    const std::vector<Facet*> faces() const ;

    // for construction / modification of a prism
    void add_point(const Point3d &p) ;

    void set_base_point(unsigned int idx, const Point3d &p) ;
    void set_top_point(unsigned int idx, const Point3d &p) ;

    //////////////////////////////////////////////////////////////////////////

    Bbox3d bbox() const ;
    void translate(const Vector3d &d);
    std::vector<Facet*> collect_faces() const;

private:
    void update_top_vertices_base_as_reference() ;
    void update_base_vertices_top_as_reference();

private:
    std::vector<Vertex*> base_vertices_;
    std::vector<Vertex*> top_vertices_;
    std::vector<Facet*> faces_;

    double height_;
    Vector3d direction_;
};

