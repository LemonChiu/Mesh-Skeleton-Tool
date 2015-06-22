#ifndef __SKELETON_H_
#define __SKELETON_H_

#include "color.h"
#include "counted.h"
#include "geometry_types.h"

#include <list>
#include <string>

class Vertex;

class  SBone
{
public:
    SBone(Vertex* v1, Vertex* v2, unsigned int index);
    ~SBone();

    Vertex *get_v1() const;
    Vertex *get_v2() const;
    unsigned int idx() const;
    Colorf color() const;
    void set_color(const Colorf &c);
    bool is_highlighted()  const;
    void set_highlighted(bool b);

private:
    Vertex *v1_;
    Vertex *v2_;
    unsigned int idx_;
    Colorf color_;
    bool is_highlighted_;
};

class Skeleton : public Counted
{
public:
    Skeleton(const std::string &strFileName = "");
    ~Skeleton();

    void add_a_node(const Point3d &point);
    void delete_a_node(Vertex* v);
    void add_a_bone(Vertex* v1, Vertex* v2);
    void delete_a_bone(SBone* bone);
    std::string get_file_name() const;
    void set_file_name(const std::string &strFileName);

    unsigned int nb_nodes() const;
    unsigned int nb_edges() const;
    const std::list<Vertex*> &get_nodes() const;
    std::list<Vertex*> &get_nodes();
    const std::list<SBone*> &get_bones() const;
    std::list<SBone*> &get_bones();

    void draw() const;
    void set_node_color(const Colorf &c);
    void set_bone_color(const Colorf &c);
    const Colorf &node_color() const;
    const Colorf &bone_color() const;
    double node_radius() const;
    double bone_radius() const;

private:
    std::list<Vertex*> listVertices_;
    std::list<SBone*>  listEdges_;
    std::string strFileName_;
    Colorf colorBone_;
    Colorf colorNode_;
};

#endif
