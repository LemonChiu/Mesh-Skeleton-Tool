
#include "prism.h"
#include "geometry_global.h"
#include "vertex.h"
#include "facet.h"

Prism::Prism()
    : height_(0.005f) // for rendering
{
}

void Prism::clear() {
    for (unsigned int i=0; i<base_vertices_.size(); ++i)
        delete base_vertices_[i];

    for (unsigned int i=0; i<top_vertices_.size(); ++i)
        delete top_vertices_[i];

    for (unsigned int i=0; i<faces_.size(); ++i)
        delete faces_[i];

    base_vertices_.clear();
    top_vertices_.clear();
    faces_.clear();
}

Prism::~Prism() {
    clear();
}


unsigned int Prism::size() const
{
    return base_vertices_.size();
}

void Prism::add_point(const Point3d &p) {
    int index = base_vertices_.size();
    base_vertices_.push_back(new Vertex(p, index));

    Point3d &q = p + direction_ * height_;
    top_vertices_.push_back(new Vertex(q, index));
}


// for modification of the prism
void Prism::set_base_point(unsigned int idx, const Point3d &p) {
    base_vertices_[idx]->set_point(p);
}


void Prism::set_top_point(unsigned int idx, const Point3d &p) {
    top_vertices_[idx]->set_point(p);
}


double Prism::height() const
{
    return height_;
}


void Prism::set_height(double h, bool bTop) {
    height_ = h;
    if(bTop)
        update_top_vertices_base_as_reference();
    else
        update_base_vertices_top_as_reference();
}

void Prism::update_base_vertices_top_as_reference()
{
    unsigned int num = top_vertices_.size();
    for (unsigned int i=0; i<num; ++i) {
        const Point3d &pos = top_vertices_[i]->point() - direction_ * height_;
        set_base_point(i, pos);
    }
}


const Vector3d &Prism::direction() const
{
    return direction_;
}
void Prism::set_direction(const Vector3d &dir)
{
    direction_ = dir;
}

void Prism::update_top_vertices_base_as_reference() {
    unsigned int num = base_vertices_.size();
    for (unsigned int i=0; i<num; ++i) {
        const Point3d &pos = base_vertices_[i]->point() + direction_ * height_;
        set_top_point(i, pos);
    }
}

Plane3d Prism::base_plane() const {
    return Plane3d(base_vertices_[0]->point(), direction_);
}

Plane3d Prism::top_plane() const {
    return Plane3d(top_vertices_[0]->point(), direction_);
}

Line3d Prism::side_edge(unsigned int i) const {
    return Line3d(base_vertices_[i]->point(), top_vertices_[i]->point());
}



std::vector<Vertex*> &Prism::base_vertices()
{
    return base_vertices_;
}

std::vector<Vertex*> &Prism::top_vertices()
{
    return top_vertices_;
}

const std::vector<Vertex*> &Prism::base_vertices() const
{
    return base_vertices_;
}
const std::vector<Vertex*> &Prism::top_vertices()  const
{
    return top_vertices_;
}

std::vector<Facet*> Prism::faces()
{
    return faces_;
}
const std::vector<Facet*> Prism::faces() const
{
    return faces_;
}

// NOTE:: you must call this when you finished the 'construction' of a prism
void Prism::end_construction() {
    if (!faces_.empty())
        faces_.clear();

    Facet* base_facet = new Facet(base_vertices_);  base_facet->reverse_normal();
    Facet* top_facet = new Facet(top_vertices_);

    faces_.push_back(base_facet);
    faces_.push_back(top_facet);

    // sides
    unsigned int count = base_vertices_.size();
    for (unsigned int i=0; i<count; ++i) {
        std::vector<Vertex*> vts;
        Vertex* a = base_vertices_[i];              vts.push_back(a);
        Vertex* b = base_vertices_[(i+1) % count];  vts.push_back(b);
        Vertex* c = top_vertices_[(i+1) % count];   vts.push_back(c);
        Vertex* d = top_vertices_[i];               vts.push_back(d);

        Facet* f = new Facet(vts);
        faces_.push_back(f);
    }
}


Bbox3d Prism::bbox() const {
    std::vector<Point3d> points;

    for (unsigned int i=0; i<base_vertices_.size(); ++i) {
        points.push_back(base_vertices_[i]->point());
        points.push_back(top_vertices_[i]->point());
    }
    return global::boundingBoxOf(points);
}

void Prism::translate(const Vector3d &d) {
    for (unsigned int i=0; i<top_vertices_.size(); ++i) {
        const Point3d &old_pos = top_vertices_[i]->point();
        Point3d new_pos = old_pos + d;

        set_top_point(i, new_pos);
    }

    for (unsigned int i=0; i<base_vertices_.size(); ++i) {
        const Point3d &old_pos = base_vertices_[i]->point();
        Point3d new_pos = old_pos + d;

        set_base_point(i, new_pos);
    }
}


Prism &Prism::operator = (const Prism &prism) {
    clear();

    set_direction(prism.direction());
    set_height(prism.height());

    const std::vector<Vertex*> &base_vts = prism.base_vertices();
    for (unsigned int i=0; i<base_vts.size(); ++i) {
        const Point3d &p = base_vts[i]->point();
        add_point(p);
    }

    end_construction();

    return *this;
}

std::vector<Facet*> Prism::collect_faces() const
{
    return faces_;
}
