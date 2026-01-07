#include "Quad.h"
#include "Camera.h"
// コンストラクタ
Quad::Quad()
	: pVertexBuffer_(nullptr)
	, pIndexBuffer_(nullptr)
	, pConstantBuffer_(nullptr)
	, pTexture_(nullptr)
{
}

// デストラクタ
Quad::~Quad() {
	Release();
}

// 初期化
HRESULT Quad::Initialize() {
	// エラー処理用
	HRESULT hr;
	// 頂点データ作成
	VERTEX vertices[] = {
		{XMVectorSet(-1.0f,1.0f,0.0f,0.0f),XMVectorSet(0.0f,0.0f,0.0f,0.0f)},	// 四角形の頂点(左上)
		{XMVectorSet(1.0f,1.0f,0.0f,0.0f), XMVectorSet(1.0f,0.0f,0.0f,0.0f)},	// 四角形の頂点(右上)
		{XMVectorSet(1.0f,-1.0f,0.0f,0.0f),XMVectorSet(1.0f,1.0f,0.0f,0.0f)},	// 四角形の頂点(右下)
		{XMVectorSet(-1.0f,-1.0f,0.0f,0.0f),XMVectorSet(0.0f,1.0f,0.0f,0.0f)}	// 四角形の頂点(左下)
	};

	// 頂点バッファ作成
	D3D11_BUFFER_DESC bd_vertex = {};
	bd_vertex.ByteWidth = sizeof(vertices);			// バッファの大きさ
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;			// 使用方法
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// 頂点バッファとして使う
	bd_vertex.CPUAccessFlags = 0;					// CPUからアクセスしない
	bd_vertex.MiscFlags = 0;						// その他オプション
	bd_vertex.StructureByteStride = 0;				// 構造体のサイズ(不明でよい)

	D3D11_SUBRESOURCE_DATA data_vertex = {};
	data_vertex.pSysMem = vertices; // 頂点データアドレス
	hr = Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
	if(FAILED(hr)) {
		MessageBox(nullptr, L"頂点バッファの作成に失敗しました。", L"エラー", MB_OK);
		return E_FAIL;
	}

	// インデックス情報
	int index[] = {0,2,3, 0,1,2};

	// インデックスバッファ作成
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;				// 使用方法
	bd.ByteWidth = sizeof(index);				// バッファの大きさ
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;		// インデックスバッファとして使う
	bd.CPUAccessFlags = 0;						// CPUからアクセスしない
	bd.MiscFlags = 0;							// その他オプション

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = index;			// インデックスデータアドレス
	initData.SysMemPitch = 0;			// 1つのデータサイズ(不明でよい)
	initData.SysMemSlicePitch = 0;		// 3Dテクスチャ用(不明でよい)
	hr = Direct3D::pDevice_->CreateBuffer(&bd, &initData, &pIndexBuffer_);
	if(FAILED(hr)) {
		MessageBox(nullptr, L"インデックスバッファの作成に失敗しました。", L"エラー", MB_OK);
		return E_FAIL;
	}

	// コンスタントバッファ作成
	D3D11_BUFFER_DESC cd = {};
	cd.ByteWidth = sizeof(CONSTANT_BUFFER);		// バッファの大きさ
	cd.Usage = D3D11_USAGE_DYNAMIC;				// 使用方法
	cd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	// コンスタントバッファとして使う
	cd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPUからアクセスする
	cd.MiscFlags = 0;							// その他オプション
	cd.StructureByteStride = 0;					// 構造体のサイズ(不明でよい)
	hr = Direct3D::pDevice_->CreateBuffer(&cd, nullptr, &pConstantBuffer_);
	if(FAILED(hr)) {
		MessageBox(nullptr, L"コンスタントバッファの作成に失敗しました。", L"エラー", MB_OK);
		return E_FAIL;
	}

	// テクスチャ読み込み
	pTexture_ = new Texture();
	pTexture_->Load("Assets\\dice.png");

	return S_OK;
}

// 描画
void Quad::Draw(XMMATRIX& wolrdMatrix) {
	// コンスタントバッファに渡す情報
	CONSTANT_BUFFER cb = {};
	// 行列の転置
	cb.matWVP = XMMatrixTranspose(wolrdMatrix * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());

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
		sizeof(CONSTANT_BUFFER),	// 書き込み先サイズ
		&cb,						// 書き込み元アドレス
		sizeof(CONSTANT_BUFFER)		// 書き込み元サイズ
	);

	// ピクセルシェーダーにサンプラー設定
	ID3D11SamplerState* pSampler = pTexture_->GetSampler();
	Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

	// ピクセルシェーダーにテクスチャ設定
	ID3D11ShaderResourceView* pSRV = pTexture_->GetSRV();
	Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);

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

	// 描画
	Direct3D::pContext_->DrawIndexed(6, 0, 0);
}

// 解放
void Quad::Release() {
	// 作った逆順に解放
	pTexture_->Release();
	SAFE_DELETE(pTexture_);

	SAFE_RELEASE(pConstantBuffer_);
	SAFE_RELEASE(pIndexBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
}