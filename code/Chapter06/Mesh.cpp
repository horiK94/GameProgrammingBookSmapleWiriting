// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Mesh.h"
#include "Renderer.h"
#include "Texture.h"
#include "VertexArray.h"
#include <fstream>
#include <sstream>
#include <rapidjson/document.h>
#include <SDL/SDL_log.h>
#include "Math.h"

Mesh::Mesh()
	:mVertexArray(nullptr)
	,mRadius(0.0f)
	,mSpecPower(100.0f)
{
}

Mesh::~Mesh()
{
}

bool Mesh::Load(const std::string & fileName, Renderer* renderer)
{
	//file�̓ǂݍ���
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("File not found: Mesh %s", fileName.c_str());
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	//file�̍ŏ�����Ō�܂�contents�ɗ�������
	std::string contents = fileStream.str();
	//
	rapidjson::StringStream jsonStr(contents.c_str());		//jaonStr�̒���json��string stream�Ƃ��ăf�[�^���i�[����Ă���(string stream�Ƃ͕�����}�̂Ƃ����ϊ��pstring�݂����Ȃ��̂��ȁB)
	//string -> float�ɂ͒��ڕς����Ȃ����ǁA1�x string -> stringstream�@�ƕϊ������ stringsteam -> float, int �Ƃ������l��stringstream�̊֐����g���ł͂��邪�ϊ����ł���
	//http://rapidjson.org/md_doc_tutorial.html Document�ɂ��Ă̂킩��₷���}�������Ă���
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);		//JSON��DOM�c���[�Ƃ��ĉ�͂����

	if (!doc.IsObject())		//DOM�c���[�ɂȂ��Ă��邩
	{
		SDL_Log("Mesh %s is not valid json", fileName.c_str());
		return false;
	}

	int ver = doc["version"].GetInt();		//version��int�Ƃ��Ď擾

	// Check the version
	if (ver != 1)
	{
		SDL_Log("Mesh %s not version 1", fileName.c_str());
		return false;
	}

	mShaderName = doc["shader"].GetString();

	// Skip the vertex format/shader for now
	// (This is changed in a later chapter's code)
	size_t vertSize = 8;			//���_�o�b�t�@1���g�p����l�̐�(8��)

	// Load textures
	const rapidjson::Value& textures = doc["textures"];		//texture�z��̎擾(�|�C���^�擾)
	if (!textures.IsArray() || textures.Size() < 1)		//0��z�񂶂�Ȃ�������G���[
	{
		SDL_Log("Mesh %s has no textures, there should be at least one", fileName.c_str());
		return false;
	}

	//���ʔ��ˎw�����w�肷��
	mSpecPower = static_cast<float>(doc["specularPower"].GetDouble());

	//�e�N�X�`���̐�����mTextures��add���Ă���
	for (rapidjson::SizeType i = 0; i < textures.Size(); i++)
	{
		// Is this texture already loaded?
		std::string texName = textures[i].GetString();
		Texture* t = renderer->GetTexture(texName);	//���[�h�ς݂�Texture���擾
		if (t == nullptr)
		{
			// Try loading the texture
			t = renderer->GetTexture(texName);		//����1�񎎂�
			if (t == nullptr)
			{
				// If it's still null, just use the default texture
				//���߂Ȃ�f�t�H���g�̃e�N�X�`����
				t = renderer->GetTexture("Assets/Default.png");
			}
		}
		mTextures.emplace_back(t);		//Mesh��Textures���X�g�̖����ɒǉ����Ă���
	}

	// Load in the vertices
	const rapidjson::Value& vertsJson = doc["vertices"];		//���_�o�b�t�@�̎擾(8�̗v�f��1�Z�b�g)
	if (!vertsJson.IsArray() || vertsJson.Size() < 1)
	{
		SDL_Log("Mesh %s has no vertices", fileName.c_str());
		return false;
	}

	std::vector<float> vertices;
	// vertsJson = vertices��DOM�c���[�̎q = ���_�o�b�t�@�̐� * vertSize (= ���_�C���f�b�N�X���\������float�̐�)
	vertices.reserve(vertsJson.Size() * vertSize);		//�\��vertices�̗e�ʂ�K�v�����������Ă������ƂōĊm�ێ��ɐV�����̈�ɗv�f���R�s�[����R�X�g���Ȃ���
	mRadius = 0.0f;		//�ő唼�a��2��(�r���܂�)(���E�l�̔��a�����߂�̂Ɏg�p)
	for (rapidjson::SizeType i = 0; i < vertsJson.Size(); i++)
	{
		// For now, just assume we have 8 elements
		const rapidjson::Value& vert = vertsJson[i];		//���_�C���f�b�N�X
		if (!vert.IsArray() || vert.Size() != 8)			//���_�C���f�b�N�X�̒��̗v�f��8�łȂ��Ă͂Ȃ�Ȃ�
		{
			SDL_Log("Unexpected vertex format for %s", fileName.c_str());
			return false;
		}

		//pos�͔��a���߂�̂ɂ����g���Ă��Ȃ�
		Vector3 pos(vert[0].GetDouble(), vert[1].GetDouble(), vert[2].GetDouble());
		mRadius = Math::Max(mRadius, pos.LengthSq());		//�ő唼�a��2��

		// Add the floats
		for (rapidjson::SizeType i = 0; i < vert.Size(); i++)		//�z��̒��ɂ��ׂĂ̐����������Ă���C���[�W([0, 1, 2, ..., 7, 8, 9, 0, ....](8�ÂÂ�؂��Č���K�v����j�j)
		{
			vertices.emplace_back(static_cast<float>(vert[i].GetDouble()));		//vertices�� 8���f�[�^�����Ă���
		}
	}

	// We were computing length squared earlier
	mRadius = Math::Sqrt(mRadius);		//�ő唼�a

	// Load in the indices
	const rapidjson::Value& indJson = doc["indices"];			//3��unsigned int��1�Z�b�g
	if (!indJson.IsArray() || indJson.Size() < 1)
	{
		SDL_Log("Mesh %s has no indices", fileName.c_str());
		return false;
	}

	std::vector<unsigned int> indices;		//�C���f�b�N�X�o�b�t�@�̃��X�g
	indices.reserve(indJson.Size() * 3);		//���������\�ߕK�v���T�C�Y�m��
	for (rapidjson::SizeType i = 0; i < indJson.Size(); i++)
	{
		const rapidjson::Value& ind = indJson[i];
		if (!ind.IsArray() || ind.Size() != 3)
		{
			SDL_Log("Invalid indices for %s", fileName.c_str());
			return false;
		}

		indices.emplace_back(ind[0].GetUint());
		indices.emplace_back(ind[1].GetUint());
		indices.emplace_back(ind[2].GetUint());
	}

	// Now create a vertex array
	//���_�C���f�b�N�X�o�b�t�@�̍쐬
	mVertexArray = new VertexArray(vertices.data(), static_cast<unsigned>(vertices.size()) / vertSize,
		indices.data(), static_cast<unsigned>(indices.size()));		//�C���f�b�N�X�o�b�t�@�͏����ɉ��Z�b�g���ł͂Ȃ��A����uint�����邩�������œn���K�v������悤��
	return true;
}

void Mesh::Unload()
{
	delete mVertexArray;
	mVertexArray = nullptr;
}

Texture* Mesh::GetTexture(size_t index)
{
	if (index < mTextures.size())
	{
		return mTextures[index];
	}
	else
	{
		return nullptr;
	}
}
