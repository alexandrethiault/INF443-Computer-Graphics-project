
#include "triangle.hpp"

#ifdef MAIN_SCENE

using namespace vcl;

float triangle::ground_collision_depth = 0.3f;
float triangle::ground_collision_stick = 0.01f;
float triangle::wall_collision_depth = 0.02f;

triangle::triangle(vec3 p1, vec3 p2, vec3 p3, vec3 fakenormal)
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

// https://www.youtube.com/watch?v=UnU7DJXiMAQ
bool triangle::collision(vec3 position, vec3& impact, vec3& normal, float margin)
{
    if (n.z >= 0.3f) { // Triangle is considered ground, hitboxes are vertical
        impact.x = position.x;
        impact.y = position.y;
        impact.z = (d - impact.x * n.x - impact.y * n.y) / n.z;
        float distance_to_plane = position.z - impact.z;
        if (distance_to_plane < -ground_collision_depth || distance_to_plane > ground_collision_stick) return false;
        normal = n;
        return (cross(p2 - p1, impact - p1).z > 0 && cross(p3 - p2, impact - p2).z > 0 && cross(p1 - p3, impact - p3).z > 0);
    }
    else if (n.z > -0.01f) { // Triangle is considered wall (I chose to classify steep ground as walls)
        float distance_to_plane = dot(position, n) - d;
        if (distance_to_plane < -wall_collision_depth || distance_to_plane > margin) return false;
        impact = position - distance_to_plane * n;
        normal = n;
        return (dot(cross(p2 - p1, impact - p1), n) > 0 && dot(cross(p3 - p2, impact - p2), n) > 0 && dot(cross(p1 - p3, impact - p3), n) > 0);
    }
    else { // Triangle is considered ceiling
        impact.x = position.x;
        impact.y = position.y;
        impact.z = (d - impact.x * n.x - impact.y * n.y) / n.z;
        float distance_to_plane = position.z - impact.z;
        if (distance_to_plane < -margin || distance_to_plane > 0) return false;
        normal = n;
        return (cross(p2 - p1, impact - p1).z < 0 && cross(p3 - p2, impact - p2).z < 0 && cross(p1 - p3, impact - p3).z < 0);
    }

}

#endif
