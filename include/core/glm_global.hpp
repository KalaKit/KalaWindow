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

//Return projection in 2D orthographic space based off of window client rect size.
//Positions 2D objects in top-left origin like UI
inline mat3 Projection2D(vec2 viewportSize)
{
	vec2 vp = clamp(viewportSize, vec2(1.0f), vec2(10000.0f));

	float w = vp.x;
	float h = vp.y;

	mat3 proj(1.0f);

	proj[0][0] = 2.0f / w;  //X scale
	proj[1][1] = -2.0f / h; //Y scale (flip Y so 0 is top)
	proj[2][0] = -1.0f;     //move X origin to left edge
	proj[2][1] = 1.0f;      //move Y origin to top edge

	return proj;
}

//Return projection in 2D orthographic space based off of custom viewport size.
//Positions 2D objects in top-left origin like UI
inline mat3 Projection2D(
	float left,
	float right,
	float top,
	float bottom)
{
	mat3 proj(1.0f);

	proj[0][0] = 2.0f / (right - left);
	proj[1][1] = 2.0f / (top - bottom); //flip Y
	proj[2][0] = -(right + left) / (right - left);
	proj[2][1] = -(top + bottom) / (top - bottom);

	return proj;
}

//Translate model in 2D orthographic space,
//should be first above Rotate2D and Scale2D
inline mat3 Translate2D(mat3& model, vec2 pos)
{
	vec2 pc = clamp(pos, vec2(-10000.0f), vec2(10000.0f));

	mat3 posMat(1.0f);
	posMat[2] = vec3(pc, 1.0f);
	return model * posMat;
}

//Rotate model in 2D orthographic space,
//should be between Translate2D and Scale2D
inline mat3 Rotate2D(mat3& model, float rot)
{
	float rc = radians(fmod(rot, 360));

	float c = cos(rc);
	float s = sin(rc);

	mat3 rotMat(
		c, s, 0.0f,
		-s, c, 0.0f,
		0.0f, 0.0f, 1.0f);

	return model * rotMat;
}

//Scale model in 2D orthographic space,
//should be last after Translate2D and Rotate2D
inline mat3 Scale2D(mat3& model, vec2 size)
{
	vec2 sc = clamp(size, vec2(1.0f), vec2(10000.0f));

	mat3 sizeMat(
		sc.x, 0.0f, 0.0f,
		0.0f, sc.y, 0.0f,
		0.0f, 0.0f, 1.0f);

	return model * sizeMat;
}