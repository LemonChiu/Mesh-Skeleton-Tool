#pragma once

/*
*   purpose: estimate pointcloud normals
*/

#include <string>

class Object;

class  NormalEstimator
{
public:
    NormalEstimator(void) {}
    ~NormalEstimator(void) {}

    static std::string title() { return "[NormalEstimator]: "; }

    // the in/out is remain to be determined
    static void apply(Object* object, unsigned int K = 10);
};
