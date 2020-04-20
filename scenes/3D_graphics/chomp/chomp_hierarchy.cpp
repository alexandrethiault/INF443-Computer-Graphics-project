
#include "chomp_hierarchy.hpp"

// AVANCEMENT : Le chomp n'a pas de dents ni de palais/langue (textures spécifiques à appliquer, faire des
// demi disques pas des disques entiers pour la bouche. Les dents sont un peu en retrait dans la boule)

#ifdef SCENE_CHOMP_HIERARCHY

using namespace vcl;

const float PI = 3.1415f;
mesh mesh_primitive_half_sphere(float radius, const vec3& p0, size_t Nu, size_t Nv);

void scene_model::setup_data(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 30.0f;
    scene.camera.apply_rotation(0, 0, 0, 1.2f);

    const float radius_chomp = 3.0f;

    // The geometry of the body is a sphere
    mesh_drawable body = mesh_primitive_half_sphere(radius_chomp, { 0,0,0 }, 9, 5);
    body.uniform.color = { 0,0,0 };

    // Geometry of the eyes: black spheres
    mesh_drawable eye = mesh_primitive_disc(0.4f, { 0,0,0 }, { 1,0,0 });
    eye.uniform.color = { 1,1,1 };

    // Le chomp regarde vers x
    hierarchy_chomp.add(body, "body_up");
    hierarchy_chomp.add(body, "body_down", "body_up", { 0,0,0 });

    float theta = 3*PI / 8;
    float phi = PI / 4;
    // Eyes positions are set with respect to some ratio of the
    vec3 left{ sin(theta) * cos(phi), -sin(theta) * sin(phi), cos(theta) };
    vec3 right{ sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta) };
    mat3 identity{ 1,0,0,0,1,0,0,0,1 };
    hierarchy_chomp.add(eye, "eye_left", "body_up", { radius_chomp * left, rotation_to_vector_mat3(left) });
    hierarchy_chomp.add(eye, "eye_right", "body_up", { radius_chomp * right, rotation_to_vector_mat3(right) });
    hierarchy_chomp.add(eye, "pupil_left", "eye_left", { {radius_chomp * 0.01f,0,0}, -0.5f*identity });
    hierarchy_chomp.add(eye, "pupil_right", "eye_right", { {radius_chomp * 0.01f,0,0}, -0.5f*identity });

    hierarchy_chomp.set_shader_for_all_elements(shaders["mesh"]);

    timer.scale = 1.0f;
    timer.t_max = 5.0f;
}

void scene_model::frame_draw(std::map<std::string, GLuint>& shaders, scene_structure& scene, gui_structure&)
{
    set_gui();
    timer.update();
    const float t = timer.t;

    // Rotation of the shoulder around the y axis
    mat3 const R_face = rotation_from_axis_angle_mat3({ 0,1,0 }, -0.2f + -0.2f * std::sin(2 * 3.14f * t));
    // Symmetry in the x-direction between the left/right parts
    mat3 const Symmetry = { 1,0,0, 0,1,0, 0,0,-1 };

    // Set the rotation to the elements in the hierarchy
    hierarchy_chomp["body_up"].transform.rotation = R_face;
    hierarchy_chomp["body_down"].transform.rotation = Symmetry * R_face * R_face;
    hierarchy_chomp.update_local_to_global_coordinates();

    if (gui_scene.surface) draw(hierarchy_chomp, scene.camera);
    if (gui_scene.wireframe) draw(hierarchy_chomp, scene.camera, shaders["wireframe"]);
}

mesh mesh_primitive_half_sphere(float radius, const vec3& p0, size_t Nu, size_t Nv)
{
    mesh shape;
    for (size_t ku = 0; ku < Nu; ++ku) {
        for (size_t kv = 0; kv < Nv; ++kv) {
            const float u = static_cast<float>(ku) / static_cast<float>(Nu - 1);
            const float v = static_cast<float>(kv) / static_cast<float>(Nv - 1);

            // Angles
            const float theta = static_cast<float>(0.5f * 3.14159f * v);
            const float phi = static_cast<float>(2 * 3.14159f * u);

            // Spherical coordinates
            const float x = radius * std::sin(theta) * std::cos(phi);
            const float y = radius * std::sin(theta) * std::sin(phi);
            const float z = radius * std::cos(theta);

            const vec3 p = { x,y,z };      // Position (centered)
            const vec3 n = normalize(p); // Normal
            const vec2 uv = { v,u };        // Texture-coordinates

            shape.position.push_back(p + p0); // Add new position (with translation of the center)
            shape.normal.push_back(n);
            shape.texture_uv.push_back(uv);
        }
    }
    shape.connectivity = connectivity_grid(Nu, Nv, false, false);
    return shape;
}

void scene_model::set_gui()
{
    ImGui::Text("Display: "); ImGui::SameLine();
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface);     ImGui::SameLine();

    ImGui::Spacing();
    ImGui::SliderFloat("Time", &timer.t, timer.t_min, timer.t_max);
    ImGui::SliderFloat("Time scale", &timer.scale, 0.1f, 3.0f);

}

#endif

