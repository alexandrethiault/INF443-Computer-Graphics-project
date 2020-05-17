
#include "map.hpp"

#include <random>
#include <fstream>
#include <string>

#ifdef MAIN_SCENE

using namespace vcl;

const std::string map_dir = "scenes/shared_assets/models/Bob-omb";

float triangle::collision_depth = 0.3f;

triangle::triangle(vec3& p1, vec3& p2, vec3& p3, vec3& fakenormal)
{
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
    this->n = normalize(cross(p2 - p1, p3 - p1));
    if (dot(fakenormal, n) < 0) {
        std::swap(this->p2, this->p3);
        n = -n;
    }
    this->d = dot(p1, n); // d = ax+by+cz
}

bool triangle::collision(vec3 position, vec3& impact, vec3& normal)
{
    float distance_to_plane = dot(position, n) - d;
    if (distance_to_plane < -collision_depth || distance_to_plane > 0) return false;
    impact = position - distance_to_plane * n;
    normal = n;
    return (dot(cross(p2 - p1, impact - p1), n) > 0 && dot(cross(p3 - p2, impact - p2), n) > 0 && dot(cross(p1 - p3, impact - p3), n) > 0);
}

void map_structure::init(const char* MTLpath, const char* OBJpath)
{
    grid.resize(grid_size);
    loadMTL(MTLpath);
    loadOBJ(OBJpath);
    create_sky(); // Skybox  
    other_objects();
}

void map_structure::create_sky()
{
    mesh _sky = mesh_primitive_sphere(100);
    for (vec2& uv : _sky.texture_uv)
        uv = { 0.5f - uv.y, uv.x };
    sky = _sky; // Conversion mesh -> mesh_drawable
    sky.uniform.shading = { 1,0,0 };
    texture_sky = create_texture_gpu(image_load_png("scenes/shared_assets/textures/skybox.png"));
}

void map_structure::other_objects()
{
    mesh _post = mesh_primitive_cylinder(0.05f, { 0,0,0 }, { 0,0,0.23f }, 17);
    for (int i = 0; i < (int)_post.texture_uv.size(); i++)  _post.texture_uv[i].x *= 4;
    post = _post;
    post.uniform.shading = { 1,0,0 };
    texture_post = create_texture_gpu(image_load_png("scenes/shared_assets/textures/dark_wood.png"));
    post_top = mesh_primitive_disc(0.05f, { 0,0,0.23f }, { 0,0,1 }, 17);
    post_top.uniform.shading = { 1,0,0 };
    texture_post_top = create_texture_gpu(image_load_png("scenes/shared_assets/textures/cut_wood.png"));
    std::fstream posts("scenes/shared_assets/coords/posts.txt");
    int n; posts >> n;
    post_positions.resize(n);
    for (int i = 0; i < n; i++)
        posts >> post_positions[i].x >> post_positions[i].y >> post_positions[i].z;

    mesh _billboard = mesh_primitive_quad({ -0.2f, 0.55f,0 }, { 0.2f, 0.55f,0 }, { 0.2f,0,0 }, { -0.2f,0,0 });
    billboard = _billboard;
    billboard.uniform.shading = { 1,0,0 };
    texture_tree = create_texture_gpu(image_load_png("scenes/shared_assets/textures/bb_tree.png"));
    std::fstream trees("scenes/shared_assets/coords/trees.txt");
    trees >> n;
    tree_positions.resize(n);
    for (int i = 0; i < n; i++)
        trees >> tree_positions[i].x >> tree_positions[i].y >> tree_positions[i].z;
}

bool map_structure::collision(vcl::vec3 position, vcl::vec3& impact, vcl::vec3& normal, float min_normal_z)
{
    int i = static_cast<int>((position.x - minx) / (maxx - minx) * grid_size);
    int j = static_cast<int>((position.y - miny) / (maxy - miny) * grid_size);
    if (i >= grid_size || i < 0 || j >= grid_size || j < 0)
        return false;
    for (triangle* triptr : grid(i, j))
        if (triptr->n.z > min_normal_z && triptr->collision(position, impact, normal))
            return true;
    return false; // impact may have been modified anyway in the process
}

