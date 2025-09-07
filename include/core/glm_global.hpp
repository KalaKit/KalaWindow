//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;

using glm::ivec2;
using glm::ivec3;
using glm::ivec4;

using glm::mat2;
using glm::mat3;
using glm::mat4;

using glm::translate;
using glm::scale;
using glm::rotate;
using glm::radians;
using glm::ortho;
using glm::perspective;
using glm::value_ptr;
using glm::quat;
using glm::mat4_cast;
using glm::lookAt;
using glm::pi;

//Return projection in 2D orthrographic space based off of window client rect size.
//Positions 2D objects in top-left origin like UI
inline mat3 Projection2D(vec2 clientRectSize)
{
	float left = 0.0f;
	float right = clientRectSize.x;
	float top = 0.0f;
	float bottom = clientRectSize.y;

	mat3 proj(1.0f);

	proj[0][0] = 2.0f / (right - left);
	proj[1][1] = 2.0f / (top - bottom); //flip Y
	proj[2][0] = -(right + left) / (right - left);
	proj[2][1] = -(top + bottom) / (top - bottom);

	return proj;
}

//Return projection in 2D orthrographic space based off of custom viewports.
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