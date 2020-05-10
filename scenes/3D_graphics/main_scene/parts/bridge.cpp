
#include "bridge.hpp"

// AVANCEMENT : le mouvement sinusoidal est le seul truc qui est fait. Même la modélisation reste à faire

#ifdef MAIN_SCENE

using namespace vcl;

void bridge_structure::setup(vec3& _center, vec3& len, vec3& wid, vec3& hei)
{
    center = _center;
    rotation_axis = wid;
    thetamax = 3.14159f / 16;
    theta = 0;
    period = 5;
    mesh _bridge = mesh_primitive_parallelepiped(-len/2-wid/2, len, wid, hei);
    _bridge.texture_uv = {
        {0,0},{5,0},{5,1},{0,1},
        {5,0},{5.33f,0},{5.33f,1},{5,1},
        {5,1},{5,0},{0,0},{0,1},
        {0,0},{0.33f,0},{0.33f,1},{0,1},
        {0.33f,1},{0.33f,0},{5.33f,0},{5.33f,1},
        {5.33f,0},{0.33f,0},{0.33f,1},{5.33f,1}
    };
    bridge = _bridge; // conversion mesh -> mesh_drawable
    bridge.uniform.transform.translation = center;
    texture_bridge = create_texture_gpu(image_load_png("scenes/shared_assets/textures/wood.png"));
}

void bridge_structure::move(float t)
{
    theta = thetamax * std::sin(2 * 3.14159f * t / period);
    bridge.uniform.transform.rotation = rotation_from_axis_angle_mat3(rotation_axis, theta);
    
    /* Forces si Mario doit peser sur le pont

    float pos_mario = 0; // entre -1 et 1 pour parler du côté où Mario appuie sur le pont
    if (mario_on_bridge()) pos_mario = get_bridge_pos(); // ces fonctions sont à coder

    const vec3 g = { 0,0,-9.81f};
    vec3 force; // A COMPLETER
    // Numerical Integration
    for (int i = 0; i < N; i++) {
        v = v + dt * force / m;
        theta = theta + dt * v;
        if (theta>thetamax) {
            theta = thetamax;
            if (v>0) v=0;
        }
        if (theta<-thetamax) {
            theta = -thetamax;
            if (v<0) v=0;
        }
    }
    */
}

void bridge_structure::draw_bridge(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf)
{
    glBindTexture(GL_TEXTURE_2D, texture_bridge);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (surf) draw(bridge, scene.camera, shaders["mesh"]);
    if (wf) draw(bridge, scene.camera, shaders["wireframe"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

#endif
