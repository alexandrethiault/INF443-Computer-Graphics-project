
#include "bobomb.hpp"
#include <string>
#include <cmath>
#include <fstream>
#include <algorithm>

#ifdef MAIN_SCENE

using namespace vcl;

const std::string bobomb_dir = "scenes/shared_assets/textures/bobomb/";
const float PI = 3.14159f;

// Generator for uniform random number
std::default_random_engine generatorb;
std::uniform_real_distribution<float> distribb(0.0, 1.0);

/// GEOMETRY

mesh mesh_primitive_boulon(float radius, float height, const vec3& p0)
{
    mesh shape;
    size_t N = 6;
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

    unsigned int Ns = static_cast<unsigned int>(N);
    for (unsigned int k = 0; k < Ns; ++k) {
        unsigned int u00 = 2 * k;
        unsigned int u01 = 2 * k + 1;
        unsigned int u10 = (2 * (k + 1)) % (2 * Ns);
        unsigned int u11 = (2 * (k + 1) + 1) % (2 * Ns);

        shape.connectivity.push_back({ u00, u01, u10 });
        shape.connectivity.push_back({ u11, u10, u01 });
        shape.connectivity.push_back({ u11, u01, 2 * Ns });
    }
    
    return shape;
}

mesh mesh_pied_bobomb(int x_sym) {
    mesh shape;
    
    shape.position.push_back({ x_sym * 1.839133f, -6.181050f, 0.771053f });
    shape.position.push_back({ x_sym * 3.808849f, 0.193048f, 0.700534f });
    shape.position.push_back({ x_sym * 2.976701f, -0.280741f, 3.174895f });
    shape.position.push_back({ x_sym * 1.731257f, 4.605570f, -1.827734f });
    shape.position.push_back({ x_sym * -1.546808f, 4.606693f, -1.686860f });
    shape.position.push_back({ x_sym * 2.968143f, 5.161891f, 0.747666f });
    shape.position.push_back({ x_sym * -2.557761f, 5.163801f, 0.985134f });
    shape.position.push_back({ x_sym * -1.270377f, -5.518005f, -1.721171f });
    shape.position.push_back({ x_sym * -1.532564f, -6.179862f, 0.915956f });
    shape.position.push_back({ x_sym * -2.859974f, 0.294817f, -1.639984f });
    shape.position.push_back({ x_sym * -3.496593f, 0.195560f, 1.014470f });
    shape.position.push_back({ x_sym * 1.352069f, -5.518919f, -1.833870f });
    shape.position.push_back({ x_sym * 3.040564f, 0.292791f, -1.893552f });
    shape.position.push_back({ x_sym * -1.407454f, 3.563577f, 3.747142f });
    shape.position.push_back({ x_sym * 2.057952f, 3.562372f, 3.598226f });
    shape.position.push_back({ x_sym * -2.455520f, -0.278860f, 3.408339f });
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
    shape.normal.push_back({ x_sym * 0.5257f, -0.7915f, 0.3117f });
    shape.normal.push_back({ x_sym * 0.9987f, -0.0463f, 0.0191f });
    shape.normal.push_back({ x_sym * 0.6234f, -0.1714f, 0.7628f });
    shape.normal.push_back({ x_sym * 0.3188f, 0.5824f, -0.7478f });
    shape.normal.push_back({ x_sym * -0.4076f, 0.5729f, -0.7111f });
    shape.normal.push_back({ x_sym * 0.6478f, 0.7590f, 0.0645f });
    shape.normal.push_back({ x_sym * -0.6465f, 0.7565f, 0.0979f });
    shape.normal.push_back({ x_sym * -0.4752f, -0.5625f, -0.6765f });
    shape.normal.push_back({ x_sym * -0.5038f, -0.7886f, 0.3525f });
    shape.normal.push_back({ x_sym * -0.6971f, 0.0076f, -0.7169f });
    shape.normal.push_back({ x_sym * -0.9973f, -0.0272f, 0.0684f });
    shape.normal.push_back({ x_sym * 0.4041f, -0.5680f, -0.7170f });
    shape.normal.push_back({ x_sym * 0.6304f, -0.0211f, -0.7760f });
    shape.normal.push_back({ x_sym * -0.3450f, 0.4193f, 0.8397f });
    shape.normal.push_back({ x_sym * 0.4321f, 0.4122f, 0.8021f });
    shape.normal.push_back({ x_sym * -0.5545f, -0.1713f, 0.8144f });

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

// INITIALIZATION

void bobombs_structure::setup(map_structure* _map, bridge_structure* _bridge, vec3 pink_position)
{
    map = _map;
    bridge = _bridge;

    scaling = 0.4f;
    radius_boulon = scaling * 0.075f;
    height_boulon = scaling * 0.06f;
    cote_corps = scaling * 0.3f;
    height_pied = scaling * 0.060f;
    height_yeux = scaling * 0.1f;
    temps_explode = 5.f;
    temps_wait = 15.f;
    temps_fire = .5f;

    radius_reach = scaling * cote_corps * 20.0f;

    max_angular_velocity = PI / 4;
    max_speed = 7.f * cote_corps / 3.0f;

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
    pied_gauche.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 0, 0, 1 }, PI);
    pied_gauche.uniform.transform.translation.y = scaling * -0.05f;

    pied_droit.uniform.transform.scaling = pied_gauche.uniform.transform.scaling;
    pied_droit.uniform.transform.rotation = pied_gauche.uniform.transform.rotation;
    pied_droit.uniform.transform.translation = pied_gauche.uniform.transform.translation;

    yeux.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 0, 0, 1 }, -PI / 2.f);

    corps.uniform.transform.translation.z = height_pied;

    texture_corps_rose = create_texture_gpu(image_load_png(bobomb_dir + "pink_corps.png"));
    texture_corps = create_texture_gpu(image_load_png(bobomb_dir + "corps.png"));
    texture_yeux = create_texture_gpu(image_load_png(bobomb_dir + "yeux.png"));
    texture_fumee = create_texture_gpu(image_load_png(bobomb_dir + "fumee.png"));
    textures_explosion = new GLuint[frame_explosion];
    for(int i = 0; i < frame_explosion; i++)
        textures_explosion[i] = create_texture_gpu(image_load_png(bobomb_dir + "explosion_" + to_string(i) + ".png"));

    couleur_pied = { 1.f, 0.8f, 0.f };
    couleur_boulon = { .8f, .8f, .8f };

    centre_corps = { 0, 0, height_pied + cote_corps / 2.f };

    hierarchy.add(mesh_drawable{}, "Super_Global");
    hierarchy.add(mesh_drawable{}, "Global", "Super_Global");
    hierarchy.add(corps, "Corps", "Super_Global");
    hierarchy.add(boulon, "Boulon", "Global", { 0, 0, cote_corps - height_boulon / 2.f + height_pied });
    hierarchy.add(pied_gauche, "Pied_Gauche", "Global", { scaling * 0.070f, 0, scaling * 0.021f });
    hierarchy.add(pied_droit, "Pied_Droit", "Global", { scaling * -0.070f, 0, scaling * 0.021f });
    hierarchy.add(yeux, "Yeux", "Global", { 0, 0, height_yeux / 2.f + cote_corps / 2.f });

    hierarchy["Pied_Gauche"].element.uniform.color = couleur_pied;
    hierarchy["Pied_Droit"].element.uniform.color = couleur_pied;
    hierarchy["Boulon"].element.uniform.color = couleur_boulon;

    fumee = mesh_primitive_quad({ -0.5f, 0.f, 0.5f }, { 0.5f, 0.f, 0.5f }, { 0.5f, 0.f, -0.5f }, { -0.5f, 0.f, -0.5f });
    explosion = mesh_primitive_quad({ -1.f, 0.f, 1.f }, { 1.f, 0.f, 1.f }, { 1.f, 0.f, 0.f }, { -1.f, 0.f, 0.f });

    std::fstream bobombs_pos("scenes/shared_assets/coords/bobomb.txt");
    int n; bobombs_pos >> n;
    bobombs.resize(n);
    vec3 ipos;
    for (int i = 0; i < n; i++) {
        bobombs_pos >> ipos.x >> ipos.y >> ipos.z;
        bobombs[i].init(ipos, this, false);
    }
    pbobomb.init(pink_position, this, true);
}

