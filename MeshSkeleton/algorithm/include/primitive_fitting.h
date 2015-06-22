#pragma once

/*
* purpose: fit basic primitives from the point cloud
*/

#include "cylinder.h"

class Vertex;
class VertexGroup;

class  PrimitiveFitting
{
public:
    PrimitiveFitting(void) {}
    ~PrimitiveFitting(void) {}

    static std::string title() { return "[PrimitiveFit]: "; }
    void  set_points(const std::vector<Vertex*> &vertices);
    void  clear_points();

    // fitting a plane to given points
    // q denotes a fitting quality (1 - lambda_min/lambda_max):
    //  1 is best (zero variance orthogonally to the fitting line)
    //  0 is worst (isotropic case, returns a plane with default direction)
    double fit_plane(Plane3d &plane);

private:
    std::vector<Point3d>  vertices_;

};
