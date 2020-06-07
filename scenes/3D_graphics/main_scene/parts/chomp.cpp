
#include "chomp.hpp"
#include <string>
#include <algorithm>

#ifdef MAIN_SCENE

using namespace vcl;

const std::string chomp_dir = "scenes/shared_assets/textures/chomp/";
const float PI = 3.14159f;

// Generator for uniform random number
std::default_random_engine generator;
std::uniform_real_distribution<float> distrib(0.0, 1.0);

mesh mesh_primitive_half_sphere(float radius, const vec3& p0, size_t Nu, size_t Nv)
{
    mesh shape;
    for (size_t ku = 0; ku < Nu; ++ku) {
        for (size_t kv = 0; kv < Nv; ++kv) {
            const float u = static_cast<float>(ku) / static_cast<float>(Nu - 1);
            const float v = static_cast<float>(kv) / static_cast<float>(Nv - 1);

            const float theta = static_cast<float>(0.5f * 3.14159f * v);
            const float phi = static_cast<float>(2 * 3.14159f * u);

            const float x = radius * std::sin(theta) * std::cos(phi);
            const float y = radius * std::sin(theta) * std::sin(phi);
            const float z = radius * std::cos(theta);

            shape.position.push_back(vec3{ x,y,z } + p0);
            shape.texture_uv.push_back({ v,u });
        }
    }
    shape.connectivity = connectivity_grid(Nu, Nv, false, false);
    return shape;
}

// Semi circle staring towards dx, on a (dx,dy) plane, centered on p0
mesh mesh_primitive_half_disc(float radius, const vec3& p0, const vec3& dx, const vec3& dy, size_t N)
{
    mesh disc;
    for (size_t k = 0; k < N; ++k) {
        const float theta = PI * (1.0f * k / (N - 1.0f) - 0.5f);

        const vec3 p = radius * (std::cos(theta) * dx + std::sin(theta) * dy);

        disc.position.push_back(p0 + p);
        disc.texture_uv.push_back({ std::sin(theta) / 2 + 0.5f, -std::cos(theta) / 2 + 0.5f });
        unsigned int uik = (unsigned int)k;
        disc.connectivity.push_back({ uik, (k == N - 1) ? 0 : uik + 1, (unsigned int)N });
    }
    disc.position.push_back(p0);
    disc.texture_uv.push_back({ 0.5f,0.5f });
    return disc;
}

mesh mesh_tooth(const vec3& p0, const vec3& p1, const vec3& p2)
{
    mesh tri;
    tri.position.push_back(p0);
    tri.position.push_back(p1);
    tri.position.push_back(p2);
    tri.texture_uv.push_back({ 0,0.7f });
    tri.texture_uv.push_back({ 1,0 });
    tri.texture_uv.push_back({ 0,-0.7f });
    tri.connectivity.push_back({ 0,1,2 });
    return tri;
}

