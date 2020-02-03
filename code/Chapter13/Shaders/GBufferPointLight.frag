// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

//GBufferGlobal.fragシェーダーと同様で、Gバッファの3種類のテクスチャのために、3つのsampler2D型uniformを宣言
//また、特定の光源に関する情報も格納する必要あり
// Request GLSL 3.3
#version 330

// Inputs from vertex shader
// Tex coord
in vec2 fragTexCoord;

// This corresponds to the output color to the color buffer
layout(location = 0) out vec4 outColor;

// Different textures from G-buffer
//Gバッファを受け取る異なるsanpler2D型変数
uniform sampler2D uGDiffuse;
uniform sampler2D uGNormal;
uniform sampler2D uGWorldPos;

// Create a struct for the point light
//点光源の構造体(PointLight)
struct PointLight
{
	// Position of light
	//光の位置(ワールド座標)
	vec3 mWorldPos;
	// Diffuse color
	//拡散色
	vec3 mDiffuseColor;
	// Radius of the light
	//内側の半径
	float mInnerRadius;
	//外側の半径
	float mOuterRadius;
};

//点光源情報
uniform PointLight uPointLight;
// Stores width/height of screen
//画面幅と高さ
uniform vec2 uScreenDimensions;

void main()
{
	// From this fragment, calculate the coordinate to sample into the G-buffer
	//Gバッファのtextureのどこをサンプリングするか(座標)計算([0,1], [0, 1]の範囲の座標に変換)
	//gl_FragCoord: シェーダが今まさに処理しようとしているピクセルの座標([0, width] * [0, height]の範囲の座標)
	vec2 gbufferCoord = gl_FragCoord.xy / uScreenDimensions;
	
	// Sample from G-buffer
	//Gバッファのalbedoからサンプリング
	vec3 gbufferDiffuse = texture(uGDiffuse, gbufferCoord).xyz;
	//Gバッファの法線からサンプリング
	vec3 gbufferNorm = texture(uGNormal, gbufferCoord).xyz;
	//Gバッファのワールド座標からサンプリング
	vec3 gbufferWorldPos = texture(uGWorldPos, gbufferCoord).xyz;
	
	// Surface normal
	//法線ベクトルを正規化
	vec3 N = normalize(gbufferNorm);
	// Vector from surface to light
	//表面からライトへのベクトル
	//uPointLight.mWorldPos: ライトのいち, gbufferWorldPos表面のワールド座標
	vec3 L = normalize(uPointLight.mWorldPos - gbufferWorldPos);

	// Compute Phong diffuse component for the light
	vec3 Phong = vec3(0.0, 0.0, 0.0);
	float NdotL = dot(N, L);
	if (NdotL > 0)
	{
		// Get the distance between the light and the world pos
		//光源とワールド位置座標との距離を求める
		float dist = distance(uPointLight.mWorldPos, gbufferWorldPos);
		// Use smoothstep to compute value in range [0,1]
		// between inner/outer radius
		//smoothstepで内側と外側の変形の場合の[0,1]での輝度を出力
		float intensity = smoothstep(uPointLight.mInnerRadius,
									 uPointLight.mOuterRadius, dist);
		// The diffuse color of the light depends on intensity
		//光の拡散反射は輝度に依存する
		//mix(x, y, a): x(1-a)+y*a
		//外側だと全く色がつかない=黒(0, 0, 0)
		vec3 DiffuseColor = mix(uPointLight.mDiffuseColor,
								vec3(0.0, 0.0, 0.0), intensity);
		//Phongの計算
		Phong = DiffuseColor * NdotL;
	}

	// Final color is texture color times phong light (alpha = 1)
	//Textureの色にPhongの拡散反射を掛けて最終的な色とする
	outColor = vec4(gbufferDiffuse * Phong, 1.0);
}
