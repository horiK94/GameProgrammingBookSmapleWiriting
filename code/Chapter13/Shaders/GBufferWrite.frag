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
//頂点シェーダーからの値
//テクスチャ座標(uv座標)
in vec2 fragTexCoord;
// Normal (in world space)
//法線(ワールド座標)
in vec3 fragNormal;
// Position (in world space)
//位置(ワールド座標)
in vec3 fragWorldPos;

// This corresponds to the outputs to the G-buffer
//Gバッファへの出力。このように、フラグメントシェーダーはマルチレンダリングターゲット(MRT)を持てる
//layoutで指定しているlocationは、Gバッファ作成時のカラーアタッチメントのインデックス値に対応している
layout(location = 0) out vec3 outDiffuse;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outWorldPos;

// This is used for the texture sampling
//アルベドテクスチャのサンプラー
uniform sampler2D uTexture;

void main()
{
	// Diffuse color is sampled from texture
	//アルベドのTextureからuv座標に対応する物を取ってくる
	outDiffuse = texture(uTexture, fragTexCoord).xyz;
	// Normal/world pos are passed directly along
	//法線とワールド座標はそのまま渡す
	outNormal = fragNormal;
	outWorldPos = fragWorldPos;
}
