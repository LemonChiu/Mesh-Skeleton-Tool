#ifndef _POINT_SET_H_
#define _POINT_SET_H_

#include "geometry_global.h"
#include "geometry_types.h"
#include "object.h"
#include "color.h"

class  PointSet : public Object
{
public:
    PointSet(const std::string &name = "") ;
    virtual ~PointSet() ;
    virtual std::string title()
    {
        return "[PointSet]: ";
    }
    virtual void clear() ;
    void draw(bool realcolor = true, //deafult to use real color
        Colorf &c = Colorf(1.0f, 1.0f, 1.0f),
        bool fast = true,
        unsigned int max_num  = 20000) const ;

private:
    void draw_whole_pointset(bool realcolor = true,
        Colorf &c = Colorf(1.0f, 1.0f, 1.0f),
        bool fast = true,
        unsigned int max_num  = 500000) const;
    Object *p_poinset;
};

#endif
