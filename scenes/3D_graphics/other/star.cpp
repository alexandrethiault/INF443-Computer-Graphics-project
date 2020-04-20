
#include "star.hpp"

#include <random>

// AVANCEMENT : fini

#ifdef SCENE_STAR

using namespace vcl;

const std::string assets_dir = "scenes/shared_assets/textures/";
const float PI = 3.14159f;
mesh create_terrain();
mesh create_star();
mesh create_star_eye();

void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 30.0f;
    scene.camera.apply_rotation(0, 0, 0, 1.2f);

    
    mesh quadrangle; // Square floor, for visualization
    quadrangle.position = {{0,0,0}, {1,0,0}, {1,1,0}, {0,1,0}};
    quadrangle.connectivity = {{0,1,2}, {0,2,3}};

    demo_surface = mesh_drawable(quadrangle);
    demo_surface.shader = shaders["mesh"];
    demo_surface.uniform.color = {1.0f, 1.0f, 0.6f};

    star = create_star();
    star.uniform.shading = { 1,0,0 };
    star.uniform.transform.translation = { 0, 0, 3 };
    star.uniform.transform.scaling = 2.0f;
    texture_star = create_texture_gpu(image_load_png(assets_dir + "star.png"));
    star_eye = create_star_eye();
    star_eye.uniform.shading = { 1,0,0 };
    star_eye.uniform.transform.translation = { 0, 0, 3 };
    star_eye.uniform.transform.scaling = 2.0f;
    texture_star_eye = create_texture_gpu(image_load_png(assets_dir + "stareye.png"));

    timer.scale = 1.0f;
    timer.t_max = 10;
}

void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    timer.update();
    const float t = timer.t;
    set_gui();
    glEnable(GL_POLYGON_OFFSET_FILL); // avoids z-fighting when displaying wireframe
    draw(demo_surface, scene.camera);

    star.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 0,0,1 },2*PI*t/timer.t_max);
    star_eye.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 0,0,1 }, 2 * PI * t / timer.t_max);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glBindTexture(GL_TEXTURE_2D, texture_star);
    if (gui_scene.surface) draw(star, scene.camera, shaders["mesh"]);
    if (gui_scene.wireframe) draw(star, scene.camera, shaders["wireframe"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(false);
    glBindTexture(GL_TEXTURE_2D, texture_star_eye);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (gui_scene.surface) draw(star_eye, scene.camera, shaders["mesh"]);
    if (gui_scene.wireframe) draw(star_eye, scene.camera, shaders["wireframe"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);
}

mesh create_star()
{
    mesh star;
    star.position.resize(12);
    star.texture_uv.resize(12);
    const float inner = 0.5f;
    for (size_t ku = 0; ku < 5; ++ku) {
        star.position[ku] = { cos(2 * PI * (4 * ku + 1) / 20), 0, sin(2 * PI * (4 * ku + 1) / 20) };
        star.position[ku+5] = { inner * cos(2 * PI * (4 * ku + 3) / 20), 0, inner * sin(2 * PI * (4 * ku + 3) / 20) };
        star.texture_uv[ku] = { (1-star.position[ku].x*1.5f) * 0.5f, (1-star.position[ku].z*1.0f) * 0.5f };
        star.texture_uv[ku+5] = { (1-star.position[ku+5].x*1.5f) * 0.5f, (1-star.position[ku+5].z*1.0f) * 0.5f };
    } // 0, 5, 1, 6, 2, 7, 3, 8, 4, 9
    star.position[10] = { 0, 0.3f, 0 };
    star.position[11] = { 0, -0.3f, 0 };
    star.texture_uv[10] = star.texture_uv[11] = { 0.5f,0.5f };
    for (unsigned int ku = 0; ku < 5; ++ku)  {
        star.connectivity.push_back({ 10,ku, ku + 5 });
        star.connectivity.push_back({ 10,ku+5, (ku+1)%5 });
        star.connectivity.push_back({ 11,ku+5, ku });
        star.connectivity.push_back({ 11,(ku+1)%5, ku+5 });
    }
    return star;
}

mesh create_star_eye()
{
    mesh quad_eye;
    quad_eye.position.push_back({ 0.25f,0.30f,-0.1f });
    quad_eye.position.push_back({ 0.25f,0.23f,0.4f });
    quad_eye.position.push_back({ -0.25f,0.23f,0.4f });
    quad_eye.position.push_back({ -0.25f,0.30f,-0.1f });
    quad_eye.texture_uv.push_back({ 0,1 });
    quad_eye.texture_uv.push_back({ 0,0 });
    quad_eye.texture_uv.push_back({ 2,0 });
    quad_eye.texture_uv.push_back({ 2,1 });
    quad_eye.connectivity.push_back({ 0,1,2 });
    quad_eye.connectivity.push_back({ 0,2,3 });
    return quad_eye;
}

void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface); ImGui::SameLine();
}

#endif

