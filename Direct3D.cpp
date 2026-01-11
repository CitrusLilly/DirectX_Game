#include <d3dcompiler.h>
#include "Direct3D.h"
#include <DirectXMath.h>
#include <vector>

// 変数
namespace Direct3D {
	ID3D11Device*			pDevice_ = nullptr;				// デバイス
	ID3D11DeviceContext*	pContext_ = nullptr;			// デバイスコンテキスト
	IDXGISwapChain*			pSwapChain_ = nullptr;			// スワップチェイン
	ID3D11RenderTargetView* pRenderTargetView_ = nullptr;	// レンダーターゲットビュー
	ID3D11Texture2D*		pDepthStencil = nullptr;		// 深度ステンシル
	ID3D11DepthStencilView* pDepthStencilView = nullptr;	// 深度ステンシルビュー

	struct SHADER_BUNDLE {
		ID3D11VertexShader* pVertexShader_ = nullptr;		// 頂点シェーダー
		ID3D11PixelShader* pPixelShader_ = nullptr;		// ピクセルシェーダー
		ID3D11InputLayout* pVertexLayout_ = nullptr;		// 頂点レイアウト
		ID3D11RasterizerState* pRasterizerState_ = nullptr;	// ラスタライザーステート
	};
	SHADER_BUNDLE shaderBundle[SHADER_MAX];
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
		MessageBox(nullptr, "デバイス、コンテキスト、スワップチェインの作成に失敗しました。", "エラー", MB_OK);
		return E_FAIL;
	}

	// スワップチェインからバックバッファを取得
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if(FAILED(hr)) {
		MessageBox(nullptr, "バックバッファの取得に失敗しました。", "エラー", MB_OK);
		return E_FAIL;
	}

	// レンダーターゲットビューの作成
	hr = pDevice_->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView_);
	if(FAILED(hr)) {
		MessageBox(nullptr, "レンダーターゲットビューの作成に失敗しました。", "エラー", MB_OK);
		return E_FAIL;
	}

	pBackBuffer->Release();			// 一時的にバックバッファを取得しただけなので解放

	// ビューポート設定(描画範囲)
	D3D11_VIEWPORT vp = {};
	vp.Width = (float)winW;		// 幅
	vp.Height = (float)winH;	// 高さ
	vp.MinDepth = 0.0f;			// 手前
	vp.MaxDepth = 1.0f;			// 奥
	vp.TopLeftX = 0;			// 左
	vp.TopLeftY = 0;			// 上

	// 深度ステンシルビューの作成
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = winW;							// 幅
	descDepth.Height = winH;						// 高さ
	descDepth.MipLevels = 1;						// 
	descDepth.ArraySize = 1;						// １枚だけのテクスチャ
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;		// 深度値0～1
	descDepth.SampleDesc.Count = 1;					// MSAA(アンチエイリアス)なし
	descDepth.SampleDesc.Quality = 0;				// MSAAなしなので0
	descDepth.Usage = D3D11_USAGE_DEFAULT;			// GPUが通常使用する深度バッファ
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// 深度ステンシルとしてバインドする
	descDepth.CPUAccessFlags = 0;					// CPUからアクセスしない
	descDepth.MiscFlags = 0;						// 用途なし

	hr = pDevice_->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);
	if (FAILED(hr)) {
		MessageBox(nullptr, "深度ステンシルの作成に失敗しました。", "エラー", MB_OK);
		return E_FAIL;
	}
	hr = pDevice_->CreateDepthStencilView(pDepthStencil,nullptr,&pDepthStencilView);
	if (FAILED(hr)) {
		MessageBox(nullptr, "深度ステンシルビューの作成に失敗しました。", "エラー", MB_OK);
		return E_FAIL;
	}

	// データを画面に描画するための一通りの設定(パイプライン)
	pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// データの入力種類
	pContext_->RSSetViewports(1, &vp);											// 描画範囲指定(vp)
	pContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView);	// 描画先を設定

	// シェーダー初期化
	hr = InitShader();
	if(FAILED(hr)) {
		MessageBox(nullptr, "シェーダーの初期化に失敗しました。", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

// シェーダー初期化
HRESULT Direct3D::InitShader() {
	// 3Dシェーダー初期化
	if (FAILED(InitShader3D())) {
		return E_FAIL;
	}

	// 2Dシェーダー初期化
	if(FAILED(InitShader2D())) {
		return E_FAIL;
	}

	return S_OK;
}

// 3Dシェーダー初期化
HRESULT Direct3D::InitShader3D() {
	using namespace Direct3D;

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
		pVSBlob->GetBufferPointer(),				// 頂点シェーダーバイナリ
		pVSBlob->GetBufferSize(),					// バイナリサイズ
		nullptr,									// クラスリンク
		&(shaderBundle[SHADER_3D].pVertexShader_)	// 完成した頂点シェーダー
	);
	if(FAILED(hr)) {
		MessageBox(nullptr, "頂点シェーダーの作成に失敗しました。", "エラー", MB_OK);
		return E_FAIL;
	}

	// 頂点レイアウト作成
	D3D11_INPUT_ELEMENT_DESC layout[]={
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },	// 頂点座標
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,sizeof(XMVECTOR),D3D11_INPUT_PER_VERTEX_DATA,0}, // UV座標
		{ "NORMAL",0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,sizeof(XMVECTOR) * 2,D3D11_INPUT_PER_VERTEX_DATA,0}, // 法線
	};
	hr = pDevice_->CreateInputLayout(
		layout,										// 頂点レイアウト構造体
		3,											// 頂点レイアウト数
		pVSBlob->GetBufferPointer(),				// 頂点シェーダーバイナリ
		pVSBlob->GetBufferSize(),					// バイナリサイズ
		&(shaderBundle[SHADER_3D].pVertexLayout_)	// 完成した頂点レイアウト
	);
	if(FAILED(hr)) {
		MessageBox(nullptr, "頂点レイアウトの作成に失敗しました。", "エラー", MB_OK);
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
		pPSBlob->GetBufferPointer(),				// ピクセルシェーダーバイナリ
		pPSBlob->GetBufferSize(),					// バイナリサイズ
		nullptr,									// クラスリンク
		&(shaderBundle[SHADER_3D].pPixelShader_)	// 完成したピクセルシェーダー
	);
	if(FAILED(hr)) {
		MessageBox(nullptr, "ピクセルシェーダーの作成に失敗しました。", "エラー", MB_OK);
		SAFE_RELEASE(pPSBlob);
		return E_FAIL;
	}

	SAFE_RELEASE(pPSBlob); // ピクセルシェーダーバイナリはもう不要なので解放

	// ラスタライザーステート作成
	D3D11_RASTERIZER_DESC rdc = {};
	// 描画する面の設定
	rdc.CullMode = D3D11_CULL_BACK;				// 背面カリング
	rdc.FillMode = D3D11_FILL_SOLID;			// 塗りつぶし
	rdc.FrontCounterClockwise = FALSE;			// 頂点は時計回りが表
	hr = pDevice_->CreateRasterizerState(&rdc, &(shaderBundle[SHADER_3D].pRasterizerState_));
	if(FAILED(hr)) {
		MessageBox(nullptr, "ラスタライザーステートの作成に失敗しました。", "エラー", MB_OK);
		return E_FAIL;
	}


	return S_OK;
}

