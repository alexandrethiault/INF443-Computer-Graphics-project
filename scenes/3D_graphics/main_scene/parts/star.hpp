#pragma once

#include "main/scene_base/base.hpp"

#ifdef MAIN_SCENE

struct star_structure {
    vcl::mesh_drawable star, star_eye;
    GLuint texture_star, texture_star_eye;

    void create_star();
    void draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    void draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf);
    void move(float t);
};

#endif
