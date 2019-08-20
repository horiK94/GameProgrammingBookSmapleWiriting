// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
// #version 330

// This should correspond to the data stored
// for each vertex in the vertex buffer.
// For now, just a position.
// in vec3 inPosition;

// void main()
// {
	// The vertex shader needs to output a 4D
	// coordinate.
	// For now set the 4th coordinate to 1.0
// 	gl_Position = vec4(inPosition, 1.0);
// }

// OpenGL3.3に対応するGLSLのバージョンの指定
#version 330

// 入力変数である位置情報の宣言
in vec3 inPosition;

void main()
{
	gl_Position = vec4(inPosition, 1.0);
}