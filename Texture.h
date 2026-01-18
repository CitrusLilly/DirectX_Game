#pragma once
#include <d3d11.h>
#include "string"
#include <wrl.h>

using namespace Microsoft::WRL;
using std::string;

namespace CoreEngine {

	class Texture {
		ID3D11SamplerState* pSampler_;		// サンプラーステート
		ID3D11ShaderResourceView* pSRV_;	// シェーダーリソースビュー
	public:
		Texture();							// コンストラクタ
		~Texture();							// デストラクタ
		HRESULT Load(string fileName);		// テクスチャ読み込み
		void Release();						// 解放
		ID3D11SamplerState* GetSampler();	// サンプラーステート取得
		ID3D11ShaderResourceView* GetSRV();	// シェーダーリソースビュー取得
	};
}
