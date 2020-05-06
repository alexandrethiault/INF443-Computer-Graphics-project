
#include "bubble.hpp"
#include <random>

#ifdef MAIN_SCENE

using namespace vcl;

// Generator for uniform random number
std::default_random_engine generator_bubbles;
std::uniform_real_distribution<float> distrib_bubbles(0.0f,1.0f);

void bubbles_structure::setup()
{
    // Create mesh for particles represented as spheres
    radius = 0.1f;
    mesh _sphere = mesh_primitive_sphere(radius);
    for (vec2& uv : _sphere.texture_uv) uv = { uv.y, uv.x };
    sphere = _sphere;
    sphere.uniform.shading.ambiant = 0.5f;

    // Delay between emission of a new particles
    timerevent.periodic_event_time_step = 5;

    texture_bubble = create_texture_gpu(image_load_png("scenes/shared_assets/textures/bubble.png"));
}

void bubbles_structure::draw_bubbles(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool surf, bool wf)
{
    glBindTexture(GL_TEXTURE_2D, texture_bubble);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    for(bubble_structure& position : positions) {
        position.squish_tranformation(sphere, radius);
        if (surf) draw(sphere, scene.camera, shaders["mesh"]);
        if (wf) draw(sphere, scene.camera, shaders["wireframe"]);
    }
}

void bubbles_structure::simulate() {
    const float dt = timerevent.update();

    // Emission of new particle if needed
    if (timerevent.event) {
        vec3 p0 = vec3{ 0.34f,6.32f,6.04f }; // r = 0.7f
        vec3 v0 = vec3{ distrib_bubbles(generator_bubbles)-0.5f, distrib_bubbles(generator_bubbles)-0.5f, 0.0f } * 5;
        positions.push_back(bubble_structure(p0, v0));
    }

    // Evolve position of particles
    const vec3 g = { 0.0f,0.0f,-9.81f };
    for (bubble_structure& position : positions) {
        if (position.squishing || position.unsquishing) continue;
        const float m = 0.01f; // Particle mass
        const float f = 0.015f; // Friction
        const vec3 F = m * g;
        position.v = position.v + dt * (m * g - f * position.v) / m;
        position.p = position.p + dt * position.v;
    }

    // Rebound for particles hitting the ground
    for (auto it = positions.begin(); it != positions.end(); ++it) {
        if (it->p.z < radius + 1.04f) {
            if (!it->squishing && !it->unsquishing) {
                it->squish_counter++;
                it->squishing = true;
                it->squish = 0.5f * dt;
            }
            else if (it->squishing) {
                it->squish += 0.7f * dt;
                if (it->squish > 0.5f) {
                    it->squish = 0.5f;
                    it->squishing = false;
                    it->unsquishing = true;
                    if (it->squish_counter == 3)
                        it->p.z = -10; // Will be properly deleted at the end of the iteration
                }
            }
            else if (it->unsquishing) {
                it->squish -= 4 * 0.7f * dt; // Unsquishing 4 times quicker than squishing
                if (it->squish < 0) {
                    it->unsquishing = false;
                    it->squish = 0;
                    it->p.z = 2 * (radius + 1.04f) - it->p.z;
                    it->v.z *= -0.6f;
                }
            }
        }
    }

    // Properly remove particles that hit their 3rd rebound
    for (auto it = positions.begin(); it != positions.end(); ++it)
        if (it->p.z < -1) {
            it = positions.erase(it);
            if (it == positions.end()) break;
        }
}

#endif
