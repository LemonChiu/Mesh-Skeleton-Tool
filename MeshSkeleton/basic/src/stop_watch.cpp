#include "stop_watch.h"
#include "basic_types.h"
#include <iostream>

/**
/*  purpose: Timer
*/

Stopwatch::Stopwatch() {
    start();
}

Stopwatch::~Stopwatch() {}

void Stopwatch::start()
{
#ifdef WIN32
    LARGE_INTEGER  largeInteger;
    QueryPerformanceFrequency(&largeInteger);
    freq_ = largeInteger.QuadPart;
    QueryPerformanceCounter(&largeInteger);
    start_count_ = largeInteger.QuadPart;
#else
    gettimeofday(&start_time_, 0);
#endif // WIN32
}

double Stopwatch::elapsed_user_time() const
{
#ifdef WIN32
    LARGE_INTEGER  largeInteger;
    QueryPerformanceCounter(&largeInteger);
    LONGLONG now_count = largeInteger.QuadPart;

    double time = (now_count - start_count_) / static_cast<double>(freq_);
    return static_cast<int>(time * 1000) / 1000.0;
#else
    timeval now;
    gettimeofday(&now, 0);
    return (now.tv_sec - start_time_.tv_sec) +
        (now.tv_usec - start_time_.tv_usec) / 1.0e6;
#endif  // WIN32
}

double Stopwatch::now()
{
#ifdef WIN32
    return double(GetTickCount()) / 1000.0 ;
#else
    clock_t end_user ;
    tms now_tms ;
    return double(times(&now_tms)) / 100.0 ;
#endif
}

void Stopwatch::print()
{
    std::cout
        << "Timing: "
        << elapsed_user_time()
        << " seconds" << std::endl ;
}