void chomp_structure::init(const vec3 _center)
{
    if (radius_chomp) {
        std::cout << "Tentative de re-initialiser un chomp deja initialise." << std::endl;
        return;
    }
    radius_chomp = 0.3f;
    radius_reach = radius_chomp * 3.0f;
    radius_eye = radius_chomp / 5.0f;
    max_angular_velocity = PI / 4;
    max_speed = radius_chomp / 6.0f;
    center = _center;
    rel_position = rush_speed = chain1 = chain2 = chain3 = chain4 = { 0,0,0 };
    angle = angular_v = speed = time_chasing = 0.0f;
    rushing = falling = false;

    mesh_drawable body = mesh_primitive_half_sphere(radius_chomp, { 0,0,0 }, 9, 5);
    body.uniform.shading = { 1,0,0 };
    mesh_drawable eye = mesh_primitive_quad({ 0,-1,-1 }, { 0,1,-1 }, { 0,1,1 }, { 0,-1,1 });
    eye.uniform.transform.scaling = radius_eye;
    eye.uniform.shading = { 1,0,0 };
    mesh_drawable mouth = mesh_primitive_half_disc(radius_chomp, { 0,0,0 }, { 1,0,0 }, { 0,1,0 }, 5);
    mouth.uniform.shading = { 1,0,0 };
    mesh_drawable tooth = mesh_tooth({ 0,0.5f,0 }, { 0,-0.5f,0 }, { -0.25f,0,-0.88f });
    tooth.uniform.transform.scaling = radius_chomp / 3.0f;
    tooth.uniform.shading = { 1,0,0 };
    chain = mesh_primitive_quad({ -0.5f,0.5f,0.0f }, { 0.5f,0.5f,0.0f }, { 0.5f,-0.5f,0.0f }, { -0.5f,-0.5f,0.0 });
    chain.uniform.transform.scaling = radius_chomp / 3.0f;
    chain.uniform.shading = { 1,0,0 };

    hierarchy.add(body, "body_up");
    hierarchy.add(body, "body_down", "body_up", { 0,0,0 });
    hierarchy.add(mouth, "mouth_up", "body_up", { 0,0,0 });
    hierarchy.add(mouth, "mouth_down", "body_down", { 0,0,0 });

    float theta = 5 * PI / 12;
    float phi = PI / 5;
    vec3 left{ std::sin(theta) * std::cos(phi), -std::sin(theta) * std::sin(phi), std::cos(theta) };
    vec3 right{ std::sin(theta) * std::cos(phi), std::sin(theta) * std::sin(phi), std::cos(theta) };
    hierarchy.add(eye, "eye_left", "body_up", { radius_chomp * 0.98f * left, rotation_to_vector_mat3(left) });
    hierarchy.add(eye, "eye_right", "body_up", { radius_chomp * 0.98f * right, rotation_to_vector_mat3(right) });

    vec3 head_m2 = radius_chomp * 0.975f * vec3{ std::cos(PI / 2), std::sin(-PI / 2), 0 };
    vec3 head_m1 = radius_chomp * 0.975f * vec3{ std::cos(PI / 4), std::sin(-PI / 4), 0 };
    vec3 head_0 = { radius_chomp * 0.975f, 0, 0 };
    vec3 head_p1 = radius_chomp * 0.975f * vec3{ std::cos(PI / 4), std::sin(PI / 4), 0 };
    vec3 head_p2 = radius_chomp * 0.975f * vec3{ std::cos(PI / 2), std::sin(PI / 2), 0 };
    hierarchy.add(tooth, "tooth_up_1", "body_up", { (3 * head_m1 + 7 * head_0) / 10,  1.36f * rotation_from_axis_angle_mat3({0,0,1},-PI / 8) });
    hierarchy.add(tooth, "tooth_up_2", "body_up", { (8 * head_m1 + 2 * head_0) / 10,  0.88f * rotation_from_axis_angle_mat3({0,0,1},-PI / 8) });
    hierarchy.add(tooth, "tooth_up_3", "body_up", { (1 * head_m2 + 6 * head_m1) / 7,  0.63f * rotation_from_axis_angle_mat3({0,0,1},-3 * PI / 8) });
    hierarchy.add(tooth, "tooth_up_4", "body_up", { (2 * head_m2 + 3 * head_m1) / 5,  0.48f * rotation_from_axis_angle_mat3({0,0,1},-3 * PI / 8) });
    hierarchy.add(tooth, "tooth_up_5", "body_up", { (3 * head_p1 + 7 * head_0) / 10,  1.36f * rotation_from_axis_angle_mat3({0,0,1},PI / 8) });
    hierarchy.add(tooth, "tooth_up_6", "body_up", { (8 * head_p1 + 2 * head_0) / 10,  0.88f * rotation_from_axis_angle_mat3({0,0,1},PI / 8) });
    hierarchy.add(tooth, "tooth_up_7", "body_up", { (1 * head_p2 + 6 * head_p1) / 7,  0.63f * rotation_from_axis_angle_mat3({0,0,1},3 * PI / 8) });
    hierarchy.add(tooth, "tooth_up_8", "body_up", { (2 * head_p2 + 3 * head_p1) / 5,  0.48f * rotation_from_axis_angle_mat3({0,0,1},3 * PI / 8) });
    tooth = mesh_tooth({ -0.3f,0,-0.88f }, { 0,0.5f,0 }, { 0,-0.5f,0 }); // cyclic rotation of texture_uv coordinates
    tooth.uniform.transform.scaling = radius_chomp / 3.0f;
    tooth.uniform.shading = { 1,0,0 };
    hierarchy.add(tooth, "tooth_down_1", "body_down", { (3 * head_m1 + 7 * head_0) / 10,  1.36f * rotation_from_axis_angle_mat3({0,0,1},-PI / 8) });
    hierarchy.add(tooth, "tooth_down_2", "body_down", { (8 * head_m1 + 2 * head_0) / 10,  0.88f * rotation_from_axis_angle_mat3({0,0,1},-PI / 8) });
    hierarchy.add(tooth, "tooth_down_3", "body_down", { (1 * head_m2 + 6 * head_m1) / 7,  0.63f * rotation_from_axis_angle_mat3({0,0,1},-3 * PI / 8) });
    hierarchy.add(tooth, "tooth_down_4", "body_down", { (2 * head_m2 + 3 * head_m1) / 5,  0.48f * rotation_from_axis_angle_mat3({0,0,1},-3 * PI / 8) });
    hierarchy.add(tooth, "tooth_down_5", "body_down", { (3 * head_p1 + 7 * head_0) / 10,  1.36f * rotation_from_axis_angle_mat3({0,0,1},PI / 8) });
    hierarchy.add(tooth, "tooth_down_6", "body_down", { (8 * head_p1 + 2 * head_0) / 10,  0.88f * rotation_from_axis_angle_mat3({0,0,1},PI / 8) });
    hierarchy.add(tooth, "tooth_down_7", "body_down", { (1 * head_p2 + 6 * head_p1) / 7,  0.63f * rotation_from_axis_angle_mat3({0,0,1},3 * PI / 8) });
    hierarchy.add(tooth, "tooth_down_8", "body_down", { (2 * head_p2 + 3 * head_p1) / 5,  0.48f * rotation_from_axis_angle_mat3({0,0,1},3 * PI / 8) });

    texture_chain = create_texture_gpu(image_load_png(chomp_dir + "bb_chomp_chain.png"));
    texture_eye = create_texture_gpu(image_load_png(chomp_dir + "bb_chomp_eye.png"));
    texture_down = create_texture_gpu(image_load_png(chomp_dir + "chomp_down.png"));
    texture_tongue = create_texture_gpu(image_load_png(chomp_dir + "chomp_tongue.png"));
    texture_tooth = create_texture_gpu(image_load_png(chomp_dir + "chomp_tooth.png"));
    texture_up = create_texture_gpu(image_load_png(chomp_dir + "chomp_up.png"));
}

