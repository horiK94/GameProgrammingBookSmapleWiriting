// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
// #version 330

// Tex coord input from vertex shader
// in vec2 fragTexCoord;

// This corresponds to the output color to the color buffer
// out vec4 outColor;

// This is used for the texture sampling
// uniform sampler2D uTexture;

// void main()
// {
	// Sample color from texture
	// outColor = texture(uTexture, fragTexCoord);
// }

#verison 330

// �o�͐F���i�[����O���[�o���ϐ����Aout�Ƃ����ϐ��C���q�Ő錾
out vec4 outColor;

void main()
{
	// ��Ԃ�
	outColor = vec4(0.0, 0.0, 1.0, 1.0);
}