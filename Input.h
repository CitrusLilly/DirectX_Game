#pragma once

#include <dinput.h>
#include <DirectXMath.h>
#include <Xinput.h>

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dInput8.lib")
#pragma comment(lib,"Xinput.lib")

namespace CoreEngine {

	namespace Input {
		// 初期化
		void Initialize(HWND hWnd);
		// 毎フレームの入力更新
		void Update();
		// 解放
		void Relese();

		//////////////////////////// キーボード ////////////////////////////
		// 指定キーが押されているか判定
		bool IsKey(int keyCode);
		// 指定キーが押されたかどうか
		bool IsKeyDown(int keyCode);
		// 指定キーが離されたかどうか
		bool IsKeyUp(int keyCode);

		//////////////////////////// マウス ////////////////////////////
		// マウスボタンが押されているか調べる
		bool IsMouseButton(int buttonCode);
		// マウスのボタンを押したか調べる
		bool IsMouseButtonDown(int buttonCode);
		// マウスのボタンを離したか調べる
		bool IsMouseButtonUp(int buttonCode);

		// マウス位置取得
		DirectX::XMFLOAT3 GetMousePosition();
		// マウス位置をセット
		void SetMousePosition(int x, int y);
		// マウスの移動量を取得
		DirectX::XMFLOAT3 GetMouseMove();

		//////////////////////////// コントローラー ////////////////////////////
		// コントローラーのボタンが押されているか調べる
		bool IsPadButton(int buttonCode, int padID = 0);
		// コントローラーのボタンを押したか調べる
		bool IsPadButtonDown(int buttonCode, int padID = 0);
		// コントローラーのボタンを離したか調べる
		bool IsPadButtonUp(int buttonCode, int padID = 0);
		// 左スティックの傾きを取得
		DirectX::XMFLOAT3 GetPadStickL(int padID = 0);
		// 右スティックの傾きを取得
		DirectX::XMFLOAT3 GetPadStickR(int padID = 0);
		// 左トリガーの押し込み具合を取得
		float GetPadTrrigerL(int padID = 0);
		// 右トリガーの押し込み具合を取得
		float GetPadTrrigerR(int padID = 0);
		// 振動させる
		void SetPadVibration(int l, int r, int padID = 0);
	};

}