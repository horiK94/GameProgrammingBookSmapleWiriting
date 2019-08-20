// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Shader.h"
#include "Texture.h"
#include <SDL/SDL.h>
#include <fstream>
#include <sstream>

Shader::Shader()
	: mShaderProgram(0)
	, mVertexShader(0)
	, mFragShader(0)
{

}

Shader::~Shader()
{

}

//bool Shader::Load(const std::string& vertName, const std::string& fragName)
//{
//	// Compile vertex and pixel shaders
//	if (!CompileShader(vertName,
//		GL_VERTEX_SHADER,
//		mVertexShader) ||
//		!CompileShader(fragName,
//			GL_FRAGMENT_SHADER,
//			mFragShader))
//	{
//		return false;
//	}
//	
//	// Now create a shader program that
//	// links together the vertex/frag shaders
//	mShaderProgram = glCreateProgram();
//	glAttachShader(mShaderProgram, mVertexShader);
//	glAttachShader(mShaderProgram, mFragShader);
//	glLinkProgram(mShaderProgram);
//	
//	// Verify that the program linked successfully
//	if (!IsValidProgram())
//	{
//		return false;
//	}
//	
//	return true;
//}

bool Shader::Load(const std::string& vertName, const std::string& fragName)
{
	//頂点シェーダーとフラグメントシェーダーをコンパイルする
	if (!CompileShader(vertName, GL_VERTEX_SHADER, mVertexShader) ||
		!CompileShader(fragName, GL_FRAGMENT_SHADER, mFragShader))
	{
		return false;
	}


	//バーテックスシェーダーとフラグメントシェーダーをリンクさせてシェーダープログラムの作成
	mShaderProgram = glCreateProgram();			//シェーダープログラムの作成
	glAttachShader(mShaderProgram, mVertexShader);
	glAttachShader(mShaderProgram, mFragShader);
	glLinkProgram(mShaderProgram);		//リンクを行う
	if (!IsValidProgram())		//リンクが上手く行われているか確認
	{
		return false;
	}
	return true;
}

void Shader::Unload()
{
	// Delete the program/shaders
	//glDeleteProgram(mShaderProgram);
	//glDeleteShader(mVertexShader);
	//glDeleteShader(mFragShader);
	glDeleteProgram(mShaderProgram);
	glDeleteShader(mVertexShader);
	glDeleteShader(mFragShader);
}

void Shader::SetActive()
{
	// Set this program as the active one
	//シェーダーをアクティブにする
	//glUseProgram(mShaderProgram);
	glUseProgram(mShaderProgram);
	//OpenGLはアクティブにしたシェーダーを使って三角形を描画する
}

//void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix)
//{
//	// Find the uniform by this name
//	GLuint loc = glGetUniformLocation(mShaderProgram, name);
//	// Send the matrix data to the uniform
//	glUniformMatrix4fv(loc, 1, GL_TRUE, matrix.GetAsFloatPtr());
//}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix)
{
	// この名前のuniformを検索(シェーダーidとuniform名を引数にして渡す)
	//同じuniformを更新する際は、同じidのためいちいち問い合わせる必要はない
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// 行列データをuniformに送る
	glUniformMatrix4fv(
		loc,		//Uniform ID
		1,			//行列の数
		GL_TRUE,	//行ベクトルを使う場合はtrue
		matrix.GetAsFloatPtr()	//行列へのfloat型ポインタ
	);
}


bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
{
	//ファイルを開くためのifStream作成
	std::ifstream shaderFile(fileName);
	if (shaderFile.is_open())
	{
		//シェーダーファイルが開けた時
		//1つの文字列にshader fileの中の文章を読み込む
		std::stringstream sstream;		//メモリ (std::basic_string) ベースのストリームに対する入出力操作を実装
		sstream << shaderFile.rdbuf();		//sstreamにshaderFileに紐付いたストリームバッファを返す
		std::string contents = sstream.str();		//sstreamに紐付けておいた文字列を取得
		const char* contentsChar = contents.c_str();			//c_str()でC言語での文字列表現に変更している

		//指定されたタイプのシェーダーを作成
		outShader = glCreateShader(shaderType);
		//コンパイル対象として読み込んだ文字列を指定
		glShaderSource(outShader, 1, &(contentsChar), nullptr);
		//コードのコンパイル
		glCompileShader(outShader);

		if (!IsCompiled(outShader))
		{
			//コンパイル失敗した時
			SDL_Log("シェーダー %s のコンパイルに失敗しました", fileName.c_str());
			return false;
		}
	}
	else
	{
		SDL_Log("シェーダーファイル %s が見つかりません", fileName.c_str());
		return false;
	}
	return true;
}

//bool Shader::CompileShader(const std::string& fileName,
//	GLenum shaderType,
//	GLuint& outShader)
//{
//	// Open file
//	std::ifstream shaderFile(fileName);
//	if (shaderFile.is_open())
//	{
//		// Read all the text into a string
//		std::stringstream sstream;
//		sstream << shaderFile.rdbuf();
//		std::string contents = sstream.str();
//		const char* contentsChar = contents.c_str();
//		
//		// Create a shader of the specified type
//		outShader = glCreateShader(shaderType);
//		// Set the source characters and try to compile
//		glShaderSource(outShader, 1, &(contentsChar), nullptr);
//		glCompileShader(outShader);
//		
//		if (!IsCompiled(outShader))
//		{
//			SDL_Log("Failed to compile shader %s", fileName.c_str());
//			return false;
//		}
//	}
//	else
//	{
//		SDL_Log("Shader file not found: %s", fileName.c_str());
//		return false;
//	}
//	
//	return true;
//}

//bool Shader::IsCompiled(GLuint shader)
//{
//	GLint status;
//	// Query the compile status
//	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
//	
//	if (status != GL_TRUE)
//	{
//		char buffer[512];
//		memset(buffer, 0, 512);
//		glGetShaderInfoLog(shader, 511, nullptr, buffer);
//		SDL_Log("GLSL Compile Failed:\n%s", buffer);
//		return false;
//	}
//	
//	return true;
//}
bool Shader::IsCompiled(GLuint shader)
{
	GLint status;
	//コンパイル状態の問い合わせ
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];		//エラー内容表示用char配列
		memset(buffer, 0, 512);		//bufferの512バイトを0(null)でセットする
		//ログ情報の取得(shaderId, logを格納する文字バッファのサイズ指定, infoLogに返される文字列の長さ[nullptrのときは文字バッファ分], 情報ログに使用する文字の配列)
		glGetShaderInfoLog(shader, 511, nullptr, buffer);
		SDL_Log("GLSLのコンパイルが失敗しました: \n%s", buffer);
		return false;
	}
	return true;
}


bool Shader::IsValidProgram()
{
	//
	//GLint status;
	//// Query the link status
	//glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
	//if (status != GL_TRUE)
	//{
	//	char buffer[512];
	//	memset(buffer, 0, 512);
	//	glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
	//	SDL_Log("GLSL Link Status:\n%s", buffer);
	//	return false;
	//}
	//
	//return true;
	GLint status;
	//リンク情報の問い合わせ
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		//ログ情報の取得
		glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
		SDL_Log("GLSLのリンクが失敗しました: \n%s", buffer);
		return false;
	}
	return true;
}
