
#include "articulated_hierarchy.hpp"
#include <string>
#include <algorithm>

// AVANCEMENT : juste un squelette pour avoir un code qui suit vaguement la forme des chomp_xxx.cpp

#ifdef SCENE_ARTICULATED_HIERARCHY

using namespace vcl;

const std::string map_dir = "scenes/shared_assets/models/Mario/";
const std::string obj_mario = "Super Mario 64 - Mario";
const std::string mario_dir = "scenes/shared_assets/textures/mario/";
const float PI = 3.14159f;

void loadMTL(const char* path, std::vector<mtltexture>& ans, std::vector<GLuint>& map_textures);
void loadOBJ(const char* path, std::vector<mesh_drawable>& obj, std::vector<int>& text_indices, const std::vector<mtltexture>& mtl);

void scene_model::setup_data(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 30.0f;
    scene.camera.apply_rotation(0, 0, 0, 1.2f);

    demo_ground = mesh_primitive_disc(20);
    demo_ground.uniform.color = { 1,1,0.5f };
    demo_ground.uniform.shading = { 1,0,0 };

    //character.init({ 0,0,0 });
    //character.hierarchy.set_shader_for_all_elements(shaders["mesh"]);
    loadMTL("scenes/shared_assets/models/Mario/Super Mario 64 - Mario.mtl", map_mtl, map_textures);
    loadOBJ("scenes/shared_assets/models/Mario/Super Mario 64 - Mario.obj", map, texture_indices, map_mtl);

    timer.scale = 1.0f;
    timer.t_max = 10.0f;
}

void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    set_gui();
    timer.update();
    const float t = timer.t;

    glEnable(GL_POLYGON_OFFSET_FILL);

    draw(demo_ground, scene.camera, shaders["mesh"]);

    //character.move(t, ((t < last_t) ? timer.t_max - timer.t_min : 0) + t - last_t);

    //character.draw_nobillboards(shaders, scene, gui_scene.surface, gui_scene.wireframe);

    // Bob omb battlefield map
    for (int i = 0; i < (int)map.size(); i++) { // 0 and 1 are billboards
        glBindTexture(GL_TEXTURE_2D, map_textures[texture_indices[i]]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        map[i].uniform.transform.scaling = .01f;
        if (gui_scene.surface) draw(map[i], scene.camera, shaders["mesh"]);
        if (gui_scene.wireframe) draw(map[i], scene.camera, shaders["wireframe"]);
    }

    //// BILLBOARDS ALWAYS LAST ////

    /*glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(false);
    character.draw_billboards(shaders, scene, gui_scene.billboards, gui_scene.wireframe);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);*/

    last_t = t;
}

void character_structure::init(const vec3& center)
{
    // initialisation de tous les champs de character_structure, notamment construire la hi�rarchie et load les textures
    // par convention, z est la verticale et � l'initialisation on fait regarder vers x 
    // le personnage doit avoir une taille coh�rente avec le rayon du chomp qui est 3.
    if (false) {
        std::cout << "Tentative de re-initialiser un mario deja initialise." << std::endl;
        return;
    }

    // texture_exemple = create_texture_gpu(image_load_png(mario_dir + "exemple.png"));
}

