#include "primitive_fitting.h"
#include "geometry_types.h"
#include "point_set.h"
#include <CGAL/bounding_box.h>
#include <CGAL/linear_least_squares_fitting_3.h>

void PrimitiveFitting::set_points(const std::vector<Vertex*> &vertices)
{
    unsigned int num = vertices.size();
    if (num == 0) {
        return;
    }

    vertices_.resize(num);
    for (unsigned int i=0; i < num; ++i) {
        const Point3d &p = vertices[i]->point();
        vertices_[i] = Point3d(p.x(), p.y(), p.z());
    }
}

void PrimitiveFitting::clear_points()
{
    vertices_.clear();
}

double PrimitiveFitting::fit_plane(Plane3d &plane)
{
    double q = CGAL::linear_least_squares_fitting_3(
        vertices_.begin(),
        vertices_.end(),
        plane,
        CGAL::Dimension_tag<0>()
        );

    return q;
}


