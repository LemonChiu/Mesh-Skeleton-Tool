#include "select_slice_tool.h"
#include "object.h"
#include "skeleton.h"
#include "center_of_points.h"
#include <QMouseEvent>
#include "paint_canvas.h"
#include "geometry_global.h"

namespace tool_skeleton
{
    SelectSliceTool::SelectSliceTool(PaintCanvas* canvas) :Tool3D(canvas)
        ,select_buffer_(NULL)
        ,select_buffer_size_(0)
        ,hit_resolution_(2)
    {
    }

    SelectSliceTool::~SelectSliceTool()
    {
        delete[] select_buffer_;
    }

    // record the cross line
    void SelectSliceTool::press(QMouseEvent *e)
    {
        unselect_all();
        reset();
        if (e->button() == Qt::LeftButton) {
            left_button_down_ = true;
        }
        if (e->button() == Qt::RightButton) {
            right_button_down_ = true;
        }
        if(left_button_down_ == true) {
            mouse_pressed_pos_ = e->pos();
            mouse_moving_pos_ = e->pos();
            mouse_select_line_ = QLine(mouse_pressed_pos_, mouse_moving_pos_);
        }
        canvas()->updateGL();
    }

    void SelectSliceTool::reset()
    {
        left_button_down_ = false;
        right_button_down_ = false;
        mouse_moving_pos_ = QPoint(0,0);
        mouse_pressed_pos_ = QPoint(0,0);
        mouse_select_line_ = QLine(mouse_pressed_pos_, mouse_moving_pos_);
    }

    void SelectSliceTool::drag(QMouseEvent *e)
    {
        move(e);
    }

    void SelectSliceTool::move(QMouseEvent *e)
    {
        if (left_button_down_ == true) {
            mouse_moving_pos_ = e->pos();
            mouse_select_line_ = QLine(mouse_pressed_pos_, mouse_moving_pos_);
        }
        canvas()->updateGL();
    }

    void SelectSliceTool::release(QMouseEvent *e)
    {
        if (left_button_down_ == true) {
            mouse_moving_pos_ = e->pos();
            mouse_select_line_ = QLine(mouse_pressed_pos_, mouse_moving_pos_);
            if (mouse_pressed_pos_ != mouse_moving_pos_) {
                select();
            }
        }
        reset();
        canvas()->updateGL();
    }

    void SelectSliceTool::initialize_select_buffer()
    {
        Object *obj = get_object();
        if (obj == NULL) {
            select_buffer_size_ = 0;
            select_buffer_ = NULL;
        } else {
            select_buffer_size_ = obj->nb_vertices() * 4;
            select_buffer_ = new unsigned int[select_buffer_size_];
        }
    }

