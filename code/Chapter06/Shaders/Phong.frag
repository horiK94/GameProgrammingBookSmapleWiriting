// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// Tex coord input from vertex shader
in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragWorldPos;
//in float fragEffectRange;

// This corresponds to the output color to the color buffer
out vec4 outColor;

//���s�����Ɏg�p����\����
struct DirectionalLight
{
	//���̈ʒu
	vec3 mPosition;
	// ���̕���
	//vec3 mDirection;
	// �g�U���ːF
	vec3 mDiffuseColor;
	// ���ʔ��ˌ�
	vec3 mSpecColor;

	//�e�����a
	float mEffectRange;
};

//���C�e�B���O�puniform
//�J�����̈ʒu(���[���h���W)
uniform vec3 uCameraPos;
//�����̋���
uniform vec3 uAmbientLight;
//�\�ʂ̋��ʔ��ˌ��w��
uniform float uSpecPower;
//���s����(����1�̂�)
uniform DirectionalLight uDirLight;

//�摜�f�[�^
uniform sampler2D uTexture;

void main()
{
	//�|���S�\�ʂ̖@��
	vec3 N = normalize(fragNormal);
	//�\�ʂ�������ւ̃x�N�g��(���s�����̋t�x�N�g��)
	vec3 L = normalize(uDirLight.mPosition - fragWorldPos);
	//�\�ʂ���J�����ւ̃x�N�g��
	vec3 V = normalize(uCameraPos - fragWorldPos);
	//N�Ɋւ���-L�̔���R
	vec3 R = normalize(reflect(-L, N));

	

	float r = distance(uDirLight.mPosition, fragWorldPos);

	//�t�H���̔��˂��v�Z
	vec3 Phong = uAmbientLight;
	float NdotL = dot(N, L);
	if(NdotL > 0)
	{
		vec3 Diffuse = uDirLight.mDiffuseColor * NdotL;
		vec3 Specular = uDirLight.mSpecColor * max(0.0f, pow(dot(R, V), uSpecPower));
		if(r <= uDirLight.mEffectRange)
		{
			Phong += (Diffuse + Specular) * (uDirLight.mEffectRange - r) / uDirLight.mEffectRange;
		}
	}
	//�ŏI�I�ȐF�̓e�N�X�`���̐F * �t�H���̌�(alpha = 1)
	outColor = texture(uTexture, fragTexCoord) * vec4(Phong, 1.0f);
}
