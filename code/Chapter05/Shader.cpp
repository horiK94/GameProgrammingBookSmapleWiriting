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
	//���_�V�F�[�_�[�ƃt���O�����g�V�F�[�_�[���R���p�C������
	if (!CompileShader(vertName, GL_VERTEX_SHADER, mVertexShader) ||
		!CompileShader(fragName, GL_FRAGMENT_SHADER, mFragShader))
	{
		return false;
	}


	//�o�[�e�b�N�X�V�F�[�_�[�ƃt���O�����g�V�F�[�_�[�������N�����ăV�F�[�_�[�v���O�����̍쐬
	mShaderProgram = glCreateProgram();			//�V�F�[�_�[�v���O�����̍쐬
	glAttachShader(mShaderProgram, mVertexShader);
	glAttachShader(mShaderProgram, mFragShader);
	glLinkProgram(mShaderProgram);		//�����N���s��
	if (!IsValidProgram())		//�����N����肭�s���Ă��邩�m�F
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
	//�V�F�[�_�[���A�N�e�B�u�ɂ���
	//glUseProgram(mShaderProgram);
	glUseProgram(mShaderProgram);
	//OpenGL�̓A�N�e�B�u�ɂ����V�F�[�_�[���g���ĎO�p�`��`�悷��
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
	// ���̖��O��uniform������(�V�F�[�_�[id��uniform���������ɂ��ēn��)
	//����uniform���X�V����ۂ́A����id�̂��߂��������₢���킹��K�v�͂Ȃ�
	GLuint loc = glGetUniformLocation(mShaderProgram, name);
	// �s��f�[�^��uniform�ɑ���
	glUniformMatrix4fv(
		loc,		//Uniform ID
		1,			//�s��̐�
		GL_TRUE,	//�s�x�N�g�����g���ꍇ��true
		matrix.GetAsFloatPtr()	//�s��ւ�float�^�|�C���^
	);
}


bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
{
	//�t�@�C�����J�����߂�ifStream�쐬
	std::ifstream shaderFile(fileName);
	if (shaderFile.is_open())
	{
		//�V�F�[�_�[�t�@�C�����J������
		//1�̕������shader file�̒��̕��͂�ǂݍ���
		std::stringstream sstream;		//������ (std::basic_string) �x�[�X�̃X�g���[���ɑ΂�����o�͑��������
		sstream << shaderFile.rdbuf();		//sstream��shaderFile�ɕR�t�����X�g���[���o�b�t�@��Ԃ�
		std::string contents = sstream.str();		//sstream�ɕR�t���Ă�������������擾
		const char* contentsChar = contents.c_str();			//c_str()��C����ł̕�����\���ɕύX���Ă���

		//�w�肳�ꂽ�^�C�v�̃V�F�[�_�[���쐬
		outShader = glCreateShader(shaderType);
		//�R���p�C���ΏۂƂ��ēǂݍ��񂾕�������w��
		glShaderSource(outShader, 1, &(contentsChar), nullptr);
		//�R�[�h�̃R���p�C��
		glCompileShader(outShader);

		if (!IsCompiled(outShader))
		{
			//�R���p�C�����s������
			SDL_Log("�V�F�[�_�[ %s �̃R���p�C���Ɏ��s���܂���", fileName.c_str());
			return false;
		}
	}
	else
	{
		SDL_Log("�V�F�[�_�[�t�@�C�� %s ��������܂���", fileName.c_str());
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
	//�R���p�C����Ԃ̖₢���킹
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];		//�G���[���e�\���pchar�z��
		memset(buffer, 0, 512);		//buffer��512�o�C�g��0(null)�ŃZ�b�g����
		//���O���̎擾(shaderId, log���i�[���镶���o�b�t�@�̃T�C�Y�w��, infoLog�ɕԂ���镶����̒���[nullptr�̂Ƃ��͕����o�b�t�@��], ��񃍃O�Ɏg�p���镶���̔z��)
		glGetShaderInfoLog(shader, 511, nullptr, buffer);
		SDL_Log("GLSL�̃R���p�C�������s���܂���: \n%s", buffer);
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
	//�����N���̖₢���킹
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		//���O���̎擾
		glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
		SDL_Log("GLSL�̃����N�����s���܂���: \n%s", buffer);
		return false;
	}
	return true;
}
