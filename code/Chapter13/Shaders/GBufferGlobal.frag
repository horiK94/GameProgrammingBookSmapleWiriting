// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// Inputs from vertex shader
// Tex coord
//uv座標
in vec2 fragTexCoord;

// This corresponds to the output color to the color buffer
//カラーバッファの出力色に対応
layout(location = 0) out vec4 outColor;

// Different textures from G-buffer
//Gバッファのテクスチャ
uniform sampler2D uGDiffuse;
uniform sampler2D uGNormal;
uniform sampler2D uGWorldPos;

// Create a struct for directional light
//照明用uniform(Phong.fragと同様)
struct DirectionalLight
{
	// Direction of light
	//ライトの方向(入射光)
	vec3 mDirection;
	// Diffuse color
	//拡散反射光
	vec3 mDiffuseColor;
	// Specular color
	//鏡面反射光(個々の表面の鏡面反射指数に依存するが、Gバッファに保存していないので今回は計算できない)
	vec3 mSpecColor;
};

// Uniforms for lighting
// Camera position (in world space)
//カメラ位置(ワールド座標)
uniform vec3 uCameraPos;
// Ambient light level
// 環境色
uniform vec3 uAmbientLight;
// Directional Light
// 平行光源
uniform DirectionalLight uDirLight;

void main()
{
	// albedo, 法線, ワールド位置座標をGバッファから取得
	//拡散反射成分
	vec3 gbufferDiffuse = texture(uGDiffuse, fragTexCoord).xyz;
	//法線
	vec3 gbufferNorm = texture(uGNormal, fragTexCoord).xyz;
	//ワールド座標
	vec3 gbufferWorldPos = texture(uGWorldPos, fragTexCoord).xyz;
	// Surface normal
	//表面法線
	vec3 N = normalize(gbufferNorm);
	// Vector from surface to light
	//表面から光源へのベクトル（入射光の逆ベクトル）
	vec3 L = normalize(-uDirLight.mDirection);
	// Vector from surface to camera
	//表面からカメラへのベクトル(つまり、カメラの位置 - 表面の位置(ワールド座標))
	vec3 V = normalize(uCameraPos - gbufferWorldPos);
	// Reflection of -L about N
	//Nに関する-Lの反射
	vec3 R = normalize(reflect(-L, N));

	// Compute phong reflection
	//Phong.fragとほぼ同様だが、鏡面反射色は取れていないので、Specular = (0, 0, 0)とみなし計算
	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		vec3 Diffuse = uDirLight.mDiffuseColor * dot(N, L);
		Phong += Diffuse;
	}
	// Clamp light between 0-1 RGB values
	Phong = clamp(Phong, 0.0, 1.0);

	// Final color is texture color times phong light (alpha = 1)
	outColor = vec4(gbufferDiffuse * Phong, 1.0);
}
