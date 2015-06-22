#include "normalizer.h"

#include "point_set.h"
#include "skeleton.h"

//pointset regularization£¬r for radius
void Normalizer::apply(Object* point_set, float r /*= 1.0f*/)
{
    std::cerr << title() << "normalizing data..." << std::endl;
    Stopwatch clock;

    const Bbox3d &bbox = point_set->bbox();
    normalized_radius_ = r;

    orig_center_ = Point3d(
        (bbox.xmax() + bbox.xmin()) / 2.0,
        (bbox.ymax() + bbox.ymin()) / 2.0,
        (bbox.zmax() + bbox.zmin()) / 2.0
        );
    Point3d top(bbox.xmax(), bbox.ymax(), bbox.zmax());
    Point3d bot(bbox.xmin(), bbox.ymin(), bbox.zmin());
    float squared_diagonal = CGAL::squared_distance(top, bot);
    orig_radius_ = std::sqrt(squared_diagonal) / 2.0f;
    float ratio = (normalized_radius_ / orig_radius_);
    std::vector<Vertex*> &vertices = point_set->vertices();
    for (unsigned i = 0; i < vertices.size(); ++i) {
        Vector3d &v = vertices[i]->point() - orig_center_ ;
        vertices[i]->set_point(CGAL::ORIGIN + v * ratio);
    }

    std::cerr << title() << "normalizing data done. Timing: "
        << clock.elapsed_user_time() << " seconds" << std::endl;
}

// pointset regularization
void  Normalizer::apply_by_old_parameter(Object* point_set)
{
    std::cerr << title() << "normalizing data using old parameters..." << std::endl;
    Stopwatch clock;

    float ratio = (normalized_radius_ / orig_radius_);
    std::vector<Vertex*> &vertices = point_set->vertices();
    for (unsigned i=0; i<vertices.size(); ++i) {
        Vector3d &v = vertices[i]->point() - orig_center_ ;
        vertices[i]->set_point(CGAL::ORIGIN + v * ratio);
    }

    std::cerr << title() << "normalizing data using old parameters... done. Timing: "
        << clock.elapsed_user_time() << " seconds" << std::endl;
}

// skeleton regularization
void Normalizer::apply_by_old_parameter(Skeleton* skeleton)
{
    std::cerr << title() << "normalizing skeleton data using old parameters..." << std::endl;
    Stopwatch clock;

    float ratio = (normalized_radius_ / orig_radius_);
    std::list<Vertex*> &vertices = skeleton->get_nodes();
    for (std::list<Vertex*>::iterator lit = vertices.begin(); lit != vertices.end(); ++lit) {
        Vertex *pv = *lit;
        Vector3d &v = pv->point() - orig_center_ ;
        pv->set_point(CGAL::ORIGIN + v * ratio);
    }

    std::cerr << title() << "normalizing skeleton data using old parameters... done. Timing: "
        << clock.elapsed_user_time() << " seconds" << std::endl;
}
