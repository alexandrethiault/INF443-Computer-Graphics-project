
#include "modeling.hpp"

#include <random>
#include <fstream>

#ifdef SCENE_BATTLEFIELD

using namespace vcl;

// Generator for uniform random number
std::default_random_engine generator;
std::uniform_real_distribution<float> distrib(0.0, 1.0);

const std::string coords_dir = "scenes/shared_assets/coords/";
const std::string map_dir = "scenes/shared_assets/models";
const std::string textures_dir = "scenes/shared_assets/textures/";

mesh create_sky();
void loadMTL(const char* path, std::vector<mtltexture>& ans, std::vector<GLuint>& map_textures);
void loadOBJ(const char* path, std::vector<mesh_drawable>& obj, std::vector<int>& text_indices, const std::vector<mtltexture>& mtl);

////////////////////////////// MAIN FUNCTIONS: SETUP_DATA AND FRAME_DRAW //////////////////////////////

void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 20.0f;
    scene.camera.apply_rotation(0,0,2,1.2f);
    
    // Skybox
    sky = create_sky();
    sky.uniform.shading = { 1,0,0 }; // set pure ambiant component (no diffuse, no specular)
    texture_sky = create_texture_gpu(image_load_png(textures_dir + "skybox.png"));

    // Create a quad for billboards
    demo_chomp = mesh_primitive_sphere(); // carré de côté 1 centré en 0
    demo_chomp.uniform.color = { 0,0,0 };
    demo_chomp.uniform.transform.translation = { -2,0,1 };
    demo_chomp.uniform.transform.scaling = 0.25f;
    demo_chomp.uniform.shading = { 1,0,0 };

    loadMTL("scenes/shared_assets/models/Bob-omb Battlefield.mtl", map_mtl, map_textures);
    loadOBJ("scenes/shared_assets/models/Bob-omb Battlefield.obj", map, texture_indices, map_mtl);

    timer.scale = 1.0f;
}

void scene_model::frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    timer.update();
    const float t = timer.t;
    const float dt = std::min(0.03f, timer_event.update());
    set_gui();

    glEnable(GL_POLYGON_OFFSET_FILL); // avoids z-fighting when displaying wireframe

    draw(demo_chomp, scene.camera, shaders["mesh"]);

    // Display sky
    glBindTexture(GL_TEXTURE_2D, texture_sky);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    sky.uniform.transform.translation = scene.camera.camera_position(); // Move sky around camera
    draw(sky, scene.camera, shaders["mesh"]);
    if (gui_scene.wireframe) draw(sky, scene.camera, shaders["wireframe"]);

    // Bob omb battlefield map
    for (int i = 2; i < (int)map.size(); i++) { // 0 and 1 are billboards
        glBindTexture(GL_TEXTURE_2D, map_textures[texture_indices[i]]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (gui_scene.surface) draw(map[i], scene.camera, shaders["mesh"]);
        if (gui_scene.wireframe) draw(map[i], scene.camera, shaders["wireframe"]);
    }

    //// BILLBOARDS ALWAYS LAST ////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(false);
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, map_textures[texture_indices[i]]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (gui_scene.surface) draw(map[i], scene.camera, shaders["mesh"]);
        if (gui_scene.wireframe) draw(map[i], scene.camera, shaders["wireframe"]);
    }
    glDepthMask(true);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

////////////////////////////// GEOMETRY FUNCTIONS //////////////////////////////

mesh create_sky()
{
    mesh sky = mesh_primitive_sphere(100);
    for (vec2& uv : sky.texture_uv)
        uv = { 0.5f - uv.y, uv.x };
    return sky;
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
    std::cout << "MTL loaded" << std::endl;
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
    std::cout << "OBJ loaded" << std::endl;
}
////////////////////////////// MARIO AUXILIARY FUNCTIONS //////////////////////////////


////////////////////////////// CHOMP AUXILIARY FUNCTIONS //////////////////////////////


////////////////////////////// STAR, BRIDGE, BUBBLE... //////////////////////////////


////////////////////////////// USER INTERFACE //////////////////////////////

/*/ Mouse click is used to select a position of the control polygon
void scene_model::mouse_click(scene_structure& scene, GLFWwindow* window, int, int, int)
{
    const bool mouse_click_left = glfw_mouse_pressed_left(window);
    const bool key_shift = glfw_key_shift_pressed(window);

    if (mouse_click_left && key_shift)
    {
        // Create the 3D ray passing by the selected point on the screen
        const ray r = picking_ray(scene.camera, glfw_cursor_coordinates_window(window));

        // Loop over all positions and get the closest intersected sphere if any
        picked_object = -1;
        float distance_min = 0.0f;
        for (int k = 0; k < keyframes.size(); k++) {
            const vec3 c = keyframes[k].p;
            const picking_info info = ray_intersect_sphere(r, c, 0.1f);

            if (info.picking_valid) { // the ray intersects a sphere
                const float distance = norm(info.intersection - r.p); // get the closest intersection
                if (picked_object == -1 || distance < distance_min) {
                    distance_min = distance;
                    picked_object = k;
                }
            }
        }
    }
}

// Translate the selected object to the new pointed mouse position within the camera plane
void scene_model::mouse_move(scene_structure& scene, GLFWwindow* window)
{
    const bool mouse_click_left = glfw_mouse_pressed_left(window);
    const bool key_shift = glfw_key_shift_pressed(window);
    if (mouse_click_left && key_shift && picked_object != -1) {
        // Get vector orthogonal to camera orientation
        const mat4 M = scene.camera.camera_matrix();
        const vec3 n = { M(0,2),M(1,2),M(2,2) };

        // Intersection between current ray and the plane orthogonal to the view direction and passing by the object
        const vec2 cursor = glfw_cursor_coordinates_window(window);
        const ray r = picking_ray(scene.camera, cursor);
        vec3& p0 = keyframes[picked_object].p;
        const picking_info info = ray_intersect_plane(r, n, p0);

        // translate the position
        p0 = info.intersection;
        equalize_speed(keyframes, timer);
    }
}*/

void scene_model::set_gui()
{
    ImGui::Text("Display: "); ImGui::SameLine();
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface); ImGui::SameLine();
    ImGui::Checkbox("Billboards", &gui_scene.billboards); ImGui::SameLine();
}

#endif