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

//頂点情報が複数になったので、どの属性スロットがどの変数に対応しているか指定する
// 入力変数である位置情報の宣言
layout(location=0) in vec3 inPosition;		//locationの値はglVertexAttributePointerに対応している
layout(location=1) in vec2 inTexCoord;
layout(location=2) in vec3 inVertexColor;

//フラグメントシェーダーにもuv座標を使用するため(ピクセルの色を決めるのに使用)out変数を宣言
out vec2 fragTexCoord;
out vec3 fragVectorColor;

void main()
{
	// posはオブジェクト空間の位置
	vec4 pos = vec4(inPosition, 1.0);
	// オブジェクト空間の位置をワールド空間を経て、クリップ空間の位置へと変える
	gl_Position = pos * uWorldTransform * uViewProj;
	//uv座標をフラグメントシェーダーに渡す
	fragTexCoord = inTexCoord;
	fragVectorColor = inVertexColor;
	//uv座標をフラグメントシェーダーに渡すだけで何故uv座標値をフラグメントシェーダーがわかるのか？
	//それはOpenGLが3個しかない頂点の情報をもとに、三角形ポリゴンの表面全てに自動的に補完しているから
	//三角形内の任意のピクセルで、対応するフラグメントシェーダーでのuv座標を、補完された座標値として得られる
}