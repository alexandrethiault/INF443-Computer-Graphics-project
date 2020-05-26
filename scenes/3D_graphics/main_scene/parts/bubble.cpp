
#include "bubble.hpp"
#include <random>

#ifdef MAIN_SCENE

using namespace vcl;

// Generator for uniform random number
std::default_random_engine generator_bubbles;
std::uniform_real_distribution<float> distrib_bubbles(0.0f,1.0f);

bubble_structure::bubble_structure() {}

bubble_structure::bubble_structure(vec3 p, vec3 v)
{
    this->p = p; this->v = v;
}

void bubble_structure::squish_tranformation(mesh_drawable& sphere, float bubble_radius)
{
    sphere.uniform.transform.scaling_axis = { std::sqrt(1/(1 - squish)), std::sqrt(1 / (1 - squish)), 1 - squish };
    sphere.uniform.transform.translation = p - bubble_radius * vcl::vec3{ 0,0, squish };
}

void bubbles_structure::setup(map_structure* map_)
{
    radius = 0.1f;

    mesh _sphere = mesh_primitive_sphere(radius);
    for (vec2& uv : _sphere.texture_uv) uv = { uv.y, uv.x };
    sphere = _sphere;
    sphere.uniform.shading.ambiant = 0.5f;

    map = map_;

    // Delay between emission of a new particles
    timerevent.periodic_event_time_step = 6;

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

void bubbles_structure::simulate(vec3& target) {
    const float dt = timerevent.update();

    // Emission of new particle if needed
    if (timerevent.event && !active) {
        vec3 p0 = vec3{ 1.3f, 5.7f, 6.0f };
        vec3 v0 = vec3{ distrib_bubbles(generator_bubbles) - 0.5f, distrib_bubbles(generator_bubbles) - 0.5f, 0.0f } *8;
        if (norm((target - p0) * vec3 { 1, 1, 0 }) < 2) // target the pink bobomb
            v0 = vec3{ target.x - p0.x, target.y - p0.y, 0.0 } * 1.84f;
        bubble = bubble_structure(p0, v0);
        active = true;
    }

    // Evolve position of particle
    const vec3 g = { 0.0f,0.0f,-9.81f };
    if (active && !bubble.squishing && !bubble.unsquishing) {
        const float m = 0.01f; // Particle mass
        const float f = 0.015f; // Friction
        const vec3 F = m * g - f * bubble.v;
        bubble.v = bubble.v + dt * F / m;
        bubble.p = bubble.p + dt * bubble.v;
    }

    // Rebound for particle hitting the ground
    vec3 impact, normal;
    if (active) {
        if (map->wall_collision(bubble.p, impact, normal, radius)) {
            bubble.p = impact + normal * radius;
            bubble.v -= 2 * dot(bubble.v, normal) * normal;
        }
        if (bubble.v.z < 0.f && map->ground_collision(bubble.p - vec3{ 0,0,radius }, impact, normal)) {
            if (!bubble.squishing && !bubble.unsquishing) {
                bubble.p = impact + vec3{ 0,0,radius-0.01f };
                bubble.squish_counter++;
                bubble.squishing = true;
                bubble.squish = 0.7f * dt;
            }
            else if (bubble.squishing) {
                bubble.squish += (0.7f + 1.4f * (bubble.squish_counter == 3)) * dt; // Faster 3rd squish
                if (bubble.squish > 0.5f + 0.3f * (bubble.squish_counter == 3)) { // Bigger 3rd squish
                    bubble.squish = 0.5f + 0.3f * (bubble.squish_counter == 3);
                    bubble.squishing = false;
                    bubble.unsquishing = true;
                    if (bubble.squish_counter == 3)
                        active = false; // Basically delete the bubble until a new one is created
                }
            }
            else if (bubble.unsquishing) {
                bubble.squish -= 4 * 0.7f * dt; // Unsquishing 4 times quicker than squishing
                if (bubble.squish < 0) {
                    bubble.unsquishing = false;
                    bubble.squish = 0;
                    if (bubble.squish_counter == 1) bubble.v *= 0.8f; // loss of energy
                    bubble.v -= 2 * dot(normal, bubble.v) * normal; // reflexion /normal
                    if (norm((target - bubble.p) * vec3 { 1, 1, 0 }) < 2) { // change direction to the target
                        vec3 direction = normalize(target - bubble.p) * std::min(norm(target - bubble.p), 2*norm(bubble.v));
                        bubble.v.x = direction.x;
                        bubble.v.y = direction.y;
                    }
                }
            }
        }
    }
}

#endif
