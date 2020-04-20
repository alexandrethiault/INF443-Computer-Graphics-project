
#include "modeling.hpp"

#include <random>
#include <fstream>

#ifdef SCENE_BATTLEFIELD

using namespace vcl;

// Generator for uniform random number
std::default_random_engine generator;
std::uniform_real_distribution<float> distrib(0.0, 1.0);

const std::string coords_dir = "scenes/shared_assets/coords/";
const std::string assets_dir = "scenes/shared_assets/textures/";

mesh create_terrain();
mesh create_tree_foliage(float radius, float height, float z_offset);
mesh create_sky();
bool cmpbillboard(std::pair<vec3, int>& u, std::pair<vec3, int>& v, vec3& cam_pos);
void particles_evolution(std::list<particle_structure>& particles, bool is_new_particle, float dt);
std::pair<const vec3, const vec3> cardinal_spline_interpolation(float t, buffer<vec3t>& keyframes, float mu);

////////////////////////////// MAIN FUNCTIONS: SETUP_DATA AND FRAME_DRAW //////////////////////////////

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{

    // Create visual terrain surface
    terrain = create_terrain();
    terrain.uniform.shading.specular = 0.0f; // non-specular terrain material
    texture_bg = create_texture_gpu(image_load_png(assets_dir + "forest_soil.png"));

    // Setup initial camera mode and position
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 30.0f;
    scene.camera.apply_rotation(0,0,0,1.2f);
    
    // Tree
    trunk = mesh_primitive_cylinder(0.2f, { 0,0,0 }, { 0,0,2 });
    trunk.uniform.color = { 0.3f,0.3f,0.1f };
    trunk.uniform.shading.specular = 0.0f;
    trunk.uniform.shading.diffuse = 0.5f;
    foliage = create_tree_foliage(0.8f, 1.6f, 0.8f);
    foliage.uniform.color = { 0.1f,0.5f,0.4f };
    foliage.uniform.shading.specular = 0.0f;
    update_tree_position(20);
    update_billboard_position(50);

    // Skybox
    sky = create_sky();
    sky.uniform.shading = { 1.0f, 0.0f, 0.0f };
    texture_sky = create_texture_gpu(image_load_png(assets_dir + "skybox.png"));

    // Create a quad with (u,v)-texture coordinates
    mesh surface_cpu;
    surface_cpu.position = { {-0.2f,0,0}, { 0.2f,0,0}, { 0.2f, 0.4f,0}, {-0.2f, 0.4f,0} };
    surface_cpu.texture_uv = { {0,1}, {1,1}, {1,0}, {0,0} };
    surface_cpu.connectivity = { {0,1,2}, {0,2,3} };
    billboard = surface_cpu;
    billboard.uniform.shading = { 1,0,0 }; // set pure ambiant component (no diffuse, no specular)

    // Load a texture (with transparent background)
    texture_flower[0] = create_texture_gpu(image_load_png(assets_dir + "bb_blue_flower.png"), GL_REPEAT, GL_REPEAT);
    texture_flower[1] = create_texture_gpu(image_load_png(assets_dir + "bb_purple_flower.png"), GL_REPEAT, GL_REPEAT);
    texture_flower[2] = create_texture_gpu(image_load_png(assets_dir + "bb_red_flower.png"), GL_REPEAT, GL_REPEAT);

    // Animated bird
    setup_birb();
    setup_birb_flight();
    // Set the same shader for all the elements
    hierarchy.set_shader_for_all_elements(shaders["mesh"]);
    hierarchy_visual_debug.init(shaders["segment_im"], shaders["mesh"]);
    keyframe_visual.shader = shaders["mesh"];
    keyframe_picked.shader = shaders["mesh"];

    // Create mesh for particles represented as spheres
    const float r = 0.1f; // radius of spheres
    sphere = mesh_primitive_sphere(r);
    sphere.uniform.color = { 0.6f, 0.6f, 1.0f };

    // Chain
    setup_chain(hierarchy["f1_left"].transform.translation);

    timer.scale = 0.5f;
}

