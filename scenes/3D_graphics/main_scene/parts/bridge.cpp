
#include "bridge.hpp"

// AVANCEMENT : le mouvement sinusoidal est le seul truc qui est fait. Même la modélisation reste à faire

#ifdef MAIN_SCENE

using namespace vcl;

void bridge_structure::setup(vec3& _center, vec3& len, vec3& wid, vec3& hei)
{
    center = _center;
    rotation_axis = wid;
    period = 5; // Period of residual movement
    thetamax = 3.14159f / 50; // Maximum angle of residual movement
    theta = 0; // Residual angle = thetamax*sin(2*pi*t/period)
    mg = 0.5f; // mass*gravity, considered constant for bobombs
    kappa = 1.0f; // torsion coefficient for tau : the lowest it is, the more the bridge will oscillate (conservative)
    invL = 1.0f; // All that matters is mg*invL and kappa*invL so this one is quite useless
    f = 0.1f; // The biggest f is, the strongest resistance to oscillation will be
    mesh _bridge = mesh_primitive_parallelepiped(-len/2-wid/2, len, wid, hei);
    _bridge.texture_uv = {
        {0,0},{5,0},{5,1},{0,1},
        {5,0},{5.33f,0},{5.33f,1},{5,1},
        {5,1},{5,0},{0,0},{0,1},
        {0,0},{0.33f,0},{0.33f,1},{0,1},
        {0.33f,1},{0.33f,0},{5.33f,0},{5.33f,1},
        {5.33f,0},{0.33f,0},{0.33f,1},{5.33f,1}
    };
    bridge = _bridge; // conversion mesh -> mesh_drawable
    bridge.uniform.transform.translation = center;
    bridge.uniform.shading = { 0.7f, 0.3f, 0.0f };
    texture_bridge = create_texture_gpu(image_load_png("scenes/shared_assets/textures/wood.png"));
    for (uint3 tri : _bridge.connectivity) {
        vec3 p1 = _bridge.position[tri[0]];
        vec3 p2 = _bridge.position[tri[1]];
        vec3 p3 = _bridge.position[tri[2]];
        vec3 fakenormal = _bridge.normal[tri[0]];
        triangles0.push_back(triangle(p1, p2, p3, fakenormal));
    }
    minx = center.x + std::min(len.x, std::min(hei.x, wid.x)) - 1.0f;
    maxx = center.x + std::max(len.x, std::max(hei.x, wid.x)) + 1.0f;
    miny = center.y + std::min(len.y, std::min(hei.y, wid.y)) - 1.0f;
    maxy = center.y + std::max(len.y, std::max(hei.y, wid.y)) + 1.0f;
}

void bridge_structure::move(float t, float dt)
{
    // Momentums: sigma, tau.
    // sigma = incremented in ground_collision, positive or negative depending on where thing(s) stand on the bridge
    // tau = torsion-spring torque: angle caused by momentum wants to go back to 0 (conservative)
    // Angular momentum theorem: L*d2phi = sigma+tau (L is a constant)

    tau = -kappa * phi;
    d2phi = (sigma + tau) * invL - f * dphi;
    dphi += d2phi * dt;
    phi += dphi * dt;
    theta = thetamax * std::sin(2 * 3.14159f * t / period);

    mat3 R = rotation_from_axis_angle_mat3(rotation_axis, theta+phi);
    bridge.uniform.transform.rotation = R;
    triangles.clear();
    for (triangle& tri : triangles0)
        triangles.push_back(triangle(R*tri.p1+center, R*tri.p2+center, R*tri.p3+center, tri.n));
    
    sigma = 0.0f; // To be recomputed next time

    // Debug : faire peser un objet de manière permanente à gauche
    // vec3 impact, normal;
    // ground_collision(R*vec3{ 0.63f,0.63f,0 }+center, impact, normal);
}

void bridge_structure::draw_bridge(std::map<std::string, GLuint>& shaders, scene_structure& scene, bool surf, bool wf)
{
    glBindTexture(GL_TEXTURE_2D, texture_bridge);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (surf) draw(bridge, scene.camera, shaders["mesh"]);
    if (wf) draw(bridge, scene.camera, shaders["wireframe"]);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}

bool bridge_structure::ground_collision(vcl::vec3 position, vcl::vec3& impact, vcl::vec3& normal)
{
    if (position.x < minx || position.x > maxx || position.y < miny || position.y > maxy)
        return false;
    for (triangle& tri : triangles)
        if (tri.n.z > 0.01f && tri.collision(position, impact, normal)) {
            sigma += cross(impact-center, rotation_axis).z * mg; // Momentum = distance to axis * force (gravity)
            return true;
        }
    return false; // impact may have been modified anyway in the process
}

#endif
