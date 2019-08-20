// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// OpenGL3.3�ɑΉ�����GLSL�̃o�[�W�����̎w��
#version 330



//�O���[�o���ϐ��̐錾(in��out�ƈႢ�A���x�V�F�[�_�[�����s����Ă��l�͕ς��Ȃ�)
// mat4: 4x4�s��. 3������Ԃ̓������W�n�ɕK�v
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// ���͕ϐ��ł���ʒu���̐錾
in vec3 inPosition;

void main()
{
	// pos�̓I�u�W�F�N�g��Ԃ̈ʒu
	vec4 pos = vec4(inPosition, 1.0);
	// �I�u�W�F�N�g��Ԃ̈ʒu�����[���h��Ԃ��o�āA�N���b�v��Ԃ̈ʒu�ւƕς���
	gl_Position = pos * uWorldTransform * uViewProj;
}