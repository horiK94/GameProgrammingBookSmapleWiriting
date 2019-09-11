// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// Tex coord input from vertex shader
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragWorldPos;

// This corresponds to the output color to the color buffer
out vec4 outColor;

//平行光源に使用する構造体
struct DirectionalLight
{
	// 光の方向
	vec3 mDirection;
	// 拡散反射色
	vec3 mDiffuseColor;
	// 鏡面反射光
	vec3 mSpecColor;
};

//ライティング用uniform
//カメラの位置(ワールド座標)
uniform vec3 uCameraPos;
//環境光の強さ
uniform vec3 uAmbientLight;
//表面の鏡面反射光指数
uniform float uSpecPower;
//平行光源(今は1つのみ)
uniform DirectionalLight uDirLight;

//画像データ
uniform sampler2D uTexture;

void main()
{
	//ポリゴ表面の法線
	vec3 N = normalize(fragNormal);
	//表面から光源へのベクトル(平行光源の逆ベクトル)
	vec3 L = normalize(-uDirLight.mDirection);
	//表面からカメラへのベクトル
	vec3 V = normalize(uCameraPos - fragWorldPos);
	//Nに関する-Lの反射R
	vec3 R = normalize(reflect(-L, N));

	//フォンの反射を計算
	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);
	if(NdotL > 0)
	{
		vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;
		vec3 Specular = uDirLight.mSpecColor * max(0.0f, pow(dot(R, V), uSpecPower));
		Phong += Diffuse + Specular;
	}
	//最終的な色はテクスチャの色 * フォンの光(alpha = 1)
	outColor = texture(uTexture, fragTexCoord) * vec4(Phong, 1.0f);
}
