#ifndef _GEO_OBJECT_H_
#define _GEO_OBJECT_H_

#include "geometry_types.h"
#include "vertex.h"
#include "kdTree.h"
#include "counted.h"
#include "color.h"

#include <string>

enum ObjectType
{
    OBJECT_MESH_TYPE,
    OBJECT_POINTSET_TYPE,
    OBJECT_UNKOWN_TYPE
};

class Object : public Counted
{
public:
    Object(const std::string &name = "", ObjectType type = OBJECT_POINTSET_TYPE);
    ~Object(void) {}

    virtual std::string title() = 0;

    std::string  name() const { return name_; }
    ObjectType type() const { return type_;}

    std::vector<Vertex*> &vertices() { return vertices_; }
    const std::vector<Vertex*> &vertices() const { return vertices_; }

    // i'th vertex
    const Vertex* vertices(unsigned int i) const { return vertices_[i]; }
    Vertex* vertices(unsigned int i)  { return vertices_[i]; }

    unsigned int nb_vertices() const  { return vertices_.size() ; }

    virtual void add_vertex(const Point3d &p) {
        Vertex* v = new Vertex(p, vertices_.size(), Vector3d(0.0f,0.0f, 0.0f), Colorf(0.1f, 0.1f, 0.1f));
        vertices_.push_back(v);
    }

    virtual void add_vertex(const Point3d&p, const Colorf &c)
    {
        Vertex* v = new Vertex(p, vertices_.size(), Vector3d(0.0f,0.0f,0.0f), c);
        vertices_.push_back(v);
    }

    virtual void add_vertex(const Point3d &p, const Vector3d &n)
    {
        Vertex* v = new Vertex(p, vertices_.size(), n, Colorf(0.1f, 0.1f, 0.1f));
        vertices_.push_back(v);
    }

    virtual void add_vertex(const Point3d &p, const Vector3d&n, const Colorf &c)
    {
        Vertex *v = new Vertex(p, vertices_.size(), n, c);
        vertices_.push_back(v);
    }

    virtual void add_vertex(Vertex* v) {
        vertices_.push_back(v);
    }

    virtual void update() const { }

    virtual void clear() ;

    virtual Bbox3d bbox() const ;
    static Bbox3d bbox_of(const std::vector<Vertex*> &vertices) ;

    virtual Point3d aveCenter() const;

    Bbox3d bbox_dirty() const ; // always compute

    void transformVertices(const AffTransformation3d &mat);

    void set_all_vertices_color(const Colorf &c);

    ObjectType getType() { return type_; }

    void set_normals_exist(bool b) { is_normal_ = b; }
    bool is_normals_exist()  { return is_normal_;}


public:
    kdtree::KdTree *kd_tree() const { return kdTree_; }

    //----------------------------------------------------------------------------------------------------------------
    // KdTree searching implementations
    //----------------------------------------------------------------------------------------------------------------
    // Construct a KdTree
    void build_kd_tree(int nMaxBucketSize = 16);

    // Search for the K nearest neighbor points of a given position v
    unsigned int get_K_nearest_points(const Point3d &v, int K, std::vector<Vertex*> &neighbors);

    // Search for the K nearest neighbor points within range specified by "fRange" of a given position v
    unsigned int get_K_nearest_points(const Point3d &v, int K, float fRange, std::vector<Vertex*> &neighbors);

    // Search for all neighbor points within range specified by "fRange" of a given position v
    unsigned int get_all_nearest_points(const Point3d &v, float fRange, std::vector<Vertex*> &neighbors);

    //----------------------------------------------------------------------------------------------------------------
    // In the following functions, if $bToLine$ is true, the points found are ordered by their distances to the line
    // segment. Otherwise, they are ordered by their distances to $v1$. All angles are in radians.
    //----------------------------------------------------------------------------------------------------------------

    // Search for the nearest K points whose distances to line segment $v1$-$v2$ are smaller than $fRange$
    unsigned int get_K_points_in_cylinder(const Point3d &v1, const Point3d &v2, int K, float fRange, std::vector<Vertex*> &neighbors, bool bToLine = true);

    // Search for all points whose distances to line segment $v1$-$v2$ are smaller than $fRange$
    unsigned int get_all_points_in_cylinder(const Point3d &v1, const Point3d &v2, float fRange, std::vector<Vertex*> &neighbors, bool bToLine = true);

    // Search for the nearest K points P_i where the angle between $v1$-P_i and $v1$-$v2$ is smaller than $fAngle$
    unsigned int get_K_points_in_cone(const Point3d &eye, const Point3d &v1, const Point3d &v2, int K, float fRange, std::vector<Vertex*> &neighbors, bool bToLine = true);

    // Search for all points P_i where the angle between $v1$-P_i and $v1$-$v2$ is smaller than $fAngle$
    unsigned int get_all_points_in_cone(const Point3d &eye, const Point3d &v1, const Point3d &v2, float fRange, std::vector<Vertex*> &neighbors, bool bToLine = true);

private:
    void collect_k_nereast_points(unsigned int K, std::vector<Vertex*> &neighbors) const ;


protected:
    std::string name_;