void bobomb_structure::init(const vec3& _center, bobombs_structure* bobombs, bool is_pink)
{
    if (radius_boulon) {
        std::cout << "Tentative de re-initialiser une bobomb deja initialisee." << std::endl;
        return;
    }

    map = bobombs->map;
    bridge = bobombs->bridge;
    //hierarchy = &(bobombs->hierarchy);

    scaling = bobombs->scaling;
    radius_boulon = bobombs->radius_boulon;
    height_boulon = bobombs->height_boulon;
    cote_corps = bobombs->cote_corps;
    height_pied = bobombs->height_pied;
    height_yeux = bobombs->height_yeux;
    temps_explode = bobombs->temps_explode;
    temps_wait = bobombs->temps_wait;
    temps_fire = bobombs->temps_fire;
    radius_reach = bobombs->radius_reach;
    max_angular_velocity = bobombs->max_angular_velocity;
    max_speed = bobombs->max_speed;
    centre_corps = bobombs->centre_corps;

    center = original_pos = _center;
    rel_position = rush_speed = { 0,0,0 };

    this->is_pink = is_pink;
}

vcl::vec3 bobomb_structure::get_position() {
    return center + rel_position;
}

// MOVEMENTS / INTERACTION

void bobomb_structure::move_black(const vcl::vec3& char_pos, float t, float dt)
{
    if (dt > 0.1f) dt = 0.1f;

    if (wait) {
        time_chasing += dt;
        wait = time_chasing <= temps_wait;
        return;
    }

    if (norm(char_pos - (center + rel_position)) >= 2.f)
        hide = true;
    if (norm(char_pos - (center + rel_position)) < 1.9f) // hysteresis to avoid flickering between hidden and visible
        hide = false;

    if (!rushing && !exploding && hide) return; // Don't hide the bomb if it's about to explode

    if (rushing) { // Rush towards Mario
        w = 8 * PI;
        ampl = .4f;
        time_chasing += dt;
        rush_speed = normalize(char_pos - (center + rel_position) ) * max_speed;
        float da = atan2(char_pos.y - (center + rel_position).y, char_pos.x - (center + rel_position).x) - angle;
        angle += da;
        rel_position += dt* vec3{ rush_speed.x, rush_speed.y, 0 };
        if (time_chasing > temps_explode) {
            rushing = false;
            exploding = true;
            time_chasing = 0.0f;
            hspeed = 0.0f;
        }
    }
    else if (exploding) { // The bomb inflates before exploding
        time_chasing += dt;
        if (time_chasing > .3f) {
            exploding = false;
            fire = true;
            time_chasing = 0.f;
        }
    }
    else if (fire) { // explosion
        time_chasing += dt;
        if (time_chasing >= temps_fire) {
            fire = false;
            wait = true;
            time_chasing = 0.f;
            center = original_pos;
            rel_position = { 0, 0, 0 };
        }
    }
    else if (norm(char_pos - (center + rel_position)) < radius_reach) { // Turn around to face Mario
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
    else { // Random movement on the map
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
            if (distribb(generatorb) * 4 < 3) hspeed = max_speed / 7.f;
            else hspeed = 0;
        }
        rel_position += dt * vec3{ hspeed * cos(angle), hspeed * sin(angle), 0 };
    }

    //float z_floor = map->get_z(center + rel_position);
    vec3 impact, normal;

    if (map->ground_collision(center + rel_position, impact, normal)) {
        rel_position = impact - center;
        vspeed = 0.f;
    }
    else if (bridge->ground_collision(center + rel_position, impact, normal)) {
        rel_position = impact - center;
        vspeed = 0.f;
    }
    else {
        vspeed += -2.5f * dt;
        rel_position.z += vspeed * dt;
    }
    if (map->wall_collision(center + rel_position + centre_corps, impact, normal, cote_corps / 2.f))
        rel_position = impact + normal * cote_corps / 1.999f - (center + centre_corps);
    if (map->wall_collision(center + rel_position + centre_corps, impact, normal, cote_corps / 2.f))
        rel_position = impact + normal * cote_corps / 2.f - (center + centre_corps);
}

