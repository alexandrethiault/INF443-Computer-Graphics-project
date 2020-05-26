#pragma once

#include "main/scene_base/base.hpp"
#include "scenes/3D_graphics/main_scene/parts/mtltexture.hpp"
#include "scenes/3D_graphics/main_scene/parts/character.hpp"
#include "scenes/3D_graphics/main_scene/parts/triangle.hpp"

#ifdef MAIN_SCENE

struct map_structure
{
    void init(const char* MTLpath, const char* OBJpath, character_structure* _character);

    vcl::mesh_drawable sky, post, post_top, billboard, coin, block, block_v;
    GLuint texture_sky, texture_post, texture_post_top, texture_tree, texture_red_block, texture_red_block_v;
    GLuint texture_coin[4];
    std::vector<vcl::vec3> post_positions, tree_positions, red_block_positions, yellow_coin_positions, red_coin_positions;
    character_structure* character = nullptr;

    void create_sky();
    void other_objects();

    float minx = 100.0, maxx = -100.0, miny = 100.0, maxy = -100.0;
    const size_t grid_size = 16;
    std::vector<triangle> map_triangle;
    vcl::buffer2D< std::vector<triangle*> > grid;
    bool ground_collision(vcl::vec3 position, vcl::vec3& impact, vcl::vec3& normal);
    bool wall_collision(vcl::vec3 position, vcl::vec3& impact, vcl::vec3& normal, float margin);
    bool ceiling_collision(vcl::vec3 position, vcl::vec3& impact, vcl::vec3& normal, float margin);

    std::vector<vcl::mesh_drawable> map;
    std::vector<mtltexture> map_mtl;
    std::vector<int> texture_indices;
    std::vector<GLuint> map_textures;
    void loadMTL(const char* path);
    void loadOBJ(const char* path);
    void draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    void draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf, const int coin_rotation);
};


#endif