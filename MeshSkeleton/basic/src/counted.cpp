#include "counted.h"

Counted::Counted()
    : nb_refs_(0)
{
}

Counted::~Counted() {
    assert(nb_refs_ == 0) ;
}

void Counted::ref() const {
    Counted* non_const_this = (Counted *)this ;
    non_const_this->nb_refs_++ ;
}

void Counted::unref() const {
    Counted* non_const_this = (Counted *)this ;
    non_const_this->nb_refs_-- ;

    assert(nb_refs_ >= 0) ;

    if(nb_refs_ == 0) {
        delete this ;
    }
}

bool Counted::is_shared() const {
    return (nb_refs_ > 1) ;
}

void Counted::ref(const Counted* counted) {
    if(counted != NULL) {
        counted->ref() ;
    }
}

void Counted::unref(const Counted* counted) {
    if(counted != NULL) {
        counted->unref() ;
    }
}
