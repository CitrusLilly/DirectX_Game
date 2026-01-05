#include "Direct3D.h"

// 変数
namespace Direct3D {
	ID3D11Device* pDevice;						// デバイス
	ID3D11DeviceContext* pContext;				// デバイスコンテキスト
	IDXGISwapChain* pSwapChain;					// スワップチェイン
	ID3D11RenderTargetView* pRenderTargetView;	// レンダーターゲットビュー
}

// 初期化
void Direct3D::Initialize(int winW, int winH, HWND hWnd) {
	// スワップチェイン
	DXGI_SWAP_CHAIN_DESC scDesc;
	ZeroMemory(&scDesc, sizeof(scDesc));
	// 描画先のフォーマット
	scDesc.BufferDesc.Width = winW;							// 幅
	scDesc.BufferDesc.Height = winH;						// 高さ
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 色の指定
	// FPS(1/60秒に1回)
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	// その他
	scDesc.Windowed = TRUE;			// ウィンドウモードかフルスクリーンか
	scDesc.OutputWindow = hWnd;		// ウィンドウハンドル
	scDesc.BufferCount = 1;			// バックバッファの枚数
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// バックバッファの使い道
	scDesc.SampleDesc.Count = 1;	// MSAA(アンチエイリアス)の設定
	scDesc.SampleDesc.Quality = 0;

	// デバイス、コンテキスト、スワップチェインを作成
	D3D_FEATURE_LEVEL level;
	D3D11CreateDeviceAndSwapChain(
		nullptr,					// ビデオアダプタの指定。nullでよし
		D3D_DRIVER_TYPE_HARDWARE,	// ドライバタイプ
		nullptr,					// 上記がSOFTWAREじゃなければnullptr
		0,							// 何らかのフラグ
		nullptr,					// デバイス、コンテキストのレベル設定。nullでよし
		0,							// 上記でレベルを何個指定したか
		D3D11_SDK_VERSION,			// SDKバージョン。必ずこれ
		&scDesc,					// 上で設定したスワップチェイン構造体
		&pSwapChain,				// 完成したスワップチェインのアドレスが返ってくる
		&pDevice,					// 完成したデバイスアドレスが返ってくる
		&level,						// 完成したデバイス、コンテキストのレベルが返ってくる
		&pContext					// 完成したコンテキストのアドレスが返ってくる
	);

	// スワップチェインからバックバッファを取得
	ID3D11Texture2D* pBackBuffer;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	// レンダーターゲットビューの作成
	pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
	// 一時的にバックバッファを取得しただけなので解放
	pBackBuffer->Release();

	// ビューポート設定(描画範囲)
	D3D11_VIEWPORT vp;
	vp.Width = (float)winW;		// 幅
	vp.Height = (float)winH;	// 高さ
	vp.MinDepth = 0.0f;			// 手前
	vp.MaxDepth = 1.0f;			// 奥
	vp.TopLeftX = 0;			// 左
	vp.TopLeftY = 0;			// 上

	// データを画面に描画するための一通りの設定(パイプライン)
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// データの入力種類
	pContext->RSSetViewports(1, &vp);											// 描画範囲指定(vp)
	pContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);				// 描画先を設定

}

// 描画開始
void Direct3D::BeginDraw() {
	// 背景色
	float clearColor[4] = { 0.0f,0.0f,1.0f,1.0f };	// R,G,B,A

	// 画面をクリア
	pContext->ClearRenderTargetView(pRenderTargetView, clearColor);
}

// 描画終了
void Direct3D::EndDraw() {
	// スワップ(バックバッファを表に表示)
	pSwapChain->Present(0, 0);
}

// 解放
void Direct3D::Release() {
	pRenderTargetView->Release();
	pSwapChain->Release();
	pContext->Release();
	pDevice->Release();
}