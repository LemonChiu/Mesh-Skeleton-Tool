#ifndef PAINT_CANVAS_H
#define PAINT_CANVAS_H

#include "qglviewer.h"
#include "geometry_types.h"

#include <set>
#include <vector>
#include <QString>

class Object;
class Skeleton;
class Normalizer;
class Vertex;
class MainWindow;
class ToolManager3D;
class QMouseEvent;
class QKeyEvent;

enum AlgrothmType {
    ALG_CENTER_TYPE,
};

class PaintCanvas : public QGLViewer
{
    Q_OBJECT
public:
    PaintCanvas(QWidget *parent);
    ~PaintCanvas();

    void forceUpdate() { repaint(); }
    void draw();
    void setModel(Object* point_set);
    void setSkl(Skeleton* skl);
    void addSelectedVertices(std::vector<Vertex*> &vertices);
    void deleteSelectedVertices(std::vector<Vertex*> &vertices);
    void clearSelectedVertices();
    Skeleton* get_skeleton();
    Normalizer* get_normalizer();
    Object* get_object();
    MainWindow* mainWindow() const { return pWindow_; }

    Point2d projectionOf(const Point3d &p);
    Point3d unProjectionOf(double winx, double winy, double winz);

    // mouse event
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

    Point3d getPointUnderMouse(const QPoint &pos) const;
    void showCoordinateUnderPixel(const QPoint &pos) const;

    // keyborad event
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    void estimate_normal();
    const AlgrothmType &get_alg_type() const { return algtype_; } 

    void resetClear();

public slots:
    void changeBackgroundColor();

    //Edit Groupbox
    void scene_manipulation(bool b);
    void add_a_node(bool b);
    void delete_a_node(bool b);
    void add_a_bone(bool b);
    void delete_a_bone(bool b);

    //View Groupbox
    void changePointWidth(int value);
    void changeNodeRaduis(double value);
    void changeMeshRedColor(double value);
    void changeMeshGreenColor(double value);
    void changeMeshBlueColor(double value);
    void changeMeshAlphaColor(double value);

    //Algorithm Groupbox
    void set_center_mode(bool b);

    void set_show_object(bool b);
    void set_show_skl_node(bool b);
    void set_show_skl_bone(bool b);
    void set_show_axis(bool b);
    void set_show_normal(bool b);

private:
    void init();
    void drawCornerAxis() const;

private:
    qglviewer::WorldConstraint *constraint_;
    Normalizer *normalizer_;
    Object *object_;
    Skeleton *skeleton_;
    MainWindow *pWindow_;
    ToolManager3D *pSelectedToolManager_;
    std::set<Vertex*> setSelectedVertices_;

    AlgrothmType algtype_;
    bool show_axis_;
    bool show_object_;
};

#endif // PAINT_CANVAS_H