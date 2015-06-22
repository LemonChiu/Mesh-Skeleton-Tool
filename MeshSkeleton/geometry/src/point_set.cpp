#include "point_set.h"
#include "GL/glut.h"

PointSet::PointSet(const std::string &name)
: Object(name, OBJECT_POINTSET_TYPE)
{
}

PointSet::~PointSet()
{
    clear();
}

void PointSet::clear()
{
    Object::clear();
}

//20000 points at most
void PointSet::draw(bool realcolor /* = true*/,
                    Colorf &c /*= Colorf(1.0f, 1.0f, 1.0f) */,
                    bool fast /* = true */,
                    unsigned int max_num /* = 20000 */) const
{
#ifdef _DEBUG
    max_num = max_num / 4;
#endif

    draw_whole_pointset(realcolor, c, fast, max_num);
}

//5000000 points at most
void PointSet::draw_whole_pointset(bool realcolor /* = true*/,
                                   Colorf &c /*= Colorf(1.0f, 1.0f, 1.0f) */,
                                   bool fast /* = true */,
                                   unsigned int max_num /* = 500000 */) const
{
    glDisable(GL_LIGHTING);
    glPointSize(global::g_fPointSize);
    unsigned int num = vertices_.size();
    unsigned int partial = 0;
    glBegin(GL_POINTS);
    if (fast && num > max_num) {
        partial = num / max_num;
    } else {
        partial = 1;
    }

    c = Colorf(global::g_meshColor.r(),global::g_meshColor.g(),global::g_meshColor.b());
    // set if the pointset color is the original
    if (global::g_bPointsRealColor) {
        for (unsigned int i=0; i<num; i+= partial) {
            vertices_[i]->draw();
        }
    } else {
        for(unsigned int i = 0; i < num; i += partial) {
            //use custom color
            vertices_[i]->draw(c);
        }
    }
    glEnd();

    //display normal
    if (global::g_bShowNormal) {
        glClear(GL_COLOR_BUFFER_BIT);
        //set blue color
        glColor3f(0.52f, 0.80f, 0.92f);
        glLineWidth(global::g_fLineWidth);
        glBegin(GL_LINES);
        for (unsigned int i = 0; i < vertices_.size(); i++) {
            Point3d p1 = vertices_[i]->point();
            Point3d p2 = p1 + vertices_[i]->normal() * global::g_fNormalLength;
            glVertex3d(p1.x(), p1.y(), p1.z());
            glVertex3d(p2.x(), p2.y(), p2.z());
        }
        glEnd();
    }

    glEnable(GL_LIGHTING);
}
