
#include "bobomb.hpp"
#include <string>
#include <cmath>
#include <algorithm>

#ifdef MAIN_SCENE

using namespace vcl;

const std::string bobomb_dir = "scenes/shared_assets/textures/bobomb/";
const float PI = 3.14159f;

// Generator for uniform random number
std::default_random_engine generatorb;
std::uniform_real_distribution<float> distribb(0.0, 1.0);

mesh mesh_primitive_boulon(float radius, float height, const vec3& p0)
{
    mesh shape;
    unsigned int N = 6;
    for (size_t ku = 0; ku < N; ++ku) {
            const float u = static_cast<float>(ku) / static_cast<float>(N);

            const float theta = static_cast<float>(2. * PI * u);

            const float x = radius * std::cos(theta);
            const float y = radius * std::sin(theta);
            const vec3 p = { std::cos(theta + PI/ static_cast<float>(N)), std::sin(theta + PI / static_cast<float>(N)) ,0 };

            shape.position.push_back(vec3{x, y, 0} + p0);
            shape.position.push_back(vec3{ x,y,height } + p0);
            shape.normal.push_back(p);
            shape.normal.push_back(p);
    }
    shape.position.push_back(vec3{ 0,0,height } + p0);
    shape.normal.push_back({ 0, 0, 1 });

    for (size_t k = 0; k < N; ++k) {
        unsigned int u00 = 2 * k;
        unsigned int u01 = 2 * k + 1;
        unsigned int u10 = (2 * (k + 1)) % (2 * N);
        unsigned int u11 = (2 * (k + 1) + 1) % (2 * N);

        shape.connectivity.push_back({ u00, u01, u10 });
        shape.connectivity.push_back({ u11, u10, u01 });
        shape.connectivity.push_back({ u11, u01, (unsigned int)(2 * N) });
    }
    
    return shape;
}

mesh mesh_pied_bobomb(int x_sym) {
    mesh shape;
    
    shape.position.push_back({ x_sym * 1.839133f, -6.181050, 0.771053 });
    shape.position.push_back({ x_sym * 3.808849f, 0.193048, 0.700534 });
    shape.position.push_back({ x_sym * 2.976701f, -0.280741, 3.174895 });
    shape.position.push_back({ x_sym * 1.731257f, 4.605570, -1.827734 });
    shape.position.push_back({ x_sym * -1.546808f, 4.606693, -1.686860 });
    shape.position.push_back({ x_sym * 2.968143f, 5.161891, 0.747666 });
    shape.position.push_back({ x_sym * -2.557761f, 5.163801, 0.985134 });
    shape.position.push_back({ x_sym * -1.270377f, -5.518005, -1.721171 });
    shape.position.push_back({ x_sym * -1.532564f, -6.179862, 0.915956 });
    shape.position.push_back({ x_sym * -2.859974f, 0.294817, -1.639984 });
    shape.position.push_back({ x_sym * -3.496593f, 0.195560, 1.014470 });
    shape.position.push_back({ x_sym * 1.352069f, -5.518919, -1.833870 });
    shape.position.push_back({ x_sym * 3.040564f, 0.292791, -1.893552 });
    shape.position.push_back({ x_sym * -1.407454f, 3.563577, 3.747142 });
    shape.position.push_back({ x_sym * 2.057952f, 3.562372, 3.598226 });
    shape.position.push_back({ x_sym * -2.455520f, -0.278860, 3.408339 });
    shape.connectivity.push_back({ 0,1,2 });
    shape.connectivity.push_back({ 3,4,5 });
    shape.connectivity.push_back({ 4,6,5 });
    shape.connectivity.push_back({ 7,8,9 });
    shape.connectivity.push_back({ 8,10,9 });
    shape.connectivity.push_back({ 11,7,12 });
    shape.connectivity.push_back({ 7,9,12 });
    shape.connectivity.push_back({ 13,14,6 });
    shape.connectivity.push_back({ 14,5,6 });
    shape.connectivity.push_back({ 7,11,8 });
    shape.connectivity.push_back({ 11,0,8 });
    shape.connectivity.push_back({ 8,15,10 });
    shape.connectivity.push_back({ 0,11,1 });
    shape.connectivity.push_back({ 11,12,1 });
    shape.connectivity.push_back({ 6,4,10 });
    shape.connectivity.push_back({ 4,9,10 });
    shape.connectivity.push_back({ 14,13,2 });
    shape.connectivity.push_back({ 13,15,2 });
    shape.connectivity.push_back({ 3,5,12 });
    shape.connectivity.push_back({ 5,1,12 });
    shape.connectivity.push_back({ 5,14,1 });
    shape.connectivity.push_back({ 14,2,1 });
    shape.connectivity.push_back({ 4,3,9 });
    shape.connectivity.push_back({ 3,12,9 });
    shape.connectivity.push_back({ 13,6,15 });
    shape.connectivity.push_back({ 6,10,15 });
    shape.connectivity.push_back({ 15,8,2 });
    shape.connectivity.push_back({ 8,0,2 });
    shape.normal.push_back({ x_sym * 0.5257f, -0.7915, 0.3117 });
    shape.normal.push_back({ x_sym * 0.9987f, -0.0463, 0.0191 });
    shape.normal.push_back({ x_sym * 0.6234f, -0.1714, 0.7628 });
    shape.normal.push_back({ x_sym * 0.3188f, 0.5824, -0.7478 });
    shape.normal.push_back({ x_sym * -0.4076f, 0.5729, -0.7111 });
    shape.normal.push_back({ x_sym * 0.6478f, 0.7590, 0.0645 });
    shape.normal.push_back({ x_sym * -0.6465f, 0.7565, 0.0979 });
    shape.normal.push_back({ x_sym * -0.4752f, -0.5625, -0.6765 });
    shape.normal.push_back({ x_sym * -0.5038f, -0.7886, 0.3525 });
    shape.normal.push_back({ x_sym * -0.6971f, 0.0076, -0.7169 });
    shape.normal.push_back({ x_sym * -0.9973f, -0.0272, 0.0684 });
    shape.normal.push_back({ x_sym * 0.4041f, -0.5680, -0.7170 });
    shape.normal.push_back({ x_sym * 0.6304f, -0.0211, -0.7760 });
    shape.normal.push_back({ x_sym * -0.3450f, 0.4193, 0.8397 });
    shape.normal.push_back({ x_sym * 0.4321f, 0.4122, 0.8021 });
    shape.normal.push_back({ x_sym * -0.5545f, -0.1713, 0.8144 });

    return shape;
}

