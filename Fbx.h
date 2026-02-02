#pragma once

#include <d3d11.h>
#include <fbxsdk.h>
#include <string>
#include "Transform.h"
#include "Texture.h"
#include <vector>
#include <map>
#include "Utillity.h"

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
			XMFLOAT3 specular;	// RGB
			float	 shininess;	// 光沢
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
			Color color;		// 頂点カラー
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
		// デストラクタ
		~Fbx();
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

	private:
		struct MeshData {
			ID3D11Buffer* pVertexBuffer;	// 頂点バッファ
			ID3D11Buffer* pIndexBuffer;	// インデックスバッファ
			std::vector<CustomVERTEX> vertex;
			std::vector<UINT> index;
			std::string materialName;

			// コンストラクタで初期化
			MeshData()
				:pVertexBuffer(nullptr)
				,pIndexBuffer(nullptr)
				,vertex()
				,index() {
			}

			// デストラクタ
			~MeshData() {
				SAFE_RELEASE(pVertexBuffer);
				SAFE_RELEASE(pIndexBuffer);
			}
		};

		std::vector<MeshData> meshList_;
		std::map<std::string, Material> materialList_;
		std::map<std::string, ID3D11ShaderResourceView*> textures_;
		std::map<std::string, ID3D11ShaderResourceView*> materialLinks_;

		// FBXファイルの読み込み
		bool LoadFbxFile(std::string fileName);
		// 頂点バッファの作成
		bool CreateVertexBuffer();
		// インデックスバッファの作成
		bool CreateIndexBuffer();

		// マテリアルの読み込み
		void LoadMaterial(FbxSurfaceMaterial* material);
		// テクスチャの読み込み
		bool LoadTexture(FbxFileTexture* texture, std::string& keyword);

		// メッシュを作成
		void CreateMesh(FbxMesh* mesh);
		// 頂点読み込み
		void LoadVertex(MeshData& meshData, FbxMesh* mesh);
		// インデックス読み込み
		void LoadIndex(MeshData& meshData, FbxMesh* mesh);
		// 法線読み込み
		void LoadNormal(MeshData& meshData, FbxMesh* mesh);
		// UV読み込み
		void LoadUV(MeshData& meshData, FbxMesh* mesh);
		// 頂点カラー読み込み
		void LoadColor(MeshData& meshData, FbxMesh* mesh);
		// マテリアル名読み込み
		void SetMaterialName(MeshData& meshData, FbxMesh* mesh);

	};
}