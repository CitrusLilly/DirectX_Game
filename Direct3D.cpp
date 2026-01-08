#include <d3dcompiler.h>
#include "Direct3D.h"
#include <DirectXMath.h>

// 変数
namespace Direct3D {
	ID3D11Device*			pDevice_ = nullptr;				// デバイス
	ID3D11DeviceContext*	pContext_ = nullptr;				// デバイスコンテキスト
	IDXGISwapChain*			pSwapChain_ = nullptr;			// スワップチェイン
	ID3D11RenderTargetView* pRenderTargetView_ = nullptr;	// レンダーターゲットビュー

	ID3D11VertexShader*		pVertexShader_ = nullptr;		// 頂点シェーダー
	ID3D11PixelShader*		pPixelShader_ = nullptr;			// ピクセルシェーダー
	ID3D11InputLayout*		pVertexLayout_ = nullptr;		// 頂点レイアウト
	ID3D11RasterizerState*	pRasterizerState_ = nullptr;		// ラスタライザーステート
}

// 初期化
HRESULT Direct3D::Initialize(int winW, int winH, HWND hWnd) {
	// エラー処理用
	HRESULT hr;
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
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,					// ビデオアダプタの指定。nullでよし
		D3D_DRIVER_TYPE_HARDWARE,	// ドライバタイプ
		nullptr,					// 上記がSOFTWAREじゃなければnullptr
		0,							// 何らかのフラグ
		nullptr,					// デバイス、コンテキストのレベル設定。nullでよし
		0,							// 上記でレベルを何個指定したか
		D3D11_SDK_VERSION,			// SDKバージョン。必ずこれ
		&scDesc,					// 上で設定したスワップチェイン構造体
		&pSwapChain_,				// 完成したスワップチェインのアドレスが返ってくる
		&pDevice_,					// 完成したデバイスアドレスが返ってくる
		&level,						// 完成したデバイス、コンテキストのレベルが返ってくる
		&pContext_					// 完成したコンテキストのアドレスが返ってくる
	);
	if (FAILED(hr)) {
		MessageBox(nullptr, L"デバイス、コンテキスト、スワップチェインの作成に失敗しました。", L"エラー", MB_OK);
		return E_FAIL;
	}

	// スワップチェインからバックバッファを取得
	ID3D11Texture2D* pBackBuffer;
	hr = pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if(FAILED(hr)) {
		MessageBox(nullptr, L"バックバッファの取得に失敗しました。", L"エラー", MB_OK);
		return E_FAIL;
	}

	// レンダーターゲットビューの作成
	hr = pDevice_->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView_);
	if(FAILED(hr)) {
		MessageBox(nullptr, L"レンダーターゲットビューの作成に失敗しました。", L"エラー", MB_OK);
		return E_FAIL;
	}

	pBackBuffer->Release();			// 一時的にバックバッファを取得しただけなので解放

	// ビューポート設定(描画範囲)
	D3D11_VIEWPORT vp;
	vp.Width = (float)winW;		// 幅
	vp.Height = (float)winH;	// 高さ
	vp.MinDepth = 0.0f;			// 手前
	vp.MaxDepth = 1.0f;			// 奥
	vp.TopLeftX = 0;			// 左
	vp.TopLeftY = 0;			// 上

	// データを画面に描画するための一通りの設定(パイプライン)
	pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// データの入力種類
	pContext_->RSSetViewports(1, &vp);											// 描画範囲指定(vp)
	pContext_->OMSetRenderTargets(1, &pRenderTargetView_, nullptr);				// 描画先を設定

	// シェーダー初期化
	hr = InitShader();
	if(FAILED(hr)) {
		MessageBox(nullptr, L"シェーダーの初期化に失敗しました。", L"エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

// シェーダー初期化
HRESULT Direct3D::InitShader() {
	using namespace DirectX;

	// エラー処理用
	HRESULT hr;
	// シェーダーのコンパイル
	ID3DBlob* pVSBlob = nullptr;	// 頂点シェーダーバイナリ格納用
	ID3DBlob* pPSBlob = nullptr;	// ピクセルシェーダーバイナリ格納用
	// 頂点シェーダーコンパイル
	D3DCompileFromFile(
		L"Simple3D.hlsl",		// シェーダーファイル名
		nullptr,				// 定義マクロ
		nullptr,				// インクルード方法
		"VS",					// エントリーポイント関数名
		"vs_5_0",				// シェーダーモデル指定
		NULL,					// コンパイルオプション
		0,						// 追加オプション
		&pVSBlob,				// 成功時にバイナリが入る
		nullptr					// エラー時にエラー内容が入る
	);
	assert(pVSBlob != nullptr); // コンパイル失敗してたら止める
	hr = pDevice_->CreateVertexShader(
		pVSBlob->GetBufferPointer(),	// 頂点シェーダーバイナリ
		pVSBlob->GetBufferSize(),		// バイナリサイズ
		NULL,							// なんかよくわからん
		&pVertexShader_					// 完成した頂点シェーダー
	);
	if(FAILED(hr)) {
		MessageBox(nullptr, L"頂点シェーダーの作成に失敗しました。", L"エラー", MB_OK);
		return E_FAIL;
	}

	// 頂点レイアウト作成
	D3D11_INPUT_ELEMENT_DESC layout[]={
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },	// 頂点座標
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,sizeof(XMVECTOR),D3D11_INPUT_PER_VERTEX_DATA,0}, // UV座標
		{ "NORMAL",0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,sizeof(XMVECTOR) * 2,D3D11_INPUT_PER_VERTEX_DATA,0}, // 法線
	};
	hr = pDevice_->CreateInputLayout(
		layout,							// 頂点レイアウト構造体
		3,								// 頂点レイアウト数
		pVSBlob->GetBufferPointer(),	// 頂点シェーダーバイナリ
		pVSBlob->GetBufferSize(),		// バイナリサイズ
		&pVertexLayout_					// 完成した頂点レイアウト
	);
	if(FAILED(hr)) {
		MessageBox(nullptr, L"頂点レイアウトの作成に失敗しました。", L"エラー", MB_OK);
		SAFE_RELEASE(pVSBlob);
		return E_FAIL;
	}

	SAFE_RELEASE(pVSBlob); // 頂点シェーダーバイナリはもう不要なので解放

	// ピクセルシェーダーコンパイル
	D3DCompileFromFile(
		L"Simple3D.hlsl",		// シェーダーファイル名
		nullptr,				// 定義マクロ
		nullptr,				// インクルード方法
		"PS",					// エントリーポイント関数名
		"ps_5_0",				// シェーダーモデル指定
		NULL,					// コンパイルオプション
		0,						// 追加オプション
		&pPSBlob,				// 成功時にバイナリが入る
		nullptr					// エラー時にエラー内容が入る
	);
	assert(pPSBlob != nullptr); // コンパイル失敗してたら止める
	hr = pDevice_->CreatePixelShader(
		pPSBlob->GetBufferPointer(),	// ピクセルシェーダーバイナリ
		pPSBlob->GetBufferSize(),		// バイナリサイズ
		nullptr,						// なんかよくわからん
		&pPixelShader_					// 完成したピクセルシェーダー
	);
	if(FAILED(hr)) {
		MessageBox(nullptr, L"ピクセルシェーダーの作成に失敗しました。", L"エラー", MB_OK);
		SAFE_RELEASE(pPSBlob);
		return E_FAIL;
	}

	SAFE_RELEASE(pPSBlob); // ピクセルシェーダーバイナリはもう不要なので解放

	// ラスタライザーステート作成
	D3D11_RASTERIZER_DESC rdc = {};
	// 描画する面の設定
	rdc.CullMode = D3D11_CULL_NONE;				// 背面カリング
	rdc.FillMode = D3D11_FILL_SOLID;			// 塗りつぶし
	rdc.FrontCounterClockwise = FALSE;			// 頂点は時計回りが表
	hr = pDevice_->CreateRasterizerState(&rdc, &pRasterizerState_);
	if(FAILED(hr)) {
		MessageBox(nullptr, L"ラスタライザーステートの作成に失敗しました。", L"エラー", MB_OK);
		return E_FAIL;
	}

	// それぞれのデバイスコンテキストにセット
	pContext_->VSSetShader(pVertexShader_, NULL, 0);					// 頂点シェーダーセット
	pContext_->PSSetShader(pPixelShader_, NULL, 0);					// ピクセルシェーダーセット
	pContext_->IASetInputLayout(pVertexLayout_);						// 頂点レイアウトセット
	pContext_->RSSetState(pRasterizerState_);							// ラスタライザーステートセット

	return S_OK;
}

// 描画開始
void Direct3D::BeginDraw() {
	// 背景色
	float clearColor[4] = { 0.0f,0.0f,1.0f,1.0f };	// R,G,B,A

	// 画面をクリア
	pContext_->ClearRenderTargetView(pRenderTargetView_, clearColor);
}

// 描画終了
void Direct3D::EndDraw() {
	// スワップ(バックバッファを表に表示)
	pSwapChain_->Present(0, 0);
}

// 解放
void Direct3D::Release() {
	SAFE_RELEASE(pRasterizerState_);
	SAFE_RELEASE(pVertexLayout_);
	SAFE_RELEASE(pPixelShader_);
	SAFE_RELEASE(pVertexShader_);

	SAFE_RELEASE(pRenderTargetView_);
	SAFE_RELEASE(pSwapChain_);
	SAFE_RELEASE(pContext_);
	SAFE_RELEASE(pDevice_);
}