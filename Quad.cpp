#include "Quad.h"

// コンストラクタ
Quad::Quad()
	: pVertexBuffer_(nullptr)
	, pIndexBuffer_(nullptr)
	, pConstantBuffer_(nullptr)
{
}

// デストラクタ
Quad::~Quad() {
}

// 初期化
void Quad::Initialize() {
	// 頂点データ作成
	XMVECTOR vertices[] = {
		XMVectorSet(-1.0f,1.0f,0.0f,0.0f),	// 左上
		XMVectorSet(1.0f,1.0f,0.0f,0.0f),	// 右上
		XMVectorSet(1.0f,-1.0f,0.0f,0.0f),	// 右下
		XMVectorSet(-1.0f,-1.0f,0.0f,0.0f),	// 左下
	};

	// 頂点バッファ作成
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(vertices);			// バッファの大きさ
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;			// 使用方法
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファとして使う
	bd_vertex.CPUAccessFlags = 0;					// CPUからアクセスしない
	bd_vertex.MiscFlags = 0;						// その他オプション
	bd_vertex.StructureByteStride = 0;				// 構造体のサイズ(不明でよい)

	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = vertices; // 頂点データアドレス
	Direct3D::pDevice->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);

	// インデックス情報
	int index[] = {0,2,3, 0,1,2};

	// インデックスバッファ作成
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;				// 使用方法
	bd.ByteWidth = sizeof(index);				// バッファの大きさ
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;		// インデックスバッファとして使う
	bd.CPUAccessFlags = 0;						// CPUからアクセスしない
	bd.MiscFlags = 0;							// その他オプション

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = index;			// インデックスデータアドレス
	initData.SysMemPitch = 0;			// 1つのデータサイズ(不明でよい)
	initData.SysMemSlicePitch = 0;		// 3Dテクスチャ用(不明でよい)
	Direct3D::pDevice->CreateBuffer(&bd, &initData, &pIndexBuffer_);

	// コンスタントバッファ作成
	D3D11_BUFFER_DESC cd;
	cd.ByteWidth = sizeof(CONSTANT_BUFFER);		// バッファの大きさ
	cd.Usage = D3D11_USAGE_DYNAMIC;				// 使用方法
	cd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// コンスタントバッファとして使う
	cd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPUからアクセスする
	cd.MiscFlags = 0;							// その他オプション
	cd.StructureByteStride = 0;					// 構造体のサイズ(不明でよい)
	Direct3D::pDevice->CreateBuffer(&cd, nullptr, &pConstantBuffer_);
}

// 描画
void Quad::Draw() {
	// コンスタントバッファに渡す情報
	XMVECTOR position = { 0,3,-10,0 }; // カメラの位置
	XMVECTOR target = { 0,0,0,0 };     // カメラの焦点
	// ビュー行列 カメラの位置、注視点、上方向ベクトル
	XMMATRIX view = XMMatrixLookAtLH(position, target, XMVectorSet(0, 1, 0, 0));
	// プロジェクション行列 レンズの設定
	XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, 800.0f / 600.0f, 0.1f, 100.0f);

	CONSTANT_BUFFER cb;
	cb.matWVP = XMMatrixTranspose(view * proj); // 行列の転置

	D3D11_MAPPED_SUBRESOURCE pdata;
	Direct3D::pContext->Map(		// GUPからのデータアクセスを止める
		pConstantBuffer_,			// マップするバッファ
		0,							// サブリソース
		D3D11_MAP_WRITE_DISCARD,	// 書き込み方法
		0,							// 追加オプション
		&pdata						// 書き込み先情報
	);
	memcpy_s(						// データ転送
		pdata.pData,				// 書き込み先アドレス
		sizeof(CONSTANT_BUFFER),	// 書き込み先サイズ
		&cb,						// 書き込み元アドレス
		sizeof(CONSTANT_BUFFER)		// 書き込み元サイズ
	);
	Direct3D::pContext->Unmap(pConstantBuffer_, 0); // GUPからのデータアクセスを再開

	// 頂点バッファセット
	UINT stride = sizeof(XMVECTOR); // 1頂点あたりのサイズ
	UINT offset = 0;                 // バッファの先頭から使う
	Direct3D::pContext->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// インデックスバッファセット
	stride = sizeof(int);	// 1インデックスあたりのサイズ
	offset = 0;				// バッファの先頭から使う
	Direct3D::pContext->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R32_UINT, offset);

	// コンスタントバッファセット
	Direct3D::pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	// 頂点シェーダー用
	Direct3D::pContext->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	// ピクセルシェーダー用

	// 描画
	Direct3D::pContext->DrawIndexed(6, 0, 0);
}

// 解放
void Quad::Release() {
	pConstantBuffer_->Release();
	pIndexBuffer_->Release();
	pVertexBuffer_->Release();
}