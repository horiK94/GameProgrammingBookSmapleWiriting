// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "VertexArray.h"
#include <GL/glew.h>

VertexArray::VertexArray(const float* verts, unsigned int numVerts,
	const unsigned int* indices, unsigned int numIndices)
	:mNumVerts(numVerts)
	,mNumIndices(numIndices)
{
	//���_�z��I�u�W�F�N�g�̍쐬
	/*
	void glGenVertexArrays(GLsizei n, GLuint *arrays)
	n �̒��_�z��I�u�W�F�N�g���쐬��, ���̃I�u�W�F�N�g�� (�����l�ŕ\����鎯�ʎq) �� arrays �Ɏw�肳�ꂽ�z��̗v�f�Ɋi�[���܂�.
	*/
	glGenVertexArrays(1, &mVertexArray);	//�����쐬���邩�ƁA���_�z��I�u�W�F�N�g��ID��n��
	/*
	void glBindVertexArray(GLuint array)
	arrays �Ɏw�肳�ꂽ�I�u�W�F�N�g���̒��_�z��I�u�W�F�N�g���������܂�. ���_�z��I�u�W�F�N�g�͌�������Ă���Ԏg�p�ł��܂�. array �� 0 �̎��͌��݌�������Ă��钸�_�z��I�u�W�F�N�g�̌������������܂�.
	*/
	//mVertexArray�𒸓_�z��I�u�W�F�N�g�Ƃ��ăo�C���h�i���_�z��I�u�W�F�N�g�Ƃ��Ďg���j�Ƃ����Ӗ��ł́H
	glBindVertexArray(mVertexArray);;

	//���_�o�b�t�@�̍쐬
	//// Create vertex buffer
	//glGenBuffers(1, &mVertexBuffer);
	glGenBuffers(1, &mVertexBuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	//GL_ARRAY_BUFFER�̓o�b�t�@�𒸓_�o�b�t�@�Ƃ��ăo�C���h�i���_�o�b�t�@�Ƃ��Ďg���j�Ƃ����Ӗ�
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	////��Œ��_�o�b�t�@�̍쐬�͊��������̂ŁA�����́u���_�o�b�t�@���R�s�[�v���Ă��
	//glBufferData(
	//	GL_ARRAY_BUFFER,		//�o�b�t�@�̎��(GL_ARRAY_BUFFER: ����������_�o�b�t�@���g�p. ID���w�肷��̂ł͂Ȃ�) => ��Ŏw�肵���o�b�t�@�̎�ނ��w��
	//	numVerts * 3 * sizeof(float),		//�R�s�[����T�C�Y
	//	verts,		//�R�s�[���|�C���^
	//	GL_STATIC_DRAW		//�f�[�^�̗��p���@(GL_STATIC_DRAW: GPU�Ńf�[�^�̃��[�h��1��s���A���̌�`��Ńf�[�^��ǂݍ���): GPU�̓�����œK�����邽�߂̃q���g�Ƃ��Ďg�p�����(�ڍ�: http://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20120909)
	//);
		//��Œ��_�o�b�t�@�̍쐬�͊��������̂ŁA�����́u���_�o�b�t�@���R�s�[�v���Ă��
	glBufferData(
		GL_ARRAY_BUFFER,		//�o�b�t�@�̎��(GL_ARRAY_BUFFER: ����������_�o�b�t�@���g�p. ID���w�肷��̂ł͂Ȃ�) => ��Ŏw�肵���o�b�t�@�̎�ނ��w��
		numVerts * 5 * sizeof(float),		//�R�s�[����T�C�Y
		verts,		//�R�s�[���|�C���^
		GL_STATIC_DRAW		//�f�[�^�̗��p���@(GL_STATIC_DRAW: GPU�Ńf�[�^�̃��[�h��1��s���A���̌�`��Ńf�[�^��ǂݍ���): GPU�̓�����œK�����邽�߂̃q���g�Ƃ��Ďg�p�����(�ڍ�: http://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20120909)
	);
	//glBufferData(GL_ARRAY_BUFFER, numVerts * 5 * sizeof(float), verts, GL_STATIC_DRAW);

	//�C���f�b�N�X�o�b�t�@�̍쐬
	//// Create index buffer
	//�����͒��_�o�b�t�@�Ƃقړ��l
	glGenBuffers(1, &mIndexBuffer);
	//glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);		//�C���f�b�N�X�o�b�t�@�Ƃ��Č�������(�C���f�b�N�X�o�b�t�@�Ƃ��Ďg��)
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		numIndices * 3 * sizeof(unsigned int),
		indices,
		GL_STATIC_DRAW
	);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	//���_���C�A�E�g(���_����)�̐ݒ� => �|���S���̒��_���ǂ̂悤�ȃf�[�^�`������ݒ�
	//// Specify the vertex attributes
	//// (For now, assume one vertex format)
	//// Position is 3 floats starting at offset 0
	//�����Ɏw�肵�����_�����z���L���ɂ���(���������A���_�����z��Ƃ����̂�����A�����L���ɂ����肵�Ă���񂾂Ǝv��)
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(
	//	0,		//�ʒu���𒸓_����0�Ɏw��(���0��L���ɂ���Ă��邱�Ƃɒ���)
	//	3,		//�v�f��(1�̒��_�̃T�C�Y)
	//	GL_FLOAT,		//�v�f�̌^
	//	GL_FALSE,		//�����̌^�ł̂ݎg�p���邽��False���w��
	//	sizeof(float) * 3,		//�X�g���C�h(=�A�����钸�_�����Ԃ̃o�C�g�I�t�Z�b�g
	//	//���_�o�b�t�@�̒��_�w�f�[�^�x�Ԃ̃p�f�B���O(=�Ԋu)�͂Ȃ����߁A�X�g���C�h�͒��_�̃T�C�Y�ƂȂ�)
	//	0		//���_�f�[�^�̊J�n�ʒu���炱�̑����܂ł̃I�t�Z�b�g(�B��̑����ł��邽�߁c�c����͂܂�ŏ��̑����ƂȂ邽��)
	//);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,		//�ʒu���𒸓_����0�Ɏw��(���0��L���ɂ���Ă��邱�Ƃɒ���)
		3,		//�v�f��(1�̒��_�̃T�C�Y)
		GL_FLOAT,		//�v�f�̌^
		GL_FALSE,		//�����̌^�ł̂ݎg�p���邽��False���w��
		sizeof(float) * 5,		//�X�g���C�h(=�A�����钸�_�����Ԃ̃o�C�g�I�t�Z�b�g
		//���_�o�b�t�@�̒��_�w�f�[�^�x�Ԃ̃p�f�B���O(=�Ԋu)�͂Ȃ����߁A�X�g���C�h�͒��_�̃T�C�Y�ƂȂ�)
		0		//���_�f�[�^�̊J�n�ʒu���炱�̑����܂ł̃I�t�Z�b�g(�B��̑����ł��邽�߁c�c����͂܂�ŏ��̑����ƂȂ邽��)
	);
	//��2�̒��_���W(uv���W)
	glEnableVertexAttribArray(1);		//1�Ԗڂ̒��_���W��L����
	glVertexAttribPointer(
		1,		//���_���Windex(n�Ԗڂ̒��_���W��񂩂������ɂ͂��邽�߁A��ŗL���ɂ������_�ԍ��Ɠ��l�̂��̂�����)
		2,		//�v�f��(uv���W��2��)
		GL_FLOAT,		//�v�f�̌^
		GL_FALSE,		//�����̌^�ł̂ݎg�p���邽��False���w��
		sizeof(float) * 5,		//�X�g���C�h(���ʁA�e���_�C���f�b�N�X�̃T�C�Y)
		reinterpret_cast<void*>(sizeof(float) * 3)		//���_�C���f�b�N�X�̊J�n�ʒu���炱�̑����܂ł̈ʒu(�������I�t�Z�b�g�|�C���^�Ƃ��ēn���K�v����)
		//���_�̐擪���牽�o�C�g���邩�m��K�v������B���ꂪ sizeof(float) * 3
		//�����A�I�t�Z�b�g�|�C���^�Ƃ��ēn���K�v�����邽�߁Areinterpret_cast(�|�C���^�^�������I�ɕʂ̃|�C���^�^�ɕϊ�����)��void*�|�C���^�^�ϐ��ɕϊ����ēn���Ă���
		//float f = 2.0f; reinterpret_cast<int&>(f) �Ƃ���ƁAfloat��int��32bit�@�ł͓����T�C�Y�̂��߁Afloat�̓��e�����̂܂�16�i���œf���o����
	);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5,
	//	reinterpret_cast<void*>(sizeof(float) * 3));
}

VertexArray::~VertexArray()
{
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteVertexArrays(1, &mVertexArray);
	//glDeleteBuffers(1, &mVertexBuffer);
	//glDeleteBuffers(1, &mIndexBuffer);
	//glDeleteVertexArrays(1, &mVertexArray);
}

void VertexArray::SetActive()
{
	//���p���钸�_�z��̎w��
	glBindVertexArray(mVertexArray);
}
