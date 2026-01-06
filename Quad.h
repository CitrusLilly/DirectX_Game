#pragma once

#include "Direct3D.h"
#include <DirectXMath.h>

using namespace DirectX;

// コンスタントバッファー
struct CONSTANT_BUFFER {
	XMMATRIX matWVP;
};

class Quad {
	// ポインタは初めにnullptrで初期化しておく
	// 最後に解放を忘れずに！
	ID3D11Buffer* pVertexBuffer_;	// 頂点バッファ
	ID3D11Buffer* pIndexBuffer_;	// インデックスバッファ
	ID3D11Buffer* pConstantBuffer_; // コンスタントバッファ
public:
	Quad();				// コンストラクタ
	~Quad();			// デストラクタ
	HRESULT Initialize();	// 初期化
	void Draw();		// 描画
	void Release();		// 解放
};