// 2Dシェーダー初期化
HRESULT Direct3D::InitShader2D() {
	HRESULT hr;
	// 頂点シェーダー読み込み
	ID3DBlob* pVSBlob = nullptr;
	D3DCompileFromFile(
		L"Simple2D.hlsl",
		nullptr,
		nullptr,
		"VS",
		"vs_5_0",
		NULL,
		0,
		&pVSBlob,
		NULL
	);
	assert(pVSBlob != nullptr);
	hr = pDevice_->CreateVertexShader(
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		NULL,
		&(shaderBundle[SHADER_2D].pVertexShader_)
	);
	if (FAILED(hr)) {
		MessageBox(nullptr, "頂点シェーダーの作成に失敗しました。", "エラー", MB_OK);
		SAFE_RELEASE(pVSBlob);
		return E_FAIL;
	}

	// 頂点レイアウト作成
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },	// 頂点座標
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,sizeof(XMVECTOR),D3D11_INPUT_PER_VERTEX_DATA,0}, // UV座標
	};
	hr = pDevice_->CreateInputLayout(
		layout.data(),
		layout.size(),
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&(shaderBundle[SHADER_2D].pVertexLayout_)
	);
	if (FAILED(hr)) {
		MessageBox(nullptr, "頂点レイアウトの作成に失敗しました。", "エラー", MB_OK);
		SAFE_RELEASE(pVSBlob);
		return E_FAIL;
	}

	SAFE_RELEASE(pVSBlob); // 頂点シェーダーバイナリはもう不要なので解放

	// ピクセルシェーダー読み込み
	ID3DBlob* pPSBlob = nullptr;
	D3DCompileFromFile(
		L"Simple2D.hlsl",
		nullptr,
		nullptr,
		"PS",
		"ps_5_0",
		NULL,
		0,
		&pPSBlob,
		NULL
	);
	assert(pPSBlob != nullptr);
	hr = pDevice_->CreatePixelShader(
		pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(),
		nullptr,
		&(shaderBundle[SHADER_2D].pPixelShader_)
	);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ピクセルシェーダーの作成に失敗しました。", "エラー", MB_OK);
		SAFE_RELEASE(pPSBlob);
		return E_FAIL;
	}

	SAFE_RELEASE(pPSBlob); // ピクセルシェーダーバイナリはもう不要なので解放

	// ラスタライザーステート作成
	D3D11_RASTERIZER_DESC rdc = {};
	// 描画する面の設定
	rdc.CullMode = D3D11_CULL_BACK;				// 背面カリング
	rdc.FillMode = D3D11_FILL_SOLID;			// 塗りつぶし
	rdc.FrontCounterClockwise = FALSE;			// 頂点は時計回りが表
	hr = pDevice_->CreateRasterizerState(&rdc, &(shaderBundle[SHADER_2D].pRasterizerState_));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ラスタライザーステートの作成に失敗しました。", "エラー", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

