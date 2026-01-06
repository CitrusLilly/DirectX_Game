#pragma once

// インクルード
#include <d3d11.h>
#include <assert.h>

// リンカ
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

// マクロ
#define SAFE_RELEASE(p) if(p) { p->Release(); p = nullptr; }	// 安全に解放
#define SAFE_DELETE(p) if(p) { delete p; p = nullptr;}			// 安全に削除

namespace Direct3D {
	// 変数
	extern ID3D11Device* pDevice;				// デバイス
	extern ID3D11DeviceContext* pContext;		// デバイスコンテキスト

	// 初期化
	HRESULT Initialize(int winW,int winH,HWND hWnd);
	// シェーダー初期化
	HRESULT InitShader();
	// 描画開始
	void BeginDraw();
	// 描画終了
	void EndDraw();
	// 解放
	void Release();
}