mesh mesh_eyes(float radius, float angle, float height) {
    mesh shape;

    shape.position.push_back({ radius * std::cos(angle), -radius * std::sin(angle), height / 2.f });
    shape.position.push_back({ radius * std::cos(angle), -radius * std::sin(angle), -height / 2.f });
    shape.position.push_back({ radius, 0, height / 2.f });
    shape.position.push_back({ radius, 0, -height / 2.f });
    shape.position.push_back({ radius * std::cos(angle), radius * std::sin(angle), height / 2.f });
    shape.position.push_back({ radius * std::cos(angle), radius * std::sin(angle), -height / 2.f });
    shape.connectivity.push_back({ 0, 1, 3 });
    shape.connectivity.push_back({ 0, 2, 3 });
    shape.connectivity.push_back({ 2, 3, 5 });
    shape.connectivity.push_back({ 2, 4, 5 });
    shape.normal.push_back({ std::cos(angle), -std::sin(angle), 0 });
    shape.normal.push_back({ std::cos(angle), -std::sin(angle), 0 });
    shape.normal.push_back({ 1, 0, 0 });
    shape.normal.push_back({ 1, 0, 0 });
    shape.normal.push_back({ std::cos(angle), std::sin(angle), 0 });
    shape.normal.push_back({ std::cos(angle), std::sin(angle), 0 });
    shape.texture_uv.push_back({ 0, 1 });
    shape.texture_uv.push_back({ 0, 0 });
    shape.texture_uv.push_back({ .5, 1 });
    shape.texture_uv.push_back({ .5, 0 });
    shape.texture_uv.push_back({ 1, 1 });
    shape.texture_uv.push_back({ 1, 0 });

    return shape;
}

