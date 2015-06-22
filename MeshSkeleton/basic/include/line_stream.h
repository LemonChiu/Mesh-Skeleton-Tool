
#ifndef _LINE_STREAM_H_
#define _LINE_STREAM_H_

#include "basic_types.h"
#include <cassert>

#include <iostream>
#include <sstream>

namespace IO {
    class LineInputStream {
    public:
        LineInputStream(
            std::istream &in
            ) : in_(in), line_in_(NULL) {   }
        ~LineInputStream() {
            delete line_in_ ; line_in_ = NULL ;
        }
        bool eof() const { return in_.eof() ; }
        bool eol() const { return line_in_ == NULL || line_in_->eof() ; }
        bool ok() const { return in_ != NULL; }

        void get_line() {
            in_.getline(buffer_, 65536) ;
            delete line_in_ ; line_in_ = new std::istringstream(buffer_) ;
        }

        std::istream &line() {
            assert(line_in_ != NULL) ;
            return *line_in_ ;
        }

        const char *current_line() const {
            return buffer_;
        }

        template <class T> LineInputStream &operator>>(T &param) {
            *line_in_ >> param;
            return *this;
        }

    private:
        std::istream &in_ ;
        std::istringstream* line_in_ ;
        char buffer_[65536] ;
    } ;
}

typedef IO::LineInputStream LineInputStream;

#endif
