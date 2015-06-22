#include "normal_estimator.h"

#include "primitive_fitting.h"
#include "object.h"
#include "attribute.h"
#include "stop_watch.h"
#include "geometry_global.h"

// TODO: using CGAL::pca_estimate_normals()
void NormalEstimator::apply(Object* object, unsigned int K)
{
    if (object == NULL) {
        std::cout << title() << "no data exists" << std::endl;
        return;
    }

    std::cout << title() << "estimating point cloud normal..." << std::endl;
    Stopwatch clock;

    PrimitiveFitting fit;
    std::vector<Vertex*> &vertices = object->vertices();

    unsigned int num = vertices.size() / 30;  // for each part
    unsigned int count = 0;

    for (unsigned int i=0; i<vertices.size(); ++i) {
        Vertex* v = vertices[i];

        // TODO: process larger number once a time
        std::vector<Vertex*> neighbors;
        object->get_K_nearest_points(v->point(), K, neighbors);

        neighbors.push_back(v);  // do NOT forget itself
        fit.set_points(neighbors);

        Plane3d plane;
        fit.fit_plane(plane);
        Vector3d n = global::normalOf(plane);
        v->set_normal(n);
    }

    std::cout << title() << "normal estimation done. Time: " << clock.elapsed_user_time() << " second" << std::endl;

    object->set_normals_exist(true);
}

static Vector3d weighted_average_normal(const Vertex* v, const std::vector<Vertex*> &neighbors) {
    unsigned int size = neighbors.size();
    std::vector<Vector3d> normals(size);
    std::vector<double>   coeff(size);

    int opposite = 0;

    Vector3d  v_n = v->normal();
    for (unsigned int j=0; j<neighbors.size(); ++j) {
        Vector3d n = neighbors[j]->normal();
        double c = n * v_n;
        if (c < 0.0f) {
            n = n * -1.0f;
            c = -c;
            ++ opposite;
        }

        normals[j] = n;
        coeff[j] = c;
    }

    if (opposite > (int)neighbors.size() / 2) {
        v_n = -v_n;
        for (unsigned int j=0; j<normals.size(); ++j) {
            normals[j] = normals[j] * -1.0f;
        }
    }

    for (unsigned int j=0; j<normals.size(); ++j) {
        v_n = v_n + normals[j] * coeff[j];
    }
    return (v_n / std::sqrt(v_n.squared_length()));
}
