#include "geometry_types.h"
#include <vector>
#include <string>
class Vertex;

class  PointsCenter
{
public:
    PointsCenter() {}
    ~PointsCenter() {}

    static std::string title() { return "[PointsCenter]: "; }
    Point3d apply_center(const std::vector<Vertex*> &points);
};