/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_model::frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    timer.update();
    const float t = timer.t;
    const float dt = std::min(0.03f, timer_event.update()); // dt: Elapsed time between last frame
    set_gui();

    glEnable(GL_POLYGON_OFFSET_FILL); // avoids z-fighting when displaying wireframe

    // Display terrain
    glBindTexture(GL_TEXTURE_2D, texture_bg);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glPolygonOffset(1.0, 1.0);
    if (gui_scene.surface) draw(terrain, scene.camera, shaders["mesh"]);
    if (gui_scene.wireframe) draw(terrain, scene.camera, shaders["wireframe"]);

    // Display sky
    glBindTexture(GL_TEXTURE_2D, texture_sky);
    sky.uniform.transform.translation = scene.camera.camera_position();
    draw(sky, scene.camera, shaders["mesh"]);
    if (gui_scene.wireframe) draw(sky, scene.camera, shaders["wireframe"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    // Display trees = trunk+foliage
    for (vec3 position : tree_position) {
        trunk.uniform.transform.translation = position + vec3{ 0,0,-0.2f };
        if (gui_scene.surface) draw(trunk, scene.camera, shaders["mesh"]);
        if (gui_scene.wireframe) draw(trunk, scene.camera, shaders["wireframe"]);
        foliage.uniform.transform.translation = position + vec3{ 0,0,1.2f };
        if (gui_scene.surface) draw(foliage, scene.camera, shaders["mesh"]);
        if (gui_scene.wireframe) draw(foliage, scene.camera, shaders["wireframe"]);
    }

    // Spline interpolation of keyframes
    std::pair<const vec3, const vec3> pdp = cardinal_spline_interpolation(t, keyframes, 0.5f);
    const vec3 p = pdp.first;
    const vec3 dp = pdp.second;

    // Draw sphere at each keyframe position
    if (gui_scene.display_keyframe)
        for (size_t k = 0; k < keyframes.size(); ++k) {
            const vec3& p_keyframe = keyframes[k].p;
            keyframe_visual.uniform.transform.translation = p_keyframe;
            draw(keyframe_visual, scene.camera);
        }

    // Draw selected sphere in red
    if (picked_object != -1) {
        const vec3& p_keyframe = keyframes[picked_object].p;
        keyframe_picked.uniform.transform.translation = p_keyframe;
        draw(keyframe_picked, scene.camera);
    }

    // Draw segments between each keyframe
    if (gui_scene.display_polygon)
        for (size_t k = 0; k < keyframes.size() - 1; ++k) {
            segment_drawer.uniform_parameter.p1 = keyframes[k].p;
            segment_drawer.uniform_parameter.p2 = keyframes[k + 1].p;
            segment_drawer.draw(shaders["segment_im"], scene.camera);
        }

    // Set a rotation that follows the trajectory for the animated bird
    set_birb_rotation(p, dp, t);
    if (gui_scene.surface) draw(hierarchy, scene.camera);
    if (gui_scene.wireframe) draw(hierarchy, scene.camera, shaders["wireframe"]);
    if (gui_scene.skeleton) hierarchy_visual_debug.draw(hierarchy, scene.camera);

    // Let particles fall/rebound during dt before displaying them
    particles_evolution(particles, timer_event.event && !gui_scene.stop_balls, dt);
    if (gui_scene.surface)
        for (particle_structure& particle : particles) {
            sphere.uniform.transform.translation = particle.p;
            draw(sphere, scene.camera, shaders["mesh"]);
        }

    // Mass-spring system for chain
    for (int i = 0; i < 4; i++) simulate_chain(timer.scale * 0.003f);
    if (gui_scene.chain) {
        cube.uniform.transform.translation = pA.p;
        cube.uniform.color = { 0,0,0 };
        draw(cube, scene.camera, shaders["mesh"]);
        for (auto pB : pBs) {
            cube.uniform.transform.translation = pB.p;
            cube.uniform.color = { 1,0,0 };
            draw(cube, scene.camera, shaders["mesh"]);
        }
        segment_drawer.uniform_parameter.p1 = pA.p;
        segment_drawer.uniform_parameter.p2 = pBs[0].p;
        segment_drawer.draw(shaders["segment_im"], scene.camera);
        for (size_t i = 0; i < N_chain - 1; i++) {
            segment_drawer.uniform_parameter.p1 = pBs[i].p;
            segment_drawer.uniform_parameter.p2 = pBs[i + 1].p;
            segment_drawer.draw(shaders["segment_im"], scene.camera);
        }
    }

    //// BILLBOARDS ALWAYS LAST ////

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(false);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // avoids sampling artifacts
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // avoids sampling artifacts

    // Display a billboard always facing the camera direction
    std::sort(billboard_position.begin(), billboard_position.end(),
        [&scene](auto u, auto v) -> bool {return cmpbillboard(u, v, scene.camera.camera_position()); });
    for (std::pair<vec3, size_t> posandtype : billboard_position) {
        glBindTexture(GL_TEXTURE_2D, texture_flower[posandtype.second]);
        billboard.uniform.transform.rotation = scene.camera.orientation;
        billboard.uniform.transform.translation = posandtype.first + vec3{ 0,0,-0.0f };
        if (gui_scene.billboards) draw(billboard, scene.camera, shaders["mesh"]);
        if (gui_scene.wireframe) draw(billboard, scene.camera, shaders["wireframe"]);
    }

    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    glDepthMask(true);
}

////////////////////////////// TERRAIN AUXILIARY FUNCTIONS //////////////////////////////

// Evaluate height of the terrain for any (u,v) \in [0,1]
float evaluate_terrain_z(float u, float v)
{
    float z = 0.0f;
    const vec2 p[] = { {0,0},{0.5f,0.5f},{0.2f,0.7f},{0.8f,0.7f} };
    const float h[] = { 3,-1.5f,1,2 };
    const float sigma[] = { 0.5f,0.15f,0.2f,0.2f };
    for (int i = 0; i < 4; i++) {
        float d = norm(vec2(u, v) - p[i]) / sigma[i];
        z += h[i] * std::exp(-d * d);
    }

    const float height = 0.5f;
    const float scaling = 3.0f;
    const int octave = 7;
    const float persistency = 0.4f;
    const float noise = perlin(scaling * u, scaling * v, octave, persistency);
    return z + height * noise;
}

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v)
{
    const float x = 20*(u-0.5f);
    const float y = 20*(v-0.5f);
    const float z = evaluate_terrain_z(u,v);
    return {x,y,z};
}

