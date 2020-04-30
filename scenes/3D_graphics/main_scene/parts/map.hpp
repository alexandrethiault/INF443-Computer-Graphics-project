#pragma once

#include "main/scene_base/base.hpp"

#ifdef MAIN_SCENE

struct mtltexture {
    float Ns = -1, Ni = -1, d = -1;
    vcl::vec3 Ka, Kd, Ks, Ke;
    int illum = -1;
    std::string name, map_Kd;
};

struct map_structure {
    vcl::mesh_drawable sky, post, post_top, billboard, shadow;
    GLuint texture_sky, texture_post, texture_post_top, texture_tree;
    std::vector<vcl::vec3> post_positions, tree_positions;
    void create_sky();
    void other_objects();

    std::vector<vcl::mesh_drawable> map;
    std::vector<mtltexture> map_mtl;
    std::vector<int> texture_indices;
    std::vector<GLuint> map_textures;
    void loadMTL(const char* path);
    void loadOBJ(const char* path);
    void draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    void draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf);
};


#endif