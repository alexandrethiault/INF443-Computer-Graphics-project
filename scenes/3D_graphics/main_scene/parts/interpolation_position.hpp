#pragma once

#include "main/scene_base/base.hpp"

#ifdef MAIN_SCENE

struct vec3t {
    vcl::vec3 p; // position
    float t = 0.0f;     // time
};

struct flight_model
{
    void setup_flight(std::map<std::string,GLuint>& shaders, scene_structure& scene);
    void draw_path(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool kf, bool pg);
    void mouse_click(scene_structure& scene, GLFWwindow* window);
    void mouse_move(scene_structure& scene, GLFWwindow* window);
    void equalize_speed(float scale, bool first_time=false);

    // Data (p_i,t_i)
    vcl::buffer<vec3t> keyframes;                          // Given (position,time)
    vcl::mesh_drawable point_visual;                       // moving point
    vcl::mesh_drawable keyframe_visual;                    // keyframe samples
    vcl::mesh_drawable keyframe_picked;                    // showing the picked sample
    vcl::segment_drawable_immediate_mode segment_drawer;   // used to draw segments between keyframe samples
    vcl::curve_dynamic_drawable trajectory;                // Draw the trajectory of the moving point as a curve
    vcl::vec3 p, dp, d2p; // position, difference since last time

    // Store the index of a selected sphere
    int picked_object;

    vcl::timer_interval timer;

};

#endif


