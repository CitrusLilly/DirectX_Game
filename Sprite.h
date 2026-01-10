#pragma once

#include "Direct3D.h"
#include "Texture.h"
#include <vector>

#define SAFE_DELETE_ARRAY(p) if(p != nullptr){delete[] p; p = nullptr;} // 安全に削除

class Sprite {
	// コンスタントバッファー
	struct CONSTANT_BUFFER {
		XMMATRIX matW;		// ワールド行列
	};

	// 頂点情報
	struct VERTEX {
		XMVECTOR position;	// 頂点座標
		XMVECTOR uv;		// UV座標
	};
protected:
	// ポインタは初めにnullptrで初期化しておく
	// 最後に解放を忘れずに！
	uint64_t vertexNum_;			// 頂点数
	std::vector<VERTEX> vertices_;	// 頂点データ配列
	ID3D11Buffer* pVertexBuffer_;	// 頂点バッファ

	uint64_t indexNum_;				// インデックス数
	std::vector<int> index_;		// インデックスデータ配列
	ID3D11Buffer* pIndexBuffer_;	// インデックスバッファ

	ID3D11Buffer* pConstantBuffer_; // コンスタントバッファ

	Texture* pTexture_;				// テクスチャ
public:
	Sprite();							// コンストラクタ
	~Sprite();							// デストラクタ
	HRESULT Initialize();				// 初期化
	void Draw(XMMATRIX& worldMatrix);	// 描画
	void Release();						// 解放
private:
	// ────────────Initializeから呼ばれる関数──────────────────
	virtual void InitVertexData();		// 頂点情報の初期化
	HRESULT CreateVertexBuffer();		// 頂点バッファ作成
	virtual void InitIndexData();		// インデックス情報の初期化
	HRESULT CreateIndexBuffer();		// インデックスバッファ作成
	HRESULT CreateConstantBuffer();		// コンスタントバッファ作成
	HRESULT LoadTexture();				// テクスチャ読み込み

	// ────────────Draw関数から呼ばれる関数────────────────────
	void PassDataToCB(XMMATRIX& worldMatrix);	// コンスタントバッファにデータを送る
	void SetBufferToPipeline();					// パイプラインに各種バッファをセットする
};