#include "Fbx.h"
#include "Direct3D.h"
#include "Camera.h"

// 初期化
Fbx::Fbx()
	: pVertexBuffer_(nullptr)
	, pIndexBuffer_(nullptr)
	, pConstantBuffer_(nullptr)
	, vertexCount_(0)
	, polygonCount_(0)
{

}

// FBXファイルを読み込む
HRESULT Fbx::Load(string fileName) {
	// マネージャを生成
	FbxManager* pFbxManager = FbxManager::Create();

	// インポーターを生成
	FbxImporter* fbxImporter = FbxImporter::Create(pFbxManager, "imp");
	fbxImporter->Initialize(fileName.c_str(), -1, pFbxManager->GetIOSettings());

	// SceneオブジェクトにFBXファイルの情報を流し込む
	FbxScene* pFbxScene = FbxScene::Create(pFbxManager, "fbxscene");
	fbxImporter->Import(pFbxScene);
	fbxImporter->Destroy();

	// メッシュ情報を取得
	FbxNode* rootNode = pFbxScene->GetRootNode();	// ルートノードを取得
	FbxNode* pNode = rootNode->GetChild(0);			// 頂点、インデックス、マテリアルを取得
	FbxMesh* mesh = pNode->GetMesh();				// メッシュを取得

	// 各情報の個数を取得
	vertexCount_ = mesh->GetControlPointsCount();	// 頂点数
	polygonCount_ = mesh->GetPolygonCount();		// ポリゴン数

	InitVertex(mesh);		// 頂点バッファの初期化
	InitIndex(mesh);		// インデックスバッファの初期化
	InitConstantBuffer();	// コンスタントバッファの初期化

	// マネージャを解放
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
			int index = mesh->GetPolygonVertex(poly,vertex);

			// 頂点の位置
			FbxVector4 pos = mesh->GetControlPointAt(index);
			vertices[index].position = XMVectorSet((float)pos[0], (float)pos[1], (float)pos[2],0.0f);
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
	hr = Direct3D::pDevice_->CreateBuffer(&bd_vertex,&data_vertex,&pVertexBuffer_);
	if (FAILED(hr)) {
		MessageBox(nullptr, L"頂点バッファの作成に失敗しました", L"エラー", MB_OK);
	}
}

// インデックスバッファの初期化
void Fbx::InitIndex(fbxsdk::FbxMesh* mesh) {
	int* index = new int[polygonCount_ * 3];
	int count = 0;

	// 全ポリゴン
	for (DWORD poly = 0; poly < polygonCount_; poly++) {
		// 3頂点分
		for (DWORD vertex = 0; vertex < 3; vertex++) {
			index[count] = mesh->GetPolygonVertex(poly, vertex);
			count++;
		}
	}

	// インデックスバッファの作成
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(int) * polygonCount_ * 3;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = index;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	HRESULT hr;
	hr = Direct3D::pDevice_->CreateBuffer(&bd, &initData, &pIndexBuffer_);
	if (FAILED(hr)) {
		MessageBox(nullptr, L"インデックスバッファの作成に失敗しました", L"エラー", MB_OK);
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
	hr = Direct3D::pDevice_->CreateBuffer(&cb,nullptr,&pConstantBuffer_);
	if (FAILED(hr)) {
		MessageBox(nullptr, L"コンスタントバッファの作成に失敗しました", L"エラー", MB_OK);
	}
}

// Transformを適用してFBXモデルを描画
void Fbx::Draw(Transform& transform) {
	Direct3D::SetShader(SHADER_3D);

	// トランスフォームの計算
	transform.Calclation();

	// コンスタントバッファに渡す情報
	CONSTANT_BUFFER cb = {};
	// 行列の転置
	cb.matWVP = XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
	cb.matNormal = XMMatrixTranspose(transform.GetNormalMatrix());

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
	Direct3D::pContext_->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, offset);

	// コンスタントバッファセット
	Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	// 頂点シェーダー用
	Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	// ピクセルシェーダー用

	Direct3D::pContext_->DrawIndexed(polygonCount_ * 3, 0, 0);
}

// 解放
void Fbx::Release() {
	SAFE_RELEASE(pConstantBuffer_);
	SAFE_RELEASE(pIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
}