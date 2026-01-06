#include <d3dcompiler.h>
#include "Direct3D.h"

// 変数
namespace Direct3D {
	ID3D11Device*			pDevice = nullptr;				// デバイス
	ID3D11DeviceContext*	pContext = nullptr;				// デバイスコンテキスト
	IDXGISwapChain*			pSwapChain = nullptr;			// スワップチェイン
	ID3D11RenderTargetView* pRenderTargetView = nullptr;	// レンダーターゲットビュー

	ID3D11VertexShader*		pVertexShader = nullptr;		// 頂点シェーダー
	ID3D11PixelShader*		pPixelShader = nullptr;			// ピクセルシェーダー
	ID3D11InputLayout*		pVertexLayout = nullptr;		// 頂点レイアウト
	ID3D11RasterizerState*	pRasterizerState = nullptr;		// ラスタライザーステート
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

	// シェーダー初期化
	InitShader();
}

// シェーダー初期化
void Direct3D::InitShader() {
	// シェーダーのコンパイル
	ID3DBlob* pVSBlob = nullptr;	// 頂点シェーダーバイナリ格納用
	ID3DBlob* pPSBlob = nullptr;	// ピクセルシェーダーバイナリ格納用
	ID3DBlob* pErrorBlob = nullptr; // エラー内容格納用
	// 頂点シェーダーコンパイル
	HRESULT hr = D3DCompileFromFile(
		L"Simple3D.hlsl",		// シェーダーファイル名
		nullptr,				// 定義マクロ
		nullptr,				// インクルード方法
		"VS",					// エントリーポイント関数名
		"vs_5_0",				// シェーダーモデル指定
		NULL,					// コンパイルオプション
		0,						// 追加オプション
		&pVSBlob,				// 成功時にバイナリが入る
		&pErrorBlob				// エラー時にエラー内容が入る
	);
	if (FAILED(hr)) {
		// エラー内容を表示
		if (pErrorBlob) {
			MessageBoxA(nullptr, (char*)pErrorBlob->GetBufferPointer(), "頂点シェーダーエラー", MB_OK);
			pErrorBlob->Release();
		}
	}
	pDevice->CreateVertexShader(
		pVSBlob->GetBufferPointer(),	// 頂点シェーダーバイナリ
		pVSBlob->GetBufferSize(),		// バイナリサイズ
		NULL,							// なんかよくわからん
		&pVertexShader					// 完成した頂点シェーダー
	);

	// 頂点レイアウト作成
	D3D11_INPUT_ELEMENT_DESC layout[]={
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },	// 頂点座標
	};
	pDevice->CreateInputLayout(
		layout,							// 頂点レイアウト構造体
		1,								// 頂点レイアウト数
		pVSBlob->GetBufferPointer(),	// 頂点シェーダーバイナリ
		pVSBlob->GetBufferSize(),		// バイナリサイズ
		&pVertexLayout					// 完成した頂点レイアウト
	);

	pVSBlob->Release(); // 頂点シェーダーバイナリはもう不要なので解放

	// ピクセルシェーダーコンパイル
	hr = D3DCompileFromFile(
		L"Simple3D.hlsl",		// シェーダーファイル名
		nullptr,				// 定義マクロ
		nullptr,				// インクルード方法
		"PS",					// エントリーポイント関数名
		"ps_5_0",				// シェーダーモデル指定
		NULL,					// コンパイルオプション
		0,						// 追加オプション
		&pPSBlob,				// 成功時にバイナリが入る
		&pErrorBlob				// エラー時にエラー
	);
	if (FAILED(hr)) {
		// エラー内容を表示
		if (pErrorBlob) {
			MessageBoxA(nullptr, (char*)pErrorBlob->GetBufferPointer(), "ピクセルシェーダーエラー", MB_OK);
			pErrorBlob->Release();
		}
	}
	pDevice->CreatePixelShader(
		pPSBlob->GetBufferPointer(),	// ピクセルシェーダーバイナリ
		pPSBlob->GetBufferSize(),		// バイナリサイズ
		nullptr,						// なんかよくわからん
		&pPixelShader					// 完成したピクセルシェーダー
	);
	pPSBlob->Release(); // ピクセルシェーダーバイナリはもう不要なので解放

	// ラスタライザーステート作成
	D3D11_RASTERIZER_DESC rdc = {};
	rdc.CullMode = D3D11_CULL_BACK;				// 背面カリング
	rdc.FillMode = D3D11_FILL_SOLID;			// 塗りつぶし
	rdc.FrontCounterClockwise = FALSE;			// 頂点は時計回りが表
	pDevice->CreateRasterizerState(&rdc, &pRasterizerState);

	// それぞれのデバイスコンテキストにセット
	pContext->VSSetShader(pVertexShader, NULL, 0);					// 頂点シェーダーセット
	pContext->PSSetShader(pPixelShader, NULL, 0);					// ピクセルシェーダーセット
	pContext->IASetInputLayout(pVertexLayout);						// 頂点レイアウトセット
	pContext->RSSetState(pRasterizerState);							// ラスタライザーステートセット
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
	pRasterizerState->Release();
	pVertexLayout->Release();
	pPixelShader->Release();
	pVertexShader->Release();

	pRenderTargetView->Release();
	pSwapChain->Release();
	pContext->Release();
	pDevice->Release();
}