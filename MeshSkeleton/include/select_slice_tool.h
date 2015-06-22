#pragma once

#include "tool3D.h"
#include <vector>
#include <string>
#include <QLine>

class Object;
class Skeleton;

namespace tool_skeleton
{
    class SelectSliceTool : public Tool3D
    {
    public:
        SelectSliceTool(PaintCanvas *canvas);
        ~SelectSliceTool();
        static std::string title() { return "[SelectSliceTool]: "; }
        virtual void press(QMouseEvent *e);
        virtual void release(QMouseEvent *e);
        virtual void move(QMouseEvent *e);
        virtual void drag(QMouseEvent *e);
        virtual void reset();
        void draw() const;

    protected:
        // NOTE: very important, you must call this function in you constructor
        void initialize_select_buffer() ;  // set the buffer size
        void draw_with_names() const;
        void begin_selection();
        void end_selection();
        void make_selection(bool b) ;
        void unselect_all();
        void select();

        Skeleton *get_skeleton() const;
        Object *get_object() const;

    protected:
        QLine mouse_select_line_;
        QPoint mouse_pressed_pos_;
        QPoint mouse_moving_pos_;
        int hit_resolution_;
        double rotate_angle_;
        int select_buffer_size_;
        unsigned int * select_buffer_;
        std::vector<int> highlighted_object_indices_;

        Vector3d view_;
        Vector3d oldUp_;
        Vector3d newUp_;

    };
}

