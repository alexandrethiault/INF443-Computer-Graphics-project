
#include "map.hpp"

#include <random>
#include <fstream>
#include <string>

#ifdef MAIN_SCENE

using namespace vcl;

const std::string map_dir = "scenes/shared_assets/models/Bob-omb";

void map_structure::create_sky()
{
    mesh _sky = mesh_primitive_sphere(100);
    for (vec2& uv : _sky.texture_uv)
        uv = { 0.5f - uv.y, uv.x };
    sky = _sky;
    sky.uniform.shading = { 1,0,0 }; // set pure ambiant component (no diffuse, no specular)
    texture_sky = create_texture_gpu(image_load_png("scenes/shared_assets/textures/skybox.png"));
}

void map_structure::loadMTL(const char* path)
{
    size_t size = map_mtl.size();
    FILE* file = fopen(path, "r");
    assert(!(file == NULL));
    while (true) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) break;
        if (strcmp(lineHeader, "newmtl") == 0) {
            size++;
            map_mtl.push_back(mtltexture());
            char name[128];
            fscanf(file, "%s\n", name);
            map_mtl[size - 1].name = name;
        }
        else if (strcmp(lineHeader, "Ns") == 0) {
            fscanf(file, "%f\n", &(map_mtl[size - 1].Ns));
        }
        else if (strcmp(lineHeader, "Ka") == 0) {
            fscanf(file, "%f %f %f\n", &(map_mtl[size - 1].Ka.x), &(map_mtl[size - 1].Ka.y), &(map_mtl[size - 1].Ka.z));
        }
        else if (strcmp(lineHeader, "Kd") == 0) {
            fscanf(file, "%f %f %f\n", &(map_mtl[size - 1].Kd.x), &(map_mtl[size - 1].Kd.y), &(map_mtl[size - 1].Kd.z));
        }
        else if (strcmp(lineHeader, "Ks") == 0) {
            fscanf(file, "%f %f %f\n", &(map_mtl[size - 1].Ks.x), &(map_mtl[size - 1].Ks.y), &(map_mtl[size - 1].Ks.z));
        }
        else if (strcmp(lineHeader, "Ke") == 0) {
            fscanf(file, "%f %f %f\n", &(map_mtl[size - 1].Ke.x), &(map_mtl[size - 1].Ke.y), &(map_mtl[size - 1].Ke.z));
        }
        else if (strcmp(lineHeader, "Ni") == 0) {
            fscanf(file, "%f\n", &(map_mtl[size - 1].Ni));
        }
        else if (strcmp(lineHeader, "d") == 0) {
            fscanf(file, "%f\n", &(map_mtl[size - 1].d));
        }
        else if (strcmp(lineHeader, "illum") == 0) {
            fscanf(file, "%d\n", &(map_mtl[size - 1].illum));
        }
        else if (strcmp(lineHeader, "map_Kd") == 0) {
            char map_Kd[128];
            fscanf(file, "%s\n", map_Kd);
            map_mtl[size - 1].map_Kd = map_Kd;
            map_textures.push_back(create_texture_gpu(image_load_png(map_dir + map_mtl[size - 1].map_Kd)));
        }
    }
    std::cout << "MTL loaded" << std::endl;
}

void map_structure::loadOBJ(const char* path)
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
                map.push_back(tri); // conversion mesh -> mesh_drawable
                map[map.size() - 1].uniform.shading = { 0.5f, map_mtl[current_mtl_index].Kd.x, 0 };
                texture_indices.push_back(current_mtl_index);
            }
            fscanf(file, "%s\n", current_mtl);
            for (int i = 0; i < (int)map_mtl.size(); i++)
                if (map_mtl[i].name == current_mtl)
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
        map.push_back(tri); // conversion mesh -> mesh_drawable
        map[map.size() - 1].uniform.shading = { map_mtl[current_mtl_index].Ka.x, 0, map_mtl[current_mtl_index].Ns };
        texture_indices.push_back(current_mtl_index);
    }
    std::cout << "OBJ loaded" << std::endl;
}

#endif