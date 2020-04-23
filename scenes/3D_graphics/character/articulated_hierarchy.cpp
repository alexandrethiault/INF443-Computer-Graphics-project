
#include "articulated_hierarchy.hpp"
#include <string>
#include <algorithm>

// AVANCEMENT : juste un squelette pour avoir un code qui suit vaguement la forme des chomp_xxx.cpp

#ifdef SCENE_ARTICULATED_HIERARCHY

using namespace vcl;

const std::string mario_dir = "scenes/shared_assets/textures/mario/";
const float PI = 3.14159f;

void scene_model::setup_data(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 30.0f;
    scene.camera.apply_rotation(0, 0, 0, 1.2f);

    demo_ground = mesh_primitive_disc(20);
    demo_ground.uniform.color = { 1,1,0.5f };

    character.init({ 0,0,0 });
    character.hierarchy.set_shader_for_all_elements(shaders["mesh"]);

    timer.scale = 1.0f;
    timer.t_max = 10.0f;
}

void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    set_gui();
    timer.update();
    const float t = timer.t;

    draw(demo_ground, scene.camera, shaders["mesh"]);

    character.move(t, ((t < last_t) ? timer.t_max - timer.t_min : 0) + t - last_t);

    character.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe);

    //// BILLBOARDS ALWAYS LAST ////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(false);
    character.draw_billboards(shaders, scene, gui_scene.billboards, gui_scene.wireframe);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);

    last_t = t;
}

void character_structure::init(const vec3& center)
{
    // initialisation de tous les champs de character_structure, notamment construire la hiérarchie et load les textures
    // par convention, z est la verticale et à l'initialisation on fait regarder vers x 
    // le personnage doit avoir une taille cohérente avec le rayon du chomp qui est 3.
    if (false) {
        std::cout << "Tentative de re-initialiser un mario deja initialise." << std::endl;
        return;
    }

    texture_exemple = create_texture_gpu(image_load_png(mario_dir + "exemple.png"));
}

void character_structure::draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf)
{
    // dessiner toutes les parties qui ne sont pas des billboards
    // copié collé de ce que j'ai fait pour le chomp

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

bool cmpbillboard(vec3& u, vec3& v, vec3& cam_pos)
{
    float norm_u = norm(u - cam_pos);
    float norm_v = norm(v - cam_pos);
    return (norm_u > norm_v);
}

void character_structure::draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf)
{
    // Cette fonction est à part parce que les billboards doivent être tracés à la fin
    // copié collé de ce que j'ai fait pour le chomp

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glBindTexture(GL_TEXTURE_2D, texture_eye);
    if (bb) draw_hierarchy_element(hierarchy["eye_left"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["eye_left"], scene.camera, shaders["wireframe"]);
    if (bb) draw_hierarchy_element(hierarchy["eye_right"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["eye_right"], scene.camera, shaders["wireframe"]);

    glBindTexture(GL_TEXTURE_2D, texture_chain);
    buffer<vec3> chains = { chain1, chain2, chain3, chain4 }; // à trier par distance à la caméra
    std::sort(chains.begin(), chains.end(),
        [&scene](auto u, auto v) -> bool {return cmpbillboard(u, v, scene.camera.camera_position()); });
    chain.uniform.transform.rotation = scene.camera.orientation;
    for (vec3 pos : chains) {
        chain.uniform.transform.translation = pos + vec3{ 0,0,1 };
        if (bb) draw(chain, scene.camera, shaders["mesh"]);
        if (wf) draw(chain, scene.camera, shaders["wireframe"]);
    }
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void character_structure::move(float t, float dt)
{
    // fonction où on décide du mouvement (en fonction de t, dt, input ?)
    // peut-être à faire seulement dans interpolation_position.cpp
    if (dt > 0.1f) dt = 0.1f;

    hierarchy.update_local_to_global_coordinates();
}

void scene_model::set_gui()
{
    ImGui::Text("Display: "); ImGui::SameLine();
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface);     ImGui::SameLine();
    ImGui::Checkbox("Skeleton", &gui_scene.skeleton);   ImGui::SameLine();

    ImGui::Spacing();
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.1f, 3.0f);

}

#endif

