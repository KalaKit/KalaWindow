//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <cmath>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL //for glm::decompose
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/norm.hpp"

using std::isfinite;
using std::fabs;
using std::min;
using std::max;
using std::numeric_limits;
using std::acos;
using std::sqrt;
using std::sin;
using std::cos;
using std::tan;
using std::clamp;

using glm::vec2; using glm::dvec2; using glm::ivec2;
using glm::vec3; using glm::dvec3; using glm::ivec3;
using glm::vec4; using glm::dvec4; using glm::ivec4;

using glm::mat2; using glm::dmat2;
using glm::mat3; using glm::dmat3;
using glm::mat4; using glm::dmat4;

using glm::translate;
using glm::scale;
using glm::rotate;
using glm::radians;
using glm::ortho;
using glm::perspective;
using glm::value_ptr;
using glm::quat;
using glm::quat_cast;
using glm::mat3_cast;
using glm::mat4_cast;
using glm::lookAt;
using glm::pi;
using glm::decompose;
using glm::degrees;
using glm::dot;
using glm::length;
using glm::length2;
using glm::normalize;
using glm::cross;
using glm::angleAxis;
using glm::inverse;

//Return projection in 2D orthographic space based on a viewport region.
//Top left is (0, 0), Y increases up. Your 2D UI vertices should be top-left and Y-up
inline mat3 Projection2D(const vec2 viewportSize)
{
	vec2 vpClamped = clamp(viewportSize, vec2(1.0f), vec2(10000.0f));

	return mat3(
		2.0f / vpClamped.x, 0.0f, 0.0f,
		0.0f, -2.0f / vpClamped.y, 0.0f,
		-1.0f, 1.0f, 1.0f);
}

//Return matrix in 2D orthographic space based on position, degree rotation and size
inline mat3 Matrix2D(
	const vec2 pos,
	const float rotDeg,
	const vec2 size)
{
	vec2 posClamped = clamp(pos, vec2(-10000.0f), vec2(10000.0f));
	float rotClamped = fmod(rotDeg, 360.0f);
	vec2 sizeClamped = clamp(size, vec2(-10000.0f), vec2(10000.0f));

	float r = radians(rotClamped);
	float c = cos(r);
	float s = sin(r);

	return mat3(
		sizeClamped.x * c, sizeClamped.y * s, 0.0f,
		-sizeClamped.x * s, sizeClamped.y * c, 0.0f,
		posClamped.x, posClamped.y, 1.0f);
}