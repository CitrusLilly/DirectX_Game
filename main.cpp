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
#define WINDOW_CLASS	"GameEngine"
#define WINDOW_TITLE	"3Dモデル表示"
#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	600

// プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Sprite* pSprite;
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

	pSprite = new Sprite();
	pSprite->Initialize();
	pSprite->SetPivot(Sprite::UpLeft);
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
			// 入力時処理
			static float angleX = 0; // 上下
			static float angleY = 0; // 左右

			if (Input::IsKey(DIK_UP))		angleX += 0.01f;
			if (Input::IsKey(DIK_DOWN))		angleX -= 0.01f;
			if (Input::IsKey(DIK_LEFT))		angleY += 0.01f;
			if (Input::IsKey(DIK_RIGHT))	angleY -= 0.01f;

			// 描画処理
			// 3Dオブジェクト
			Transform transform;
			transform.position_.y = -1.0f;
			transform.rotate_.y = angleY;
			transform.rotate_.x = angleX;

			pFbx->Draw(transform);

			// スプライト
			Transform spriteTransform;
			spriteTransform.scale_.x = 256.0f / 800.0f;
			spriteTransform.scale_.y = 48.0f / 600.0f;
			spriteTransform.position_.x = -1.0f;
			spriteTransform.position_.y = 1.0f;

			pSprite->Draw(spriteTransform);

			Direct3D::EndDraw();
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
	case WM_MOUSEMOVE:		// マウスが移動したら
		Input::SetMousePosition(LOWORD(lParam),HIWORD(lParam));
		return 0;
	case WM_DESTROY:		// ウィンドウの×ボタンが押された場合
		PostQuitMessage(0);	// 終了
		return 0;
	}
	// それ以外は使ってないやつを投げる
	return DefWindowProc(hWnd,msg,wParam,lParam);
}
