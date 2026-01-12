// インクルード
#include <Windows.h>
#include <d3d11.h>
#include "Direct3D.h"
#include "Quad.h"
#include "Camera.h"
#include "Dice.h"
#include "Sprite.h"
#include "Transform.h"
#include "Fbx.h"
#include "Input.h"

// リンカ
#pragma comment(lib,"d3d11.lib")

// マクロ
#define WINDOW_CLASS	"SampleGame"
#define WINDOW_TITLE	"サンプルゲーム"
#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	600

// プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//Quad* pQuad;
//Dice* pDice;
//Sprite* pSprite;
Fbx* pFbx;

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

	// DIrectInputの初期化
	Input::Initialize(hWnd);

	// カメラ初期化
	Camera::Initialize();

	pFbx = new Fbx();
	pFbx->Load("Assets/Oden.fbx");

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
			// カメラ更新
			Camera::Update();
			// ゲーム処理
			Direct3D::BeginDraw();
			// 入力情報の更新
			Input::Update();
			// 描画処理
			static float angle = 0;
			angle += 0.01f;

			Transform fbxTransform;
			fbxTransform.position_.z = 3.0f;
			fbxTransform.rotate_.y = angle;

			pFbx->Draw(fbxTransform);

			Direct3D::EndDraw();

			if (Input::IsKeyUp(DIK_ESCAPE)) {
				static int cnt = 0;
				cnt++;
				if(cnt >= 3) {
					PostQuitMessage(0);
				}
			}
		}
	}

	// 解放処理
	Input::Relese();
	SAFE_DELETE(pFbx);
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
