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

//GLM version of clamp for vector math, not compatible with std::clamp
template <typename T, typename U, typename V>
inline T clampGLM(const T& x, const U& minVal, const V& maxVal)
{
	return glm::clamp(x, minVal, maxVal);
}

//Return projection in 2D orthographic space based off of window client rect size.
//Positions 2D objects in top-left origin like UI
inline mat3 Projection2D(vec2 clientRectSize)
{
	float w = clientRectSize.x;
	float h = clientRectSize.y;

	mat3 proj(1.0f);

	proj[0][0] = 2.0f / w;  //X scale
	proj[1][1] = -2.0f / h; //Y scale (flip Y so 0 is top)
	proj[2][0] = -1.0f;     //move X origin to left edge
	proj[2][1] = 1.0f;      //move Y origin to top edge

	return proj;
}

//Return projection in 2D orthographic space based off of custom viewports.
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

//Translate model in 2D orthographic space
inline mat3 Translate2D(mat3& model, vec2 pos)
{
	mat3 trans(1.0f);
	trans[2] = vec3(pos, 1.0f);
	return model * trans;
}

//Rotate model in 2D orthographic space
inline mat3 Rotate2D(mat3& model, float radians)
{
	float c = cos(radians);
	float s = sin(radians);

	mat3 rot(
		c, s, 0.0f,
		-s, c, 0.0f,
		0.0f, 0.0f, 1.0f);

	return model * rot;
}

//Scale model in 2D orthographic space
inline mat3 Scale2D(mat3& model, const vec2& scale)
{
	mat3 sc(
		scale.x, 0.0f, 0.0f,
		0.0f, scale.y, 0.0f,
		0.0f, 0.0f, 1.0f);

	return model * sc;
}

//Returns true if all values in a quat are not Nan and not Inf
inline bool IsFiniteQuat(const quat& q)
{
	return
		isfinite(q.x)
		&& isfinite(q.y)
		&& isfinite(q.z)
		&& isfinite(q.w);
}
//Returns true if all values in a vec2 are not Nan and not Inf
inline bool IsFiniteVec2(const vec2& v)
{
	return
		isfinite(v.x)
		&& isfinite(v.y);
}
//Returns true if all values in a vec3 are not Nan and not Inf
inline bool IsFiniteVec3(const vec3& v)
{
	return
		isfinite(v.x)
		&& isfinite(v.y)
		&& isfinite(v.z);
}
//Returns true if all values in a vec4 are not Nan and not Inf
inline bool IsFiniteVec4(const vec4& v)
{
	return
		isfinite(v.x)
		&& isfinite(v.y)
		&& isfinite(v.z)
		&& isfinite(v.w);
}
//Returns true if all values in a mat3 are not Nan and not Inf
inline bool IsFiniteMat3(const mat3& m)
{
	return
		IsFiniteVec3(m[0])
		&& IsFiniteVec3(m[1])
		&& IsFiniteVec3(m[2]);
}
//Returns true if all values in a mat4 are not Nan and not Inf
inline bool IsFiniteMat4(const mat4& m)
{
	return
		IsFiniteVec4(m[0])
		&& IsFiniteVec4(m[1])
		&& IsFiniteVec4(m[2])
		&& IsFiniteVec4(m[3]);
}