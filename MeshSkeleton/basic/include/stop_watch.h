#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

#ifdef WIN32
#   include <windows.h>
#else
#   include <sys/time.h>
#endif // WIN32

/**
*
* The purpose of this file is to make a timer function
* that is as precise as possible on any given platform.
*
* SystemStopwatch prints to cerr the elapsed time between
* the line where it is declared (or restart) and the end
* of the scope where it is declared.
*
* usage example:
*   {
*      SystemStopwatch clock ;
*      do_something() ;
*      clock.print_elapsed_time() ;
*   }
*/

class  Stopwatch
{
public :
    Stopwatch() ;
    ~Stopwatch() ;

    /**
    * prints elapsed time since the construction (or restart) of
    * this SystemStopWatch (in seconds).
    */
    void print() ;

    /**
    * returns user elapsed time since the construction (or restart)
    * of this SystemStopWatch (in seconds).
    */
    double elapsed_user_time() const ;

    void  start() ;

    /**
    * returns current time (in seconds).
    */
    static double now() ;

private:

#ifdef WIN32
    LONGLONG  freq_;
    LONGLONG  start_count_;
#else
    timeval start_time_;
#endif

} ;


#endif

