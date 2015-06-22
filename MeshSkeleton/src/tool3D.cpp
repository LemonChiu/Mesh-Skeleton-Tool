#include "tool3d.h"

#include "paint_canvas.h"
#include "object.h"
#include "attribute.h"
#include "cylinder.h"
#include "box_facet.h"
#include "box_3d.h"
#include "geometry_global.h"
#include "prism.h"

#include <QMouseEvent>

namespace tool_skeleton
{
    Tool3D::Tool3D(PaintCanvas *canvas) 
    : canvas_(canvas)
    , left_button_down_(false)
    , right_button_down_(false)
    {
    }

    Tool3D::~Tool3D()
    {
        delete canvas_;
        canvas_ = NULL;
    }

    PaintCanvas *Tool3D::canvas() const {
        return canvas_ ;
    }

    void Tool3D::set_canvas(PaintCanvas* canvas) {
        canvas_ = canvas ;
    }

    void Tool3D::status(const std::string &value) {
        std::cout << value << "TODO: prompt user in status bar" << std::endl;
        // status_message() ;
    }

    Line3d Tool3D::get_mouse_line_in_3d_space(const QPoint &mouse) const {
        QPoint pos = mouse;
        Point3d p_near = canvas()->unProjectionOf(pos.x(), pos.y(), 0.0f);
        Point3d p_far  = canvas()->unProjectionOf(pos.x(), pos.y(), 1.0f);

        return Line3d(p_near, p_far);
    }

    Vector3d Tool3D::get_view_direction_in_3d_space(const QPoint &mouse) const {
        Line3d line = get_mouse_line_in_3d_space(mouse);
        Vector3d dir = -line.to_vector();
        global::normalize(dir);
        return dir;
    }
        
    // for plane
    Point3d Tool3D::get_intersect_point(const Plane3d &plane, const QPoint &mouse, bool &success) const {
        Line3d line = get_mouse_line_in_3d_space(mouse);
        CGAL::Object obj = CGAL::intersection(line, plane);
        if (const Point3d * point = CGAL::object_cast<Point3d>(&obj)) {
            success = true;
            return *point;
        } else {
            success = false;
            return Point3d(0.0f, 0.0f, 0.0f);
        }
    }

    // for cylinder
    Point3d Tool3D::get_intersect_point(const CylinderMy *cylinder, const QPoint &mouse, bool &success) const
    {
        Prism* prism = cylinder->to_prism();
        const std::vector<Facet*> &facets = prism->faces();
        int idx = nearest_intersected_facet(mouse, facets);
        if (idx == -1) {
            success = false;
            return Point3d(0, 0, 0);
        }
        Plane3d plane = facets[idx]->supporting_plane();
        return get_intersect_point(plane, mouse, success);
    }

    // for sphere
    Point3d Tool3D::get_intersect_point(const Sphere3d &sphere, const QPoint &mouse, bool &success) const
    {
        Line3d line = get_mouse_line_in_3d_space(mouse);
        double sqRaduis = sphere.squared_radius() ;
        Point3d center = sphere.center();
        Point3d proCenter = line.projection(center);
        double sqDis =(proCenter-center).squared_length() ;
        if (sqDis <= sqRaduis) {
            success = true;
            return proCenter;
        } else {
            success = false;
            return Point3d(0,0,0);
        }
    }

    // for intersected facet
    int Tool3D::nearest_intersected_facet(const QPoint &mouse, const std::vector<Facet*> &faces) const {
        if (faces.empty()) {
            return -1;
        } else if (faces.size() == 1) {
            return 0;
        }
        std::vector<Point3d>  intersctions;
        std::vector<my_bool>  valid;

        for (unsigned int i=0; i<faces.size(); ++i) {
            const Facet* f = faces[i];
            const std::vector<Vertex*> &vertices = f->vertices();
            Plane3d pl(vertices[0]->point(), vertices[1]->point(), vertices[2]->point());

            bool success = false;

            // point in polygon(2D) test first
            std::vector<Point2d> plgn;
            for (unsigned int i=0; i<vertices.size(); ++i) {
                Point2d p = canvas()->projectionOf(vertices[i]->point());
                plgn.push_back(p);
            }

            Point2d pos(double(mouse.x()), double(mouse.y()));
            bool point_in_polygon2d = global::isPointInPolygon2D(pos, plgn);
            if (!point_in_polygon2d) {
                intersctions.push_back(Point3d());
                valid.push_back(MY_FALSE);
                continue;
            } else {            
                Point3d p = get_intersect_point(pl, mouse, success);
                if (success) {
                    intersctions.push_back(p);
                    valid.push_back(MY_TRUE);
                } else {
                    //std::output(title()) << "Should have intersection." << std::endl;
                    return -1;
                }
            }
        }

        assert(intersctions.size() == faces.size());
        assert(intersctions.size() == valid.size());

        Point3d p_near = canvas()->unProjectionOf(mouse.x(), mouse.y(), 0.0f);
        double squred_distance = FLT_MAX;
        int selected_index = -1;

        for (unsigned int i=0; i<intersctions.size(); ++i) {
            if (valid[i] == MY_TRUE) {
                double s = CGAL::squared_distance(intersctions[i], p_near);
                if (s < squred_distance) {
                    squred_distance = s;
                    selected_index = i;
                }
            }
        }
        return selected_index;
    }

    EmptyTool3D::EmptyTool3D(PaintCanvas *canvas) 
    : Tool3D(canvas)
    {
    }
}