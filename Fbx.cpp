#include "CoreEngine.h"
#include "Fbx.h"
#include "Direct3D.h"
#include "Camera.h"
#include <assert.h>

namespace CoreEngine {

	// 初期化
	Fbx::Fbx()
		: pVertexBuffer_(nullptr)
		, pIndexBuffer_(nullptr)
		, pConstantBuffer_(nullptr)
		, pMaterialList_(nullptr)
		, vertexCount_(0)
		, polygonCount_(0)
		, materialCount_(0) {

	}

	// デストラクタ
	Fbx::~Fbx() {
		Release();
	}

	// FBXファイルを読み込む
	HRESULT Fbx::Load(string fileName) {
		// マネージャを生成
		FbxManager* pFbxManager = FbxManager::Create();
		if (!pFbxManager) {
			Debug::Log("FBXマネージャー生成失敗");
			pFbxManager->Destroy();
			return E_FAIL;
		}

		// シーンを生成
		FbxScene* pFbxScene = FbxScene::Create(pFbxManager, "fbxscene");
		if (!pFbxScene) {
			Debug::Log("FBXシーン生成失敗");
			pFbxManager->Destroy();
			return E_FAIL;
		}

		// インポーターを生成
		FbxImporter* fbxImporter = FbxImporter::Create(pFbxManager, "imp");
		if (!fbxImporter) {
			Debug::Log("FBXインポーター生成失敗");
			pFbxManager->Destroy();
			pFbxScene->Destroy();
			return E_FAIL;
		}
		// インポート
		if (!fbxImporter->Initialize(fileName.c_str(), -1, pFbxManager->GetIOSettings())) {
			Debug::Log("FBXインポート失敗");
			fbxImporter->Destroy();
			pFbxScene->Destroy();
			pFbxManager->Destroy();
			return E_FAIL;
		}

		// SceneオブジェクトにFBXファイルの情報を流し込む
		if (!fbxImporter->Import(pFbxScene)) {
			Debug::Log("インポート失敗");
			fbxImporter->Destroy();
			pFbxScene->Destroy();
			pFbxManager->Destroy();
			return E_FAIL;
		}

		// ポリゴンを三角形に変換
		FbxGeometryConverter converter(pFbxManager);
		converter.Triangulate(pFbxScene, true);

		// メッシュ情報を取得
		FbxNode* rootNode = pFbxScene->GetRootNode();	// ルートノードを取得
		FbxNode* pNode = rootNode->GetChild(0);			// 頂点、インデックス、マテリアルを取得
		FbxMesh* mesh = pNode->GetMesh();				// メッシュを取得

		// 各情報の個数を取得
		vertexCount_ = mesh->GetControlPointsCount();	// 頂点数
		polygonCount_ = mesh->GetPolygonCount();		// ポリゴン数
		materialCount_ = pNode->GetMaterialCount();		// マテリアル数

		// 現在のcurrentディレクトリを覚えておく
		char defaultCurrentDir[MAX_PATH];
		GetCurrentDirectoryA(MAX_PATH, defaultCurrentDir);

		// 引数のfileNameからディレクトリ部分を取得
		char dir[MAX_PATH];
		_splitpath_s(fileName.c_str(), nullptr, 0, dir, MAX_PATH, nullptr, 0, nullptr, 0);

		// カレントディレクトリを変更
		SetCurrentDirectoryA(dir);

		InitVertex(mesh);		// 頂点バッファの初期化
		InitIndex(mesh);		// インデックスバッファの初期化
		InitConstantBuffer();	// コンスタントバッファの初期化
		InitMaterial(pNode);	// マテリアルの初期化

		// カレントディレクトリを戻す
		SetCurrentDirectoryA(defaultCurrentDir);

		// 解放
		fbxImporter->Destroy();
		pFbxScene->Destroy();
		pFbxManager->Destroy();
		return S_OK;
	}

	// 頂点バッファの初期化
	void Fbx::InitVertex(fbxsdk::FbxMesh* mesh) {
		// 頂点情報を入れる配列
		VERTEX* vertices = new VERTEX[vertexCount_];

		// 全ポリゴン
		for (DWORD poly = 0; poly < polygonCount_; poly++) {
			// 3頂点分
			for (int vertex = 0; vertex < 3; vertex++) {
				// 調べる頂点の番号
				int index = mesh->GetPolygonVertex(poly, vertex);

				// 頂点の位置
				FbxVector4 pos = mesh->GetControlPointAt(index);
				vertices[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2], 0.0f);

				// 頂点のUV
				// UVセット名を取得
				FbxStringList uvSetNames;
				mesh->GetUVSetNames(uvSetNames);
				const char* uvSetName = uvSetNames[0];

				// UV取得
				FbxVector2 uv;
				bool unmapped;
				mesh->GetPolygonVertexUV(poly, vertex, uvSetName, uv, unmapped);
				vertices[index].uv = XMVectorSet((float)uv[0], (float)(1.0f - uv[1]), 0, 0);

				// 頂点の法線
				FbxVector4 normal;
				mesh->GetPolygonVertexNormal(poly, vertex, normal);
				vertices[index].normal = XMVectorSet((float)normal[0], (float)normal[1], (float)normal[2], 0);
			}
		}

