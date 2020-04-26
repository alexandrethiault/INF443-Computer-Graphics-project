#pragma once

#include "../../vec/vec.hpp"
#include "../../mat/mat.hpp"


namespace vcl
{

/** Return matrix corresponding to a 3D rotation parameterized by an axis and an angle
    ex. rotation_from_axis_angle_mat3({1,0,0}, 3.14f/2) // rotation of pi/2 around the x-axis
    The axis is normalized in the function.
*/
mat3 rotation_from_axis_angle_mat3(const vec3& axis, float angle);

/** Return a matrix of rotation R such that b = R a
 * a and b are normalized in the function
 * R is defined by:
 *   - the rotation-axis:  n = a x b
 *   - the rotation-angle: theta = acos( a.b )
 * Note that R is generally not the unique possible rotation */
mat3 rotation_between_vector_mat3(const vec3& a, const vec3& b);

/** Return a matrix of rotation R such that 
 * - a = R * mat3{1, 0, 0}
 * - the transformed y axis has no z component
 * - the transformed z axis has positive z component
 * Useful to apply on animated objects that must 
 * "keep their eyes on the same horizontal plane".
 * If a.x != 0 or a.y != 0 the matrix is unique
 * If a.x == 0 and a.y == 0 the matrix returned is
 * (0 0 1)
 * (0 1 0)
 * (1 0 0) */
mat3 rotation_to_vector_mat3(const vec3& a);
}
