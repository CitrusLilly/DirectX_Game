#pragma once

#include <DirectXMath.h>
#include "Direct3D.h"
#include "Texture.h"

using namespace DirectX;

// コンスタントバッファー
struct CONSTANT_BUFFER {
	XMMATRIX matWVP;	// ワールドビュー・プロジェクション行列
	XMMATRIX matW;		// ワールド行列
};

// 頂点情報
struct VERTEX {
	XMVECTOR position;	// 頂点座標
	XMVECTOR uv;		// UV座標
	XMVECTOR normal;	// 法線ベクトル
};

class Quad {
	// ポインタは初めにnullptrで初期化しておく
	// 最後に解放を忘れずに！
	ID3D11Buffer* pVertexBuffer_;	// 頂点バッファ
	ID3D11Buffer* pIndexBuffer_;	// インデックスバッファ
	ID3D11Buffer* pConstantBuffer_; // コンスタントバッファ

	Texture* pTexture_;				// テクスチャ
public:
	Quad();								// コンストラクタ
	~Quad();							// デストラクタ
	HRESULT Initialize();				// 初期化
	void Draw(XMMATRIX& worldMatrix);	// 描画
	void Release();						// 解放
};