    void SelectSliceTool::draw() const
    {
        if (left_button_down_ == true) {
            canvas()->startScreenCoordinatesSystem();
            glDisable(GL_LIGHTING);

            Point2d pmin(mouse_pressed_pos_.x(), mouse_pressed_pos_.y());
            Point2d pmax(mouse_moving_pos_.x(), mouse_moving_pos_.y());
            glLineWidth(hit_resolution_ * global::g_fPointSize);
            glColor4fv(global::g_skelNodeColor.data());
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

    Object* SelectSliceTool::get_object() const
    {
        if (canvas() == NULL) {
            return NULL;
        }
        
        return canvas()->get_object();
    }

    Skeleton* SelectSliceTool::get_skeleton() const
    {
        if (canvas()==NULL) {
            return NULL;
        }
        
        return canvas()->get_skeleton();
    }

    void SelectSliceTool::draw_with_names() const
    {
        const std::vector<Vertex*> &vertices = get_object()->vertices();
        for (unsigned int i=0; i < vertices.size(); ++i) {
            vertices[i]->draw_with_name(i);
        }
    }

    void SelectSliceTool::select()
    {
        highlighted_object_indices_.clear();    
        begin_selection();
        draw_with_names();
        end_selection();
        make_selection(false);
    }

    void SelectSliceTool::begin_selection()
    {
        qglviewer::Vec view = canvas()->camera()->viewDirection();
        view_ = Vector3d(view.x, view.y, view.z);
        qglviewer::Vec oldUP = canvas()->camera()->upVector();
        oldUp_ = Vector3d(oldUP.x, oldUP.y, oldUP.z);

        Point3d pmin = canvas()->unProjectionOf(mouse_pressed_pos_.x(), mouse_pressed_pos_.y(), 0);
        Point3d pmax = canvas()->unProjectionOf(mouse_moving_pos_.x(), mouse_moving_pos_.y(), 0);
        Point3d pcen = pmin + (pmax - pmin) / 2;
        newUp_ = pmax - pmin;
        global::normalize(newUp_);
        qglviewer::Vec newUp(newUp_.x(), newUp_.y(), newUp_.z());

        canvas()->camera()->setUpVector(newUp);
        canvas()->camera()->setViewDirection(view);
        canvas()->camera()->computeModelViewMatrix();
        canvas()->camera()->computeProjectionMatrix();

        double dx1 = mouse_select_line_.dx();
        double dy1 = -mouse_select_line_.dy();
        double rectangleWidth = std::sqrt(dx1 * dx1 + dy1 * dy1);
        double rectangleHeight = hit_resolution_ * global::g_fPointSize;
        canvas()->makeCurrent();
        Point2d rectangleCenter = canvas()->projectionOf(pcen);

        initialize_select_buffer();

        if (select_buffer_size_ <= 0){
            return;
        }

        // Prepare the selection mode
        glSelectBuffer(select_buffer_size_, select_buffer_);
        glRenderMode(GL_SELECT);
        glInitNames();

        // Loads the matrices
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        static GLint viewport[4];
        canvas()->camera()->getViewport(viewport);
        gluPickMatrix(rectangleCenter.x(), rectangleCenter.y(), rectangleHeight, rectangleWidth, viewport);

        // loadProjectionMatrix() first resets the GL_PROJECTION matrix with a glLoadIdentity().
        // The false parameter prevents this and hence multiplies the matrices.
        canvas()->camera()->loadProjectionMatrix(false);
        // Reset the original (world coordinates) modelview matrix
        canvas()->camera()->loadModelViewMatrix();
    }

    void SelectSliceTool::end_selection()
    {
        // Flush GL buffers
        glFlush();

        // Get the number of objects that were seen through the pick matrix frustum. Reset GL_RENDER mode.
        GLint nbHits = glRenderMode(GL_RENDER);
        if (nbHits > 0) {
            // Interpret results : each object created 4 values in the selectBuffer().
            // (selectBuffer())[4*i+3] is the id pushed on the stack.
            for (int i = 0; i < nbHits; ++i) {
                highlighted_object_indices_.push_back(select_buffer_[4*i+3]);
            }
        }
        qglviewer::Vec oldUp(oldUp_.x(), oldUp_.y(), oldUp_.z());
        canvas()->camera()->setUpVector(oldUp);
        qglviewer::Vec view(view_.x(), view_.y(), view_.z());
        canvas()->camera()->setViewDirection(view);
    }

    void SelectSliceTool::make_selection(bool b)
    {
        if (b == false) {
            unselect_all();
        }
        Object *obj = get_object();
        if (obj == NULL) {
            return;
        }
        if (highlighted_object_indices_.size() == 0) {
            return;
        }
        std::vector<Vertex*> &vertices = obj->vertices();
        std::vector<Vertex*> selectedVertices;
        for (unsigned int i = 0 ; i != highlighted_object_indices_.size(); ++i) {
            unsigned int kid = highlighted_object_indices_[i];
            vertices[kid]->set_highlighted(true);
            selectedVertices.push_back(vertices[kid]);
        }

        Skeleton *skl = get_skeleton();
        if (skl != NULL) {
            PointsCenter pointsCenter;
            Point3d center;
            if (canvas()->get_alg_type() == ALG_CENTER_TYPE) {
                center = pointsCenter.apply_center(selectedVertices);
            }
            
            skl->add_a_node(center);
        }   
    }

    void SelectSliceTool::unselect_all()
    {
        Object *obj = get_object();
        if (obj == NULL) {
            return;
        }
        std::vector<Vertex*> &vertices = obj->vertices();
        for (unsigned int i = 0 ; i != vertices.size(); ++i) {
            vertices[i]->set_highlighted(false);
        }
    }
}