void find_uv(vec3& xyz, float&u, float&v)
{
    u = xyz.x * 0.05f + 0.5f;
    v = xyz.y * 0.05f + 0.5f;
}

// Generate terrain mesh
mesh create_terrain()
{
    // Number of samples of the terrain is N x N
    const size_t N = 100;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N * N);
    terrain.texture_uv.resize(N * N);

    // Fill terrain geometry
    for(size_t ku=0; ku<N; ++ku) {
        for(size_t kv=0; kv<N; ++kv) {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);
            // Compute coordinates
            terrain.position[kv + N * ku] = evaluate_terrain(u, v);
            terrain.texture_uv[kv + N * ku] = { u, v };
        }
    }
    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    const unsigned int Ns = N;
    for(unsigned int ku=0; ku<Ns-1; ++ku) {
        for(unsigned int kv=0; kv<Ns-1; ++kv) {
            const unsigned int idx = kv + N*ku; // current vertex offset
            const uint3 triangle_1 = {idx, idx+1+Ns, idx+1};
            const uint3 triangle_2 = {idx, idx+Ns, idx+1+Ns};
            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }
    return terrain;
}

mesh create_tree_foliage(float radius, float height, float z_offset)
{
    mesh m = mesh_primitive_cone(radius, { 0,0,0 }, { 0,0,height });
    m.push_back(mesh_primitive_cone(0.8f*radius, { 0,0,z_offset }, { 0,0,z_offset + 0.8f*height }));
    m.push_back(mesh_primitive_cone(0.64f*radius, { 0,0,2*z_offset }, { 0,0,2*z_offset + 0.64f*height }));
    return m;
}

void scene_model::update_tree_position(size_t n) {
    float u, v;
    bool ok;
    std::vector<float> rem;
    for (size_t i = 0; i < n; i++) {
        while (true) {
            u = rand_interval();
            v = rand_interval();
            ok = true;
            for (size_t j = 0; j < i; j++) {
                if (abs(rem[2 * j] - u) + abs(rem[2 * j + 1] - v) < 0.04f) {
                    ok = false;
                    break;
                }
            }
            if (ok) break;
        }
        rem.push_back(u);
        rem.push_back(v);
        tree_position.push_back(evaluate_terrain(u, v));
    }
}