		// 頂点バッファ作成
		HRESULT hr;
		D3D11_BUFFER_DESC bd_vertex;
		bd_vertex.ByteWidth = sizeof(VERTEX) * vertexCount_;
		bd_vertex.Usage = D3D11_USAGE_DEFAULT;
		bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd_vertex.CPUAccessFlags = 0;
		bd_vertex.MiscFlags = 0;
		bd_vertex.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA data_vertex;
		data_vertex.pSysMem = vertices;
		hr = Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
		if (FAILED(hr)) {
			MessageBox(nullptr, "頂点バッファの作成に失敗しました", "エラー", MB_OK);
		}

		SAFE_DELETE_ARRAY(vertices);
	}

	// インデックスバッファの初期化
	void Fbx::InitIndex(fbxsdk::FbxMesh* mesh) {
		pIndexBuffer_ = new ID3D11Buffer * [materialCount_];
		indexCount_ = vector<int>(materialCount_);

		

		for (int i = 0; i < materialCount_; i++) {
			// ポリゴン数 * 3 ＝ 全頂点分用意
			vector<int> index;
			index.reserve(polygonCount_ * 3);
			int count = 0;

			// 全ポリゴン
			for (DWORD poly = 0; poly < polygonCount_; poly++) {
				FbxLayerElementMaterial* mtl = mesh->GetLayer(0)->GetMaterials();
				int mtlID = mtl->GetIndexArray().GetAt(poly);

				if (mtlID == i) {
					// 3頂点分
					for (DWORD vertex = 0; vertex < 3; vertex++) {
						index.push_back(mesh->GetPolygonVertex(poly, vertex));
						count++;
					}
				}
			}
			indexCount_[i] = count;

			// インデックスバッファの作成
			D3D11_BUFFER_DESC bd;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(int) * count;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = index.data();
			initData.SysMemPitch = 0;
			initData.SysMemSlicePitch = 0;

			HRESULT hr;
			hr = Direct3D::pDevice_->CreateBuffer(&bd, &initData, &pIndexBuffer_[i]);
			if (FAILED(hr)) {
				MessageBox(nullptr, "インデックスバッファの作成に失敗しました", "エラー", MB_OK);
			}
		}
	}

	// コンスタントバッファの初期化
	void Fbx::InitConstantBuffer() {
		D3D11_BUFFER_DESC cb;
		cb.ByteWidth = sizeof(CONSTANT_BUFFER);
		cb.Usage = D3D11_USAGE_DYNAMIC;
		cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cb.MiscFlags = 0;
		cb.StructureByteStride = 0;

		HRESULT hr;
		hr = Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
		if (FAILED(hr)) {
			MessageBox(nullptr, "コンスタントバッファの作成に失敗しました", "エラー", MB_OK);
		}
	}

	// マテリアルの初期化
	void Fbx::InitMaterial(fbxsdk::FbxNode* pNode) {
		pMaterialList_ = new MATERIAL[materialCount_];

		for (int i = 0; i < materialCount_; i++) {
			// i番目のマテリアル情報を取得
			FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);

			// テクスチャ情報
			FbxProperty lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

			// テクスチャの枚数
			int fileTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();

			// テクスチャあり
			if (fileTextureCount) {
				// 画像ファイルをロードする
				FbxFileTexture* textureInfo = lProperty.GetSrcObject<FbxFileTexture>(0);
				const char* textureFilePath = textureInfo->GetRelativeFileName();

				// ファイル名＋拡張子だけにする
				char name[_MAX_FNAME];	// ファイル名
				char ext[_MAX_EXT];		// 拡張子
				// 必要ないところはnullprt、サイズは0
				_splitpath_s(
					textureFilePath,	// 元のファイルパス
					nullptr,			// ドライブ名を入れる変数
					0,					// ドライブ名の最大文字数
					nullptr,			// フォルダ名を入れる変数
					0,					// フォルダ名の最大文字数
					name,				// ファイル名を入れる変数
					_MAX_FNAME,			// ファイル名の最大文字数
					ext,				// 拡張子を入れる変数
					_MAX_EXT			// 拡張子の最大文字数
				);
				char fileName[_MAX_PATH];
				sprintf_s(fileName, "%s%s", name, ext);

				pMaterialList_[i].pTexture = new Texture;
				HRESULT hr = pMaterialList_[i].pTexture->Load(fileName);
				assert(hr == S_OK);
				// テクスチャなし
			} else {
				pMaterialList_[i].pTexture = nullptr;

				// マテリアルの色を適用
				// Lambertマテリアルの場合
				if (pMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
					Debug::Log("Lambertマテリアル適用");
					FbxSurfaceLambert* pLambert = (FbxSurfaceLambert*)pMaterial;

					// Diffuse
					FbxDouble3 diffuse = pLambert->Diffuse;
					pMaterialList_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
				// Phongマテリアルの場合
				} else if(pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId)) {
					Debug::Log("Phongマテリアル適用");
					FbxSurfacePhong* pPhong = (FbxSurfacePhong*)pMaterial;

					// Diffuse
					FbxDouble3 diffuse = pPhong->Diffuse;
					pMaterialList_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);

					// Specular
					FbxDouble3 specular = pPhong->Specular;
					pMaterialList_[i].specular = XMFLOAT3((float)specular[0], (float)specular[1], (float)specular[2]);

					// Shininess
					pMaterialList_[i].shininess = (float)pPhong->Shininess;
				}
				
			}
		}
	}


	// Transformを適用してFBXモデルを描画
	void Fbx::Draw(Transform& transform) {
		Direct3D::SetShader(SHADER_3D);

		// トランスフォームの計算
		transform.Calclation();

		for (int i = 0; i < materialCount_; i++) {
			// コンスタントバッファに渡す情報
			CONSTANT_BUFFER cb = {};
			cb.matWVP = XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
			cb.matNormal = XMMatrixTranspose(transform.GetNormalMatrix());
			cb.diffuseColor = pMaterialList_[i].diffuse;
			cb.isTexture = pMaterialList_[i].pTexture != nullptr;

			D3D11_MAPPED_SUBRESOURCE pdata;
			Direct3D::pContext_->Map(		// GUPからのデータアクセスを止める
				pConstantBuffer_,			// マップするバッファ
				0,							// サブリソース
				D3D11_MAP_WRITE_DISCARD,	// 書き込み方法
				0,							// 追加オプション
				&pdata						// 書き込み先情報
			);
			memcpy_s(						// データ転送
				pdata.pData,				// 書き込み先アドレス
				pdata.RowPitch,				// 書き込み先サイズ
				(void*)(&cb),				// 書き込み元アドレス
				sizeof(cb)					// 書き込み元サイズ
			);

			Direct3D::pContext_->Unmap(pConstantBuffer_, 0); // GUPからのデータアクセスを再開

			// 頂点バッファセット
			UINT stride = sizeof(VERTEX);	// 1頂点あたりのサイズ
			UINT offset = 0;                // バッファの先頭から使う
			Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

			// インデックスバッファセット
			stride = sizeof(int);	// 1インデックスあたりのサイズ
			offset = 0;				// バッファの先頭から使う
			Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_[i], DXGI_FORMAT_R32_UINT, offset);

			// コンスタントバッファセット
			Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	// 頂点シェーダー用
			Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	// ピクセルシェーダー用

			if (pMaterialList_[i].pTexture) {
				ID3D11SamplerState* pSampler = pMaterialList_[i].pTexture->GetSampler();
				Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

				ID3D11ShaderResourceView* pSRV = pMaterialList_[i].pTexture->GetSRV();
				Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);
			}

			// 描画
			Direct3D::pContext_->DrawIndexed(indexCount_[i], 0, 0);
		}
	}

	// 解放
	void Fbx::Release() {
		for (int i = 0; i < materialCount_; i++) {
			if (pIndexBuffer_[i] != nullptr) {
				SAFE_RELEASE(pIndexBuffer_[i]);
			}
		}
		SAFE_DELETE_ARRAY(pIndexBuffer_);

		for (int i = 0; i < materialCount_; i++) {
			if (pMaterialList_[i].pTexture != nullptr) {
				pMaterialList_[i].pTexture->Release();
			}
		}
		SAFE_DELETE_ARRAY(pMaterialList_);
		
		SAFE_RELEASE(pConstantBuffer_);
		SAFE_RELEASE(pVertexBuffer_);
	}
}