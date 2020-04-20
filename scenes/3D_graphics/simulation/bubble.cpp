
#include "bubble.hpp"

#include <random>

// AVANCEMENT : il reste à trouver comment rendre les bulles transparentes

#ifdef SCENE_BUBBLE

using namespace vcl;

// Generator for uniform random number
std::default_random_engine generator;
std::uniform_real_distribution<float> distrib(0.0,1.0); // en vrai, les bulles doivent viser mario
void squish_tranformation(bubble_structure& bubble, mesh_drawable& sphere, float bubble_radius);

void scene_model::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& gui)
{
    scene.camera.camera_type = camera_control_spherical_coordinates;
    scene.camera.scale = 30.0f;
    scene.camera.apply_rotation(0, 0, 0, 1.2f);

    gui.show_frame_worldspace = true;
    gui.show_frame_camera = false;

    // Create mesh for particles represented as spheres
    bubble_radius = 0.2f;
    sphere = mesh_primitive_sphere(bubble_radius);
    sphere.uniform.color = {0.6f, 0.6f, 1.0f};

    // Create mesh for the ground displayed as a disc
    demo_ground = mesh_primitive_disc(5.0f, { 0,0,0 }, { 0,0,1 }, 80);
    demo_ground.uniform.color = {1,1,1};

    // Delay between emission of a new particles
    timerevent.periodic_event_time_step = 5;
}

void scene_model::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    const float dt = timerevent.update(); // dt: Elapsed time between last frame
    set_gui();
    simulate_particles(dt);

    // Display particles
    for(bubble_structure& particle : particles) {
        squish_tranformation(particle, sphere, bubble_radius);
        draw(sphere, scene.camera, shaders["mesh"]);
    }

    // Display ground
    draw(demo_ground, scene.camera, shaders["mesh"]);
}

void squish_tranformation(bubble_structure& bubble, mesh_drawable& sphere, float bubble_radius) {
    sphere.uniform.transform.scaling_axis = { 1+bubble.squish, 1+bubble.squish, 1.0f - bubble.squish };
    sphere.uniform.transform.translation = bubble.p - bubble_radius*vec3{0,0, bubble.squish };
}

void scene_model::simulate_particles(float dt) {
    // Emission of new particle if needed
    const bool is_new_particle = timerevent.event;
    if (is_new_particle) {
        const vec3 p0 = { 0,0,3 };
        // Initial speed is random. (x,z) components are uniformly distributed along a circle.
        const float theta = 2 * 3.14f * distrib(generator);
        const vec3 v0 = vec3(std::cos(theta), std::sin(theta), 0.0f);
        particles.push_back(bubble_structure(p0, v0));
    }

    // Evolve position of particles
    const vec3 g = { 0.0f,0.0f,-9.81f };
    for (bubble_structure& particle : particles) {
        if (particle.squishing || particle.unsquishing) continue;
        const float m = 0.01f; // particle mass
        const vec3 F = m * g;
        particle.v = particle.v + dt * F / m;
        particle.p = particle.p + dt * particle.v;
    }

    // Rebound for particles on the disk at h<r=0.05f
    for (auto it = particles.begin(); it != particles.end(); ++it) {
        float normxy = it->p.x * it->p.x + it->p.y * it->p.y;
        if (it->p.z < bubble_radius && normxy < 25.0f) {
            if (!it->squishing && !it->unsquishing) {
                it->squish_counter++;
                it->squishing = true;
                it->squish = 0.01f;
            }
            else if (it->squishing) {
                it->squish += 0.01f;
                if (it->squish > 0.5f) {
                    it->squish = 0.5;
                    it->squishing = false;
                    it->unsquishing = true;
                    if (it->squish_counter == 3)
                        it->p.z = -100; // sera effacé proprement à la fin de l'itération
                }
            }
            else if (it->unsquishing) {
                it->squish -= 0.02f;
                if (it->squish < 0) {
                    it->unsquishing = false;
                    it->squish = 0;
                    it->p.z = 2 * bubble_radius - it->p.z;
                    it->v.z *= -0.6f;
                }
            }
        }
    }

    // Remove particles that are too low
    for (auto it = particles.begin(); it != particles.end(); ++it)
        if (it->p.z < -3) {
            it = particles.erase(it);
            if (it == particles.end()) break;
        }
}

void scene_model::set_gui()
{
    ImGui::Checkbox("Wireframe", &gui_scene.wireframe); ImGui::SameLine();
    ImGui::Checkbox("Surface", &gui_scene.surface); ImGui::SameLine();
    ImGui::SliderFloat("Time scale", &timerevent.t, 0.2f, 5);

    // Start and stop animation
    if (ImGui::Button("Stop")) timerevent.stop();
    if (ImGui::Button("Start")) timerevent.start();
}


#endif
