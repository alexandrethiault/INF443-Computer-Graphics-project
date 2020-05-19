#pragma once

#include "main/scene_base/base.hpp"
#include "scenes/3D_graphics/main_scene/parts/map.hpp"
// importer la classe triangle dans un fichier hpp à part qui lui serait propre serait mieux

#ifdef MAIN_SCENE

struct bridge_structure
{
    void setup(vcl::vec3& _center, vcl::vec3& len, vcl::vec3& wid, vcl::vec3& hei);
    void move(float t, float dt);
    void draw_bridge(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);

    float period, thetamax, theta; // periodic, residual movement (no movement is boring)
    float f, mg, kappa, invL, sigma, tau, phi, dphi, d2phi; // momentum things, caused by objects on the bridge
    vcl::vec3 center, rotation_axis;

    vcl::mesh_drawable bridge;
    GLuint texture_bridge;

    float minx = 100.0, maxx = -100.0, miny = 100.0, maxy = -100.0;
    std::vector<triangle> triangles0, triangles;
    bool ground_collision(vcl::vec3 position, vcl::vec3& impact, vcl::vec3& normal); // walls can be ignored

};

#endif
