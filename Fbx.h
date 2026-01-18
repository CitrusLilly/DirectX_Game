#pragma once

#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include "Transform.h"
#include "Texture.h"
#include <vector>

#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "zlib-md.lib")

using std::string;
using std::vector;

namespace CoreEngine {

	class Fbx {
		// マテリアル
		struct MATERIAL {
			Texture* pTexture;	// テクスチャ
			XMFLOAT4 diffuse;	// 色
		};

		// コンスタントバッファー
		struct CONSTANT_BUFFER {
			XMMATRIX matWVP;		// ワールドビュー・プロジェクション行列
			XMMATRIX matNormal;		// ワールド行列
			XMFLOAT4 diffuseColor;	// マテリアルの色
			int isTexture;			// テクスチャを貼っているかどうか
		};

		// 頂点情報
		struct VERTEX {
			XMVECTOR position;	// 頂点座標
			XMVECTOR uv;		// UV
			XMVECTOR normal;	// 法線ベクトル
		};

		int vertexCount_;	// 頂点数
		int polygonCount_;	// ポリゴン数
		int materialCount_;	// マテリアル数

		ID3D11Buffer* pVertexBuffer_;	// 頂点バッファ
		ID3D11Buffer** pIndexBuffer_;	// インデックスバッファ
		ID3D11Buffer* pConstantBuffer_; // コンスタントバッファ
		MATERIAL* pMaterialList_;		// マテリアル
		vector<int> indexCount_;

	public:
		// 初期化
		Fbx();
		// FBXファイルを読み込む
		HRESULT Load(string fileName);
		// 頂点バッファの初期化
		void InitVertex(fbxsdk::FbxMesh* mesh);
		// インデックスバッファの初期化
		void InitIndex(fbxsdk::FbxMesh* mesh);
		// コンスタントバッファの初期化
		void InitConstantBuffer();
		// マテリアルの初期化
		void InitMaterial(fbxsdk::FbxNode* pNode);
		// Transformを適用してFBXモデルを描画
		void Draw(Transform& transform);
		// 解放
		void Release();
	};
}