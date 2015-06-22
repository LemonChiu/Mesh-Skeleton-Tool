#include "model.h"

#include "object.h"
#include "skeleton.h"
#include "point_set.h"
#include "mesh.h"
#include "geometry_global.h"

Model::Model(Object* obj)
:colors_(ColorMap())
{
    m_object = obj;
    m_skl = new Skeleton(m_object->name());
    object_skl_corr_.set_object(m_object);
    object_skl_corr_.set_default(-1);
}

Model::~Model()
{
    object_skl_corr_.clear();
    if (m_object) {
        delete m_object;
    }
    if (m_skl) {
        delete m_skl;
    }
}

void Model::draw() const
{
    // draw skeleton
    if (m_skl != NULL && global::g_bShowSkel) {
        m_skl->draw();
    }
    // draw model
    if (m_object != NULL && global::g_bShowPointSet)    {
        if (m_object->getType() == OBJECT_POINTSET_TYPE) { // pointset
            PointSet *ps = dynamic_cast<PointSet*>(m_object);
            ps->draw() ;
        } else { // mesh
            Mesh* mesh = dynamic_cast<Mesh*>(m_object);
            mesh->draw();
        }
    }
}
