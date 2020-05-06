
#include "bridge.hpp"

// AVANCEMENT : le mouvement sinusoidal est le seul truc qui est fait. M�me la mod�lisation reste � faire

#ifdef SCENE_BRIDGE

using namespace vcl;

mesh scene_model::create_bridge(vec3& center, vec3& len, vec3& wid, vec3& hei)
{
    rotation_axis = wid;
    thetamax = 3.14159f / 8;
    theta = 0;
    period = 5; // 5 secondes ?
    /*{p000, p100, p110, p010,
        p100, p101, p111, p110,
        p110, p111, p011, p010,
        p000, p001, p011, p010,
        p000, p001, p101, p100,
        p101, p001, p011, p111};*/
    mesh bridge = mesh_primitive_parallelepiped(center -len/2-wid/2, len, wid, hei);
    bridge.texture_uv = {
        {0,0},{5,0},{5,1},{0,1},
        {5,0},{5.33f,0},{5.33f,1},{5,1},
        {5,1},{5,0},{0,0},{0,1},
        {0,0},{0.33f,0},{0.33f,1},{0,1},
        {0.33f,1},{0.33f,0},{5.33f,0},{5.33f,1},
        {5.33f,0},{0.33f,0},{0.33f,1},{5.33f,1}
    };
    texture_bridge = create_texture_gpu(image_load_png("scenes/shared_assets/textures/wood.png"));
    timer.scale = 1.0f;
    return bridge;
}



void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 30.0f;
    scene.camera.apply_rotation(0, 0, 0, 1.2f);
    bridge = create_bridge(vec3{ 0,0,0 }, vec3{ 2,0,0 }, vec3{ 0,0.5f,0 }, vec3{ 0,0,0.1f });
}

void scene_model::frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    set_gui();
    float t = timer.t;
    glEnable(GL_POLYGON_OFFSET_FILL); // avoids z-fighting when displaying wireframe

    theta = thetamax * std::sin(2 * 3.14159f * t / period);
    bridge.uniform.transform.rotation = rotation_from_axis_angle_mat3(rotation_axis, theta);

    /* Forces si Mario doit peser sur le pont

    float pos_mario = 0; // entre -1 et 1 pour parler du c�t� o� Mario appuie sur le pont
    if (mario_on_bridge()) pos_mario = get_bridge_pos(); // ces fonctions sont � coder

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (gui_scene.surface) draw(bridge, scene.camera, shaders["mesh"]);
    if (gui_scene.wireframe) draw(bridge, scene.camera, shaders["wireframe"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface); ImGui::SameLine();
    ImGui::SliderFloat("Time scale", &timer.t, 0.2f, 5);
}

#endif
