
#include "articulated_hierarchy.hpp"

// AVANCEMENT : ben en gros on a mon code du TD là donc c'est un oiseau pour l'instant.
// convention : axe z = vertical, axe x = direction du regard par défaut !

#ifdef SCENE_ARTICULATED_HIERARCHY

using namespace vcl;

void scene_model::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 30.0f;
    scene.camera.apply_rotation(0, 0, 0, 1.2f);

    const float radius_body = 0.25f;
    const float radius_head = 0.2f;
    const float length_arm = 0.2f;
    const float breadth_arm = length_arm * 1.2f;

    // The geometry of the body is a sphere
    mesh_drawable body = mesh_drawable( mesh_primitive_sphere(radius_body, {0,0,0}, 40, 40));
    body.uniform.transform.scaling_axis = vec3(2, 1, 1);
    body.uniform.shading.specular = 20.0f;
    mesh_drawable head = mesh_drawable(mesh_primitive_sphere(radius_head, {0,0,0}, 40, 40));
    head.uniform.shading.specular = 20.0f;

    // Geometry of the eyes: black spheres
    mesh_drawable eye = mesh_drawable(mesh_primitive_sphere(0.04f, {0,0,0}, 20, 20));
    eye.uniform.color = {0,0,0};

    mesh_drawable nose = mesh_drawable(mesh_primitive_cone(radius_head * 0.4f, { 0,0,0 }, { 0.2f,0,0 }, 20, 10));
    nose.uniform.color = { 1, 0.5f, 0 };

    // Shoulder part and arm are displayed as cylinder
    //mesh_drawable shoulder = mesh_primitive_cylinder(radius_arm, {0,0,0}, {-length_arm,0,0});
    //mesh_drawable arm = mesh_primitive_cylinder(radius_arm, {0,0,0}, {-length_arm/1.5f,-length_arm/1.0f,0});
    mesh_drawable feather_1 = mesh_primitive_quad({ breadth_arm, 0, 0 }, { -breadth_arm, 0, 0 }, { -breadth_arm, -length_arm, 0 }, { breadth_arm, -length_arm, 0 });
    mesh_drawable feather_2 = mesh_primitive_quad({ breadth_arm, 0, 0 }, { -breadth_arm, 0, 0 }, { 0, -length_arm, 0 }, { length_arm , -length_arm, 0 });

    // Build the hierarchy:
    // hierarchy.add(visual_element, element_name, parent_name, (opt)[translation, rotation])
    hierarchy.add(body, "body");

    hierarchy.add(head, "head", "body", radius_body * vec3(2, 0, 1));

    // Eyes positions are set with respect to some ratio of the
    hierarchy.add(eye, "eye_left", "head" , radius_head * vec3(2 / 3.0f, 1/3.0f, 1/2.0f));
    hierarchy.add(eye, "eye_right", "head", radius_head * vec3(2 / 3.0f , -1/3.0f, 1/2.0f));
    hierarchy.add(nose, "nose", "head", radius_head * vec3(0.9f, 0, 0));

    hierarchy.add(feather_1, "f1_left", "body", { 0,-radius_body + 0.05f,0 }); // extremity of the spherical body
    hierarchy.add(feather_2, "f2_left", "f1_left", { 0,-length_arm,0 });     // place the elbow the extremity of the "shoulder cylinder"
    hierarchy.add(feather_1, "f1_right", "body", { {0,radius_body - 0.05f,0}, {1,0,0, 0,-1,0, 0,0,1} });
    hierarchy.add(feather_2, "f2_right", "f1_right", { 0,-length_arm,0 });

    // Set the same shader for all the elements
    hierarchy.set_shader_for_all_elements(shaders["mesh"]);

    // Initialize helper structure to display the hierarchy skeleton
    hierarchy_visual_debug.init(shaders["segment_im"], shaders["mesh"]);

    timer.scale = 1.0f;
    timer.t_max = 5.0f;
}

void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    set_gui();
    timer.update();
    const float t = timer.t;

    // Rotation of the shoulder around the y axis
    mat3 const R_face = rotation_from_axis_angle_mat3({ 0,1,0 }, 0.2f + 0.1f*std::sin(2 * 3.14f * t));
    // Rotation of the shoulder around the y axis
    mat3 const R_feather1 = rotation_from_axis_angle_mat3({1,0,0}, std::sin(4 * 3.14f*(t-0.4f)) );
    // Rotation of the arm around the y axis (delayed with respect to the shoulder)
    mat3 const R_feather2 = rotation_from_axis_angle_mat3({1,0,0}, std::sin(4 * 3.14f*(t-0.6f)) );
    // Symmetry in the x-direction between the left/right parts
    mat3 const Symmetry = {1,0,0, 0,-1,0, 0,0,1};

    // Set the rotation to the elements in the hierarchy
    hierarchy["head"].transform.rotation = R_face;
    hierarchy["f1_left"].transform.rotation = R_feather1;
    hierarchy["f2_left"].transform.rotation = R_feather2;

    hierarchy["f1_right"].transform.rotation = Symmetry * R_feather1; // apply the symmetry
    hierarchy["f2_right"].transform.rotation = R_feather2; //note that the symmetry is already applied by the parent element

    hierarchy.update_local_to_global_coordinates();

    if(gui_scene.surface) draw(hierarchy, scene.camera);
    if(gui_scene.wireframe) draw(hierarchy, scene.camera, shaders["wireframe"]);
    if(gui_scene.skeleton) hierarchy_visual_debug.draw(hierarchy, scene.camera);
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

