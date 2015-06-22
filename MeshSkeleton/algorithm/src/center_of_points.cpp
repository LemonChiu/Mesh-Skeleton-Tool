#include "center_of_points.h"
#include "vertex.h"

//get pointset center
Point3d PointsCenter::apply_center(const std::vector<Vertex*> &points)
{
    Point3d center(0,0,0);
    for (unsigned int i = 0; i != points.size(); i++) {
        Point3d p = points[i]->point();
        center = center + (p - CGAL::ORIGIN);
    }
    center = CGAL::ORIGIN + (center - CGAL::ORIGIN) / points.size();

    return center;
}
