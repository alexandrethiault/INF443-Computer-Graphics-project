
#include "star.hpp"

#ifdef MAIN_SCENE

using namespace vcl;

void star_structure::create_star()
{
    mesh _star;
    _star.position.resize(12);
    _star.texture_uv.resize(12);
    const float inner = 0.5f;
    const float PI = 3.14159f;
    for (size_t ku = 0; ku < 5; ++ku) {
        _star.position[ku] = { cos(2 * PI * (4 * ku + 1) / 20), 0, sin(2 * PI * (4 * ku + 1) / 20) };
        _star.position[ku + 5] = { inner * cos(2 * PI * (4 * ku + 3) / 20), 0, inner * sin(2 * PI * (4 * ku + 3) / 20) };
        _star.texture_uv[ku] = { (1 - _star.position[ku].x * 1.5f) * 0.5f, (1 - _star.position[ku].z * 1.0f) * 0.5f };
        _star.texture_uv[ku + 5] = { (1 - _star.position[ku + 5].x * 1.5f) * 0.5f, (1 - _star.position[ku + 5].z * 1.0f) * 0.5f };
    } // 0, 5, 1, 6, 2, 7, 3, 8, 4, 9
    _star.position[10] = { 0, 0.3f, 0 };
    _star.position[11] = { 0, -0.3f, 0 };
    _star.texture_uv[10] = _star.texture_uv[11] = { 0.5f,0.5f };
    for (unsigned int ku = 0; ku < 5; ++ku) {
        _star.connectivity.push_back({ 10,ku, ku + 5 });
        _star.connectivity.push_back({ 10,ku + 5, (ku + 1) % 5 });
        _star.connectivity.push_back({ 11,ku + 5, ku });
        _star.connectivity.push_back({ 11,(ku + 1) % 5, ku + 5 });
    }
    star = _star;
    star.uniform.shading = { 1,0,0 };
    star.uniform.transform.translation = { 0, 0, 3 };
    star.uniform.transform.scaling = 0.1f;
    texture_star = create_texture_gpu(image_load_png("scenes/shared_assets/textures/star.png"));

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
    star_eye = quad_eye;
    star_eye.uniform.shading = { 1,0,0 };
    star_eye.uniform.transform.translation = { 0, 0, 3 };
    star_eye.uniform.transform.scaling = 0.1f;
    texture_star_eye = create_texture_gpu(image_load_png("scenes/shared_assets/textures/stareye.png"));
}

void star_structure::draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf)
{
    glBindTexture(GL_TEXTURE_2D, texture_star);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    for (vec3 pos : { vec3{ -0.997f, -0.720f, 1.225f }, vec3{ -3.079f,6.765f,1.126f }, vec3{ 4.008f,-2.082f,4.742f }}) {
        star.uniform.transform.translation = pos;
        if (surf) draw(star, scene.camera, shaders["mesh"]);
        if (wf) draw(star, scene.camera, shaders["wireframe"]);
    }
}

void star_structure::draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf)
{
    glBindTexture(GL_TEXTURE_2D, texture_star_eye);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    for (vec3 pos : { vec3{ -0.997f, -0.720f, 1.225f }, vec3{ -3.079f,6.765f,1.126f }, vec3{ 4.008f,-2.082f,4.742f }}) {
        star_eye.uniform.transform.translation = pos;
        if (bb) draw(star_eye, scene.camera, shaders["mesh"]);
        if (wf) draw(star_eye, scene.camera, shaders["wireframe"]);
    }
}

void star_structure::move(float t)
{
    const float PI = 3.14159f;
    star.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 0,0,1 }, 2 * PI * t * 0.75f);
    star_eye.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 0,0,1 }, 2 * PI * t * 0.75f);
}

#endif
