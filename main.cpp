// インクルード
#include <Windows.h>
#include <d3d11.h>
#include "Direct3D.h"
#include "Quad.h"

// リンカ
#pragma comment(lib,"d3d11.lib")

// マクロ
#define WINDOW_CLASS	L"SampleGame"
#define WINDOW_TITLE	L"サンプルゲーム"
#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	600

// プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Quad* pQuad;

// エントリーポイント
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nCmdShow) {
	// ウィンドウクラスを作成
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);				// この構造体のサイズ
	wc.hInstance = hInstance;					// インスタンスハンドル
	wc.lpszClassName = WINDOW_CLASS;			// ウィンドウクラス名
	wc.lpfnWndProc = WndProc;					// ウィンドウプロシージャ
	wc.style = CS_VREDRAW | CS_HREDRAW;			// スタイル(デフォルト)
	wc.hIcon = LoadIcon(NULL,IDI_APPLICATION);	// アイコン
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);	// 小さいアイコン
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);	// マウスカーソル
	wc.lpszMenuName = NULL;						// メニューなし
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 背景(白)

	RegisterClassEx(&wc); // クラス登録

	// ウィンドウサイズ計算
	RECT winRect = { 0,0,WINDOW_WIDTH,WINDOW_HEIGHT };
	// 描画領域をサイズに合わせる
	AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
	int winW = winRect.right - winRect.left;
	int winH = winRect.bottom - winRect.top;

	// ウィンドウ作成
	HWND hWnd = CreateWindow(
		WINDOW_CLASS,			// クラス名
		WINDOW_TITLE,			// タイトルバーのテキスト
		WS_OVERLAPPEDWINDOW,	// スタイル(普通のウィンドウ)
		CW_USEDEFAULT,			// 表示位置左(おまかせ)
		CW_USEDEFAULT,			// 表示位置上(おまかせ)
		winW,					// 幅
		winH,					// 高さ
		NULL,					// 親ウィンドウなし
		NULL,					// メニューなし
		hInstance,				// インスタンス
		NULL					// パラメータ
		);
	
	// ウィンドウ表示
	ShowWindow(hWnd, nCmdShow);
	
	HRESULT hr;
	// Direct3D初期化
	hr = Direct3D::Initialize(winW, winH, hWnd);
	if(FAILED(hr)) {
		PostQuitMessage(0);	// エラーが起きたら強制終了
	}

	pQuad = new Quad();
	if (FAILED(pQuad->Initialize())) {
		return 0;
	}

	// メッセージループ
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT) {

		// メッセージあり
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		// メッセージなし
		} else {
			// ゲーム処理
			Direct3D::BeginDraw();
			// 描画処理
			pQuad->Draw();
			Direct3D::EndDraw();
		}
	}

	// 解放処理
	SAFE_DELETE(pQuad);
	Direct3D::Release();

	return 0;
}

// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// msgはメッセージが入っている
	switch (msg) {
	case WM_DESTROY:		// ウィンドウの×ボタンが押された場合
		PostQuitMessage(0);	// 終了
		return 0;
	}
	// それ以外は使ってないやつを投げる
	return DefWindowProc(hWnd,msg,wParam,lParam);
}
