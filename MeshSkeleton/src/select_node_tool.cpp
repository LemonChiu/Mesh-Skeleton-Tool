#include "select_node_tool.h"

#include "skeleton.h"
#include "cylinder.h"
#include "vertex.h"
#include "geometry_global.h"
#include "paint_canvas.h"
#include <QMouseEvent>

namespace tool_skeleton
{
    SelectNodeBoneTool::SelectNodeBoneTool(PaintCanvas *canvas) :Tool3D(canvas)
    {
        mouse_pressed_pos_ = QPoint(0,0);
        mouse_moving_pos_ = QPoint(0,0);
    }

    // record the mouse position
    void SelectNodeBoneTool::press(QMouseEvent *e)
    {
        reset();
        unhighlight_all_nodes();
        unhighlight_all_bones();
        if (e->button() == Qt::LeftButton) {
            left_button_down_ = true;
        }
        if (e->button() == Qt::RightButton) {
            right_button_down_ = true;
        }
        if (left_button_down_ == true) {
            mouse_pressed_pos_ = e->pos();
            mouse_moving_pos_ = e->pos();
        }
        //highlight(mouse_moving_pos_);
        //canvas()->updateGL();
    }

    void SelectNodeBoneTool::reset() 
    {
        mouse_pressed_pos_ = QPoint(0,0);
        mouse_moving_pos_ = QPoint(0,0);
        left_button_down_ = false;
        right_button_down_ = false;
    }

    // record the move position
    void SelectNodeBoneTool::move(QMouseEvent *e)
    {
        if (left_button_down_ == true) {
            mouse_moving_pos_ = e->pos();
        }
    }

    // record the release position
    void SelectNodeBoneTool::release(QMouseEvent *e)
    {
        if(left_button_down_ == true)
        {
            mouse_moving_pos_ = e->pos();
            mouse_pressed_pos_ = e->pos();
        }
    }

    void SelectNodeBoneTool::drag(QMouseEvent *e)
    {
        move(e);
    }

    Vertex *SelectNodeBoneTool::compute_select_node_id(const QPoint &mouse, bool &success)
    {
        Skeleton *skl = get_skeleton();
        if (skl == NULL) {
            success = false;
            return NULL;
        }
        double radius = skl->node_radius();
        std::list<Vertex*> &nodes = skl->get_nodes();
        for (std::list<Vertex*>::iterator lit = nodes.begin(); lit != nodes.end(); ++lit) {
            Point3d p = (*lit)->point();
            Sphere3d sphere(p, radius*radius);
            get_intersect_point(sphere, mouse, success);
            if (success == true) {
                return (*lit);
            }
        }
        success = false;

        return NULL;
    }

    SBone *SelectNodeBoneTool::compute_select_bone_id(const QPoint &mouse, bool &success)
    {
        Skeleton *skl = get_skeleton();
        if (skl == NULL) {
            success = false;
            return NULL;
        }
        double raduis = skl->bone_radius();
        std::list<SBone*> &bones = skl->get_bones();
        for (std::list<SBone*>::iterator lit = bones.begin(); lit != bones.end(); ++lit)    {
            Point3d p1 = (*lit)->get_v1()->point();
            Point3d p2 = (*lit)->get_v2()->point();
            CylinderMy cylinder(raduis, p1, p2);
            get_intersect_point(&cylinder, mouse, success);
            if (success == true) {
                return (*lit);
            }
        }
        success = false;

        return NULL;
    }

    Skeleton *SelectNodeBoneTool::get_skeleton() const
    {
        if (canvas() != NULL) {
            return canvas()->get_skeleton();
        } else {
            return NULL;
        }
    }

    void SelectNodeBoneTool::unhighlight_all_nodes()
    {
        Skeleton *skl = get_skeleton();
        if (skl != NULL) {
            std::list<Vertex*> &nodes = skl->get_nodes();
            for (std::list<Vertex*>::iterator lit = nodes.begin(); lit != nodes.end(); ++lit) {
                (*lit)->set_highlighted(false);
            }
        }
    }

    void SelectNodeBoneTool::unhighlight_all_bones()
    {
        Skeleton *skl = get_skeleton();
        if (skl != NULL) {
            std::list<SBone*> &bones = skl->get_bones();
            for (std::list<SBone*>::iterator lit = bones.begin(); lit != bones.end(); ++lit) {
                (*lit)->set_highlighted(false);
            }
        }
    }
    
    void SelectNodeBoneTool::highlight_node(const QPoint &mouse)
    {
        bool success = false;
        Vertex *v = compute_select_node_id(mouse, success);
        if (success == true) {
            v->set_highlighted(true);
        }
    }

    void SelectNodeBoneTool::highlight_bone(const QPoint &mouse)
    {
        bool success = false;
        SBone *bone = compute_select_bone_id(mouse,success);
        if (success == true) {
            bone->set_highlighted(true);
        }
    }

    SelectOneNodeTool::SelectOneNodeTool(PaintCanvas* canvas)
        :SelectNodeBoneTool(canvas)
    {
    }

    SelectOneNodeTool::~SelectOneNodeTool()
    {
    }

