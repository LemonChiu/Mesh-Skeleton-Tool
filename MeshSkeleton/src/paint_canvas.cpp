#include "paint_canvas.h"

#include "mainwindow.h"
#include "geometry_global.h"
#include "normalizer.h"
#include "skeleton.h"
#include "point_set.h"
#include "mesh.h"
#include "tool_manager3d.h"
#include "tool_name.h"
#include "normal_estimator.h"

#include <windows.h> 
#include <QMouseEvent>
#include <QKeyEvent>

using namespace qglviewer;

PaintCanvas::PaintCanvas(QWidget *parent)
: QGLViewer(parent)
, show_axis_(true)
, show_object_(true)
, object_(NULL)
, skeleton_(NULL)
, algtype_(ALG_CENTER_TYPE)
{
    constraint_ = new qglviewer::WorldConstraint();
    camera()->setPosition(Vec(0.0, 0.0, 0.0));
    camera()->setType(Camera::ORTHOGRAPHIC);
    camera()->lookAt(sceneCenter());
    camera()->showEntireScene();
    normalizer_ = new Normalizer();
    pSelectedToolManager_ = new ToolManager3D(this);
    pWindow_ = dynamic_cast<MainWindow*>(parent);
}

PaintCanvas::~PaintCanvas()
{
    resetClear();
    //delete pSelectedToolManager_;
    //pSelectedToolManager_ = NULL;
}

/// clean pointers
void PaintCanvas::resetClear()
{
    delete constraint_;
    constraint_ = NULL;

    delete skeleton_;
    skeleton_ = NULL;

    delete normalizer_;
    normalizer_ = NULL;

    delete object_;
    object_ = NULL;
}

//draw axis model skeleton selection
void PaintCanvas::draw()
{
    if (show_object_) {
        if (object_ != NULL) {
            if (object_->type() == OBJECT_POINTSET_TYPE) {
                dynamic_cast<PointSet*>(object_)->draw();
            }
            else if(object_->type() == OBJECT_MESH_TYPE) {
                dynamic_cast<Mesh*>(object_)->draw();
            }
        }
    }
    if (skeleton_ != NULL) {
        skeleton_->draw();
    }
    if (pSelectedToolManager_ != NULL) {
        pSelectedToolManager_->current_tool()->draw();
    }
    if (show_axis_) {
        drawCornerAxis();
    }
}

//intial canvas
void PaintCanvas::init()
{
    glEnable (GL_POINT_SMOOTH);
    glHint (GL_POINT_SMOOTH, GL_NICEST);

    glEnable (GL_LINE_SMOOTH);
    glHint (GL_LINE_SMOOTH, GL_NICEST);

    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POLYGON_SMOOTH, GL_NICEST);
    
    setBackgroundColor(Qt::white);
    //camera()->frame()->setSpinningSensitivity(100.0f);
    setMouseTracking(true);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); /*GL_FALSE*/
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

    // set light source
    static GLfloat pos[4] = {5.0, 5.0, 10.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glDisable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   
}