void bobomb_structure::move_pink(float t, float dt)
{
    assert(is_pink);
    if (dt > 0.1f) dt = 0.1f;

    angle += angular_v * dt;
    rel_position += dt * vec3{ hspeed * std::cos(angle), hspeed * std::sin(angle), vspeed };

    vec3 impact, normal;

    if (map->ceiling_collision(center + rel_position + centre_corps, impact, normal, cote_corps / 2.f)) {
        rel_position = impact + normal * cote_corps / 2.f - (center + centre_corps);
        vspeed = -0.025f;
    }
    else if (map->ground_collision(center + rel_position, impact, normal) &&
        ((center + rel_position + centre_corps).z - impact.z >= 0 || vspeed < -1.f)) {
        rel_position = impact - center;
        vspeed = 0.f;
    }
    else if (bridge->ground_collision(center + rel_position, impact, normal)) {
        rel_position = impact - center;
        vspeed = 0.f;
    }
    else {
        const float g = -2.5f;
        const float m = 0.01f;
        const float f = 0.004f;
        const float F = m * g - f * vspeed;
        vspeed += dt * F / m;
        rel_position.z += dt * vspeed;
    }

    if (map->wall_collision(center + rel_position + centre_corps, impact, normal, cote_corps / 2.f))
        rel_position = impact + normal * cote_corps / 1.999f - (center + centre_corps);
    if (map->wall_collision(center + rel_position + centre_corps, impact, normal, cote_corps / 2.f))
        rel_position = impact + normal * cote_corps / 2.f - (center + centre_corps);
}

