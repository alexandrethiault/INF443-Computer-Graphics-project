#pragma once

#include "main/scene_base/base.hpp"

#ifdef MAIN_SCENE

struct bridge_structure : scene_base
{
    void setup(vcl::vec3& _center, vcl::vec3& len, vcl::vec3& wid, vcl::vec3& hei);
    void move(float t);
    void draw_bridge(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);

    float thetamax, theta; // l'angle theta bascule de -thetamax à thetamax
    int period; // périodicité period : angle = thetamax * sin(2*pi*t/period)
    vcl::vec3 center, rotation_axis;

    vcl::mesh_drawable bridge;
    GLuint texture_bridge;

};

#endif