void PaintCanvas::drawCornerAxis() const 
{
    int viewport[4];
    int scissor[4];

    glDisable(GL_LIGHTING);

    // The viewport and the scissor are changed to fit the lower left
    // corner. Original values are saved.
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetIntegerv(GL_SCISSOR_BOX, scissor);

    // Axis viewport size, in pixels
    const int size = 150;
    glViewport(0, 0, size, size);
    glScissor(0, 0, size, size);

    // The Z-buffer is cleared to make the axis appear over the
    // original image.
    glClear(GL_DEPTH_BUFFER_BIT);

    // Tune for best line rendering
    glLineWidth(3.0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMultMatrixd(camera()->orientation().inverse().matrix());

    // red
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3d(0.0f, 0.0f, 0.0f);
    glVertex3d(1.0f, 0.0f, 0.0f);

    // green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3d(0.0f, 0.0f, 0.0f);
    glVertex3d(0.0f, 1.0f, 0.0f);

    // blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3d(0.0f, 0.0f, 0.0f);
    glVertex3d(0.0f, 0.0f, 1.0f);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // The viewport and the scissor are restored.
    glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    glEnable(GL_LIGHTING);
}

void PaintCanvas::mousePressEvent(QMouseEvent *e)
{
    if (pSelectedToolManager_->current_tool_name() == tool_skeleton::TOOL_EMPTY || e->modifiers() == Qt::ShiftModifier) {
        QGLViewer::mousePressEvent(e);
    } else {
        pSelectedToolManager_->current_tool()->press(e);
    }
}

void PaintCanvas::mouseMoveEvent(QMouseEvent *e)
{
    if (pSelectedToolManager_->current_tool_name() == tool_skeleton::TOOL_EMPTY) {
        QGLViewer::mouseMoveEvent(e); 
    } else {
        pSelectedToolManager_->current_tool()->move(e);
    }
    showCoordinateUnderPixel(e->pos());
}

void PaintCanvas::showCoordinateUnderPixel(const QPoint &pos) const {
    Point3d coord = getPointUnderMouse(pos);
    //preserve 5 char
    QString qstr_x = QString::number(coord.x()).left(5);
    QString qstr_y = QString::number(coord.y()).left(5);
    QString qstr_z = QString::number(coord.z()).left(5);
    pWindow_->updateSatusBarCoord(qstr_x, qstr_y, qstr_z);
}

//get mouse position in 3D
Point3d PaintCanvas::getPointUnderMouse(const QPoint &pos) const {
    bool found = false;
    Vec vec_q = camera()->pointUnderPixel(pos, found);
    return Point3d(vec_q.x, vec_q.y, vec_q.z);
}

//compute normal
void PaintCanvas::estimate_normal()
{
    if (object_ == NULL) {
        return;
    }
    if (object_->type() == OBJECT_POINTSET_TYPE) {
        NormalEstimator ne;
        ne.apply(object_);
    } else if (object_->type() == OBJECT_MESH_TYPE) {
        dynamic_cast<Mesh*>(object_)->computeNormal();
    }
}

void PaintCanvas::mouseReleaseEvent(QMouseEvent* e)
{
    if (pSelectedToolManager_->current_tool_name() == tool_skeleton::TOOL_EMPTY) {
        QGLViewer::mouseReleaseEvent(e); 
    } else {
        pSelectedToolManager_->current_tool()->release(e);
    }
}

void PaintCanvas::keyReleaseEvent(QKeyEvent *e) {
    //press Control to restore
    if (e->key() == Qt::Key_Control) {
        global::g_bPointsRealColor = false;
        update();
    }
}

void PaintCanvas::keyPressEvent(QKeyEvent *e) {
    //press Control to display original color
    if (e->key() == Qt::Key_Control) {
        global::g_bPointsRealColor = true;
        update();
    }
}

void PaintCanvas::setModel(Object *point_set)
{
    if (point_set != NULL) {
        if (object_ != NULL) {
            delete object_;
        }
        if (skeleton_ != NULL) {
            delete skeleton_;
        }
        object_ = point_set;
        normalizer_->apply(point_set);
        skeleton_ = new Skeleton();
        skeleton_->set_bone_color(global::g_skelBoneColor);
        skeleton_->set_node_color(global::g_skelNodeColor);
        object_->build_kd_tree();
        if(object_->is_normals_exist() == false) {
            estimate_normal();
        }
    }
}

void PaintCanvas::setSkl(Skeleton *skl)
{
    if (normalizer_ == NULL) {
        return;
    }
    normalizer_->apply_by_old_parameter(skl);
    if (skeleton_ != NULL) {
        delete skeleton_;
    }
    skeleton_ = skl;
    skeleton_->set_bone_color(global::g_skelBoneColor);
    skeleton_->set_node_color(global::g_skelNodeColor);
}

Skeleton *PaintCanvas::get_skeleton()
{
    return skeleton_;
}

Object *PaintCanvas::get_object()
{
    return object_;
}

Normalizer *PaintCanvas::get_normalizer()
{
    return normalizer_;
}

void PaintCanvas::clearSelectedVertices() {
    if (setSelectedVertices_.empty()) {
        return;
    }

    std::set<Vertex*>::iterator itr = setSelectedVertices_.begin();
    for (; itr != setSelectedVertices_.end(); ++itr) {
        (*itr)->set_highlighted(false);
    }
    setSelectedVertices_.clear();
}

void PaintCanvas::addSelectedVertices(std::vector<Vertex*> &vertices) {
    for (unsigned int i=0; i < vertices.size(); ++i) {
        setSelectedVertices_.insert(vertices[i]);
    }
}

void PaintCanvas::deleteSelectedVertices(std::vector<Vertex*> &vertices) {
    for (unsigned int i=0; i < vertices.size(); ++i) {
        setSelectedVertices_.erase(vertices[i]);
    }
}

Point2d PaintCanvas::projectionOf(const Point3d &p) {    // point to screen
    Vec v = camera()->projectedCoordinatesOf(Vec(p.x(), p.y(), p.z()));
    return Point2d(v.x, v.y);
}

Point3d PaintCanvas::unProjectionOf(double winx, double winy, double winz) {  // screen to point    
    Vec v = camera()->unprojectedCoordinatesOf(Vec(winx, winy, winz));
    return Point3d(v.x, v.y, v.z);
}

void PaintCanvas::add_a_node(bool b)
{
    if (b == true) {
        pSelectedToolManager_->set_tool(tool_skeleton::TOOL_SELECT_VERTICES);
        setCursor(Qt::ArrowCursor);
        update();
    }
}

void PaintCanvas::delete_a_node(bool b)
{
    if (b == true) {
        pSelectedToolManager_->set_tool(tool_skeleton::TOOL_SELECT_NODE);
        setCursor(Qt::PointingHandCursor);
        update();
    }
}

void PaintCanvas::add_a_bone(bool b)
{
    if (b == true) {
        pSelectedToolManager_->set_tool(tool_skeleton::TOOL_SELECT_TWONODES);
        setCursor(Qt::ArrowCursor);
        update();
    }
}

void PaintCanvas::delete_a_bone(bool b)
{
    if (b == true) {
        pSelectedToolManager_->set_tool(tool_skeleton::TOOL_SELECT_BONE);
        setCursor(Qt::PointingHandCursor);
        update();
    }
}

void PaintCanvas::scene_manipulation(bool b)
{
    if (b == true) {
        pSelectedToolManager_->set_tool(tool_skeleton::TOOL_EMPTY);
        setCursor(Qt::ArrowCursor);
        update();
    }
}

void PaintCanvas::changePointWidth(int value)
{
    global::g_fPointSize = value;
    update();
}

void PaintCanvas::changeMeshRedColor(double value)
{
    global::g_meshColor.set_r(value);
    update();
}

void PaintCanvas::changeMeshGreenColor(double value)
{
    global::g_meshColor.set_g(value);
    update();
}

void PaintCanvas::changeMeshBlueColor(double value)
{
    global::g_meshColor.set_b(value);
    update();
}

void PaintCanvas::changeMeshAlphaColor(double value)
{
    global::g_meshColor.set_a(value);
    update();
}

void PaintCanvas::changeNodeRaduis(double value)
{
    global::g_dBoneNodeSize = value;
    update();
}

void PaintCanvas::set_center_mode(bool b)
{
    if (b == true) {
        algtype_ = ALG_CENTER_TYPE;
    }
}

void PaintCanvas::set_show_object(bool b)
{
    show_object_ = b;
    update();
}

void PaintCanvas::set_show_skl_bone(bool b)
{
    global::g_bShowSkel = b;
    update();
}

void PaintCanvas::set_show_skl_node(bool b)
{
    global::g_bDrawSkelNode = b;
    update();
}

void PaintCanvas::set_show_axis(bool b)
{
    show_axis_ = b;
    update();
}

void PaintCanvas::set_show_normal(bool b)
{
    global::g_bShowNormal = b;
    update();
}

void PaintCanvas::changeBackgroundColor()
{
    if (backgroundColor() == Qt::white) {
        setBackgroundColor(Qt::black);
    } else if (backgroundColor() == Qt::black) {
        setBackgroundColor(Qt::white);
    }
    update();
}