void scene_model::update_billboard_position(size_t n) {
    float u, v;
    bool ok;
    std::vector<float> rem;
    for (size_t i = 0; i < n; i++) {
        while (true) {
            u = rand_interval();
            v = rand_interval();
            ok = true;
            for (vec3 vectree : tree_position)
                if (abs(vectree.x - u) + abs(vectree.y - v) < 0.04f) {
                    ok = false;
                    break;
                }
            if (!ok) continue;
            for (size_t j = 0; j < i; j++)
                if (abs(rem[2 * j] - u) + abs(rem[2 * j + 1] - v) < 0.01f) {
                    ok = false;
                    break;
                }
            if (ok) break;
        }
        rem.push_back(u);
        rem.push_back(v);
        billboard_position.push_back({ evaluate_terrain(u, v), i%3});
    }
}

bool cmpbillboard(std::pair<vec3, size_t>& u, std::pair<vec3, size_t>& v, vec3& cam_pos)
{
    float norm_u = norm(u.first - cam_pos);
    float norm_v = norm(v.first - cam_pos);
    return (norm_u > norm_v);
}

mesh create_sky()
{
    mesh sky;
    /*0  1--2  3  4  à gauche (x=-inf) si i%5 == 0 ou 1 ou 4 soit (i+1)%5 <= 2
      5--6  7--8--9  en bas (z=-inf) si i>=10
     10-11 12-13-14  au fond (y=-inf) si i n'est pas 6 ou 7 ou 11 ou 12 soit abs(abs(i-9)-2.5f)>1 
     15 16-17 18 19 */
    const float inf = 100.0f;
    std::vector<float> us { 0.000f, 0.251f, 0.499f, 0.750f, 1.000f };
    std::vector<float> vs { 0.000f, 0.334f, 0.666f, 1.000f };
    for (int i = 0; i < 20; i++) {
        sky.position.push_back(vec3{((i+1)%5<=2) ? -inf : inf, (abs(abs(i-9)-2.5f)>1) ? -inf : inf, (i>=10) ? -inf : inf });
        sky.texture_uv.push_back({ us[i % 5], vs[i / 5] });
    }
    for (unsigned int i = 0; i < 15; i++) {
        sky.connectivity.push_back({ i, i + 5, i + 6 });
        sky.connectivity.push_back({ i, i + 6, i + 1 });
        if (i % 5 == 3) i++;
    }
    return sky;
}

////////////////////////////// BIRB AUXILIARY FUNCTIONS //////////////////////////////

void scene_model::setup_birb()
{
    const float radius_body = 0.1f;
    const float radius_head = 0.08f;
    const float length_arm = 0.08f;
    const float breadth_arm = length_arm * 1.2f;

    // The geometry of the body is a sphere
    mesh_drawable body = mesh_drawable(mesh_primitive_sphere(radius_body, { 0,0,0 }, 40, 40));
    body.uniform.transform.scaling_axis = vec3(1, 1, 2);
    body.uniform.shading.specular = 20.0f;
    mesh_drawable head = mesh_drawable(mesh_primitive_sphere(radius_head, { 0,0,0 }, 40, 40));
    head.uniform.shading.specular = 20.0f;

    // Geometry of the eyes: black spheres, nose: orange cone, wings: two quadrilaterals feather_1 and _2
    mesh_drawable eye = mesh_drawable(mesh_primitive_sphere(radius_head * 0.2f, { 0,0,0 }, 20, 20));
    eye.uniform.color = { 0,0,0 };
    mesh_drawable nose = mesh_drawable(mesh_primitive_cone(radius_head * 0.4f, { 0,0,0 }, { 0,0,0.05f }, 20, 10));
    nose.uniform.color = { 1, 0.5f, 0 };
    mesh_drawable feather_1 = mesh_primitive_quad(
        { 0, 0, breadth_arm }, { 0, 0, -breadth_arm }, { -length_arm, 0, -breadth_arm }, { -length_arm, 0, breadth_arm });
    mesh_drawable feather_2 = mesh_primitive_quad(
        { 0, 0, breadth_arm }, { 0, 0, -breadth_arm }, { -length_arm, 0, 0 }, { -length_arm, 0, length_arm });

    // Build the hierarchy: hierarchy.add(visual_element, element_name, parent_name, (opt)[translation, rotation])
    hierarchy.add(body, "body");
    hierarchy.add(head, "head", "body", radius_body * vec3(0, 1, 2));
    hierarchy.add(eye, "eye_left", "head", radius_head * vec3(1 / 3.0f, 1 / 2.0f, 2 / 3.0f));
    hierarchy.add(eye, "eye_right", "head", radius_head * vec3(-1 / 3.0f, 1 / 2.0f, 2 / 3.0f));
    hierarchy.add(nose, "nose", "head", radius_head * vec3(0, 0, 0.9f));
    hierarchy.add(feather_1, "f1_left", "body", { -radius_body * 0.8f,0,0 });
    hierarchy.add(feather_2, "f2_left", "f1_left", { -length_arm,0,0 });
    hierarchy.add(feather_1, "f1_right", "body", { {radius_body * 0.8f,0,0}, {-1,0,0, 0,1,0, 0,0,1} });
    hierarchy.add(feather_2, "f2_right", "f1_right", { -length_arm,0,0 });
}

