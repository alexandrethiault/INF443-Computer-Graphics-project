
#include "interpolation_position.hpp"
#include <fstream>

#ifdef MAIN_SCENE

using namespace vcl;

size_t index_at_value(float t, buffer<vec3t> const& v)
{
    const size_t N = v.size();
    assert(v.size() >= 2 && t >= v[0].t && t < v[N - 1].t);
    size_t k = 0;
    while (v[++k + 1].t < t) {}
    return k;
}

void cardinal_spline_interpolation(float t, buffer<vec3t>& keyframes, float mu, vec3& p, vec3& dp, vec3& d2p)
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
    p = (2 * s * s * s - 3 * s * s + 1) * p1 + (s * s * s - 2 * s * s + s) * d1 + (-2 * s * s * s + 3 * s * s) * p2 + (s * s * s - s * s) * d2;
    dp = (6 * s * s - 6 * s) * p1 + (3 * s * s - 4 * s + 1) * d1 + (-6 * s * s + 6 * s) * p2 + (3 * s * s - 2 * s) * d2;
    d2p = (12 * s - 6) * p1 + (6 * s - 4) * d1 + (-12 * s + 6) * p2 + (6 * s - 2) * d2;
}

void flight_model::setup_flight(std::map<std::string,GLuint>& shaders, scene_structure& scene)
{
    std::fstream kf("scenes/shared_assets/coords/keyframes.txt");
    int n; kf >> n;
    keyframes.resize(n);
    for (int i = 0; i < n; i++)
        kf >> keyframes[i].p.x >> keyframes[i].p.y >> keyframes[i].p.z;

    // Modify time stamps so that the particle keeps the approximate same speed 
    equalize_speed(0.6f, true);

    // Prepare the visual elements
    point_visual = mesh_primitive_parallelepiped({ -2,-2,-0.5f }, { 4,0,0 }, { 0,4,0 });
    point_visual.shader = shaders["mesh"];
    point_visual.uniform.color = {1,1,1};
    point_visual.uniform.transform.scaling = 0.04f;

    keyframe_visual = mesh_primitive_sphere();
    keyframe_visual.shader = shaders["mesh"];
    keyframe_visual.uniform.color = {1,1,1};
    keyframe_visual.uniform.transform.scaling = 0.05f;

    keyframe_picked = mesh_primitive_sphere();
    keyframe_picked.shader = shaders["mesh"];
    keyframe_picked.uniform.color = {1,0,0};
    keyframe_picked.uniform.transform.scaling = 0.055f;

    segment_drawer.init();

    trajectory = curve_dynamic_drawable(100); // Number of steps stored in the trajectory
    trajectory.uniform.color = {0,0,1};

    picked_object=-1;
}

void flight_model::draw_path(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool kf, bool pg)
{
    timer.update();
    const float t = timer.t;
    if (t<timer.t_min+0.1f) trajectory.clear();

    const float mu = 0.5f;
    cardinal_spline_interpolation(t, keyframes, mu, p, dp, d2p);

    // Store current trajectory of point p
    trajectory.add_point(p);

    // Draw current position
    float inclination = 0.2f * dot(mat3{ 0,1,0,-1,0,0,0,0,0 } * dp, d2p);
    if (inclination > 0.6f) inclination = 0.6f;
    if (inclination < -0.6f) inclination = -0.6f;
    point_visual.uniform.transform.translation = p; // à remplacer par Mario
    point_visual.uniform.transform.rotation = rotation_to_vector_mat3(dp) *
        rotation_from_axis_angle_mat3({ 1,0,0 }, inclination); // à remplacer par Mario
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
    draw(point_visual, scene.camera); // à remplacer par Mario

    // Draw moving point trajectory
    if (pg) trajectory.draw(shaders["curve"], scene.camera);

    // Draw sphere at each keyframe position
    if (kf) {
        for(size_t k=0; k<keyframes.size(); ++k) {
            keyframe_visual.uniform.transform.translation = keyframes[k].p;
            draw(keyframe_visual, scene.camera);
        }
    }

    // Draw selected sphere in red
    if (picked_object!=-1) {
        keyframe_picked.uniform.transform.translation = keyframes[picked_object].p;
        draw(keyframe_picked, scene.camera);
    }

    // Draw segments between each keyframe
    if (pg) {
        for(size_t k=0; k<keyframes.size()-1; ++k) {
            segment_drawer.uniform_parameter.p1 = keyframes[k].p;
            segment_drawer.uniform_parameter.p2 = keyframes[k + 1].p;
            segment_drawer.draw(shaders["segment_im"], scene.camera);
        }
    }
}

void flight_model::equalize_speed(float scale, bool first_time)
{
    float sumdist = keyframes[0].t = 0.0f, newdist;
    for (size_t i = 1; i < keyframes.size(); i++) {
        newdist = norm(keyframes[i].p - keyframes[i - 1].p);
        sumdist += newdist;
        keyframes[i].t = sumdist * scale;
    }
    float u = (first_time) ? 0.0f : (timer.t - timer.t_min) / (timer.t_max - timer.t_min);
    timer.t_min = keyframes[1].t;                   // first time of the keyframe
    timer.t_max = keyframes[keyframes.size() - 2].t;  // last time of the keyframe
    timer.t = u * timer.t_max + (1 - u) * timer.t_min;
}

// Mouse click is used to select a position of the control polygon
void flight_model::mouse_click(scene_structure& scene, GLFWwindow* window)
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
void flight_model::mouse_move(scene_structure& scene, GLFWwindow* window)
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
        equalize_speed(0.6f);
    }
}

#endif
