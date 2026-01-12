#pragma once

#include <dinput.h>

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dInput8.lib")

#define SAFE_RELEASE(p) if(p) { p->Release(); p = nullptr; }	// 安全に解放

namespace Input {
	// 初期化
	void Initialize(HWND hWnd);
	// 毎フレームの入力更新
	void Update();
	// 指定キーが押されているか判定
	bool IsKey(int keyCode);
	// 指定キーが押されたかどうか
	bool IsKeyDown(int keyCode);
	// 指定キーが離されたかどうか
	bool IsKeyUp(int keyCode);
	// 解放
	void Relese();
};