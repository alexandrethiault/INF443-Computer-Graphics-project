
#include "bubble.hpp"
#include <random>

#ifdef MAIN_SCENE

using namespace vcl;

// Generator for uniform random number
std::default_random_engine generator_bubbles;
std::uniform_real_distribution<float> distrib_bubbles(0.0f,1.0f);

void bubbles_structure::setup(map_structure* map_)
{
    active = false;

    // Create mesh for particles represented as spheres
    radius = 0.1f;
    mesh _sphere = mesh_primitive_sphere(radius);
    for (vec2& uv : _sphere.texture_uv) uv = { uv.y, uv.x };
    sphere = _sphere;
    sphere.uniform.shading.ambiant = 0.5f;

    map = map_;

    // Delay between emission of a new particles
    timerevent.periodic_event_time_step = 5;

    texture_bubble = create_texture_gpu(image_load_png("scenes/shared_assets/textures/bubble.png"));
}

void bubbles_structure::draw_bubbles(std::map<std::string,GLuint>& shaders, scene_structure& scene, bool surf, bool wf)
{
    glBindTexture(GL_TEXTURE_2D, texture_bubble);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (active) {
        bubble.squish_tranformation(sphere, radius);
        if (surf) draw(sphere, scene.camera, shaders["mesh"]);
        if (wf) draw(sphere, scene.camera, shaders["wireframe"]);
    }
}

void bubbles_structure::simulate() {
    const float dt = timerevent.update();

    // Emission of new particle if needed
    if (timerevent.event) {
        vec3 p0 = vec3{ 1.3f, 5.7f, 6.0f }; // r = 0.7f
        vec3 v0 = vec3{ distrib_bubbles(generator_bubbles)-0.5f, distrib_bubbles(generator_bubbles)-0.5f, 0.0f } * 8;
        bubble = bubble_structure(p0, v0);
        active = true;
    }

    // Evolve position of particle
    const vec3 g = { 0.0f,0.0f,-9.81f };
    if (active && !bubble.squishing && !bubble.unsquishing) {
        const float m = 0.01f; // Particle mass
        const float f = 0.015f; // Friction
        const vec3 F = m * g;
        bubble.v = bubble.v + dt * (m * g - f * bubble.v) / m;
        bubble.p = bubble.p + dt * bubble.v;
    }

    // Rebound for particle hitting the ground
    vec3 impact, normal;
    if (active) {
        if (map->collision(bubble.p - vec3{ 0, 0, radius }, impact, normal, 0.1f) ||
            map->collision(bubble.p, impact, normal)) { // 2nd test to avoid phasing through corners
            if (!bubble.squishing && !bubble.unsquishing) {
                bubble.p = impact + vec3{ 0,0,radius-0.01f };
                bubble.squish_counter++;
                bubble.squishing = true;
                bubble.squish = 0.5f * dt;
            }
            else if (bubble.squishing) {
                bubble.squish += 0.7f * dt;
                if (bubble.squish > 0.5f) {
                    bubble.squish = 0.5f;
                    bubble.squishing = false;
                    bubble.unsquishing = true;
                    if (bubble.squish_counter == 3)
                        bubble.p.z = -10; // Will be properly deleted at the end of the iteration
                }
            }
            else if (bubble.unsquishing) {
                bubble.squish -= 4 * 0.7f * dt; // Unsquishing 4 times quicker than squishing
                if (bubble.squish < 0) {
                    bubble.unsquishing = false;
                    bubble.squish = 0;
                    bubble.v.z *= 0.75f; // loss of energy
                    bubble.v -= 2 * dot(normal, bubble.v) * normal; // reflexion /normal
                }
            }
        }
    }

    // Lazily remove particle that hit its 3rd rebound
    if (active && bubble.p.z < -5)
        active = false;
}

#endif
