#pragma once

#include "main/scene_base/base.hpp"

#ifdef MAIN_SCENE

struct mtltexture {
    float Ns = -1, Ni = -1, d = -1;
    vcl::vec3 Ka, Kd, Ks, Ke;
    int illum;
    std::string name, map_Kd;
};

struct map_structure {
    vcl::mesh_drawable sky;
    GLuint texture_sky;
    void create_sky();

    std::vector<vcl::mesh_drawable> map;
    std::vector<mtltexture> map_mtl;
    std::vector<int> texture_indices;
    std::vector<GLuint> map_textures;
    void loadMTL(const char* path);
    void loadOBJ(const char* path);
};


#endif