
#include "chomp_moving.hpp"
#include <string>
#include <algorithm>

// AVANCEMENT : fini

#ifdef SCENE_CHOMP_MOVING

using namespace vcl;

const std::string chomp_dir = "scenes/shared_assets/textures/chomp/";
const float PI = 3.14159f;

// Generator for uniform random number
std::default_random_engine generator;
std::uniform_real_distribution<float> distrib(0.0, 1.0);

void scene_model::setup_data(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 5.0f;
    scene.camera.apply_rotation(0, 0, 0, 1.2f);

    demo_ground = mesh_primitive_disc(4);
    demo_ground.uniform.color = { 1,1,0.5f };
    demo_ground.uniform.shading = { 1,0,0 };

    chomp.init({ 0,0,0 });
    chomp.hierarchy.set_shader_for_all_elements(shaders["mesh"]);

    timer.scale = 1.0f;
    timer.t_max = 10.0f;
}

void scene_model::frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    set_gui();
    timer.update();
    const float t = timer.t;

    draw(demo_ground, scene.camera, shaders["mesh"]);

    chomp.move(t, ((t<last_t) ? timer.t_max-timer.t_min : 0) + t-last_t);

    chomp.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe);

    //// BILLBOARDS ALWAYS LAST ////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(false);
    chomp.draw_billboards(shaders, scene, gui_scene.billboards, gui_scene.wireframe);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);

    last_t = t;
}

mesh mesh_primitive_half_sphere(float radius, const vec3& p0, size_t Nu, size_t Nv)
{
    mesh shape;
    for (size_t ku = 0; ku < Nu; ++ku) {
        for (size_t kv = 0; kv < Nv; ++kv) {
            const float u = static_cast<float>(ku) / static_cast<float>(Nu - 1);
            const float v = static_cast<float>(kv) / static_cast<float>(Nv - 1);

            // Angles
            const float theta = static_cast<float>(0.5f * 3.14159f * v);
            const float phi = static_cast<float>(2 * 3.14159f * u);

            // Spherical coordinates
            const float x = radius * std::sin(theta) * std::cos(phi);
            const float y = radius * std::sin(theta) * std::sin(phi);
            const float z = radius * std::cos(theta);

            const vec3 p = { x,y,z };      // Position (centered)
            const vec3 n = normalize(p); // Normal
            const vec2 uv = { v,u };        // Texture-coordinates

            shape.position.push_back(p + p0); // Add new position (with translation of the center)
            shape.normal.push_back(n);
            shape.texture_uv.push_back(uv);
        }
    }
    shape.connectivity = connectivity_grid(Nu, Nv, false, false);
    return shape;
}