    std::vector<Vertex*> vertices_;
    kdtree::KdTree *kdTree_;

    mutable Bbox3d bbox_;
    mutable bool bbox_exist_;

    ObjectType type_;
    bool is_normal_;
};


inline unsigned int Object::get_K_nearest_points(const Point3d &v, int K, std::vector<Vertex*> &neighbors)
{
    assert(kdTree_);
    kdtree::Vector3D v3d(v.x(), v.y(), v.z());
    kdTree_->setNOfNeighbours(K);
    kdTree_->queryPosition(v3d);

    unsigned int num = kdTree_->getNOfFoundNeighbours();
    collect_k_nereast_points(num, neighbors);
    return num;
}

inline unsigned int Object::get_K_nearest_points(const Point3d &v, int K, float fRange, std::vector<Vertex*> &neighbors)
{
    assert(kdTree_);
    kdtree::Vector3D v3d(v.x(), v.y(), v.z());
    kdTree_->setNOfNeighbours(K);
    kdTree_->queryRange(v3d, fRange*fRange);

    unsigned int num = kdTree_->getNOfFoundNeighbours();
    collect_k_nereast_points(num, neighbors);
    return num;
}

inline unsigned int Object::get_all_nearest_points(const Point3d &v, float fRange, std::vector<Vertex*> &neighbors)
{
    assert(kdTree_);
    kdtree::Vector3D v3d(v.x(), v.y(), v.z());
    kdTree_->setNOfNeighbours(32);
    kdTree_->queryRange(v3d, fRange*fRange, true);

    unsigned int num = kdTree_->getNOfFoundNeighbours();
    collect_k_nereast_points(num, neighbors);
    return num;
}

inline unsigned int Object::get_K_points_in_cylinder(const Point3d &v1, const Point3d &v2, int K, float fRange, std::vector<Vertex*> &neighbors, bool bToLine)
{
    assert(kdTree_);
    kdtree::Vector3D v3d1(v1.x(), v1.y(), v1.z()), v3d2(v2.x(), v2.y(), v2.z());
    kdTree_->setNOfNeighbours(K);
    kdTree_->queryLineIntersection(v3d1, v3d2, fRange, bToLine != 0);

    unsigned int num = kdTree_->getNOfFoundNeighbours();
    collect_k_nereast_points(num, neighbors);
    return num;
}

inline unsigned int Object::get_all_points_in_cylinder(const Point3d &v1, const Point3d &v2, float fRange, std::vector<Vertex*> &neighbors, bool bToLine)
{
    assert(kdTree_);
    kdtree::Vector3D v3d1(v1.x(), v1.y(), v1.z()), v3d2(v2.x(), v2.y(), v2.z());
    kdTree_->setNOfNeighbours(32);
    kdTree_->queryLineIntersection(v3d1, v3d2, fRange, bToLine != 0, true);

    unsigned int num = kdTree_->getNOfFoundNeighbours();
    collect_k_nereast_points(num, neighbors);
    return num;
}

inline unsigned int Object::get_K_points_in_cone(const Point3d &eye, const Point3d &v1, const Point3d &v2, int K, float fAngle, std::vector<Vertex*> &neighbors, bool bToLine)
{
    assert(kdTree_);
    kdtree::Vector3D eye3d(eye.x(), eye.y(), eye.z()), v3d1(v1.x(), v1.y(), v1.z()), v3d2(v2.x(), v2.y(), v2.z());
    kdTree_->setNOfNeighbours(K);
    kdTree_->queryConeIntersection(eye3d, v3d1, v3d2, fAngle, bToLine != 0);

    unsigned int num = kdTree_->getNOfFoundNeighbours();
    collect_k_nereast_points(num, neighbors);
    return num;
}

inline unsigned int Object::get_all_points_in_cone(const Point3d &eye, const Point3d &v1, const Point3d &v2, float fAngle, std::vector<Vertex*> &neighbors, bool bToLine)
{
    assert(kdTree_);
    kdtree::Vector3D eye3d(eye.x(), eye.y(), eye.z()), v3d1(v1.x(), v1.y(), v1.z()), v3d2(v2.x(), v2.y(), v2.z());
    kdTree_->setNOfNeighbours(32);
    kdTree_->queryConeIntersection(eye3d, v3d1, v3d2, fAngle, bToLine != 0, true);

    unsigned int num = kdTree_->getNOfFoundNeighbours();
    collect_k_nereast_points(num, neighbors);
    return num;
}

inline void Object::collect_k_nereast_points(unsigned int K, std::vector<Vertex*> &neighbors) const {
    // Get the i-th point found in the search
    neighbors.resize(K);
    for(unsigned int i=0; i<K; ++i)
    {
        neighbors[i] = vertices_[ kdTree_->getNeighbourPositionIndex(i) ];
        // you may also get the distance by calling GetNearestPointDistance(i);
        // or the square of the distance by calling GetNearestPointDistanceSq(i);
    }
}

typedef SmartPointer<Object> Object_var;

#endif
