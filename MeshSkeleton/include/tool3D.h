#ifndef _TOOL3D_H_
#define _TOOL3D_H_

#include "paint_canvas.h"
#include "counted.h"
#include "geometry_types.h"
#include <QPoint>

class Facet;
class Object ;
class CylinderMy;
class PaintCanvas;
class QMouseEvent;

namespace tool_skeleton
{
    class Tool3D: public Counted
    {
    public:
        Tool3D(PaintCanvas *canvas);
        virtual ~Tool3D();

    public:
        virtual void press(QMouseEvent *e) = 0;
        virtual void release(QMouseEvent *e) = 0;
        virtual void move(QMouseEvent *e) = 0;
        virtual void drag(QMouseEvent *e) = 0;
        virtual void reset() = 0;
        virtual void draw() const = 0;
        virtual void status(const std::string &value);

    public:
        virtual PaintCanvas *canvas() const;
        virtual void set_canvas(PaintCanvas* canvas);

        Line3d get_mouse_line_in_3d_space(const QPoint &mouse) const ;
        // NOTE: pointing to the outside of the screen
        Vector3d get_view_direction_in_3d_space(const QPoint &mouse) const ;
        Point3d get_intersect_point(const Plane3d &plane, const QPoint &mouse, bool &success) const;
        Point3d get_intersect_point(const Sphere3d &sphere, const QPoint &mouse, bool &success) const;
        Point3d get_intersect_point(const CylinderMy *cylinder, const QPoint &mouse, bool &success) const;
        int nearest_intersected_facet(const QPoint &mouse, const std::vector<Facet*> &faces) const;

    protected:
        PaintCanvas *canvas_;
        bool  left_button_down_;
        bool  right_button_down_;
    } ;

    class EmptyTool3D : public Tool3D
    {
    public:
        EmptyTool3D(PaintCanvas *canvas);
        ~EmptyTool3D() {}

    public:
        virtual void press(QMouseEvent *) {}
        virtual void release(QMouseEvent *) {}
        virtual void move(QMouseEvent *) {}
        virtual void drag(QMouseEvent *) {}
        virtual void reset() {}
        virtual void draw() const {}
    };

}

typedef SmartPointer<tool_skeleton::Tool3D> Tool3D_var ;

#endif