// Set keyframes times proportionally with respect to the distance between the points
void equalize_speed(buffer<vec3t>& keyframes, timer_interval& timer)
{
    float sumdist = 0.0f, newdist;
    for (size_t i = 1; i < keyframes.size(); i++) {
        newdist = norm(keyframes[i].p - keyframes[i - 1].p);
        sumdist += newdist;
        keyframes[i].t = sumdist / 5;
    }
    float u = (timer.t - timer.t_min) / (timer.t_max - timer.t_min);
    timer.t_min = keyframes[1].t;                   // first time of the keyframe
    timer.t_max = keyframes[keyframes.size() - 2].t;  // last time of the keyframe
    timer.t = u * timer.t_max + (1 - u) * timer.t_min;
}

void scene_model::setup_birb_flight()
{
    // Initial Keyframe data vector of (position, time)
    std::fstream kfin(coords_dir + "keyframes.txt");
    int n;
    kfin >> n;
    keyframes.resize(n);
    for (int i = 0; i < n; i++)
        kfin >> keyframes[i].p.x >> keyframes[i].p.y >> keyframes[i].p.z >> keyframes[i].t;
    equalize_speed(keyframes, timer);

    keyframe_visual = mesh_primitive_sphere();
    keyframe_visual.uniform.color = { 1,1,1 };
    keyframe_visual.uniform.transform.scaling = 0.05f;

    keyframe_picked = mesh_primitive_sphere();
    keyframe_picked.uniform.color = { 1,0,0 };
    keyframe_picked.uniform.transform.scaling = 0.055f;

    segment_drawer.init();
    picked_object = -1;
}

size_t index_at_value(float t, buffer<vec3t> const& v)
{
    const size_t N = v.size();
    assert(v.size() >= 2 && t >= v[0].t && t < v[N - 1].t);
    size_t k = 0;
    while (v[++k + 1].t < t) {}
    return k;
}

std::pair<const vec3, const vec3> cardinal_spline_interpolation(float t, buffer<vec3t>& keyframes, float mu)
{
    const size_t idx = index_at_value(t, keyframes);
    const float t0 = keyframes[idx - 1].t; // = t_{i-1}
    const float t1 = keyframes[idx].t; // = t_i
    const float t2 = keyframes[idx + 1].t; // = t_{i+1}
    const float t3 = keyframes[idx + 2].t; // = t_{i+2}
    const vec3& p0 = keyframes[idx - 1].p; // = p_{i-1}
    const vec3& p1 = keyframes[idx].p; // = p_i
    const vec3& p2 = keyframes[idx + 1].p; // = p_{i+1}
    const vec3& p3 = keyframes[idx + 2].p; // = p_{i+2}
    const float s = (t - t1) / (t2 - t1);
    const vec3 d1 = 2 * mu * (p2 - p0) / (t2 - t0);
    const vec3 d2 = 2 * mu * (p3 - p1) / (t3 - t1);
    const vec3 p = (2*s*s*s - 3*s*s + 1)*p1 + (s*s*s - 2*s*s + s)*d1 + (-2*s*s*s + 3*s*s)*p2 + (s*s*s - s*s)*d2;
    const vec3 dp = (6*s*s - 6*s)*p1 + (3*s*s - 4*s + 1)*d1 + (-6*s*s + 6*s )*p2 + (3*s*s - 2*s)*d2;
    return std::make_pair(p, dp);
}

