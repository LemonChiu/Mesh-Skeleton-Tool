#ifndef _ATTRIBUTE_H_
#define _ATTRIBUTE_H_

#include "object.h"
#include <vector>

typedef int my_bool;
#define MY_FALSE 0
#define MY_TRUE 1

namespace Attributes
{
    /**
    * This classes gives ease adding and accessing informations to vertices,
    * tex_vertices, halfedges and facets by their pointers and iterators.
    *
    * Since std::vector doesn't maintain "bool" in byte, so you can NOT return
    * a reference to "bool" and to modify it. The following two will work:
    *
    * 1. Use const "int" type instead of "bool", for example:
    *       typedef    int        my_bool;
    *       #define    my_false         0
    *       #define    my_true          1
    * 2. Use STL's value types
    */

    template <class T>
    class AttributeBase
    {
    protected:
        typedef T                                       Attrib_type;
        typedef std::vector<Attrib_type>                Vector_type;
        typedef typename Vector_type::reference         Reference;
        typedef typename Vector_type::const_reference   Const_reference;
        typedef typename Vector_type::size_type         Size_type;

    protected:
        // with optional textual name for the attribute.
        AttributeBase(const Object* obj = NULL, const std::string &name = "<unknown>")
            : object_(obj)
            , name_(name)
            , value_is_correct_(false)
        {}

        virtual ~AttributeBase() { object_ = NULL; }

    public:
        virtual void prepare() = 0;
        virtual bool size_match() const = 0;
        virtual void set_default(const Attrib_type &attrib) = 0;

        virtual void set_object(const Object* obj) { object_ = obj; };
        const Object* object() const { return object_; }

        // return the name of the attribute
        const std::string &name() const { return name_; }

        // return the size of the attribute
        unsigned int size() const { return attributes_.size(); }

        // test whether the attribute is empty
        bool empty() const { return size() == 0; }

        bool is_valid() const {
            return (size_match() && (value_is_correct_));
        }

        // this two just change the value flag
        void validate() { value_is_correct_ = true; }
        void invalidate() { value_is_correct_ = false; }

        // clear the attribute
        void clear() { attributes_.clear(); }

        //_______________________________ access ______________________________
        // access interface
        template<class Iterator>
        Reference operator[](Iterator it) {
            return attributes_[(*it)->idx()] ;
        }

        // const access interface
        template<class Iterator>
        Const_reference operator[](Iterator it) const {
            return attributes_[(*it)->idx()] ;
        }

        // return the values of this attribute
        Vector_type &data() { return attributes_; }
        const Vector_type &data() const { return attributes_; }

        // direct access via indices
        Reference operator[](unsigned int idx) {
            return attributes_[idx];
        }
        Const_reference operator[](unsigned int idx) const {
            return attributes_[idx];
        }

    protected:
        const Object*   object_;
        Vector_type     attributes_;
        bool            value_is_correct_;
        std::string     name_;
    };


    // ****************************************************************
    //________________________Vertex Attribute_________________________

    template <class T>
    class VertexAttribute : public AttributeBase<T>
    {
    public:
        //_________________constructors___________________
        VertexAttribute(
            const Object* obj = NULL, const std::string &name = "<unknown>"
            ): AttributeBase(obj, name)
        {
            if (obj != NULL) {
                attributes_.resize(obj->nb_vertices());
            }
        }

        // with default attribute value
        VertexAttribute(
            const Object* obj, Attrib_type attrib, const std::string &name = "<unknown>"
            ) : AttributeBase(obj, name)
        {
            if (obj != NULL) {
                attributes_.resize(obj->nb_vertices(), attrib);
            }
        }

        void set_default(const Attrib_type &attrib) {
            attributes_.assign(object_->nb_vertices(), attrib);
        }

        bool size_match() const {
            return (attributes_.size() == object_->nb_vertices());
        }

        bool is_valid() const {
            return (size_match() && (value_is_correct_));
        }

        void prepare() {
            attributes_.resize(object_->nb_vertices());
        }

    };

}

typedef Attributes::VertexAttribute<std::vector<double> > WeightAttribute;
typedef Attributes::VertexAttribute<Vector3d> Vector3dAttribute;
typedef Attributes::VertexAttribute<Point3d> Point3dAttribute;
typedef Attributes::VertexAttribute<float> FloatAttribute;
typedef Attributes::VertexAttribute<int> IntAttribute;

#endif
