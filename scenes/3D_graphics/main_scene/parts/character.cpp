
#include "character.hpp"
#include <string>
#include <algorithm>

#ifdef MAIN_SCENE

using namespace vcl;

const std::string mario_dir = "scenes/shared_assets/models/Mario GU/";
const float PI = 3.14159f;

void character_structure::init(const vec3& center)
{
    if (false) {
        std::cout << "Tentative de re-initialiser un mario deja initialise." << std::endl;
        return;
    }

    loadMTL("scenes/shared_assets/models/Mario GU/V13.mtl");
    loadOBJ("scenes/shared_assets/models/Mario GU/V13.obj");

    hierarchy.add(mario[find_mesh_index("Corps")], "Corps");
    hierarchy.add(mario[find_mesh_index("Boutons_GU")], "Boutons_GU", "Corps");
    hierarchy.add(mario[find_mesh_index("Bassin")], "Bassin", "Corps");
    hierarchy.add(mario[find_mesh_index("Ceinture")], "Ceinture", "Bassin");
    hierarchy.add(mario[find_mesh_index("Left_Upper_Leg")], "Left_Upper_Leg", "Bassin");
    hierarchy.add(mario[find_mesh_index("Right_Upper_Leg")], "Right_Upper_Leg", "Bassin");
    hierarchy.add(mario[find_mesh_index("Left_Lower_Leg")], "Left_Lower_Leg", "Left_Upper_Leg");
    hierarchy.add(mario[find_mesh_index("Right_Lower_Leg")], "Right_Lower_Leg", "Right_Upper_Leg");
    hierarchy.add(mario[find_mesh_index("Left_Foot")], "Left_Foot", "Left_Lower_Leg");
    hierarchy.add(mario[find_mesh_index("Right_Foot")], "Right_Foot", "Right_Lower_Leg");
    hierarchy.add(mario[find_mesh_index("Large_Bande_1")], "Large_Bande_1", "Right_Upper_Leg");
    hierarchy.add(mario[find_mesh_index("Small_Bande_1")], "Small_Bande_1", "Right_Lower_Leg");
    hierarchy.add(mario[find_mesh_index("Large_Bande_2")], "Large_Bande_2", "Left_Upper_Leg");
    hierarchy.add(mario[find_mesh_index("Small_Bande_2")], "Small_Bande_2", "Left_Lower_Leg");
    hierarchy.add(mario[find_mesh_index("Left_Upper_Arm")], "Left_Upper_Arm", "Corps");
    hierarchy.add(mario[find_mesh_index("Left_Lower_Arm")], "Left_Lower_Arm", "Left_Upper_Arm");
    hierarchy.add(mario[find_mesh_index("Left_Hand")], "Left_Hand", "Left_Lower_Arm");
    hierarchy.add(mario[find_mesh_index("Right_Upper_Arm")], "Right_Upper_Arm", "Corps");
    hierarchy.add(mario[find_mesh_index("Right_Lower_Arm")], "Right_Lower_Arm", "Right_Upper_Arm");
    hierarchy.add(mario[find_mesh_index("Right_Hand")], "Right_Hand", "Right_Lower_Arm");
    hierarchy.add(mario[find_mesh_index("Epaule_Gauche")], "Epaule_Gauche", "Corps");
    hierarchy.add(mario[find_mesh_index("Epaulette_Gauche")], "Epaulette_Gauche", "Epaule_Gauche");
    hierarchy.add(mario[find_mesh_index("Epaule_Droite")], "Epaule_Droite", "Corps");
    hierarchy.add(mario[find_mesh_index("Epaulette_Droite")], "Epaulette_Droite", "Epaule_Droite");
    hierarchy.add(mario[find_mesh_index("Cou")], "Cou", "Corps");
    hierarchy.add(mario[find_mesh_index("Skin")], "Skin", "Cou");
    hierarchy.add(mario[find_mesh_index("Hair")], "Hair", "Skin");
    hierarchy.add(mario[find_mesh_index("Hair_Piece")], "Hair_Piece", "Skin");
    hierarchy.add(mario[find_mesh_index("Eyes")], "Eyes", "Skin");
    hierarchy.add(mario[find_mesh_index("Bicorne")], "Bicorne", "Skin");
    hierarchy.add(mario[find_mesh_index("Cocbleu")], "Cocbleu", "Bicorne");
    hierarchy.add(mario[find_mesh_index("Cocrouge")], "Cocrouge", "Bicorne");
    hierarchy.add(mario[find_mesh_index("Cocblanc")], "Cocblanc", "Bicorne");
    hierarchy.add(mario[find_mesh_index("Broche")], "Broche", "Bicorne");
    hierarchy.add(mario[find_mesh_index("Bouton_Bicorne")], "Bouton_Bicorne", "Bicorne");
    hierarchy.add(mario[find_mesh_index("Moustache")], "Moustache", "Skin");
}

void character_structure::draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf)
{
    // dessiner toutes les parties qui ne sont pas des billboards
    // copi� coll� de ce que j'ai fait pour le chomp

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    
    for (int i = 0; i < (int)part_name.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, mario_textures[texture_indices[i]]);
        hierarchy[part_name[i]].global_transform.scaling = .001f;
        if (surf) draw_hierarchy_element(hierarchy[part_name[i]], scene.camera, shaders["mesh"]);
        if (wf) draw_hierarchy_element(hierarchy[part_name[i]], scene.camera, shaders["wireframe"]);
    }

    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