void bobomb_structure::init(const vec3& _center, map_structure* _map)
{
    if (radius_boulon) {
        std::cout << "Tentative de re-initialiser un chomp deja initialise." << std::endl;
        return;
    }

    map = _map;

    scaling = 0.3f;
    radius_boulon = scaling * 0.075f;
    height_boulon = scaling * 0.06f;
    cote_corps = scaling * 0.3f;
    height_pied = scaling * 0.060f;
    height_yeux = scaling * 0.1f;

    radius_reach = scaling * cote_corps * 20.0f;

    max_angular_velocity = PI / 4;
    max_speed = cote_corps / 3.0f;
    center = _center;
    rel_position = rush_speed = { 0,0,0 };
    angular_v = hspeed = vspeed = time_chasing = 0.0f;
    angle = 0.0f;
    rushing = exploding = disappear = falling = false;

    mesh_drawable corps = mesh_primitive_quad({ -cote_corps / 2.f, 0, 0 }, { cote_corps / 2.f, 0, 0 }, { cote_corps / 2.f , 0, cote_corps }, { -cote_corps / 2.f , 0, cote_corps });
    mesh_drawable boulon = mesh_primitive_boulon(radius_boulon, height_boulon, { 0, 0, 0 });
    mesh_drawable pied_gauche = mesh_pied_bobomb(1);
    mesh_drawable pied_droit = mesh_pied_bobomb(-1);
    mesh_drawable yeux = mesh_eyes(cote_corps / 2.f, PI / 6.f, height_yeux);

    yeux.uniform.shading = { 0, 1, 0 };
    pied_droit.uniform.shading = { 0, 1, 0 };
    pied_gauche.uniform.shading = { 0, 1, 0 };
    boulon.uniform.shading = { 0, 1, 0 };
    corps.uniform.shading = { 0, 1, 0 };

    pied_gauche.uniform.transform.scaling = scaling * 0.013f;
    pied_gauche.uniform.transform.rotation = rotation_from_axis_angle_mat3({0, 0, 1}, PI);
    pied_gauche.uniform.transform.translation.y = scaling * -0.05f;

    pied_droit.uniform.transform.scaling = pied_gauche.uniform.transform.scaling;
    pied_droit.uniform.transform.rotation = pied_gauche.uniform.transform.rotation;
    pied_droit.uniform.transform.translation = pied_gauche.uniform.transform.translation;

    yeux.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 0, 0, 1 }, -PI / 2.f);

    corps.uniform.transform.translation.z = height_pied;

    texture_corps = create_texture_gpu(image_load_png(bobomb_dir + "corps.png"));
    texture_yeux = create_texture_gpu(image_load_png(bobomb_dir + "yeux.png"));
    couleur_pied = { 1.f, 0.8f, 0.f };
    couleur_boulon = { .8f, .8f, .8f };

    centre_corps = { 0, 0, height_pied + cote_corps / 2.f };

    hierarchy.add(mesh_drawable{}, "Super_Global");
    hierarchy.add(mesh_drawable{}, "Global", "Super_Global");
    hierarchy.add(corps, "Corps", "Super_Global");
    hierarchy.add(boulon, "Boulon", "Global", { 0, 0, cote_corps - height_boulon / 2.f + height_pied });
    hierarchy.add(pied_gauche, "Pied_Gauche", "Global", { scaling * 0.070f, 0, scaling * 0.021f});
    hierarchy.add(pied_droit, "Pied_Droit", "Global", { scaling * -0.070f, 0, scaling * 0.021f });
    hierarchy.add(yeux, "Yeux", "Global", {0, 0, height_yeux / 2.f + cote_corps / 2.f });

    hierarchy["Pied_Gauche"].element.uniform.color = couleur_pied;
    hierarchy["Pied_Droit"].element.uniform.color = couleur_pied;
    hierarchy["Boulon"].element.uniform.color = couleur_boulon;
}

