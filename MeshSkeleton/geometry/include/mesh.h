#ifndef _MESH_H_
#define _MESH_H_

#include "geometry_types.h"
#include "object.h"
#include "color.h"
#include "geometry_global.h"
#include <set>
#include <QWidget>
#include <QMainWindow>

// a face of the mesh
class TriangleFace
{
public:
    TriangleFace(unsigned int a_id, unsigned int b_id, unsigned int c_id)
    {
        a_id_ = a_id;
        b_id_  = b_id;
        c_id_ = c_id;
    }
    TriangleFace(const TriangleFace &tri)
    {
        a_id_ = tri.a_id_;
        b_id_ = tri.b_id_;
        c_id_ =  tri.c_id_;
    }
    unsigned int &operator[](unsigned int i)
    {
        i  = i % 3;
        switch (i) {
        case 0:
            return a_id_;
            break;
        case 1:
            return b_id_;
            break;
        case 2:
            return c_id_;
            break;
        }
    }
public:
    unsigned int a_id_;
    unsigned int b_id_;
    unsigned int c_id_;
};


// an edge of the mesh
class Edge3D
{
public:
    int index0_;
    int index1_;

    bool isBorder_;    // number of relevant triangles
    int  neighbor0_;   // two relevant triangles
    int  neighbor1_;

public:
    Edge3D(const Edge3D &e) {
        index0_ = e.index0_;
        index1_ = e.index1_;
        isBorder_ = e.isBorder_;
        neighbor0_ = e.neighbor0_;
        neighbor1_ = e.neighbor1_;
    }

    Edge3D(int i0, int i1) : isBorder_(true), neighbor0_(-1), neighbor1_(-1) {
        //assert((i0 != i1) && (i0 > 0) && (i1 > 0));
        index0_ = std::min(i0, i1);
        index1_ = std::max(i0, i1);
    }

    void operator=(const Edge3D &edge) {
        index0_ = edge.index0_;
        index1_ = edge.index1_;
        isBorder_ = edge.isBorder_;
        neighbor0_ = edge.neighbor0_;
        neighbor1_ = edge.neighbor1_;
    }

    bool operator<(const Edge3D &edge) const {
        return index0_ < edge.index0_ || ((index0_ == edge.index0_) && (index1_ < edge.index1_));
    }
    bool operator==(const Edge3D &edge) const {
        return (index0_ == edge.index0_) && (index1_ == edge.index1_);
    }
    bool operator!=(const Edge3D &edge) const {
        return (index0_ != edge.index0_) || (index1_ != edge.index1_);
    }

    ~Edge3D() {};
};

class  Mesh : public Object
{
public:
    Mesh(const std::string &name = "") ;
    Mesh(const Mesh &m);
    Mesh(const std::vector<Point3d> &points, const std::vector<TriangleFace> &faces);
    virtual ~Mesh() ;
    virtual std::string title()
    {
        return "[Mesh]: ";
    }

    void initEdges();

    std::vector<TriangleFace> &getFaces()
    {
        return triangle_faces_;
    }
    const std::vector<TriangleFace> &getFaces() const
    {
        return triangle_faces_;
    }
    std::set<Edge3D> &getEdges()
    {
        return edges_;
    }
    const std::set<Edge3D> &getEdges() const
    {
        return edges_;
    }
    void setFaces(const std::vector<TriangleFace> &faces);
    unsigned int nb_faces() const
    {
        return triangle_faces_.size();
    }
    unsigned int nb_edges() const
    {
        return edges_.size();
    }

    void setPointsMode();

    // compute the normal of each vertex
    void computeNormal();

    // compute the arithmetic mean of the edge length of the mesh
    float computeMeanLength();

    // compute the length of edges
    void computeEdgesLength(std::vector<Vector3d> &edgevs);

    // compute the Laplacian matrix of the mesh
    void computeLaplacianMatrix(std::vector<std::map<int,double>> &rows);

    // draw
    virtual void draw()  const;

private:
    virtual void clear();

protected:
    std::vector< TriangleFace >  triangle_faces_;
    std::set< Edge3D >  edges_;
};

#endif
