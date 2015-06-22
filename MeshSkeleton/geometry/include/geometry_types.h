#ifndef _GEOMETRY_TYPES_H_
#define _GEOMETRY_TYPES_H_

/**
*   purpose: wrapper of CGAL types
*/

#include <CGAL/Cartesian.h>
#include <CGAL/Bbox_2.h>
#include <CGAL/Bbox_3.h>
#include <cgal/Aff_transformation_3.h>
#include <cgal/Linear_algebraCd.h>
#include <vector>


typedef float                FT;
typedef CGAL::Cartesian<FT>  K;

typedef double               DT;
typedef CGAL::Cartesian<DT>  DK;

typedef DK::Point_2           Point2d;
typedef DK::Vector_2          Vector2d;
typedef DK::Iso_rectangle_2   Rectangle2d;
typedef CGAL::Bbox_2          Bbox2d;

typedef DK::Point_3          Point3d;
typedef DK::Vector_3         Vector3d;
typedef DK::Direction_3      Direction3d;

typedef DK::Line_3           Line3d;
typedef DK::Segment_3        Segment3d;
typedef DK::Circle_3         Circle3d;
typedef DK::Plane_3          Plane3d;

typedef CGAL::Bbox_3         Bbox3d;
typedef DK::Iso_cuboid_3     Cuboid3d;
typedef DK::Sphere_3         Sphere3d;
typedef DK::Tetrahedron_3    Tetrahedron3d;

typedef DK::Aff_transformation_3  AffTransformation3d;

#endif


