#include "cylinder.h"

#include "geometry_types.h"
#include "prism.h"
#include "object.h"
#include "facet.h"
#include "geometry_global.h"

#include "GL/glut.h"

CylinderMy::CylinderMy()
: radius_(0.0f)
, base_center_(0.0f, 0.0f, 0.0f)
, top_center_(0.0f, 0.0f, 0.0f)
, internal_rep_(NULL)
{
    facet_color_ = Colorf(0.0f, 0.0f, 0.0f, 1.0f);
}

CylinderMy::CylinderMy(const CylinderMy &cld)
: radius_(cld.radius())
, base_center_(cld.base_center())
, top_center_(cld.top_center())
, internal_rep_(NULL)
, facet_color_(cld.facet_color())
{
}

CylinderMy::CylinderMy(double radius, const Point3d &base_center, const Point3d &top_center)
: radius_(radius)
, base_center_(base_center)
, top_center_(top_center)
, internal_rep_(NULL)
{
    facet_color_ = Colorf(0.0f, 0.0f, 0.0f, 1.0f);
}

CylinderMy::CylinderMy(double radius, const Point3d &base_center, double height, const Vector3d &direction)
: radius_(radius)
, base_center_(base_center)
, internal_rep_(NULL)
{
    Vector3d dir = direction;
    dir = dir / std::sqrt(dir.squared_length());
    top_center_ = base_center_ + dir * height;
    facet_color_ = Colorf(0.0f, 0.0f, 0.0f, 1.0f);
}

CylinderMy::~CylinderMy() {
    if (internal_rep_) {
        delete internal_rep_;
    }
}

double CylinderMy::radius() const
{
    return radius_;
}

void CylinderMy::set_radius(double r)
{
    radius_ = r;
}

void CylinderMy::set_color(const Colorf &color)
{
    facet_color_ = color;
}

const Colorf &CylinderMy::facet_color() const
{
    return facet_color_;
}

void CylinderMy::draw(bool wireframe/* = false*/) const
{
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

    if (wireframe) {
        glDisable(GL_LIGHTING);
        glLineWidth(global::g_dBoneNodeSize * 2);
        glColor4fv(facet_color_.data());
    } else {
        glEnable(GL_LIGHTING);
        glColor4fv(facet_color_.data());
    }

    GLUquadricObj* qobj = gluNewQuadric();
    glPushMatrix();
    glTranslatef(base_center_.x(), base_center_.y(), base_center_.z());
    double angle = 0.0f;
    double x = 1.0f;
    double y = 0.0f;
    double z = 0.0f;
    // compute the normal vector
    // TODO: Comparing doubles with == or != is not safe
    Vector3d dir = direction();
    if ((dir.x() == 0.0) && (dir.y() == 0.0)) {
        if (dir.z() > 0) {
            angle = 0.0f;
        } else {
            angle = 180.0f;
        }
    } else {
        Vector3d k(0.0f, 0.0f, 1.0f);
        Vector3d tmp = CGAL::cross_product(dir, k);
        angle = std::acos(dir * k);
        angle = -180.0f * angle / M_PI;
        x = tmp[0];
        y = tmp[1];
        z = tmp[2];
    }
    glRotatef(angle, x, y, z);
    gluCylinder(qobj, radius_, radius_, height(), 20, 1);
    gluDisk(qobj, 0.0, radius_, 20, 1);

    glPushMatrix();
    glTranslatef(0, 0, height());
    gluDisk(qobj, 0.0, radius_, 20, 1);
    glPopMatrix();

    glPopMatrix();
    gluDeleteQuadric(qobj);
}

