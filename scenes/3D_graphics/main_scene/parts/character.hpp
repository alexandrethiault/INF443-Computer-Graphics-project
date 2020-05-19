#pragma once

#include "main/scene_base/base.hpp"
#include "scenes/3D_graphics/main_scene/parts/mtltexture.hpp"

#ifdef MAIN_SCENE

struct character_structure
{
    // pr�rempli avec ce que j'ai pris pour le chomp
    float scale;
    vcl::vec3 center;
    vcl::hierarchy_mesh_drawable hierarchy;

    void init(const vcl::vec3& center);
    void draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    int find_mesh_index(std::string name);
    void move(float t, float dt);
    vcl::vec3 get_translation();
    void set_translation(vcl::vec3& p);
    void set_rotation(vcl::mat3& R);

    void loadMTL(const char* path);
    void loadOBJ(const char* path);

    std::vector<vcl::mesh_drawable> mario;
    std::vector<mtltexture> mario_mtl;
    std::vector<int> texture_indices;
    std::vector<std::string> part_name;
    std::vector<GLuint> mario_textures;
    std::map<std::string, vcl::vec3> position_centres;
};

#endif


