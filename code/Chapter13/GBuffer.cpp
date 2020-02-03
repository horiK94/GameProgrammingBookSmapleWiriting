// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "GBuffer.h"
#include <GL/glew.h>
#include "Texture.h"

GBuffer::GBuffer()
	:mBufferID(0)
{
	
}

GBuffer::~GBuffer()
{
	
}

//指定された幅と高さのGバッファを作成
bool GBuffer::Create(int width, int height)
{
	// Create the framebuffer object
	//FBOの作成
	glGenFramebuffers(1, &mBufferID);
	//FBOとしてバインドする
	glBindFramebuffer(GL_FRAMEBUFFER, mBufferID);
	
	// Add a depth buffer to this target
	//深度バッファの作成
	GLuint depthBuffer;		//深度バッファのID
	//深度バッファの作成
	glGenRenderbuffers(1, &depthBuffer);
	//描画バッファとしてバインドする
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	//レンダーバッファにデプスバッファとして使用することを宣言(サイズをここで指定)
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
						  width, height);
	//フレームバッファ(一画面分の描画情報を保存しておく領域)にレンダーバッファをデプスバッファとしてアタッチ
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							  GL_RENDERBUFFER, depthBuffer);
	
	// Create textures for each output in the G-buffer
	//Gバッファの出力用にテクスチャを作成
	for (int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
	{
		Texture* tex = new Texture();
		// We want three 32-bit float components for each texture
		//width * heightのGL_RGB32F(32bit floatx3成分のフォーマット)のテクスチャを作成
		tex->CreateForRendering(width, height, GL_RGB32F);
		mTextures.emplace_back(tex);		//配列に保存
		// Attach this texture to a color output
		//フレームバッファにtextureをカラー出力としてアタッチ
		//テクスチャをカラーアタッチメントスロットにアタッチしているらしい
		//テクスチャオブジェクトのレベルをフレームバッファーオブジェクトの論理バッファーとしてアタッチします
		glFramebufferTexture(GL_FRAMEBUFFER,		//フレームバッファがバインドされるターゲットを指定します。
			GL_COLOR_ATTACHMENT0 + i,				//フレームバッファの接続点を指定します。
			tex->GetTextureID(),					//アタッチする既存のテクスチャオブジェクトの名前を指定します。
			0);										//アタッチするテクスチャオブジェクトのミップマップレベルを指定します。
		//OpenGLではカラーアタッチメントを連続番号として定義している
	}
	
	// Create a vector of the color attachments
	//カラーアタッチメントの配列を作成
	std::vector<GLenum> attachments;
	for (int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
	{
		attachments.emplace_back(GL_COLOR_ATTACHMENT0 + i);
	}
	
	// Set the list of buffers to draw to
	//描画先バッファのリストを作成
	//Gバッファのテクスチャアタッチメントを設定
	glDrawBuffers(static_cast<GLsizei>(attachments.size()),
				  attachments.data());
	
	// Make sure everything worked
	//問題なく動くかチェック
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Destroy();
		return false;
	}
	
	return true;
}

void GBuffer::Destroy()
{
	glDeleteFramebuffers(1, &mBufferID);
	for (Texture* t : mTextures)
	{
		t->Unload();
		delete t;
	}
}

Texture* GBuffer::GetTexture(Type type)
{
	if (mTextures.size() > 0)
	{
		return mTextures[type];
	}
	else
	{
		return nullptr;
	}
}

void GBuffer::SetTexturesActive()
{
	//Gバッファの各Textureを、対応するTextureインデックスにバインド
	for (int i = 0; i < NUM_GBUFFER_TEXTURES; i++)
	{
		mTextures[i]->SetActive(i);
	}
}