// demi cercle regardant vers dx, d'extrêmité dy, centré en p0
mesh mesh_primitive_half_disc(float radius, const vec3& p0, const vec3& dx, const vec3& dy, size_t N)
{
    mesh disc;

    for (size_t k = 0; k < N; ++k)
    {
        const float u = 1.0f * k / (N - 1.0f) - 0.5f;
        const float theta = PI * u;
        const vec3 p = radius * (std::cos(theta) * dx + std::sin(theta) * dy);

        disc.position.push_back(p0 + p);
        disc.normal.push_back(cross(dx, dy));
        disc.texture_uv.push_back({ std::sin(theta) / 2 + 0.5f, -std::cos(theta) / 2 + 0.5f });
        unsigned int uik = (unsigned int) k;
        disc.connectivity.push_back({ uik, (k == N - 1) ? 0 : uik + 1, (unsigned int)N });
    }
    disc.position.push_back(p0);
    disc.normal.push_back(cross(dx, dy));
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

void chomp_structure::init(const vec3& _center)
{
    if (radius_chomp) {
        std::cout << "Tentative de re-initialiser un chomp deja initialise." << std::endl;
        return;
    }

    radius_chomp = 0.25f;
    radius_reach = radius_chomp * 3.0f;
    radius_eye = radius_chomp / 5.0f;
    max_angular_velocity = PI / 4;
    max_speed = radius_chomp / 3.0f;
    center = _center;
    rel_position = { 0,0,0 };
    angle = angular_v = speed = 0.;
    chain1 = chain2 = chain3 = chain4 = { 0,0,0 };


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

    // Le chomp regarde vers x
    hierarchy.add(body, "body_up");
    hierarchy.add(body, "body_down", "body_up", { 0,0,0 });
    hierarchy.add(mouth, "mouth_up", "body_up", { 0,0,0 });
    hierarchy.add(mouth, "mouth_down", "body_down", { 0,0,0 });

    float theta = 5 * PI / 12;
    float phi = PI / 5;
    vec3 left{ sin(theta) * cos(phi), -sin(theta) * sin(phi), cos(theta) };
    vec3 right{ sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta) };
    hierarchy.add(eye, "eye_left", "body_up", { radius_chomp * 0.98f * left, rotation_to_vector_mat3(left) });
    hierarchy.add(eye, "eye_right", "body_up", { radius_chomp * 0.98f * right, rotation_to_vector_mat3(right) });

    vec3 head_m2 = radius_chomp * 0.975f * vec3{ cos(PI / 2), sin(-PI / 2), 0 };
    vec3 head_m1 = radius_chomp * 0.975f * vec3{ cos(PI / 4), sin(-PI / 4), 0 };
    vec3 head_0 = { radius_chomp * 0.975f, 0, 0 };
    vec3 head_p1 = radius_chomp * 0.975f * vec3{ cos(PI / 4), sin(PI / 4), 0 };
    vec3 head_p2 = radius_chomp * 0.975f * vec3{ cos(PI / 2), sin(PI / 2), 0 };
    hierarchy.add(tooth, "tooth_up_1", "body_up", { (3 * head_m1 + 7 * head_0) / 10,  1.36f * rotation_from_axis_angle_mat3({0,0,1},-PI / 8) });
    hierarchy.add(tooth, "tooth_up_2", "body_up", { (8 * head_m1 + 2 * head_0) / 10,  0.88f * rotation_from_axis_angle_mat3({0,0,1},-PI / 8) });
    hierarchy.add(tooth, "tooth_up_3", "body_up", { (1 * head_m2 + 6 * head_m1) / 7,  0.63f * rotation_from_axis_angle_mat3({0,0,1},-3 * PI / 8) });
    hierarchy.add(tooth, "tooth_up_4", "body_up", { (2 * head_m2 + 3 * head_m1) / 5,  0.48f * rotation_from_axis_angle_mat3({0,0,1},-3 * PI / 8) });
    hierarchy.add(tooth, "tooth_up_5", "body_up", { (3 * head_p1 + 7 * head_0) / 10,  1.36f * rotation_from_axis_angle_mat3({0,0,1},PI / 8) });
    hierarchy.add(tooth, "tooth_up_6", "body_up", { (8 * head_p1 + 2 * head_0) / 10,  0.88f * rotation_from_axis_angle_mat3({0,0,1},PI / 8) });
    hierarchy.add(tooth, "tooth_up_7", "body_up", { (1 * head_p2 + 6 * head_p1) / 7,  0.63f * rotation_from_axis_angle_mat3({0,0,1},3 * PI / 8) });
    hierarchy.add(tooth, "tooth_up_8", "body_up", { (2 * head_p2 + 3 * head_p1) / 5,  0.48f * rotation_from_axis_angle_mat3({0,0,1},3 * PI / 8) });
    tooth = mesh_tooth({ -0.3f,0,-0.88f }, { 0,0.5f,0 }, { 0,-0.5f,0 });
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // avoids sampling artifacts
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glBindTexture(GL_TEXTURE_2D, texture_eye);
    if (bb) draw_hierarchy_element(hierarchy["eye_left"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["eye_left"], scene.camera, shaders["wireframe"]);
    if (bb) draw_hierarchy_element(hierarchy["eye_right"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["eye_right"], scene.camera, shaders["wireframe"]);

    glBindTexture(GL_TEXTURE_2D, texture_chain);
    buffer<vec3> chains = { chain1, chain2, chain3, chain4 };
    std::sort(chains.begin(), chains.end(),
        [&](const vec3& u, const vec3& v) {return cmpbillboard(u, v, scene.camera.camera_position()); });
    chain.uniform.transform.rotation = scene.camera.orientation;
    for (vec3 pos : chains) {
        chain.uniform.transform.translation = pos + vec3{ 0,0,radius_chomp/3.0f };
        if (bb) draw(chain, scene.camera, shaders["mesh"]);
        if (wf) draw(chain, scene.camera, shaders["wireframe"]);
    }
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void chomp_structure::move(float t, float dt)
{
    if (dt > 0.1f) dt = 0.1f;

    // Open / close jaw
    mat3 const R_jaw = rotation_from_axis_angle_mat3({ 0,1,0 }, -0.3f + -0.3f * std::sin(5 * 3.14f * t));
    mat3 const Symmetry = { 1,0,0, 0,1,0, 0,0,-1 };
    
    // Move on the plane
    if (distrib(generator) < 0.01f)
        if (distrib(generator) * 3 < 1) angular_v = max_angular_velocity;
        else if (distrib(generator) * 2 < 1) angular_v = -max_angular_velocity;
        else angular_v = 0;
    angle += angular_v * dt;
    if (distrib(generator) < 0.01f)
        if (distrib(generator) * 2 < 1) speed = max_speed;
        else speed = 0;
    rel_position += dt * vec3{ speed* cos(angle), speed* sin(angle), 0 };
    if (norm(rel_position) > radius_reach) rel_position /= norm(rel_position)/radius_reach;

    hierarchy["body_up"].transform.translation = center + rel_position + vec3{0, 0, radius_chomp};
    hierarchy["body_up"].transform.rotation = rotation_from_axis_angle_mat3({ 0,0,1 }, angle) * R_jaw;
    hierarchy["body_down"].transform.rotation = Symmetry * R_jaw * R_jaw;
    hierarchy.update_local_to_global_coordinates();

    // Mouvement des chaînes
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

void scene_model::set_gui()
{
    ImGui::Text("Display: "); ImGui::SameLine();
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface);     ImGui::SameLine();
    ImGui::Checkbox("Billboards", &gui_scene.billboards);     ImGui::SameLine();

    ImGui::Spacing();
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.1f, 3.0f);
}

#endif

