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
	//頂点配列オブジェクトの作成
	/*
	void glGenVertexArrays(GLsizei n, GLuint *arrays)
	n 個の頂点配列オブジェクトを作成し, そのオブジェクト名 (整数値で表される識別子) を arrays に指定された配列の要素に格納します.
	*/
	glGenVertexArrays(1, &mVertexArray);	//いくつ作成するかと、頂点配列オブジェクトのIDを渡す
	/*
	void glBindVertexArray(GLuint array)
	arrays に指定されたオブジェクト名の頂点配列オブジェクトを結合します. 頂点配列オブジェクトは結合されている間使用できます. array が 0 の時は現在結合されている頂点配列オブジェクトの結合を解除します.
	*/
	//mVertexArrayを頂点配列オブジェクトとしてバインド（頂点配列オブジェクトとして使う）という意味では？
	glBindVertexArray(mVertexArray);;

	//頂点バッファの作成
	//// Create vertex buffer
	//glGenBuffers(1, &mVertexBuffer);
	glGenBuffers(1, &mVertexBuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	//GL_ARRAY_BUFFERはバッファを頂点バッファとしてバインド（頂点バッファとして使う）という意味
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	////上で頂点バッファの作成は完了したので、引数の「頂点バッファをコピー」してやる
	//glBufferData(
	//	GL_ARRAY_BUFFER,		//バッファの種類(GL_ARRAY_BUFFER: 今作った頂点バッファを使用. IDを指定するのではない) => 上で指定したバッファの種類を指定
	//	numVerts * 3 * sizeof(float),		//コピーするサイズ
	//	verts,		//コピー元ポインタ
	//	GL_STATIC_DRAW		//データの利用方法(GL_STATIC_DRAW: GPUでデータのロードを1回行い、その後描画でデータを読み込む): GPUの動作を最適化するためのヒントとして使用される(詳細: http://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20120909)
	//);
		//上で頂点バッファの作成は完了したので、引数の「頂点バッファをコピー」してやる
	glBufferData(
		GL_ARRAY_BUFFER,		//バッファの種類(GL_ARRAY_BUFFER: 今作った頂点バッファを使用. IDを指定するのではない) => 上で指定したバッファの種類を指定
		numVerts * 5 * sizeof(float),		//コピーするサイズ
		verts,		//コピー元ポインタ
		GL_STATIC_DRAW		//データの利用方法(GL_STATIC_DRAW: GPUでデータのロードを1回行い、その後描画でデータを読み込む): GPUの動作を最適化するためのヒントとして使用される(詳細: http://marina.sys.wakayama-u.ac.jp/~tokoi/?date=20120909)
	);
	//glBufferData(GL_ARRAY_BUFFER, numVerts * 5 * sizeof(float), verts, GL_STATIC_DRAW);

	//インデックスバッファの作成
	//// Create index buffer
	//作り方は頂点バッファとほぼ同様
	glGenBuffers(1, &mIndexBuffer);
	//glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);		//インデックスバッファとして結合する(インデックスバッファとして使う)
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		numIndices * 3 * sizeof(unsigned int),
		indices,
		GL_STATIC_DRAW
	);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	//頂点レイアウト(頂点属性)の設定 => ポリゴンの頂点がどのようなデータ形式かを設定
	//// Specify the vertex attributes
	//// (For now, assume one vertex format)
	//// Position is 3 floats starting at offset 0
	//引数に指定した頂点属性配列を有効にする(多分だが、頂点属性配列というのがあり、それを有効にしたりしているんだと思う)
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(
	//	0,		//位置情報を頂点属性0に指定(上で0を有効にされていることに注意)
	//	3,		//要素数(1つの頂点のサイズ)
	//	GL_FLOAT,		//要素の型
	//	GL_FALSE,		//整数の型でのみ使用するためFalseを指定
	//	sizeof(float) * 3,		//ストライド(=連続する頂点属性間のバイトオフセット
	//	//頂点バッファの頂点『データ』間のパディング(=間隔)はないため、ストライドは頂点のサイズとなる)
	//	0		//頂点データの開始位置からこの属性までのオフセット(唯一の属性であるため……それはつまり最初の属性となるため)
	//);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,		//位置情報を頂点属性0に指定(上で0を有効にされていることに注意)
		3,		//要素数(1つの頂点のサイズ)
		GL_FLOAT,		//要素の型
		GL_FALSE,		//整数の型でのみ使用するためFalseを指定
		sizeof(float) * 5,		//ストライド(=連続する頂点属性間のバイトオフセット
		//頂点バッファの頂点『データ』間のパディング(=間隔)はないため、ストライドは頂点のサイズとなる)
		0		//頂点データの開始位置からこの属性までのオフセット(唯一の属性であるため……それはつまり最初の属性となるため)
	);
	//第2の頂点座標(uv座標)
	glEnableVertexAttribArray(1);		//1番目の頂点座標を有効に
	glVertexAttribPointer(
		1,		//頂点座標index(n番目の頂点座標情報かがここにはいるため、上で有効にした頂点番号と同様のものが入る)
		2,		//要素数(uv座標は2つ)
		GL_FLOAT,		//要素の型
		GL_FALSE,		//整数の型でのみ使用するためFalseを指定
		sizeof(float) * 5,		//ストライド(普通、各頂点インデックスのサイズ)
		reinterpret_cast<void*>(sizeof(float) * 3)		//頂点インデックスの開始位置からこの属性までの位置(ただしオフセットポインタとして渡す必要あり)
		//頂点の先頭から何バイトあるか知る必要がある。これが sizeof(float) * 3
		//ただ、オフセットポインタとして渡す必要があるため、reinterpret_cast(ポインタ型を強制的に別のポインタ型に変換する)でvoid*ポインタ型変数に変換して渡している
		//float f = 2.0f; reinterpret_cast<int&>(f) とすると、floatとintは32bit機では同じサイズのため、floatの内容をそのまま16進数で吐き出せる
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
	//利用する頂点配列の指定
	glBindVertexArray(mVertexArray);
}
