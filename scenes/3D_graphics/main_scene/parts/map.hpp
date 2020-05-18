#pragma once

#include "main/scene_base/base.hpp"
#include "scenes/3D_graphics/main_scene/parts/mtltexture.hpp"

#ifdef MAIN_SCENE

struct triangle
{
    static float collision_depth;
    float d; // p1,p2,p3 lie on a plane ax+by+cz = d with (a,b,c) = n
    vcl::vec3 p1, p2, p3, n;
    triangle(vcl::vec3& p1, vcl::vec3& p2, vcl::vec3& p3, vcl::vec3& fakenormal);
    bool collision(vcl::vec3 position, vcl::vec3& impact, vcl::vec3& normal, float margin=0.0f);
};

struct map_structure
{
    void init(const char* MTLpath, const char* OBJpath);

    vcl::mesh_drawable sky, post, post_top, billboard, coin, block, block_v, lift, lift_side;
    GLuint texture_sky, texture_post, texture_post_top, texture_tree, texture_red_block, texture_red_block_v, texture_lift, texture_lift_side;
    GLuint texture_coin[4];
    std::vector<vcl::vec3> post_positions, tree_positions, red_block_positions, yellow_coin_positions, red_coin_positions;

    void create_sky();
    void other_objects();

    float minx = 100.0, maxx = -100.0, miny = 100.0, maxy = -100.0, minz = 100.0, maxz = -100.0;
    const size_t grid_size = 16;
    std::vector<triangle> map_triangle;
    vcl::buffer2D< std::vector<triangle*> > grid;
    bool collision(vcl::vec3 position, vcl::vec3& impact, vcl::vec3& normal, float min_normal_z = -0.2f);
    bool collision_sphere(vcl::vec3 position, float radius_hitbox, vcl::vec3& impact, vcl::vec3& normal, float min_normal_z = -0.2f);
    float get_z(vcl::vec3 position);

    std::vector<vcl::mesh_drawable> map;
    std::vector<mtltexture> map_mtl;
    std::vector<int> texture_indices;
    std::vector<GLuint> map_textures;
    void loadMTL(const char* path);
    void loadOBJ(const char* path);
    void draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf);
    void draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf, int coin_rotation);
};


#endif