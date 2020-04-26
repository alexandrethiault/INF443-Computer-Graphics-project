#pragma once

#include "main/scene_base/base.hpp"

#ifdef SCENE_BATTLEFIELD

struct vec3t {
    vcl::vec3 p; // position
    float t;     // time
};

struct mtltexture {
    float Ns, Ni, d;
    vcl::vec3 Ka, Kd, Ks, Ke;
    int illum;
    std::string name, map_Kd;
};

struct gui_scene_structure
{
    bool wireframe = false;
    bool surface = true;
    bool billboards = true;
};

struct scene_model : scene_base
{
    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    //void mouse_click(scene_structure& scene, GLFWwindow* window, int button, int action, int mods);
    //void mouse_move(scene_structure& scene, GLFWwindow* window);
    void set_gui();

    // visual representation of a surface
    vcl::mesh_drawable sky, demo_chomp;
    GLuint texture_sky;

    std::vector<vcl::mesh_drawable> map;
    std::vector<mtltexture> map_mtl;
    std::vector<int> texture_indices;
    std::vector<GLuint> map_textures;
    gui_scene_structure gui_scene;
    vcl::timer_interval timer;
    vcl::timer_event timer_event;
};

#endif