    // click event of select one node
    void SelectOneNodeTool::press(QMouseEvent *e)
    {
        SelectNodeBoneTool::press(e);
        if (left_button_down_ == true) {
            unhighlight_all_nodes();
            highlight_node(mouse_moving_pos_);
        }
        canvas()->updateGL();
    }

    // move event of select one node
    void SelectOneNodeTool::move(QMouseEvent *e)
    {
        SelectNodeBoneTool::move(e);
        if (left_button_down_ == true) {
            unhighlight_all_nodes();
            highlight_node(mouse_moving_pos_);
        }
        canvas()->updateGL();
    }

    // relase event of select one node
    void SelectOneNodeTool::release(QMouseEvent *e)
    {
        SelectNodeBoneTool::release(e);
        unhighlight_all_nodes();
        bool success;
        Vertex *v = compute_select_node_id(mouse_pressed_pos_, success);
        if (success == true) {
            Skeleton *skl = get_skeleton();
            skl->delete_a_node(v);
        }
        reset();
        canvas()->updateGL();
    }

    /* class for select two node */
    SelectTwoNodeTool::SelectTwoNodeTool(PaintCanvas* canvas)
        :SelectNodeBoneTool(canvas)
    {
    }

    SelectTwoNodeTool::~SelectTwoNodeTool()
    {
    }

    // press event of select two node
    void SelectTwoNodeTool::press(QMouseEvent *e)
    {
        SelectNodeBoneTool::press(e);
        if (left_button_down_ == true) {
            unhighlight_all_nodes();
            highlight_node(mouse_pressed_pos_);
        }
        canvas()->updateGL();
    }

    // move event of select two node
    void SelectTwoNodeTool::move(QMouseEvent *e)
    {
        SelectNodeBoneTool::move(e);
        if (left_button_down_ == true) {
            unhighlight_all_nodes();
            highlight_node(mouse_pressed_pos_);
            highlight_node(mouse_moving_pos_);
        }
        canvas()->updateGL();
    }

    // release event of select two node
    void SelectTwoNodeTool::release(QMouseEvent *e)
    {
        unhighlight_all_nodes();
        bool success1, success2;
        Vertex *v1 = compute_select_node_id(mouse_pressed_pos_, success1);
        SelectNodeBoneTool::release(e);
        Vertex *v2 = compute_select_node_id(mouse_pressed_pos_, success2);
        if (success2 == true && success1 == true && v1 != v2) {
            Skeleton* skl = get_skeleton();
            skl->add_a_bone(v1, v2);
        }
        if (v1 != NULL) {
            v1->set_highlighted(true);
        }
        if(v2 != NULL) {
            v2->set_highlighted(true);
        }
        reset();
        canvas()->updateGL();
    }

    // draw function when select two node
    void SelectTwoNodeTool::draw() const
    {
        if (left_button_down_ == true) {
            canvas()->startScreenCoordinatesSystem();
            glDisable(GL_LIGHTING);

            Point2d pmin(mouse_pressed_pos_.x(), mouse_pressed_pos_.y());
            Point2d pmax(mouse_moving_pos_.x(), mouse_moving_pos_.y());
            glLineWidth(global::g_fPointSize);
            glColor4fv(global::g_skelBoneColor.data());
            glBegin(GL_LINE_LOOP);
            glVertex2d(pmin.x(), pmin.y());
            glVertex2d(pmax.x(), pmax.y());
            glEnd();    

            glEnable(GL_BLEND);
            glDepthMask(GL_FALSE);
            glColor4f(0.0, 0.0, 0.4f, 0.3f);
            glBegin(GL_LINE_LOOP);
            glVertex2d(pmin.x(), pmin.y());
            glVertex2d(pmax.x(), pmax.y());
            glEnd();
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);

            glEnable(GL_LIGHTING);
            canvas()->stopScreenCoordinatesSystem();
        }
    }

    /* class for selecting one bone */
    SelectOneBoneTool::SelectOneBoneTool(PaintCanvas* canvas)
        :SelectNodeBoneTool(canvas)
    {
    }

    SelectOneBoneTool::~SelectOneBoneTool()
    {
    }

    // press event of select one bone
    void SelectOneBoneTool::press(QMouseEvent *e)
    {
        reset();
        unhighlight_all_bones();
        SelectNodeBoneTool::press(e);
        if (left_button_down_ == true) {
            unhighlight_all_nodes();
            highlight_bone(mouse_pressed_pos_);     
        }
        canvas()->updateGL();
    }

    // move event of select one bone
    void SelectOneBoneTool::move(QMouseEvent *e)
    {
        SelectNodeBoneTool::move(e);
        if (left_button_down_ == true) {
            unhighlight_all_bones();
            highlight_bone(mouse_moving_pos_);
        }
        canvas()->updateGL();
    }

    // release event of select one bone
    void SelectOneBoneTool::release(QMouseEvent *e)
    {
        SelectNodeBoneTool::release(e);
        if (left_button_down_ == true) {
            unhighlight_all_bones();
            bool success = false;
            SBone *bone = compute_select_bone_id(mouse_pressed_pos_, success);
            Skeleton *skl = get_skeleton();
            if (success == true) {
                skl->delete_a_bone(bone);
            }
        }
        reset();
    }
}