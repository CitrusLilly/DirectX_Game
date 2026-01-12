#include "Direct3D.h"
#include "Texture.h"

#include <DirectXTex.h>
// DirextXTexのライブラリをリンク
#pragma comment(lib,"DirectXTex.lib")

// コンストラクタ
Texture::Texture()
	:pSampler_(nullptr),
	pSRV_(nullptr)
{

}

// デストラクタ
Texture::~Texture() {
	Release();
}

// テクスチャ読み込み
HRESULT Texture::Load(string fileName) {
	using namespace DirectX;

	// マルチバイト文字列をワイド文字列に変換
	wchar_t wtext[FILENAME_MAX];
	size_t ret;
	mbstowcs_s(&ret, wtext, fileName.c_str(), fileName.length());

	TexMetadata metadata;	// 画像の付属情報
	ScratchImage image;		// 画像本体
	HRESULT hr;
	// WIC形式の画像を読み込む
	hr = LoadFromWICFile(wtext, WIC_FLAGS::WIC_FLAGS_NONE, &metadata, image);
	if(FAILED(hr)) {
		return E_FAIL;
	}

	// テクスチャ生成
	D3D11_SAMPLER_DESC samDesc = {};
	ZeroMemory(&samDesc, sizeof(D3D11_SAMPLER_DESC));
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	// 線形補間
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;		// テクスチャ座標U方向のはみ出しをクランプ
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;		// テクスチャ座標V方向のはみ出しをクランプ
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;		// テクスチャ座標W方向のはみ出しをクランプ
	hr = Direct3D::pDevice_->CreateSamplerState(&samDesc, &pSampler_);
	if (FAILED(hr)) {
		return E_FAIL;
	}

	// シェーダーリソースビューの生成
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// データ形式
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	srv.Texture2D.MipLevels = 1;				// ミップマップのレベル数

	hr = CreateShaderResourceView(
		Direct3D::pDevice_,		// デバイス
		image.GetImages(),		// 画像データ
		image.GetImageCount(),	// 画像データの数
		metadata,				// 画像の付属情報
		&pSRV_					// シェーダーリソースビューのポインタ
	);
	if(FAILED(hr)) {
		return E_FAIL;
	}
	
	return S_OK;
}

// 解放
void Texture::Release() {
	SAFE_RELEASE(pSampler_);
	SAFE_RELEASE(pSRV_);
}

// サンプラーステート取得
ID3D11SamplerState* Texture::GetSampler() {
	return pSampler_;
}

// シェーダーリソースビュー取得
ID3D11ShaderResourceView* Texture::GetSRV() {
	return pSRV_;
}