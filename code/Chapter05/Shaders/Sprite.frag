// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// �o�[�e�b�N�X�V�F�[�_�[��out�Ɠ����^�������O�ɂ���K�v������
in vec2 fragTexCoord;
in vec3 fragVectorColor;

//�^����ꂽuv���W�ɏ]���ăe�N�X�`������F���擾����TextureSampler�̂��߂�uniform��ǉ�
//smapler2D: 2DTexture���T���v�����O
uniform sampler2D uTexture;

// This corresponds to the output color
// to the color buffer
out vec4 outColor;

void main()
{
	//�e�N�X�`���̐F���T���v�����O�����. �i���_�V�F�[�_�[����n���ꂽuv���W�Łj
    // outColor = texture(uTexture, fragTexCoord);
    // �e���_�J���[�ɑ΂���T���v�����O���s����
	// outColor = vec4(fragVectorColor, 1.0f);
	outColor = (texture(uTexture, fragTexCoord) + vec4(fragVectorColor, 1.0f)) / 2.0f;
}