void bobomb_structure::draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf)
{
    if (disappear) return;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (surf) draw_hierarchy_element(hierarchy["Pied_Droit"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["Pied_Droit"], scene.camera, shaders["wireframe"]);
    if (surf) draw_hierarchy_element(hierarchy["Pied_Gauche"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["Pied_Gauche"], scene.camera, shaders["wireframe"]);
    if (surf) draw_hierarchy_element(hierarchy["Boulon"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["Boulon"], scene.camera, shaders["wireframe"]);

    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void bobomb_structure::draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf)
{
    if (disappear) return;

    int index;
    mat3 R;

    R = scene.camera.orientation * rotation_from_axis_angle_mat3({ 1, 0, 0 }, -PI / 2.);
    hierarchy["Corps"].transform.rotation = R;
    hierarchy["Corps"].transform.translation = centre_corps - R * centre_corps;

    glBindTexture(GL_TEXTURE_2D, texture_corps);
    if (bb) draw_hierarchy_element(hierarchy["Corps"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["Corps"], scene.camera, shaders["wireframe"]);

    glBindTexture(GL_TEXTURE_2D, texture_yeux);
    if (bb) draw_hierarchy_element(hierarchy["Yeux"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["Yeux"], scene.camera, shaders["wireframe"]);
}

void bobomb_structure::move(const vcl::vec3& char_pos, float t, float dt)
{
    if (disappear) return;
    if (dt > 0.1f) dt = 0.1f;

    float w = .0f;
    float ampl = .0f;

    if (rushing) {
        w = 8 * PI;
        ampl = .4f;
        time_chasing += dt;
        rush_speed = normalize(char_pos - (center + rel_position) ) * 7 * max_speed;
        float da = atan2(char_pos.y - (center + rel_position).y, char_pos.x - (center + rel_position).x) - angle;
        angle += da;
        rel_position += dt* vec3{ rush_speed.x, rush_speed.y, 0 };
        if (time_chasing > 50.f) {
            rushing = false;
            exploding = true;
            time_chasing = 0.0f;
            hspeed = 0.0f;
        }
    }
    else if (exploding) {
        hierarchy["Super_Global"].transform.scaling = 1.f + .5f * time_chasing / .3f;
        time_chasing += dt;
        if (time_chasing > .3f) {
            exploding = false;
            disappear = true;
        }
    }
    else if (norm(char_pos - (center + rel_position)) < radius_reach) { // Target before attack
        w = 2 * PI;
        ampl = .2f;
        time_chasing += dt;
        float da = atan2(char_pos.y - (center + rel_position).y, char_pos.x - (center + rel_position).x) - angle;
        angle += da;
        rel_position += dt * vec3{ hspeed * cos(angle), hspeed * sin(angle), 0 };
        if (time_chasing > .01f) {
            rushing = true;
            time_chasing = 0.0f;
        }
    }
    else { // Random movement on the plane
        w = 2 * PI;
        ampl = .2f;
        time_chasing = 0.0f;
        if (distribb(generatorb) < 0.01f) {
            if (distribb(generatorb) * 3 < 1) angular_v = max_angular_velocity;
            else if (distribb(generatorb) * 2 < 1) angular_v = -max_angular_velocity;
            else angular_v = 0;
        }
        angle += angular_v * dt;
        if (distribb(generatorb) < 0.05f) {
            if (distribb(generatorb) * 4 < 3) hspeed = max_speed;
            else hspeed = 0;
        }
        rel_position += dt * vec3{ hspeed * cos(angle), hspeed * sin(angle), 0 };
    }

    float z_floor = map->get_z(center + rel_position);
    vec3 impact, normal;

    if (map->collision_sphere(center + rel_position + centre_corps, cote_corps / 2.f, impact, normal) && dot(center + rel_position + centre_corps - cote_corps/2.f * normal - impact, normal) < 0.f)
        rel_position -= dot(center + rel_position + centre_corps - cote_corps / 2.f * normal -  impact, normal) * normal;

    if ((center + rel_position).z - z_floor >= -0.1f) {
        if (falling && (center + rel_position).z - z_floor <= 0.01f) {
            falling = false;
            vspeed = 0.f;
        }

        if (!falling && (center + rel_position).z - z_floor <= .1f)
            rel_position.z = z_floor - center.z;
        else {
            vspeed += -2.5f * dt;
            rel_position.z += vspeed * dt;
            rel_position.z = std::max(rel_position.z, z_floor - center.z);
            falling = true;
        }
    }

    mat3 R_pied_droit = rotation_from_axis_angle_mat3({ 1, 0, 0 }, ampl * std::sin(w * t));
    mat3 R_pied_gauche = rotation_from_axis_angle_mat3({ 1, 0, 0 }, ampl * std::sin(w * t - PI));

    hierarchy["Pied_Droit"].transform.rotation = R_pied_droit;
    hierarchy["Pied_Gauche"].transform.rotation = R_pied_gauche;
    hierarchy["Global"].transform.rotation = rotation_from_axis_angle_mat3({ 0,0,1 }, angle + PI / 2);
    hierarchy["Super_Global"].transform.translation = center + rel_position;
    hierarchy.update_local_to_global_coordinates();
}

vcl::vec3 bobomb_structure::get_position() {
    return center + rel_position;
}

#endif