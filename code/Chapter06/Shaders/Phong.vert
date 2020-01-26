// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// Uniforms for world transform and view-proj
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// Attribute 0 is position, 1 is normal, 2 is tex coords.
layout(location=0) in vec3 inPosition;		//locationの値はglVertexAttributePointerに対応している
layout(location=1) in vec3 inNormal;
layout(location=2) in vec2 inTexCoord;

// Any vertex outputs (other than position)
out vec2 fragTexCoord;
// 法線ベクトル(ワールド座標)
out vec3 fragNormal;
// 位置のワールド座標
out vec3 fragWorldPos;

void main()
{
	//位置を同次座標系に
	vec4 pos = vec4(inPosition, 1.0);
	//位置をワールド座標に
	pos = pos * uWorldTransform;
	//ワールド空間の位置を保持
	fragWorldPos = pos.xyz;
	//クリップ空間に変換
	//バーテックスシェーダーでは、gl_Positionに頂点データを渡す必要がある
	gl_Position = pos * uViewProj;

	//法線をワールド空間へ変換
	fragNormal = (vec4(inNormal, 0.0f) * uWorldTransform).xyz;

	//テクスチャ座標をフラグメントシェーダーに渡す
	fragTexCoord = inTexCoord;

	// Convert position to homogeneous coordinates
	// vec4 pos = vec4(inPosition, 1.0);
	// Transform to position world space, then clip space
	// l_Position = pos * uWorldTransform * uViewProj;

	// Pass along the texture coordinate to frag shader
	// fragTexCoord = inTexCoord;
}