void chomp_structure::draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf)
{
    glBindTexture(GL_TEXTURE_2D, texture_up);
    if (surf) draw_hierarchy_element(hierarchy["body_up"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["body_up"], scene.camera, shaders["wireframe"]);

    glBindTexture(GL_TEXTURE_2D, texture_down);
    if (surf) draw_hierarchy_element(hierarchy["body_down"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["body_down"], scene.camera, shaders["wireframe"]);

    glBindTexture(GL_TEXTURE_2D, texture_tongue);
    if (surf) draw_hierarchy_element(hierarchy["mouth_up"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["mouth_up"], scene.camera, shaders["wireframe"]);
    if (surf) draw_hierarchy_element(hierarchy["mouth_down"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["mouth_down"], scene.camera, shaders["wireframe"]);

    glBindTexture(GL_TEXTURE_2D, texture_tooth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    for (int i = 1; i <= 8; i++) {
        if (surf) draw_hierarchy_element(hierarchy["tooth_up_" + str(i)], scene.camera, shaders["mesh"]);
        if (wf) draw_hierarchy_element(hierarchy["tooth_up_" + str(i)], scene.camera, shaders["wireframe"]);
        if (surf) draw_hierarchy_element(hierarchy["tooth_down_" + str(i)], scene.camera, shaders["mesh"]);
        if (wf) draw_hierarchy_element(hierarchy["tooth_down_" + str(i)], scene.camera, shaders["wireframe"]);
    }
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

bool cmpbillboard(const vec3& u, const vec3& v, const vec3& cam_pos) { return (norm(u - cam_pos) > norm(v - cam_pos)); }

void chomp_structure::draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf)
{
    glBindTexture(GL_TEXTURE_2D, texture_eye); // Eyes first: a chain will always be in front of an eye OR invisible
    if (bb) draw_hierarchy_element(hierarchy["eye_left"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["eye_left"], scene.camera, shaders["wireframe"]);
    if (bb) draw_hierarchy_element(hierarchy["eye_right"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["eye_right"], scene.camera, shaders["wireframe"]);

    glBindTexture(GL_TEXTURE_2D, texture_chain);
    buffer<vec3> chains = { chain1 + center, chain2 + center, chain3 + center, chain4 + center };
    std::sort(chains.begin(), chains.end(),
        [&](const vec3& u, const vec3& v) {return cmpbillboard(u, v, scene.camera.camera_position()); });
    chain.uniform.transform.rotation = scene.camera.orientation;
    for (vec3 pos : chains) {
        chain.uniform.transform.translation = pos + vec3{ 0,0,radius_chomp / 3.0f };
        if (bb) draw(chain, scene.camera, shaders["mesh"]);
        if (wf) draw(chain, scene.camera, shaders["wireframe"]);
    }
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void chomp_structure::move(const vcl::vec3 char_pos, float t, float dt)
{
    if (dt > 0.1f) dt = 0.1f;

    // Open / close jaw, default to periodic
    mat3 R_jaw = rotation_from_axis_angle_mat3({ 0,1,0 }, -0.3f + -0.3f * std::sin(5 * 3.14f * t));
    mat3 R_vertical = { 1,0,0, 0,1,0, 0,0,1 };
    mat3 Symmetry = { 1,0,0, 0,1,0, 0,0,-1 };

    if (rushing) { // Finish rushing before falling
        R_jaw = rotation_from_axis_angle_mat3({ 0,1,0 }, -0.6f); // Fully open
        R_vertical = rotation_from_axis_angle_mat3({ 0,1,0 }, -atan2(rush_speed.z, norm(rush_speed * vec3{ 1,1,0 })));
        rel_position += dt * rush_speed;
        if (norm(rel_position) > radius_reach) {
            rel_position /= norm(rel_position) / radius_reach;
            rushing = false;
            falling = true;
            time_chasing = 0.0f; // Used here as if it was a "time_falling"
            speed = 0.0f;
        }
    }
    else if (falling) { // Finish falling before attacking again
        R_jaw = rotation_from_axis_angle_mat3({ 0,1,0 }, -0.6f);
        time_chasing += dt;
        if (time_chasing > 1) speed += -2.5f * dt; // g = 2.5
        rel_position += {0, 0, speed * dt};
        if (rel_position.z < 0) {
            falling = false;
            speed = 0.0f;
            time_chasing = 0.0f;
            rel_position.z = 0.0f;
            rush_speed = { 0.0f,0.0f,0.0f };
        }
        for (vec3* chain : { &chain1, &chain2, &chain3, &chain4 })
            chain->z = std::min(chain->z, rel_position.z);
    }
    else if (norm(char_pos - center) < 2 * radius_reach) { // Target before attack
        time_chasing += dt;
        float da = atan2(char_pos.y - (center + rel_position).y, char_pos.x - (center + rel_position).x) - angle;
        while (da > PI) {
            angle += 2*PI;
            da -= 2*PI;
        }
        while (da < -PI) {
            angle -= 2 * PI;
            da += 2 * PI;
        }
        angle += da * std::min(1.0f, 5 * max_angular_velocity * dt / abs(da));
        if (time_chasing > 1.75f && std::sin(5 * 3.14f * t) > 0.75f) { // Avoid discontinuity in mouth opening
            rushing = true;
            rush_speed = normalize(char_pos - (center + rel_position + vec3{ 0,0,radius_chomp })) * 100 * max_speed;
            if (rush_speed.z < 0) rush_speed.z = 0;
        }
        R_vertical = rotation_from_axis_angle_mat3({ 0,1,0 }, -atan2(rush_speed.z, norm(rush_speed * vec3{ 1,1,0 })));
    }
    else { // Random movement on the plane
        time_chasing = 0.0f;
        if (distrib(generator) < 0.01f) {
            if (distrib(generator) * 3 < 1) angular_v = max_angular_velocity;
            else if (distrib(generator) * 2 < 1) angular_v = -max_angular_velocity;
            else angular_v = 0;
        }
        angle += angular_v * dt;

        if (distrib(generator) < 0.05f) {
            if (distrib(generator) * 4 < 3) speed = max_speed;
            else speed = 0;
        }
        rel_position += dt * vec3{ speed * std::cos(angle), speed * std::sin(angle), 0 };
        if (norm(rel_position) > radius_reach) rel_position /= norm(rel_position) / radius_reach;
    }

    hierarchy["body_up"].transform.translation = center + rel_position + vec3{ 0, 0, radius_chomp };
    hierarchy["body_up"].transform.rotation = rotation_from_axis_angle_mat3({ 0,0,1 }, angle) * R_vertical * R_jaw;
    hierarchy["body_down"].transform.rotation = Symmetry * R_jaw * R_jaw;
    hierarchy.update_local_to_global_coordinates();

    // Movement of the 4 chains
    float d = radius_reach / 4.9999f;
    int cnt = 0;
    while (norm(chain4 - rel_position) > d * 1.0001f && cnt < 10) {
        if (norm(chain4 - rel_position) > d) chain4 = rel_position + normalize(chain4 - rel_position) * d;
        if (norm(chain3 - chain4) > d) chain3 = chain4 + normalize(chain3 - chain4) * d;
        if (norm(chain2 - chain3) > d) chain2 = chain3 + normalize(chain2 - chain3) * d;
        if (norm(chain1 - chain2) > d) chain1 = chain2 + normalize(chain1 - chain2) * d;
        if (norm(chain1) > d) {
            chain4 /= norm(chain1) / d;
            chain3 /= norm(chain1) / d;
            chain2 /= norm(chain1) / d;
            chain1 /= norm(chain1) / d;
        }
        cnt++;
    }
}

#endif