void character_structure::draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf)
{
    // dessiner toutes les parties qui ne sont pas des billboards
    // copi� coll� de ce que j'ai fait pour le chomp

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
    // Cette fonction est � part parce que les billboards doivent �tre trac�s � la fin
    // copi� coll� de ce que j'ai fait pour le chomp

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glBindTexture(GL_TEXTURE_2D, texture_eye);
    if (bb) draw_hierarchy_element(hierarchy["eye_left"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["eye_left"], scene.camera, shaders["wireframe"]);
    if (bb) draw_hierarchy_element(hierarchy["eye_right"], scene.camera, shaders["mesh"]);
    if (wf) draw_hierarchy_element(hierarchy["eye_right"], scene.camera, shaders["wireframe"]);

    glBindTexture(GL_TEXTURE_2D, texture_chain);
    /*
    buffer<vec3> chains = { chain1, chain2, chain3, chain4 }; // � trier par distance � la cam�ra
    std::sort(chains.begin(), chains.end(),
        [&scene](auto u, auto v) -> bool {return cmpbillboard(u, v, scene.camera.camera_position()); });
    chain.uniform.transform.rotation = scene.camera.orientation;
    for (vec3 pos : chains) {
        chain.uniform.transform.translation = pos + vec3{ 0,0,1 };
        if (bb) draw(chain, scene.camera, shaders["mesh"]);
        if (wf) draw(chain, scene.camera, shaders["wireframe"]);
    }
    */
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void character_structure::move(float t, float dt)
{
    // fonction o� on d�cide du mouvement (en fonction de t, dt, input ?)
    // peut-�tre � faire seulement dans interpolation_position.cpp
    if (dt > 0.1f) dt = 0.1f;

    //hierarchy.update_local_to_global_coordinates();
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

void loadMTL(const char* path, std::vector<mtltexture>& ans, std::vector<GLuint>& map_textures)
{
    size_t size = ans.size();
    FILE* file = fopen(path, "r");
    assert(!(file == NULL));
    while (true) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) break;
        if (strcmp(lineHeader, "newmtl") == 0) {
            size++;
            ans.push_back(mtltexture());
            char name[128];
            fscanf(file, "%s\n", name);
            ans[size - 1].name = name;
        }
        else if (strcmp(lineHeader, "Ns") == 0) {
            fscanf(file, "%f\n", &(ans[size - 1].Ns));
        }
        else if (strcmp(lineHeader, "Ka") == 0) {
            fscanf(file, "%f %f %f\n", &(ans[size - 1].Ka.x), &(ans[size - 1].Ka.y), &(ans[size - 1].Ka.z));
        }
        else if (strcmp(lineHeader, "Kd") == 0) {
            fscanf(file, "%f %f %f\n", &(ans[size - 1].Kd.x), &(ans[size - 1].Kd.y), &(ans[size - 1].Kd.z));
        }
        else if (strcmp(lineHeader, "Ks") == 0) {
            fscanf(file, "%f %f %f\n", &(ans[size - 1].Ks.x), &(ans[size - 1].Ks.y), &(ans[size - 1].Ks.z));
        }
        else if (strcmp(lineHeader, "Ke") == 0) {
            fscanf(file, "%f %f %f\n", &(ans[size - 1].Ke.x), &(ans[size - 1].Ke.y), &(ans[size - 1].Ke.z));
        }
        else if (strcmp(lineHeader, "Ni") == 0) {
            fscanf(file, "%f\n", &(ans[size - 1].Ni));
        }
        else if (strcmp(lineHeader, "d") == 0) {
            fscanf(file, "%f\n", &(ans[size - 1].d));
        }
        else if (strcmp(lineHeader, "illum") == 0) {
            fscanf(file, "%d\n", &(ans[size - 1].illum));
        }
        else if (strcmp(lineHeader, "map_Kd") == 0) {
            char map_Kd[128];
            fscanf(file, "%s\n", map_Kd);
            ans[size - 1].map_Kd = map_Kd;
            map_textures.push_back(create_texture_gpu(image_load_png(map_dir + ans[size - 1].map_Kd)));
        }
    }
    std::cout << "\tMTL loaded\t[OK]" << std::endl;
}

void loadOBJ(const char* path, std::vector<mesh_drawable>& obj, std::vector<int>& texture_indices, const std::vector<mtltexture>& mtl)
{
    std::vector<vec3> temp_vertices;
    std::vector<vec2> temp_uvs;
    std::vector<vec3> temp_normals;
    mesh tri;
    unsigned int nbtri = 0;
    char current_mtl[128];
    int current_mtl_index = -1;
    FILE* file = fopen(path, "r");
    assert(!(file == NULL));
    while (true) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) break;
        if (strcmp(lineHeader, "v") == 0) {
            vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "usemtl") == 0) {
            if (nbtri) {
                obj.push_back(tri); // conversion mesh -> mesh_drawable
                obj[obj.size() - 1].uniform.shading = { 0.5f, mtl[current_mtl_index].Kd.x, 0 };
                texture_indices.push_back(current_mtl_index);
            }
            fscanf(file, "%s\n", current_mtl);
            for (int i = 0; i < (int)mtl.size(); i++)
                if (mtl[i].name == current_mtl)
                    current_mtl_index = i;
            tri.connectivity.clear();
            tri.normal.clear();
            tri.position.clear();
            tri.texture_uv.clear();
            nbtri = 0;
        }
        else if (strcmp(lineHeader, "f") == 0) {
            assert(current_mtl_index != -1);
            unsigned int vIndex[3], uvIndex[3], nIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vIndex[0], &uvIndex[0], &nIndex[0], &vIndex[1], &uvIndex[1], &nIndex[1], &vIndex[2], &uvIndex[2], &nIndex[2]);
            assert(matches == 9);
            tri.position.push_back({ temp_vertices[vIndex[0] - 1].z,temp_vertices[vIndex[0] - 1].x,temp_vertices[vIndex[0] - 1].y });
            tri.position.push_back({ temp_vertices[vIndex[1] - 1].z,temp_vertices[vIndex[1] - 1].x,temp_vertices[vIndex[1] - 1].y });
            tri.position.push_back({ temp_vertices[vIndex[2] - 1].z,temp_vertices[vIndex[2] - 1].x,temp_vertices[vIndex[2] - 1].y });
            tri.texture_uv.push_back({ temp_uvs[uvIndex[0] - 1].x,1-temp_uvs[uvIndex[0] - 1].y });
            tri.texture_uv.push_back({ temp_uvs[uvIndex[1] - 1].x,1-temp_uvs[uvIndex[1] - 1].y });
            tri.texture_uv.push_back({ temp_uvs[uvIndex[2] - 1].x,1-temp_uvs[uvIndex[2] - 1].y });
            tri.normal.push_back({ -temp_normals[nIndex[0] - 1].z,-temp_normals[nIndex[0] - 1].x,-temp_normals[nIndex[0] - 1].y });
            tri.normal.push_back({ -temp_normals[nIndex[1] - 1].z,-temp_normals[nIndex[1] - 1].x,-temp_normals[nIndex[1] - 1].y });
            tri.normal.push_back({ -temp_normals[nIndex[2] - 1].z,-temp_normals[nIndex[2] - 1].x,-temp_normals[nIndex[2] - 1].y });
            tri.connectivity.push_back({ nbtri++,nbtri++,nbtri++ });
        }
    }
    if (nbtri) {
        obj.push_back(tri); // conversion mesh -> mesh_drawable
        obj[obj.size() - 1].uniform.shading = { mtl[current_mtl_index].Ka.x, 0, mtl[current_mtl_index].Ns };
        texture_indices.push_back(current_mtl_index);
    }
    std::cout << "\tOBJ loaded\t[OK]" << std::endl;
}

#endif

