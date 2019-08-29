// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// バーテックスシェーダーのoutと同じ型同じ名前にする必要がある
in vec2 fragTexCoord;

//与えられたuv座標に従ってテクスチャから色を取得するTextureSamplerのためにuniformを追加
//smapler2D: 2DTextureをサンプリング
uniform sampler2D uTexture;

// This corresponds to the output color
// to the color buffer
out vec4 outColor;

void main()
{
	//テクスチャの色がサンプリングされる. （頂点シェーダーから渡されたuv座標で）
    outColor = texture(uTexture, fragTexCoord);
}
