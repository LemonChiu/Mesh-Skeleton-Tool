#ifndef _GEOMETRY_GLOBAL_H_
#define _GEOMETRY_GLOBAL_H_

#include "geometry_types.h"
#include "color.h"

//define Mesh display mode
enum MeshDrawMode
{
    POINTSMODE,
    ALLLINESMODE,
    FRONTLINESMODE,
    SURFACEMODE
};

namespace global
{
    extern bool g_bDrawSkelNode;
    extern bool g_bShowSkel;
    extern bool g_bShowPointSet;
    extern double g_dBoneNodeSize;
    extern float g_fPointSize;
    extern float g_fNormalLength ;
    extern bool g_bShowNormal;
    extern bool g_bLighting;
    extern MeshDrawMode g_meshDrawMode;
    extern float g_fLineWidth;
    extern Colorf g_meshColor;
    extern Colorf g_skelNodeColor;
    extern Colorf g_skelBoneColor;
    extern bool g_bPointsRealColor;

    // global functions
    double radianToDegree(double rad);
    double degreeToRadian(double deg);
    double normalize(Vector3d &v) ;
    double lengthOf(const Vector3d &v);
    double angleOfTwoVectors(const Vector3d &v1, const Vector3d &v2, const Vector3d &axis);  // from v1 to v2;
    bool isPointInPolygon2D(const Point2d &p, const std::vector<Point2d> &polygon);
    Point3d rotateInDegree(const Point3d &p, const Line3d &axis, double angle);
    Bbox3d boundingBoxOf(const std::vector<Point3d> &points);
    AffTransformation3d rotateAroundAxis(double angle, const Vector3d &v);
    Vector3d normalOf(const Plane3d &plane);
}

#endif
