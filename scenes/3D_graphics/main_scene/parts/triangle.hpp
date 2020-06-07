#pragma once

#include "main/scene_base/base.hpp"

#ifdef MAIN_SCENE

struct triangle
{
    static float ground_collision_depth, ground_collision_stick, wall_collision_depth;
    float d; // p1,p2,p3 lie on a plane ax+by+cz = d with (a,b,c) = n
    vcl::vec3 p1, p2, p3, n;
    triangle(vcl::vec3 p1, vcl::vec3 p2, vcl::vec3 p3, vcl::vec3 fakenormal);
    bool collision(vcl::vec3 position, vcl::vec3& impact, vcl::vec3& normal, float margin=0.0f);
};

#endif
