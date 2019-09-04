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

//���_��񂪕����ɂȂ����̂ŁA�ǂ̑����X���b�g���ǂ̕ϐ��ɑΉ����Ă��邩�w�肷��
// ���͕ϐ��ł���ʒu���̐錾
layout(location=0) in vec3 inPosition;		//location�̒l��glVertexAttributePointer�ɑΉ����Ă���
layout(location=1) in vec2 inTexCoord;
layout(location=2) in vec3 inVertexColor;

//�t���O�����g�V�F�[�_�[�ɂ�uv���W���g�p���邽��(�s�N�Z���̐F�����߂�̂Ɏg�p)out�ϐ���錾
out vec2 fragTexCoord;
out vec3 fragVectorColor;

void main()
{
	// pos�̓I�u�W�F�N�g��Ԃ̈ʒu
	vec4 pos = vec4(inPosition, 1.0);
	// �I�u�W�F�N�g��Ԃ̈ʒu�����[���h��Ԃ��o�āA�N���b�v��Ԃ̈ʒu�ւƕς���
	gl_Position = pos * uWorldTransform * uViewProj;
	//uv���W���t���O�����g�V�F�[�_�[�ɓn��
	fragTexCoord = inTexCoord;
	fragVectorColor = inVertexColor;
	//uv���W���t���O�����g�V�F�[�_�[�ɓn�������ŉ���uv���W�l���t���O�����g�V�F�[�_�[���킩��̂��H
	//�����OpenGL��3�����Ȃ����_�̏������ƂɁA�O�p�`�|���S���̕\�ʑS�ĂɎ����I�ɕ⊮���Ă��邩��
	//�O�p�`���̔C�ӂ̃s�N�Z���ŁA�Ή�����t���O�����g�V�F�[�_�[�ł�uv���W���A�⊮���ꂽ���W�l�Ƃ��ē�����
}