float side(vec3& p1, vec3& p2, vec3& p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

float map_structure::get_z(vcl::vec3 position) {
    int i = static_cast<int>((position.x - minx) / (maxx - minx) * grid_size);
    int j = static_cast<int>((position.y - miny) / (maxy - miny) * grid_size);
    if (i >= grid_size || i < 0 || j >= grid_size || j < 0)
        return 0.f;

    float d1, d2, d3;
    bool has_neg, has_pos;
    float nearest = -1000.f, z;

    for (triangle* triptr : grid(i, j))
        if (triptr->n.z > 0.1f) {
            d1 = side(position, triptr->p1, triptr->p2);
            d2 = side(position, triptr->p2, triptr->p3);
            d3 = side(position, triptr->p3, triptr->p1);

            has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
            has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

            z = triptr->p1.z - (triptr->n.x * (position.x - triptr->p1.x) + triptr->n.y * (position.y - triptr->p1.y)) / triptr->n.z;

            if (!(has_neg && has_pos) && abs(nearest - position.z) > abs(z - position.z))
                nearest = z;
        }

    return nearest;
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
    std::cout << "\t\t[OK] Level MTL loaded" << std::endl;
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
            maxx = std::max(maxx, vertex.z);
            minx = std::min(minx, vertex.z);
            maxy = std::max(maxy, vertex.x);
            miny = std::min(miny, vertex.x);
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
                map[map.size() - 1].uniform.shading = { 0.7f, 0.3f, 0 };
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
            

            map_triangle.push_back(triangle(
                vec3{ temp_vertices[vIndex[0] - 1].z,temp_vertices[vIndex[0] - 1].x,temp_vertices[vIndex[0] - 1].y },
                vec3{ temp_vertices[vIndex[1] - 1].z,temp_vertices[vIndex[1] - 1].x,temp_vertices[vIndex[1] - 1].y },
                vec3{ temp_vertices[vIndex[2] - 1].z,temp_vertices[vIndex[2] - 1].x,temp_vertices[vIndex[2] - 1].y },
                vec3{ -temp_normals[nIndex[0] - 1].z,-temp_normals[nIndex[0] - 1].x,-temp_normals[nIndex[0] - 1].y }
            ));
            tri.position.push_back({ temp_vertices[vIndex[0] - 1].z,temp_vertices[vIndex[0] - 1].x,temp_vertices[vIndex[0] - 1].y });
            tri.position.push_back({ temp_vertices[vIndex[1] - 1].z,temp_vertices[vIndex[1] - 1].x,temp_vertices[vIndex[1] - 1].y });
            tri.position.push_back({ temp_vertices[vIndex[2] - 1].z,temp_vertices[vIndex[2] - 1].x,temp_vertices[vIndex[2] - 1].y });
            tri.texture_uv.push_back({ temp_uvs[uvIndex[0] - 1].x,1 - temp_uvs[uvIndex[0] - 1].y });
            tri.texture_uv.push_back({ temp_uvs[uvIndex[1] - 1].x,1 - temp_uvs[uvIndex[1] - 1].y });
            tri.texture_uv.push_back({ temp_uvs[uvIndex[2] - 1].x,1 - temp_uvs[uvIndex[2] - 1].y });
            tri.normal.push_back({ -temp_normals[nIndex[0] - 1].z,-temp_normals[nIndex[0] - 1].x,-temp_normals[nIndex[0] - 1].y });
            tri.normal.push_back({ -temp_normals[nIndex[1] - 1].z,-temp_normals[nIndex[1] - 1].x,-temp_normals[nIndex[1] - 1].y });
            tri.normal.push_back({ -temp_normals[nIndex[2] - 1].z,-temp_normals[nIndex[2] - 1].x,-temp_normals[nIndex[2] - 1].y });
            tri.connectivity.push_back({ nbtri++,nbtri++,nbtri++ });
        }
    }
    if (nbtri) {
        map.push_back(tri); // conversion mesh -> mesh_drawable
        map[map.size() - 1].uniform.shading = { 0.7f, 0.3f, 0 };
        texture_indices.push_back(current_mtl_index);
    }

    maxx += triangle::collision_depth;
    maxy += triangle::collision_depth;
    minx -= triangle::collision_depth;
    miny -= triangle::collision_depth;
    int ind = 0;
    for (triangle& tri : map_triangle) {
        int mini = grid_size, minj = grid_size, maxi = 0, maxj = 0, i, j;
        for (vec3 p : {tri.p1, tri.p2, tri.p3}) {
            i = static_cast<int>((p.x - minx) / (maxx - minx) * grid_size);
            j = static_cast<int>((p.y - miny) / (maxy - miny) * grid_size);
            mini = std::min(mini, i);
            minj = std::min(minj, j);
            maxi = std::max(maxi, i);
            maxj = std::max(maxj, j);
            i = static_cast<int>((p.x - triangle::collision_depth * tri.n.x - minx) / (maxx - minx) * grid_size);
            j = static_cast<int>((p.y - triangle::collision_depth * tri.n.y - miny) / (maxy - miny) * grid_size);
            if (i == grid_size) i--;
            if (j == grid_size) j--;
            mini = std::min(mini, i);
            minj = std::min(minj, j);
            maxi = std::max(maxi, i);
            maxj = std::max(maxj, j);
        }
        for (i = mini; i <= maxi; i++) {
            for (j = minj; j <= maxj; j++) {
                grid(i, j).push_back(&tri);
            }
        }
    }

    std::cout << "\t\t[OK] Level OBJ loaded" << std::endl;
}


