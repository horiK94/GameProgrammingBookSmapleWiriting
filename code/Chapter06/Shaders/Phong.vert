// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// Uniforms for world transform and view-proj
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

// Attribute 0 is position, 1 is normal, 2 is tex coords.
layout(location=0) in vec3 inPosition;		//location�̒l��glVertexAttributePointer�ɑΉ����Ă���
layout(location=1) in vec3 inNormal;
layout(location=2) in vec2 inTexCoord;

// Any vertex outputs (other than position)
out vec2 fragTexCoord;
// �@���x�N�g��(���[���h���W)
out vec3 fragNormal;
// �ʒu�̃��[���h���W
out vec3 fragWorldPos;

void main()
{
	//�ʒu�𓯎����W�n��
	vec4 pos = vec4(inPosition, 1.0);
	//�ʒu�����[���h���W��
	pos = pos * uWorldTransform;
	//���[���h��Ԃ̈ʒu��ێ�
	fragWorldPos = pos.xyz;
	//�N���b�v��Ԃɕϊ�
	//�o�[�e�b�N�X�V�F�[�_�[�ł́Agl_Position�ɒ��_�f�[�^��n���K�v������
	gl_Position = pos * uViewProj;

	//�@�������[���h��Ԃ֕ϊ�
	fragNormal = (vec4(inNormal, 0.0f) * uWorldTransform).xyz;

	//�e�N�X�`�����W���t���O�����g�V�F�[�_�[�ɓn��
	fragTexCoord = inTexCoord;

	// Convert position to homogeneous coordinates
	// vec4 pos = vec4(inPosition, 1.0);
	// Transform to position world space, then clip space
	// l_Position = pos * uWorldTransform * uViewProj;

	// Pass along the texture coordinate to frag shader
	// fragTexCoord = inTexCoord;
}
