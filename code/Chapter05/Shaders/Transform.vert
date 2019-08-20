// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// OpenGL3.3に対応するGLSLのバージョンの指定
#version 330



//グローバル変数の宣言(inやoutと違い、何度シェーダーが実行されても値は変わらない)
// mat4: 4x4行列. 3次元空間の同次座標系に必要
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// 入力変数である位置情報の宣言
in vec3 inPosition;

void main()
{
	// posはオブジェクト空間の位置
	vec4 pos = vec4(inPosition, 1.0);
	// オブジェクト空間の位置をワールド空間を経て、クリップ空間の位置へと変える
	gl_Position = pos * uWorldTransform * uViewProj;
}