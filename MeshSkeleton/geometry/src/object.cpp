#include "object.h"

#include "stop_watch.h"
#include <cgal/bounding_box.h>
#include <list>

using namespace kdtree;

Object::Object(const std::string &name, ObjectType type)
: name_(name)
, kdTree_(NULL)
, bbox_exist_(false)
,type_(type)
, is_normal_(false)
{
}

Bbox3d Object::bbox() const {
    assert(vertices_.size() > 0);

    if (!bbox_exist_) {
        return bbox_dirty();
    }

    return bbox_;
}

// bounding box details
Bbox3d Object::bbox_dirty() const {
    assert(vertices_.size() > 0);

    unsigned int minimum1 = 1000000;
    unsigned int minimun2 = 100000;
    unsigned int partial = 1;
    if (vertices_.size() > minimum1) {
        partial = 100;
    }else if (vertices_.size() > minimun2) {
        partial = 10;
    }

    std::list<Point3d> points;
    for (unsigned int i = 0; i < vertices_.size(); i += partial) { // sampling
        points.push_back(vertices_[i]->point()) ;
    }

    Cuboid3d ret = CGAL::bounding_box(points.begin(), points.end());
    bbox_ = Bbox3d(ret.xmin(), ret.ymin(), ret.zmin(), ret.xmax(), ret.ymax(), ret.zmax());
    bbox_exist_ = true;

    return bbox_;
}

// get bounding box of vertices
Bbox3d Object::bbox_of(const std::vector<Vertex*> &vertices)
{
    assert(vertices.size() > 0);

    unsigned int minimum1 = 1000000;
    unsigned int minimun2 = 100000;
    unsigned int partial = 1;
    if (vertices.size() > minimum1) {
        partial = 100;
    }else if (vertices.size() > minimun2) {
        partial = 10;
    }

    std::list<Point3d> points;
    for (unsigned int i = 0; i < vertices.size(); i += partial) { // sampling
        points.push_back(vertices[i]->point()) ;
    }

    Cuboid3d ret = CGAL::bounding_box(points.begin(), points.end());
    Bbox3d boundbox = Bbox3d(ret.xmin(), ret.ymin(), ret.zmin(), ret.xmax(), ret.ymax(), ret.zmax());

    return boundbox;
}

void Object::build_kd_tree(int nMaxBucketSize /* = 16 */)
{
    Stopwatch clock;
    std::cout << title() << "building kd_tree..." << std::endl;
    unsigned int num = nb_vertices();
    if (num == 0) {
        std::cout << title() << "no data exists" << std::endl;
        return;
    }

    Vector3D *points = new Vector3D[num];
    for (unsigned int i=0; i<vertices_.size(); ++i) {
        Point3d p = vertices_[i]->point();
        points[i].x = p.x();
        points[i].y = p.y();
        points[i].z = p.z();
    }

    if (kdTree_) {
        delete kdTree_;
    }

    kdTree_ = new KdTree(points, num, nMaxBucketSize);
    delete points;

    std::cout << title()
        << "building kd_tree done. Timing: "
        << clock.elapsed_user_time() << " seconds" << std::endl;
}

void Object::clear() {
    std::vector<Vertex*>::const_iterator it = vertices_.begin();
    for ( ; it != vertices_.end(); ++it) {
        delete (*it);
    }
    vertices_.clear() ;

    if (kdTree_) {
        delete kdTree_;
        kdTree_ = NULL;
    }
}

Point3d Object::aveCenter() const
{
    double x = 0, y = 0, z = 0;
    for(unsigned int i = 0; i < vertices_.size(); i++)
    {
        Point3d p = vertices_[i]->point();
        x += p.x();
        y += p.y();
        z += p.z();
    }
    float avex = float(x/vertices_.size());
    float avey = float(y/vertices_.size());
    float avez = float(z/vertices_.size());
    return Point3d(avex, avey, avez);
}

void Object::transformVertices(const AffTransformation3d &mat)
{
    for (unsigned int i = 0; i < vertices_.size(); i++) {
        Point3d p = vertices_[i]->point();
        Point3d p2 =  mat.transform(p);
        vertices_[i]->set_point(p2);
    }
}

void Object::set_all_vertices_color(const Colorf &c) {
    unsigned int num = vertices_.size();
    for (unsigned int i=0; i<num; ++i) {
        vertices_[i]->set_color(c);
    }
}
