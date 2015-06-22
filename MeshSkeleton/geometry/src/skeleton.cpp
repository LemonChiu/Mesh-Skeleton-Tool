#include "skeleton.h"

#include "cylinder.h"
#include "vertex.h"
#include "geometry_global.h"

#include <ctime>
#include "GL/glut.h"

SBone::SBone(Vertex* v1, Vertex* v2, unsigned int index)
    :is_highlighted_(false)
{
    v1_ = v1;
    v2_ = v2;
    idx_ = index;
}

SBone::~SBone()
{
    v1_ = NULL;
    v2_ = NULL;
    idx_ = 0;
}

Vertex* SBone::get_v1() const
{
    return v1_;
}

Vertex* SBone::get_v2() const
{
    return v2_;
}

unsigned int SBone::idx() const
{
    return idx_;
}

Colorf SBone::color() const
{
    if(is_highlighted_)
        return Colorf(1.0f,0,0,color_.a());
    else
        return color_;
}

void SBone::set_color(const Colorf &c)
{
    color_ = c;
}

bool SBone::is_highlighted() const
{
    return is_highlighted_;
}

void SBone::set_highlighted(bool b)
{
    is_highlighted_ = b;
}

Skeleton::Skeleton(const std::string &strFileName /* = ""*/)
:strFileName_(strFileName)
{
}

Skeleton::~Skeleton()
{
    for (std::list<SBone*>::iterator lit = listEdges_.begin(); lit != listEdges_.end(); ++lit) {
        delete *lit;
    }
    listEdges_.clear();
    for(std::list<Vertex*>::iterator lit = listVertices_.begin(); lit != listVertices_.end(); ++lit) {
        delete *lit;
    }
    listVertices_.clear();
}

void Skeleton::add_a_node(const Point3d &point)
{
    if (listVertices_.size() != 0) {
        Vertex *v = new Vertex(point, listVertices_.back()->idx() + 1);
        v->set_color(colorNode_);
        listVertices_.push_back(v);
    } else {
        Vertex* v = new Vertex(point, 0);
        v->set_color(colorNode_);
        listVertices_.push_back(v);
    }
}

void Skeleton::delete_a_node(Vertex *v)
{
    std::vector<SBone*> vecDeleteEdges;
    for (std::list<SBone*>::iterator lit = listEdges_.begin(); lit != listEdges_.end(); ++lit) {
        if ((*lit)->get_v1() == v || (*lit)->get_v2() == v) {
            vecDeleteEdges.push_back(*lit);
        }
    }
    // delete skeleton with node v
    for (unsigned int i = 0; i != vecDeleteEdges.size(); ++i) {
        listEdges_.remove(vecDeleteEdges[i]);
        delete vecDeleteEdges[i];
    }
    listVertices_.remove(v);
    delete v;
}

void Skeleton::add_a_bone(Vertex *v1, Vertex *v2)
{
    if (listEdges_.size() != 0) {
        SBone *pbone = new SBone(v1, v2, listEdges_.back()->idx() + 1);
        pbone->set_color(colorBone_);
        listEdges_.push_back(pbone);
    } else {
        SBone *pbone = new SBone(v1, v2, 0);
        pbone->set_color(colorBone_);
        listEdges_.push_back(pbone);
    }
}

void Skeleton::delete_a_bone(SBone* bone)
{
    listEdges_.remove(bone);
    delete bone;
}

std::string Skeleton::get_file_name() const
{
    return strFileName_;
}

void Skeleton::set_file_name(const std::string &strFileName)
{
    strFileName_ = strFileName;
}

unsigned int Skeleton::nb_nodes() const
{
    return listVertices_.size();
}

unsigned int Skeleton::nb_edges() const
{
     return listEdges_.size();
}

const std::list<Vertex*> &Skeleton::get_nodes() const
{
    return listVertices_;
}

std::list<Vertex*> &Skeleton::get_nodes()
{
    return listVertices_;
}

std::list<SBone*> &Skeleton::get_bones()
{
    return listEdges_;
}

const std::list<SBone*> &Skeleton::get_bones() const
{
    return listEdges_;
}

void Skeleton::draw() const
{
    glEnable(GL_LIGHTING);
    double radius_sphere = node_radius();
    double radius_cylinder = bone_radius();

     // display skeleton node
    if (global::g_bShowSkel == true && global::g_bDrawSkelNode == true) {
        for(std::list<Vertex*>::const_iterator lit = listVertices_.begin(); lit != listVertices_.end(); ++lit) {
            glColor4fv((*lit)->color().data());
            Point3d p = (*lit)->point();
            glPushMatrix();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glTranslatef(p.x(), p.y(), p.z());
            glutSolidSphere(radius_sphere, 10, 10);
            glPopMatrix();
        }
    }

    // display skeleton
    if (global::g_bShowSkel == true) {
        for (std::list<SBone*>::const_iterator lit = listEdges_.begin(); lit != listEdges_.end(); ++lit) {
            Point3d p1 = (*lit)->get_v1()->point();
            Point3d p2 = (*lit)->get_v2()->point();
            CylinderMy cylinder(radius_cylinder, p1, p2);
            cylinder.set_color((*lit)->color());
            cylinder.draw();
        }
    }

    glDisable(GL_LIGHTING);
}

const Colorf &Skeleton::node_color() const
{
    return colorNode_;
}

const Colorf &Skeleton::bone_color() const
{
    return colorBone_;
}

double Skeleton::node_radius() const
{
    return global::g_dBoneNodeSize;
}

double Skeleton::bone_radius() const
{
    return global::g_dBoneNodeSize * 0.3;
}

void Skeleton::set_node_color(const Colorf &c)
{
    for (std::list<Vertex*>::iterator lit = listVertices_.begin(); lit != listVertices_.end(); ++lit) {
        (*lit)->set_color(c);
    }
    colorNode_ = c;
}

void Skeleton::set_bone_color(const Colorf &c)
{
    for (std::list<SBone*>::iterator lit = listEdges_.begin(); lit != listEdges_.end(); ++lit){
        (*lit)->set_color(c);
    }
    colorBone_ = c;
}
