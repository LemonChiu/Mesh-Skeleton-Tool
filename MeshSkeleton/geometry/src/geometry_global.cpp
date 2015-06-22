#include "geometry_global.h"
#include "basic_types.h"
#include <CGAL/bounding_box.h>

//intial global properties
namespace global
{
    bool g_bDrawSkelNode = true;
    bool g_bShowSkel = true;
    bool g_bShowPointSet = true;
    bool g_bShowNormal = false;
    bool g_bLighting = true; //default to use lighting
    bool g_bPointsRealColor = false; //default not to use real color
    double g_dBoneNodeSize = 0.012f;
    float g_fPointSize = 2.0f;
    float g_fNormalLength = 0.12f;
    float g_fLineWidth = 1.0f;
    MeshDrawMode g_meshDrawMode = FRONTLINESMODE;   //set display default to frontline
    Colorf g_meshColor = Colorf(0.60f, 0.60f, 0.60f, 1.0f);     //mesh's initial color:gray
    Colorf g_skelNodeColor = Colorf(0.65f, 0.23f, 0.82f, 1.0f); //set node's initial color: purple
    Colorf g_skelBoneColor = Colorf(0.35f, 0.77f, 0.18f, 1.0f); //set skeleton's initial color: green

    double radianToDegree(double rad)
    {
        return (rad * 180.0f / M_PI);
    }

    double degreeToRadian(double deg)
    {
        return (M_PI * deg / 180.0f);
    }

    Point3d rotateInRadian(const Point3d &p, const Line3d &axis, double angle)
    {
        Point3d p1 = axis.projection(p);

        // compute the translation
        AffTransformation3d mat1(CGAL::Translation(), Point3d(0.0f, 0.0f, 0.0f) - p1);

        // compute the rotation
        Vector3d u = axis.to_vector();
        global::normalize(u);

        double cos_angle = std::cos(angle);
        double sin_angle = std::sin(angle);

        double rot[3][3];
        rot[0][0] = u.x() * u.x() * (1.0f - cos_angle) + cos_angle;
        rot[0][1] = u.x() * u.y() * (1.0f - cos_angle) - u.z() * sin_angle;
        rot[0][2] = u.x() * u.z() * (1.0f - cos_angle) + u.y() * sin_angle;
        rot[1][0] = u.y() * u.x() * (1.0f - cos_angle) + u.z() * sin_angle;
        rot[1][1] = u.y() * u.y() * (1.0f - cos_angle) + cos_angle;
        rot[1][2] = u.y() * u.z() * (1.0f - cos_angle) - u.x() * sin_angle;
        rot[2][0] = u.z() * u.x() * (1.0f - cos_angle) - u.y() * sin_angle;
        rot[2][1] = u.z() * u.y() * (1.0f - cos_angle) + u.x() * sin_angle;
        rot[2][2] = u.z() * u.z() * (1.0f - cos_angle) + cos_angle;

        AffTransformation3d mat2(
            rot[0][0], rot[0][1], rot[0][2],
            rot[1][0], rot[1][1], rot[1][2],
            rot[2][0], rot[2][1], rot[2][2]);
        AffTransformation3d mat3(CGAL::Translation(), p1 - Point3d(0.0f, 0.0f, 0.0f));
        AffTransformation3d mat = mat3 * mat2 * mat1;

        return mat.transform(p);
}

    Point3d rotateInDegree(const Point3d &p, const Line3d &axis, double angle)
    {
        double rad = degreeToRadian(angle);
        return rotateInRadian(p, axis, rad);
    }

    double lengthOf(const Vector3d &v)
    {
        return std::sqrt(v.squared_length());
    }

    double normalize(Vector3d &v)
    {
        double len = std::sqrt(v.squared_length());
        if (len != 0.0f)
            v = v / len;
        return len;
    }

    Bbox3d boundingBoxOf(const std::vector<Point3d> &points)
    {
        // axis-aligned bounding box of 3D points
        Cuboid3d ret = CGAL::bounding_box(points.begin(), points.end());

        Bbox3d box = Bbox3d(ret.xmin(), ret.ymin(), ret.zmin(), ret.xmax(), ret.ymax(), ret.zmax());
        return box;
    }

    bool isPointInPolygon2D(const Point2d &P, const std::vector<Point2d> &polygon)
    {
        bool bIsInside = false;
        int N = (int)polygon.size();
        int i = 0;
        int j = N-1;

        for (; i < N; j = i, ++i) {
            const Point2d &U0 = polygon[i];
            const Point2d &U1 = polygon[j];  // current edge

            if (((U0.y() <= P.y()) && (P.y() < U1.y()))       // U1 is above the ray, U0 is on or below the ray
                || ((U1.y() <= P.y()) && (P.y() < U0.y()))) { // U0 is above the ray, U1 is on or below the ray
                // find x-intersection of current edge with the ray.
                // Only consider edge crossings on the ray to the right of P.
                double x = U0.x() + (P.y() - U0.y()) * (U1.x() - U0.x()) / (U1.y() - U0.y());
                if (x > P.x())
                    bIsInside = !bIsInside;
            }
        }
        return bIsInside;
    }

    AffTransformation3d rotateAroundAxis(double angle, const Vector3d &v)
    {
        Vector3d axis = v;
        normalize(axis);

        double c = std::cos(angle);
        double s = std::sin(angle);
        double x = axis.x();
        double y = axis.y();
        double z = axis.z();
        AffTransformation3d mat(x*x + (1-x*x)*x, x*y*(1-c)-z*s, x*z*(1-c)+y*s,
            x*y*(1-c)+z*s, y*y+(1-y*y)*c, y*z*(1-c)-x*s,
            x*z*(1-c)-y*s, y*z*(1-c)+x*s, z*z + (1-z*z)*c);
        return mat;
    }

    Vector3d normalOf(const Plane3d &plane)
    {
        Vector3d n = plane.orthogonal_vector();
        normalize(n);
        return n;
    }

    double angleOfTwoVectors(const Vector3d &v1,const  Vector3d &v2, const Vector3d &axis)
    {
        Vector3d nv1 = v1;
        normalize(nv1);
        Vector3d nv2 = v2;
        normalize(nv2);
        double angle = std::acos(nv1*nv2);

        if (CGAL::cross_product(v1,v2) * axis  < 0) {
            angle = -angle;
        }
        return angle;
    }
}
