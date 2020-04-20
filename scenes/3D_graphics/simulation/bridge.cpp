
#include "bridge.hpp"

// AVANCEMENT : le mouvement sinusoidal est le seul truc qui est fait. Même la modélisation reste à faire

#ifdef SCENE_BRIDGE

using namespace vcl;

const float PI = 3.14159f;
const std::string assets_dir = "scenes/shared_assets/textures/";
mesh create_bridge();
mesh create_bridge_support();



void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{
    thetamax = PI / 8;
    theta = 0;
    period = 5; // 5 secondes ?
    bridge = create_bridge();
    bridge_support = create_bridge_support();
    texture_bridge = create_texture_gpu(image_load_png(assets_dir + "bridge.png"));
    texture_support = create_texture_gpu(image_load_png(assets_dir + "bridge_support.png"));
    timer.scale = 1.0f;
}

void scene_model::frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    set_gui();
    float t = timer.t;
    glEnable(GL_POLYGON_OFFSET_FILL); // avoids z-fighting when displaying wireframe

    theta = thetamax * std::sin(2 * PI * t / period);
    bridge.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 1,0,0 }, theta);

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

    glBindTexture(GL_TEXTURE_2D, texture_bridge);
    if (gui_scene.surface) draw(bridge, scene.camera, shaders["mesh"]);
    if (gui_scene.wireframe) draw(bridge, scene.camera, shaders["wireframe"]);
    glBindTexture(GL_TEXTURE_2D, texture_support);
    if (gui_scene.surface) draw(bridge_support, scene.camera, shaders["mesh"]);
    if (gui_scene.wireframe) draw(bridge_support, scene.camera, shaders["wireframe"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

mesh create_bridge()
{
    mesh bridge;

    return bridge;
}

mesh create_bridge_support()
{
    mesh bridge_support;

    return bridge_support;
}

void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface); ImGui::SameLine();
    ImGui::SliderFloat("Time scale", &timer.t, 0.2f, 5);
}

#endif