void bobomb_structure::keyboard_input(bool Z, bool W, bool D, bool S, bool A, bool Q, bool SPACE)
{
    hspeed = 0.f;
    angular_v = 0.f;
    w = 0.f;
    ampl = .0f;

    if (Z || W) {
        hspeed = 2 * max_speed;
        w = 8 * PI;
        ampl = .4f;
    }
    if (A || Q)
        angular_v = (S ? -1 : 1) * 3 * max_angular_velocity;
    if (D)
        angular_v = (S ? 1 : -1) * 3 * max_angular_velocity;
    if (S) {
        hspeed = -2 * max_speed;
        w = 8 * PI;
        ampl = .4f;
    }
    if (SPACE) {
        vspeed = 3 * max_speed;
        w = 16 * PI;
        ampl = .1f;
    }
}

void bobombs_structure::move(const vcl::vec3& char_pos, float t, float dt) {
    this->t = t;
    for (auto i = bobombs.begin(); i != bobombs.end(); i++)
        i->move_black(char_pos, t, dt);
    pbobomb.move_pink(t, dt);
}

/// DRAWING

bool cmpbillboard(vec3& u, vec3& v, vec3& cam_pos) {
    return (norm(u - cam_pos) > norm(v - cam_pos));
}

void bobombs_structure::draw_part_nogl(std::string name, std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surfbb, bool wf) {
    if (surfbb) draw_hierarchy_element(hierarchy[name], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy[name], scene.camera, shaders["wireframe"]);
}

