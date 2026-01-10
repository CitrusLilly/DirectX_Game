#pragma once

// インクルード
#include <d3d11.h>
#include <assert.h>
#include <DirectXMath.h>

// リンカ
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

// マクロ
#define SAFE_RELEASE(p) if(p) { p->Release(); p = nullptr; }	// 安全に解放
#define SAFE_DELETE(p) if(p) { delete p; p = nullptr;}			// 安全に削除

using namespace DirectX;

enum SHADER_TYPE {
	SHADER_2D,
	SHADER_3D,
	SHADER_MAX,
};

namespace Direct3D {
	// 変数
	extern ID3D11Device* pDevice_;				// デバイス
	extern ID3D11DeviceContext* pContext_;		// デバイスコンテキスト

	// 初期化
	HRESULT Initialize(int winW,int winH,HWND hWnd);
	// シェーダー初期化
	HRESULT InitShader();
	HRESULT InitShader3D();
	HRESULT InitShader2D();
	void SetShader(SHADER_TYPE type);
	// 描画開始
	void BeginDraw();
	// 描画終了
	void EndDraw();
	// 解放
	void Release();
}