void scene_model::set_birb_rotation(const vec3& p, const vec3& dp, const float t) {
    mat3 const R_global = rotation_to_vector_mat3(dp) * mat3 { 0, 0, 1, 1, 0, 0, 0, 1, 0 };
    mat3 const R_face = rotation_from_axis_angle_mat3({ 1,0,0 }, 0.2f + 0.1f * std::sin(8 * 3.14f * t));
    mat3 const R_feather1 = rotation_from_axis_angle_mat3({ 0,0,1 }, std::sin(16 * 3.14f * (t - 0.4f)));
    mat3 const R_feather2 = rotation_from_axis_angle_mat3({ 0,0,1 }, std::sin(16 * 3.14f * (t - 0.6f)));
    mat3 const Symmetry = { -1,0,0, 0,1,0, 0,0,1 };
    hierarchy["body"].transform.translation = p;
    hierarchy["body"].transform.rotation = R_global;
    hierarchy["head"].transform.rotation = R_face;
    hierarchy["f1_left"].transform.rotation = R_feather1;
    hierarchy["f2_left"].transform.rotation = R_feather2;
    hierarchy["f1_right"].transform.rotation = Symmetry * R_feather1; // apply the symmetry
    hierarchy["f2_right"].transform.rotation = R_feather2; //symmetry is already applied by the parent
    hierarchy.update_local_to_global_coordinates();
    pA.p = hierarchy["body"].global_transform.translation;
}

////////////////////////////// PARTICLES AUXILIARY FUNCTIONS //////////////////////////////

void reflect_direction(vec3& itv, vec3& itp, float z, vec3& normale)
{
    itv -= 2 * normale * dot(normale, itv); // reflection
    itv *= {0.95f, 0.95f, 0.85f}; // energy loss
    itp.z = z + 0.1f; // if the particle penetrated the terrain between 2 frames, get it back at the surface
}

void particles_evolution(std::list<particle_structure>& particles, bool is_new_particle, float dt)
{
    if (is_new_particle) {
        const vec3 p0 = evaluate_terrain(0.5f, 0.5f);
        // Initial speed is random. (x,y) components are uniformly distributed along a circle.
        const float theta = 2 * 3.14f * distrib(generator);
        const vec3 v0 = 2 * vec3(std::cos(theta), std::sin(theta), 5.0f);
        particles.push_back({ p0,v0 });
    }

    // Evolve position of particles
    const vec3 g = { 0.0f,0.0f,-9.81f };
    for (particle_structure& particle : particles) {
        particle.v += dt * g;
        particle.p += dt * particle.v;
    }

    // Rebound for particles on the disk at h<r=0.05f
    float u, v;
    for (auto it = particles.begin(); it != particles.end(); ++it) {
        find_uv(it->p, u, v);
        if (norm(vec2{ u-0.5f,v-0.5f }) < 0.003f) continue;
        if (u < 0 || u > 1 || v < 0 || v > 1) { // outside square, don't do rebounds and let fall
            if (it->p.z < -10)
                it = particles.erase(it);
        }
        else if (norm(it->v * vec3 { 1, 1, 0.001f }) <= 0.001f || it->p.z > 20) // very low speed: won't move again
            it = particles.erase(it);
        else {
            vec3 xyz = evaluate_terrain(u, v);
            if (it->p.z - 0.1f < xyz.z) {
                vec3 dx = (evaluate_terrain(u + 0.01f, v)-xyz);
                vec3 dy = (evaluate_terrain(u, v + 0.01f)-xyz);
                reflect_direction(it->v, it->p, xyz.z, normalize(cross(dx, dy)));
            }
            else if (it->p.z < xyz.z - 1) // somehow
                it = particles.erase(it);
        }
        if (it == particles.end()) break;
    }
}