void bobombs_structure::draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf) {

    // Black bobombs

    for (auto i = bobombs.begin(); i != bobombs.end(); i++) {
        if ((!i->rushing && !i->exploding && i->hide) || i->wait || i->fire) continue;
        
        mat3 R_pied_droit = rotation_from_axis_angle_mat3({ 1, 0, 0 }, i->ampl * std::sin(i->w * t));
        mat3 R_pied_gauche = rotation_from_axis_angle_mat3({ 1, 0, 0 }, i->ampl * std::sin(i->w * t - PI));
        hierarchy["Super_Global"].transform.scaling = (i->exploding) ? 1.f + .5f * i->time_chasing / .3f : 1.f;
        hierarchy["Pied_Droit"].transform.rotation = R_pied_droit;
        hierarchy["Pied_Gauche"].transform.rotation = R_pied_gauche;
        hierarchy["Global"].transform.rotation = rotation_from_axis_angle_mat3({ 0,0,1 }, i->angle + PI / 2);
        hierarchy["Super_Global"].transform.translation = i->center + i->rel_position;
        hierarchy.update_local_to_global_coordinates();

        draw_part_nogl("Boulon", shaders, scene, surf, wf);
        draw_part_nogl("Pied_Gauche", shaders, scene, surf, wf);
        draw_part_nogl("Pied_Droit", shaders, scene, surf, wf);
    }

    // Pink bobomb

    bobomb_structure* i = &pbobomb;

    mat3 R_pied_droit = rotation_from_axis_angle_mat3({ 1, 0, 0 }, i->ampl * std::sin(i->w * t));
    mat3 R_pied_gauche = rotation_from_axis_angle_mat3({ 1, 0, 0 }, i->ampl * std::sin(i->w * t - PI));

    hierarchy["Super_Global"].transform.scaling = 1.f;
    hierarchy["Pied_Droit"].transform.rotation = R_pied_droit;
    hierarchy["Pied_Gauche"].transform.rotation = R_pied_gauche;
    hierarchy["Global"].transform.rotation = rotation_from_axis_angle_mat3({ 0,0,1 }, i->angle + PI / 2);
    hierarchy["Super_Global"].transform.translation = i->center + i->rel_position;
    hierarchy.update_local_to_global_coordinates();

    draw_part_nogl("Boulon", shaders, scene, surf, wf);
    draw_part_nogl("Pied_Gauche", shaders, scene, surf, wf);
    draw_part_nogl("Pied_Droit", shaders, scene, surf, wf);

    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void bobombs_structure::draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf)
{
    // Black bobombs

    mat3 R = scene.camera.orientation * rotation_from_axis_angle_mat3({ 1, 0, 0 }, -PI / 2.);

    glBindTexture(GL_TEXTURE_2D, texture_corps);

    for (auto i = bobombs.begin(); i != bobombs.end(); i++) {
        if ((!i->rushing && !i->exploding && i->hide) || i->wait || i->fire) continue;

        hierarchy["Super_Global"].transform.scaling = (i->exploding) ? 1.f + .5f * i->time_chasing / .3f : 1.f;
        hierarchy["Global"].transform.rotation = rotation_from_axis_angle_mat3({ 0,0,1 }, i->angle + PI / 2);
        hierarchy["Super_Global"].transform.translation = i->center + i->rel_position;
        hierarchy["Corps"].transform.rotation = R;
        hierarchy["Corps"].transform.translation = i->centre_corps - R * i->centre_corps;
        hierarchy.update_local_to_global_coordinates();

        draw_part_nogl("Corps", shaders, scene, bb, wf);
    }

    glBindTexture(GL_TEXTURE_2D, texture_fumee);

    for (auto i = bobombs.begin(); i != bobombs.end(); i++) {
        if (!i->rushing) continue;

        fumee.uniform.transform.scaling = i->radius_boulon * ( 1 + 2 * (10 * i->time_chasing / temps_explode - floor(10 * i->time_chasing / temps_explode)));
        fumee.uniform.transform.rotation = R * rotation_from_axis_angle_mat3({ 0, 1, 0 }, 24 * PI * i->time_chasing / temps_explode);
        fumee.uniform.transform.translation = i->get_position();
        fumee.uniform.transform.translation.z += height_pied + cote_corps + height_boulon + (10 * i->time_chasing / temps_explode - floor(10 * i->time_chasing / temps_explode)) * .05f;

        if (bb) draw(fumee, scene.camera, shaders["mesh"]);
        if (wf) draw(fumee, scene.camera, shaders["wireframe"]);
    }

    int frame;

    for (auto i = bobombs.begin(); i != bobombs.end(); i++) {
        if (!i->fire) continue;

        frame = floor(i->time_chasing / temps_fire * frame_explosion);

        glBindTexture(GL_TEXTURE_2D, textures_explosion[frame]);

        vec3 dpos = scene.camera.camera_position() - i->get_position();
        explosion.uniform.transform.scaling = 3 * i->cote_corps;
        explosion.uniform.transform.rotation = rotation_from_axis_angle_mat3({ 0, 0, 1 }, PI) * rotation_from_axis_angle_mat3({ 0,0,1 }, atan2(dpos.y, dpos.x)) * mat3 { 0, 0, 1, 1, 0, 0, 0, 1, 0 } * rotation_from_axis_angle_mat3({ 1, 0, 0 }, -PI / 2.);
        explosion.uniform.transform.translation = i->get_position();

        if (bb) draw(explosion, scene.camera, shaders["mesh"]);
        if (wf) draw(explosion, scene.camera, shaders["wireframe"]);
    }

    glBindTexture(GL_TEXTURE_2D, texture_yeux);

    for (auto i = bobombs.begin(); i != bobombs.end(); i++) {
        if ((!i->rushing && !i->exploding && i->hide) || i->wait || i->fire) continue;

        hierarchy["Super_Global"].transform.scaling = (i->exploding) ? 1.f + .5f * i->time_chasing / .3f : 1.f;
        hierarchy["Global"].transform.rotation = rotation_from_axis_angle_mat3({ 0,0,1 }, i->angle + PI / 2);
        hierarchy["Super_Global"].transform.translation = i->center + i->rel_position;
        hierarchy.update_local_to_global_coordinates();

        if (!cmpbillboard(i->center + i->rel_position + vec3{ i->cote_corps / 2.f * std::cos(i->angle), i->cote_corps / 2.f * std::sin(i->angle), i->height_yeux / 2.f + i->cote_corps / 2.f }, i->center + i->rel_position + i->centre_corps, scene.camera.camera_position()))
            draw_part_nogl("Yeux", shaders, scene, bb, wf);
    }

    // Pink bobomb

    bobomb_structure* i = &pbobomb;

    glBindTexture(GL_TEXTURE_2D, texture_corps_rose);

    hierarchy["Super_Global"].transform.scaling = 1.f;
    hierarchy["Global"].transform.rotation = rotation_from_axis_angle_mat3({ 0,0,1 }, i->angle + PI / 2);
    hierarchy["Super_Global"].transform.translation = i->center + i->rel_position;
    hierarchy["Corps"].transform.rotation = R;
    hierarchy["Corps"].transform.translation = i->centre_corps - R * i->centre_corps;
    hierarchy.update_local_to_global_coordinates();

    draw_part_nogl("Corps", shaders, scene, bb, wf);

    if (!cmpbillboard(i->center + i->rel_position + vec3{ i->cote_corps / 2.f * std::cos(i->angle), i->cote_corps / 2.f * std::sin(i->angle), i->height_yeux / 2.f + i->cote_corps / 2.f }, i->center + i->rel_position + i->centre_corps, scene.camera.camera_position())) {
        glBindTexture(GL_TEXTURE_2D, texture_yeux);
        draw_part_nogl("Yeux", shaders, scene, bb, wf);
    }
}


#endif