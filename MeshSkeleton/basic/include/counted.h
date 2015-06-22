#ifndef _COUNTED_H_
#define _COUNTED_H_

#include "basic_types.h"
#include "smart_pointer.h"
#include <cassert>

//____________________________________________________________________________

/**
* This is the base class to be used for objects having
* "reference count" memory management. They can be
* referred to by using SmartPointer<T>, calling ref()
* and unref() when necessary.
* @see SmartPointer
*/

class  Counted {

public:
    Counted() ;
    ~Counted() ;

    void ref() const ;
    void unref() const ;
    bool is_shared() const ;

    static void ref(const Counted* counted) ;
    static void unref(const Counted* counted) ;

protected:
private:
    int nb_refs_ ;
} ;

#endif