void Direct3D::SetShader(SHADER_TYPE type) {
	// それぞれのデバイスコンテキストにセット
	pContext_->VSSetShader(shaderBundle[type].pVertexShader_, NULL, 0);	// 頂点シェーダーセット
	pContext_->PSSetShader(shaderBundle[type].pPixelShader_, NULL, 0);	// ピクセルシェーダーセット
	pContext_->IASetInputLayout(shaderBundle[type].pVertexLayout_);		// 頂点レイアウトセット
	pContext_->RSSetState(shaderBundle[type].pRasterizerState_);		// ラスタライザーステートセット
}

// 描画開始
void Direct3D::BeginDraw() {
	// 背景色
	float clearColor[4] = { 0.0f,0.0f,1.0f,1.0f };	// R,G,B,A

	// 画面をクリア
	pContext_->ClearRenderTargetView(pRenderTargetView_, clearColor);

	// 深度バッファクリア
	pContext_->ClearDepthStencilView(pDepthStencilView,D3D11_CLEAR_DEPTH,1.0f,0);
}

// 描画終了
void Direct3D::EndDraw() {
	// スワップ(バックバッファを表に表示)
	pSwapChain_->Present(0, 0);
}

// 解放
void Direct3D::Release() {
	for (int i = 0; i < SHADER_MAX; i++) {
		SAFE_RELEASE(shaderBundle[i].pRasterizerState_);
		SAFE_RELEASE(shaderBundle[i].pVertexLayout_);
		SAFE_RELEASE(shaderBundle[i].pPixelShader_);
		SAFE_RELEASE(shaderBundle[i].pVertexShader_);
	}

	SAFE_RELEASE(pDepthStencilView);
	SAFE_RELEASE(pDepthStencil);
	SAFE_RELEASE(pRenderTargetView_);
	SAFE_RELEASE(pSwapChain_);
	SAFE_RELEASE(pContext_);
	SAFE_RELEASE(pDevice_);
}