void CylinderMy::draw_with_name(unsigned int i) const {
    glPushName(i);
    {
        GLUquadricObj* qobj = gluNewQuadric();
        glPushMatrix();
        glTranslatef(base_center_.x(), base_center_.y(), base_center_.z());

        double angle = 0.0;
        double x = 1.0;
        double y = 0.0;
        double z = 0.0;

        //--- Compute orientation of normal
        // TODO: Comparing doubles with == or != is not safe
        Vector3d dir = direction();
        if((dir.x() == 0.0f) && (dir.y() == 0.0f)) {
            if(dir.z() > 0)
                angle = 0.0f;
            else
                angle = 180.0f;
        }
        else {
            Vector3d k(0.0f, 0.0f, 1.0f);

            Vector3d tmp = CGAL::cross_product(dir, k);
            angle = std::acos(dir * k);
            angle = -180.0f * angle / M_PI;
            x = tmp[0];
            y = tmp[1];
            z = tmp[2];
        }

        glRotatef(angle, x, y, z);
        gluCylinder(qobj, radius_, radius_, height(), 30, 1);
        glPopMatrix();

        gluDeleteQuadric(qobj);
    }
    glPopName();
}

Bbox3d CylinderMy::bbox() const {
    Prism *prism = to_prism();
    std::vector<Vertex*> top_vertices = prism->top_vertices();
    std::vector<Vertex*> base_vertices = prism->base_vertices();
    base_vertices.insert(base_vertices.end(), top_vertices.begin(), top_vertices.end());

    return Object::bbox_of(base_vertices);
}

Prism* CylinderMy::to_prism() const {
    if (internal_rep_ != NULL) {
        return internal_rep_;
    }

    // find a vector perpendicular to the direction
    Vector3d dir = direction();
    double h = height();
    Plane3d plane(base_center_, dir);
    Vector3d perp = plane.base1();
    perp = perp / std::sqrt(perp.squared_length());
    Point3d p = base_center_ + perp * radius_;
    Line3d  axis(base_center_, dir);

    internal_rep_ = new Prism;
    internal_rep_->set_direction(dir);
    internal_rep_->set_height(h);

    const unsigned int count = 30;
    const double ang_part = 360.0 / count;
    for (unsigned int i=0; i < count; ++i) {
        Point3d q = global::rotateInDegree(p, axis, double(i * ang_part));
        internal_rep_->add_point(q);
    }

    internal_rep_->end_construction();
    return internal_rep_;
}

double CylinderMy::height() const
{
    return std::sqrt(CGAL::squared_distance(base_center_, top_center_));
}

const Point3d &CylinderMy::base_center() const
{
    return base_center_;
}

Point3d CylinderMy::top_center() const
{
    return top_center_;
}

Vector3d CylinderMy::direction() const
{
    Vector3d dir(base_center_, top_center_);
    dir = dir / std::sqrt(dir.squared_length());
    return dir;
}

void CylinderMy::set_base_center(const Point3d &c) {
    base_center_ = c;

    if (internal_rep_) {
        delete internal_rep_;
        internal_rep_ = NULL;
        internal_rep_ = to_prism();
    }
}

void CylinderMy::set_top_center(const Point3d &c) {
    top_center_ = c;

    if (internal_rep_) {
        delete internal_rep_;
        internal_rep_ = NULL;
        internal_rep_ = to_prism();
    }
}

std::vector<Facet*> CylinderMy::collect_faces() const {
    if (internal_rep_ == NULL)
        internal_rep_ = to_prism();

    return internal_rep_->collect_faces();
}

void CylinderMy::translate(const Vector3d &d) {
    base_center_ = base_center_ + d;
    top_center_ = top_center_ + d;
    if (internal_rep_ != NULL) {
        internal_rep_->translate(d);
    }
}

void CylinderMy::reverse_direction() {
    Point3d tmp = base_center_;
    base_center_ = top_center_;
    top_center_ = tmp;
}

CylinderMy &CylinderMy::operator = (const CylinderMy &cld) {
    internal_rep_ = NULL;
    std::cout << "CylinderMy: here may be a bug" << std::endl;

    radius_ = cld.radius();
    base_center_ = cld.base_center();
    top_center_ = cld.top_center();
    return *this;
}
