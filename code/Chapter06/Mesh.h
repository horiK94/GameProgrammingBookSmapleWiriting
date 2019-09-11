// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <vector>
#include <string>

class Mesh		//���b�V���f�[�^
{
public:
	Mesh();
	~Mesh();
	// Load/unload mesh
	//���[�h����Texture��������rendere�̃|�C���^���󂯎��
	bool Load(const std::string& fileName, class Renderer* renderer);
	void Unload();
	// Get the vertex array associated with this mesh
	//���b�V���Ɋ��蓖�Ă�ꂽ���_�z��(���_�o�b�t�@, �C���f�b�N�X�o�b�t�@)���擾
	class VertexArray* GetVertexArray() { return mVertexArray; }
	// Get a texture from specified index
	//�e�N�X�`���̎擾
	class Texture* GetTexture(size_t index);
	// Get name of shader
	//�V�F�[�_�[���̎擾
	const std::string& GetShaderName() const { return mShaderName; }
	// Get object space bounding sphere radius
	float GetRadius() const { return mRadius; }
	// Get specular power of mesh
	float GetSpecPower() const { return mSpecPower; }
private:
	// Textures associated with this mesh
	//���b�V���̃e�N�X�`���Q
	std::vector<class Texture*> mTextures;
	// Vertex array associated with this mesh
	//���_�z��̃|�C���^
	class VertexArray* mVertexArray;
	// Name of shader specified by mesh
	//�V�F�[�_�[��
	std::string mShaderName;
	// Stores object space bounding sphere radius
	//���a
	float mRadius;
	// Specular power of surface
	//���ʔ��ˎw��
	float mSpecPower;
};