void map_structure::draw_nobillboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf)
{
    glBindTexture(GL_TEXTURE_2D, texture_sky);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    sky.uniform.transform.translation = scene.camera.camera_position(); // Move sky around camera
    draw(sky, scene.camera, shaders["mesh"]);
    if (wf) draw(sky, scene.camera, shaders["wireframe"]);

    for (vec3 post_position : post_positions) {
        glBindTexture(GL_TEXTURE_2D, texture_post);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        post.uniform.transform.translation = post_position;
        if (surf) draw(post, scene.camera, shaders["mesh"]);
        if (wf) draw(post, scene.camera, shaders["wireframe"]);
        glBindTexture(GL_TEXTURE_2D, texture_post_top);
        post_top.uniform.transform.translation = post_position;
        if (surf) draw(post_top, scene.camera, shaders["mesh"]);
        if (wf) draw(post_top, scene.camera, shaders["wireframe"]);
    }

    // Bob omb battlefield map
    for (int i = 2; i < (int)map.size(); i++) { // 0 and 1 are billboards
        glBindTexture(GL_TEXTURE_2D, map_textures[texture_indices[i]]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (surf) draw(map[i], scene.camera, shaders["mesh"]);
        if (wf) draw(map[i], scene.camera, shaders["wireframe"]);
    }
}

bool cmpbillboard(vec3& u, vec3& v, vec3& cam_pos) { return (norm(u - cam_pos) > norm(v - cam_pos)); }

void map_structure::draw_billboards(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool bb, bool wf)
{
    for (int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, map_textures[texture_indices[i]]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (bb) draw(map[i], scene.camera, shaders["mesh"]);
        if (wf) draw(map[i], scene.camera, shaders["wireframe"]);
    }

    std::sort(tree_positions.begin(), tree_positions.end(),
        [&scene](auto u, auto v) -> bool {return cmpbillboard(u, v, scene.camera.camera_position()); });
    for (vec3 tree_position : tree_positions) {
        vec3 dpos = scene.camera.camera_position() - tree_position;
        billboard.uniform.transform.rotation =rotation_from_axis_angle_mat3({ 0,0,1 }, atan2(dpos.y, dpos.x)) * mat3{0, 0, 1, 1, 0, 0, 0, 1, 0};
        billboard.uniform.transform.translation = tree_position;
        glBindTexture(GL_TEXTURE_2D, texture_tree);
        if (bb) draw(billboard, scene.camera, shaders["mesh"]);
        if (wf) draw(billboard, scene.camera, shaders["wireframe"]);
    }
}

#endif