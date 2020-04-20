#pragma once

#include "main/scene_base/base.hpp"

#ifdef SCENE_BATTLEFIELD

struct vec3t {
    vcl::vec3 p; // position
    float t;     // time
};

struct particle_structure
{
    vcl::vec3 p; // Position
    vcl::vec3 v; // Speed
};

struct gui_scene_structure
{
    bool wireframe = false;
    bool surface = true;
    bool billboards = true;
    bool chain = true;
    bool skeleton = false;
    bool display_keyframe = false;
    bool display_polygon = false;
    bool stop_balls = false;
};

struct scene_model : scene_base
{
    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void update_tree_position(size_t n);
    void update_billboard_position(size_t n);
    void setup_birb();
    void setup_birb_flight();
    void set_birb_rotation(const vcl::vec3& p, const vcl::vec3& dp, const float t);
    void setup_chain(vcl::vec3& pApos);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void simulate_chain(float dt);
    void mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods);
    void mouse_move(scene_structure& scene, GLFWwindow* window);
    void set_gui();

    // visual representation of a surface
    vcl::mesh_drawable terrain, trunk, foliage, sky, billboard;
    vcl::buffer<vcl::vec3> tree_position;
    vcl::buffer<std::pair<vcl::vec3,size_t>> billboard_position;
    GLuint texture_bg, texture_flower[10], texture_sky;

    vcl::buffer<vec3t> keyframes; // Given (position,time)
    vcl::mesh_drawable keyframe_visual;                    // keyframe samples
    vcl::mesh_drawable keyframe_picked;                    // showing the picked sample
    vcl::segment_drawable_immediate_mode segment_drawer;   // used to draw segments between keyframe samples
    int picked_object; // Store the index of a selected sphere

    vcl::hierarchy_mesh_drawable hierarchy;
    vcl::hierarchy_mesh_drawable_display_skeleton hierarchy_visual_debug;

    std::list<particle_structure> particles; // Storage of all currently active particles
    vcl::mesh_drawable sphere; // Visual representation of a particle - a sphere

    gui_scene_structure gui_scene;
    vcl::timer_interval timer;
    vcl::timer_event timer_event;

    particle_structure pA;
    vcl::buffer<particle_structure> pBs;
    float L0; // Rest length between points
    size_t N_chain; // Number of points from chain
    vcl::mesh_drawable cube;      // Visual display of chain particles
};

#endif