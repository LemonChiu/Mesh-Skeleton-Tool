#include "attribute.h"
#include "colormap.h"

class Object;
class Skeleton;

class Model
{
public:
    Model(Object *obj);
    Model(Object *obj, Skeleton *skl);
    ~Model();

    Object *getObj()
    {
        return m_object;
    }
    const Object *getObj() const
    {
        return m_object;
    }
    Skeleton *getSkl()
    {
        return m_skl;
    }
    const Skeleton *getSkl()  const
    {
        return m_skl ;
    }
    void draw() const;

private:
    Object *m_object;
    Skeleton *m_skl;
    IntAttribute object_skl_corr_;
    ColorMap colors_;
};
