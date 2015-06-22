#pragma once

#include "tool3D.h"

class Skeleton;
class Vertex;
class SBone;

namespace tool_skeleton {
    class SelectNodeBoneTool : public Tool3D
    {
    public:
        SelectNodeBoneTool(PaintCanvas* canvas);
        ~SelectNodeBoneTool() {}
        virtual void press(QMouseEvent *e);
        virtual void release(QMouseEvent *e);
        virtual void move(QMouseEvent *e);
        virtual void drag(QMouseEvent *e);
        virtual void reset();
        virtual void draw() const {}

    protected:
        Skeleton* get_skeleton() const;
        void highlight_node(const QPoint &mouse);
        void unhighlight_all_nodes();
        Vertex* compute_select_node_id(const QPoint &mouse, bool &success);
        SBone* compute_select_bone_id(const QPoint &mouse, bool &success);
        void highlight_bone(const QPoint &mouse);
        void unhighlight_all_bones();
        
        QPoint mouse_pressed_pos_;
        QPoint mouse_moving_pos_;   
    };

    class SelectOneNodeTool : public SelectNodeBoneTool
    {
    public:
        SelectOneNodeTool(PaintCanvas* canvas);
        ~SelectOneNodeTool();
        static std::string title() { return "[SelectOneNodeTool]: "; }
        virtual void press(QMouseEvent *e);
        virtual void release(QMouseEvent *e);
        virtual void move(QMouseEvent *e);
    };

    class SelectTwoNodeTool: public SelectNodeBoneTool
    {
    public:
        SelectTwoNodeTool(PaintCanvas* canvas);
        ~SelectTwoNodeTool();
        static std::string title() { return "[SelectTwoNodeTool]: "; }
        virtual void press(QMouseEvent *e);
        virtual void release(QMouseEvent *e);
        virtual void move(QMouseEvent *e);
        virtual void draw() const;
    };

    class SelectOneBoneTool : public SelectNodeBoneTool
    {
    public:
        SelectOneBoneTool(PaintCanvas *canvas);
        ~SelectOneBoneTool();
        static std::string title() { return "[SelectOneBoneTool] ";}
        virtual void press(QMouseEvent *e);
        virtual void release(QMouseEvent *e);
        virtual void move(QMouseEvent *e);
    };
}