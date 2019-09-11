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
	//fileの読み込み
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		SDL_Log("File not found: Mesh %s", fileName.c_str());
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	//fileの最初から最後までcontentsに流し込む
	std::string contents = fileStream.str();
	//
	rapidjson::StringStream jsonStr(contents.c_str());		//jaonStrの中にjsonのstring streamとしてデータが格納されている(string streamとは文字を媒体とした変換用stringみたいなものかな。)
	//string -> floatには直接変えられないけど、1度 string -> stringstream　と変換すれば stringsteam -> float, int といった値にstringstreamの関数を使いつつではあるが変換ができる
	//http://rapidjson.org/md_doc_tutorial.html Documentについてのわかりやすい図が書いてある
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);		//JSONはDOMツリーとして解析される

	if (!doc.IsObject())		//DOMツリーになっているか
	{
		SDL_Log("Mesh %s is not valid json", fileName.c_str());
		return false;
	}

	int ver = doc["version"].GetInt();		//versionをintとして取得

	// Check the version
	if (ver != 1)
	{
		SDL_Log("Mesh %s not version 1", fileName.c_str());
		return false;
	}

	mShaderName = doc["shader"].GetString();

	// Skip the vertex format/shader for now
	// (This is changed in a later chapter's code)
	size_t vertSize = 8;			//頂点バッファ1つが使用する値の数(8つ)

	// Load textures
	const rapidjson::Value& textures = doc["textures"];		//texture配列の取得(ポインタ取得)
	if (!textures.IsArray() || textures.Size() < 1)		//0個や配列じゃなかったらエラー
	{
		SDL_Log("Mesh %s has no textures, there should be at least one", fileName.c_str());
		return false;
	}

	//鏡面反射指数を指定する
	mSpecPower = static_cast<float>(doc["specularPower"].GetDouble());

	//テクスチャの数だけmTexturesにaddしていく
	for (rapidjson::SizeType i = 0; i < textures.Size(); i++)
	{
		// Is this texture already loaded?
		std::string texName = textures[i].GetString();
		Texture* t = renderer->GetTexture(texName);	//ロード済みのTextureを取得
		if (t == nullptr)
		{
			// Try loading the texture
			t = renderer->GetTexture(texName);		//もう1回試す
			if (t == nullptr)
			{
				// If it's still null, just use the default texture
				//だめならデフォルトのテクスチャで
				t = renderer->GetTexture("Assets/Default.png");
			}
		}
		mTextures.emplace_back(t);		//MeshのTexturesリストの末尾に追加していく
	}

	// Load in the vertices
	const rapidjson::Value& vertsJson = doc["vertices"];		//頂点バッファの取得(8つの要素で1セット)
	if (!vertsJson.IsArray() || vertsJson.Size() < 1)
	{
		SDL_Log("Mesh %s has no vertices", fileName.c_str());
		return false;
	}

	std::vector<float> vertices;
	// vertsJson = verticesのDOMツリーの子 = 頂点バッファの数 * vertSize (= 頂点インデックスを構成するfloatの数)
	vertices.reserve(vertsJson.Size() * vertSize);		//予めverticesの容量を必要分増加させておくことで再確保時に新しい領域に要素をコピーするコストが省ける
	mRadius = 0.0f;		//最大半径の2乗(途中まで)(境界値の半径を求めるのに使用)
	for (rapidjson::SizeType i = 0; i < vertsJson.Size(); i++)
	{
		// For now, just assume we have 8 elements
		const rapidjson::Value& vert = vertsJson[i];		//頂点インデックス
		if (!vert.IsArray() || vert.Size() != 8)			//頂点インデックスの中の要素は8つでなくてはならない
		{
			SDL_Log("Unexpected vertex format for %s", fileName.c_str());
			return false;
		}

		//posは半径求めるのにしか使っていない
		Vector3 pos(vert[0].GetDouble(), vert[1].GetDouble(), vert[2].GetDouble());
		mRadius = Math::Max(mRadius, pos.LengthSq());		//最大半径の2乗

		// Add the floats
		for (rapidjson::SizeType i = 0; i < vert.Size(); i++)		//配列の中にすべての数字が入っているイメージ([0, 1, 2, ..., 7, 8, 9, 0, ....](8津づつ区切って見る必要あり））)
		{
			vertices.emplace_back(static_cast<float>(vert[i].GetDouble()));		//verticesに 8つずつデータを入れていく
		}
	}

	// We were computing length squared earlier
	mRadius = Math::Sqrt(mRadius);		//最大半径

	// Load in the indices
	const rapidjson::Value& indJson = doc["indices"];			//3つのunsigned intで1セット
	if (!indJson.IsArray() || indJson.Size() < 1)
	{
		SDL_Log("Mesh %s has no indices", fileName.c_str());
		return false;
	}

	std::vector<unsigned int> indices;		//インデックスバッファのリスト
	indices.reserve(indJson.Size() * 3);		//こっちも予め必要分サイズ確保
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
	//頂点インデックスバッファの作成
	mVertexArray = new VertexArray(vertices.data(), static_cast<unsigned>(vertices.size()) / vertSize,
		indices.data(), static_cast<unsigned>(indices.size()));		//インデックスバッファは純粋に何セットかではなく、何個uintがあるかを引数で渡す必要があるようだ
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