// Initial position and speed of particles, initialization of square mesh
void scene_model::setup_chain(vec3& pApos)
{
    N_chain = 20;
    assert(N_chain >= 2);
    pA.p = pApos; // Initial position of particle A
    pA.v = { 0,0,0 }; // Initial speed of particle A
    float dx = -0.2f;
    particle_structure pB;
    for (size_t i = 0; i < N_chain; i++) {
        pB.p = vec3{ 0, 0, dx -= 0.05f } + pApos;
        pB.v = { 0,0,0 };
        pBs.push_back(pB);
    }
    L0 = 0.05f; // Rest length between A and B

    cube = mesh_primitive_parallelepiped();
    cube.uniform.transform.scaling = 0.005f;
}

// Compute spring force applied on particle pi from particle pj
vec3 spring_force(const vec3& pi, const vec3& pj, float L0, float K)
{
    float L = norm(pi - pj);
    float Fnorm = -K * (L - L0);
    vec3 direction = (pi - pj) / L;
    return Fnorm * direction;
}

void scene_model::simulate_chain(float dt)
{
    // Simulation parameters
    const float m = 0.001f;        // particle mass
    const float K = 100.0f;         // spring stiffness
    const float mu = 0.005f;       // damping coefficient
    const vec3 g = { 0,0,-9.81f }; // gravity

    // Forces
    std::vector<vec3> forces;
    {
        const vec3 fB_spring = spring_force(pBs[0].p, pA.p, 5 * L0, K) + spring_force(pBs[0].p, pBs[1].p, L0, K);
        const vec3 fB_weight = m * g;
        const vec3 fB_damping = -mu * pBs[0].v;
        const vec3 FB = fB_spring + fB_weight + fB_damping;
        forces.push_back(FB);
    }
    for (size_t i = 1; i < N_chain - 1; i++) {
        const vec3 fB_spring = spring_force(pBs[i].p, pBs[i - 1].p, L0, K) + spring_force(pBs[i].p, pBs[i + 1].p, L0, K);
        const vec3 fB_weight = m * g;
        const vec3 fB_damping = -mu * pBs[i].v;
        const vec3 FB = fB_spring + fB_weight + fB_damping;
        forces.push_back(FB);
    }
    {   const vec3 fB_spring = spring_force(pBs[N_chain - 1].p, pBs[N_chain - 2].p, L0, K);
        const vec3 fB_weight = m * g;
        const vec3 fB_damping = -mu * pBs[N_chain - 1].v;
        const vec3 FB = fB_spring + fB_weight + fB_damping;
        forces.push_back(FB);
    }
    // Numerical Integration
    for (size_t i = 0; i < N_chain; i++) {
        pBs[i].v += dt * forces[i] / m;
        pBs[i].p += dt * pBs[i].v;
    }
}

////////////////////////////// USER INTERFACE //////////////////////////////

// Mouse click is used to select a position of the control polygon
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
}

void scene_model::set_gui()
{
    ImGui::Text("Display: "); ImGui::SameLine();
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface); ImGui::SameLine();
    ImGui::Checkbox("Billboards", &gui_scene.billboards); ImGui::SameLine();
    ImGui::Checkbox("Chain", &gui_scene.chain);
    ImGui::Spacing();
    ImGui::Text("Bird: "); ImGui::SameLine();
    ImGui::Checkbox("Skeleton", &gui_scene.skeleton); ImGui::SameLine();
    ImGui::Checkbox("Keyframe", &gui_scene.display_keyframe); ImGui::SameLine();
    ImGui::Checkbox("Keyframe polygon", &gui_scene.display_polygon);
    ImGui::SliderFloat("Bird time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Bird time scale", &timer.scale, 0.1f, 1.0f);
    ImGui::Spacing();
    ImGui::SliderFloat("Balls time scale", &timer_event.scale, 0.05f, 2.0f);
    if (ImGui::Button("Stop balls")) gui_scene.stop_balls = true; ImGui::SameLine();
    if (ImGui::Button("Start balls")) gui_scene.stop_balls = false;

    if (ImGui::Button("Print Keyframe")) {
        std::cout << "keyframe_position={";
        for (auto kf: keyframes)
            std::cout << "{" << kf.p.x << "f," << kf.p.y << "f," << kf.p.z << "f}, ";
        std::cout << "}" << std::endl;
    }
}

#endif