void character_structure::move(float t, float dt)
{
    // fonction o� on d�cide du mouvement (en fonction de t, dt, input ?)
    // peut-�tre � faire seulement dans interpolation_position.cpp
    if (dt > 0.1f) dt = 0.1f;


    //hierarchy["Corps"].transform.rotation = R;

    //hierarchy.update_local_to_global_coordinates();
}

void character_structure::set_translation(vcl::vec3& p) {
    hierarchy["Corps"].transform.translation = p;
    hierarchy.update_local_to_global_coordinates();
}


void character_structure::set_rotation(vcl::mat3& R) {
    hierarchy["Corps"].transform.rotation = R * rotation_from_axis_angle_mat3({0, 1, 0}, PI/2.);
    hierarchy.update_local_to_global_coordinates();
}

int character_structure::find_mesh_index(std::string name) {
    int i = 0;
    while(i < part_name.size() && part_name[i] != name)
        i++;
    return i;
}

void character_structure::loadMTL(const char* path)
{
    size_t size = mario_mtl.size();
    FILE* file = fopen(path, "r");
    assert(!(file == NULL));
    while (true) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) break;
        if (strcmp(lineHeader, "newmtl") == 0) {
            size++;
            mario_mtl.push_back(mtltexture());
            char name[128];
            fscanf(file, "%s\n", name);
            mario_mtl[size - 1].name = name;
        }
        else if (strcmp(lineHeader, "Ns") == 0) {
            fscanf(file, "%f\n", &(mario_mtl[size - 1].Ns));
        }
        else if (strcmp(lineHeader, "Ka") == 0) {
            fscanf(file, "%f %f %f\n", &(mario_mtl[size - 1].Ka.x), &(mario_mtl[size - 1].Ka.y), &(mario_mtl[size - 1].Ka.z));
        }
        else if (strcmp(lineHeader, "Kd") == 0) {
            fscanf(file, "%f %f %f\n", &(mario_mtl[size - 1].Kd.x), &(mario_mtl[size - 1].Kd.y), &(mario_mtl[size - 1].Kd.z));
        }
        else if (strcmp(lineHeader, "Ks") == 0) {
            fscanf(file, "%f %f %f\n", &(mario_mtl[size - 1].Ks.x), &(mario_mtl[size - 1].Ks.y), &(mario_mtl[size - 1].Ks.z));
        }
        else if (strcmp(lineHeader, "Ke") == 0) {
            fscanf(file, "%f %f %f\n", &(mario_mtl[size - 1].Ke.x), &(mario_mtl[size - 1].Ke.y), &(mario_mtl[size - 1].Ke.z));
        }
        else if (strcmp(lineHeader, "Ni") == 0) {
            fscanf(file, "%f\n", &(mario_mtl[size - 1].Ni));
        }
        else if (strcmp(lineHeader, "d") == 0) {
            fscanf(file, "%f\n", &(mario_mtl[size - 1].d));
        }
        else if (strcmp(lineHeader, "illum") == 0) {
            fscanf(file, "%d\n", &(mario_mtl[size - 1].illum));
        }
        else if (strcmp(lineHeader, "map_Kd") == 0) {
            char map_Kd[128];
            fscanf(file, "%s\n", map_Kd);
            mario_mtl[size - 1].map_Kd = map_Kd;
            mario_textures.push_back(create_texture_gpu(image_load_png(mario_dir + mario_mtl[size - 1].map_Kd)));
        }
    }
    std::cout << "\tMario MTL loaded\t[OK]" << std::endl;
}

void character_structure::loadOBJ(const char* path)
{
    std::vector<vec3> temp_vertices;
    std::vector<vec2> temp_uvs;
    std::vector<vec3> temp_normals;
    mesh tri;
    unsigned int nbtri = 0;
    char current_mtl[128];
    char current_name[128];
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
        else if (strcmp(lineHeader, "g") == 0) {
            if (nbtri) {
                mario.push_back(tri); // conversion mesh -> mesh_drawable
                mario[mario.size() - 1].uniform.shading = { 0.5f, mario_mtl[current_mtl_index].Kd.x, 0 };
                std::string name = std::string(current_name);
                part_name.push_back(name);
                texture_indices.push_back(current_mtl_index);
            }
            memset(current_name, '\0', sizeof(current_name));
            fscanf(file, "%s\n", current_name);

            tri.connectivity.clear();
            tri.normal.clear();
            tri.position.clear();
            tri.texture_uv.clear();
            nbtri = 0;
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
            fscanf(file, "%s\n", current_mtl);
            for (int i = 0; i < (int)mario_mtl.size(); i++)
                if (mario_mtl[i].name == current_mtl)
                    current_mtl_index = i;
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
            tri.normal.push_back({ temp_normals[nIndex[0] - 1].z,temp_normals[nIndex[0] - 1].x,temp_normals[nIndex[0] - 1].y });
            tri.normal.push_back({ temp_normals[nIndex[1] - 1].z,temp_normals[nIndex[1] - 1].x,temp_normals[nIndex[1] - 1].y });
            tri.normal.push_back({ temp_normals[nIndex[2] - 1].z,temp_normals[nIndex[2] - 1].x,temp_normals[nIndex[2] - 1].y });
            tri.connectivity.push_back({ nbtri++,nbtri++,nbtri++ });
        }
    }
    if (nbtri) {
        mario.push_back(tri); // conversion mesh -> mesh_drawable
        mario[mario.size() - 1].uniform.shading = { mario_mtl[current_mtl_index].Ka.x, 0, mario_mtl[current_mtl_index].Ns };
        texture_indices.push_back(current_mtl_index);
        std::string name = std::string(current_name);
        part_name.push_back(name);
    }
    std::cout << "\tMario OBJ loaded\t[OK]" << std::endl;
}

#endif

