// // ----------------------------------------------------------------
// // From Game Programming in C++ by Sanjay Madhav
// // Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// // 
// // Released under the BSD License
// // See LICENSE in root directory for full details.
// // ----------------------------------------------------------------

// // Request GLSL 3.3
// #version 330

// // Uniforms for world transform and view-proj
// uniform mat4 uWorldTransform;
// uniform mat4 uViewProj;
// // Uniform for matrix palette
// uniform mat4 uMatrixPalette[96];

// // Attribute 0 is position, 1 is normal,
// // 2 is bone indices, 3 is weights,
// // 4 is tex coords.
// layout(location = 0) in vec3 inPosition;
// layout(location = 1) in vec3 inNormal;
// layout(location = 2) in uvec4 inSkinBones;
// layout(location = 3) in vec4 inSkinWeights;
// layout(location = 4) in vec2 inTexCoord;

// // Any vertex outputs (other than position)
// out vec2 fragTexCoord;
// // Normal (in world space)
// out vec3 fragNormal;
// // Position (in world space)
// out vec3 fragWorldPos;

// void main()
// {
// 	// Convert position to homogeneous coordinates
// 	vec4 pos = vec4(inPosition, 1.0);
	
// 	// Skin the position
// 	vec4 skinnedPos = (pos * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x;
// 	skinnedPos += (pos * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y;
// 	skinnedPos += (pos * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z;
// 	skinnedPos += (pos * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;

// 	// Transform position to world space
// 	skinnedPos = skinnedPos * uWorldTransform;
// 	// Save world position
// 	fragWorldPos = skinnedPos.xyz;
// 	// Transform to clip space
// 	gl_Position = skinnedPos * uViewProj;

// 	// Skin the vertex normal
// 	vec4 skinnedNormal = vec4(inNormal, 0.0f);
// 	skinnedNormal = (skinnedNormal * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x
// 		+ (skinnedNormal * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y
// 		+ (skinnedNormal * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z
// 		+ (skinnedNormal * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;
// 	// Transform normal into world space (w = 0)
// 	fragNormal = (skinnedNormal * uWorldTransform).xyz;

// 	// Pass along the texture coordinate to frag shader
// 	fragTexCoord = inTexCoord;
// }

// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// Uniforms for world transform and view-proj
uniform mat4 uWorldTransform;
uniform mat4 uViewProj;
//行列パレット(配列)の取得(各ポイントの情報で渡すべきデータではないため)
uniform mat4 uMatrixPalette[96];

// Attribute 0 is position, 1 is normal, 2 is tex coords.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in uvec4 inSkinBones;
layout(location = 3) in vec4 inSkinWeights;
layout(location = 4) in vec2 inTexCoord;

// Any vertex outputs (other than position)
out vec2 fragTexCoord;
// Normal (in world space)
out vec3 fragNormal;
// Position (in world space)
out vec3 fragWorldPos;

void main()
{
	// Convert position to homogeneous coordinates
	//位置を同次座標に変換(モデル座標)
	vec4 pos = vec4(inPosition, 1.0);

	//位置のスキニング(uMatrixPaletteはモデル空間上での位置変換行列. つまり、skinnedPosはモデル上の位置となる)
	vec4 skinnedPos = (pos * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x;		//inSkinBones, inSkinWeightsは4次元ベクトルのため, x, y, z, wでアクセスする
	skinnedPos +=  (pos * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y;
	skinnedPos +=  (pos * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z;
	skinnedPos +=  (pos * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;

	//位置をワールド座標に変換
	skinnedPos = skinnedPos * uWorldTransform;
	//ワールド座標を保存(フラグメントシェーダーに送るため)
	fragWorldPos = skinnedPos.xyz;
	//クリップ座標に変換(頂点シェーダーではgl_Positionという組み込み変数に頂点データを渡さなければならない)
	gl_Position = skinnedPos * uViewProj;

	//頂点法線のスキニング
	vec4 skinnedNormal = vec4(inNormal, 0.0f);
	skinnedNormal = (skinnedNormal * uMatrixPalette[inSkinBones.x]) * inSkinWeights.x
	+ (skinnedNormal * uMatrixPalette[inSkinBones.y]) * inSkinWeights.y
	+ (skinnedNormal * uMatrixPalette[inSkinBones.z]) * inSkinWeights.z
	+ (skinnedNormal * uMatrixPalette[inSkinBones.w]) * inSkinWeights.w;
	//法線をワールド座標に変換(w = 0)し、フラグメントシェーダーに送る
	fragNormal = (skinnedNormal * uWorldTransform).xyz;
	//テクスチャ座標をフラグメントシェーダーに渡す
	fragTexCoord = inTexCoord;
}
