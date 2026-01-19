#include "CoreEngine.h"
#include "Input.h"

using namespace DirectX;

namespace CoreEngine {

	namespace Input {

		LPDIRECTINPUT8 pDInput = nullptr;			// DirectInput本体のインターフェース
		LPDIRECTINPUTDEVICE8 pKeyDevice = nullptr;	// キーボードデバイスオブジェクト
		BYTE keyState[256] = { 0 };					// 現在のキーの状態
		BYTE prevKeyState[256] = { 0 };				// 前フレームでのキーの状態

		XMVECTOR mousePosition;						// マウスカーソル位置

		// 初期化
		void Initialize(HWND hWnd) {
			DirectInput8Create(
				GetModuleHandle(nullptr),	// アプリのインスタンス取得
				DIRECTINPUT_VERSION,		// バージョン指定
				IID_IDirectInput8,			// 生成するインターフェースID
				(void**)&pDInput,			// 作成されたDirectInput受け取り先
				nullptr						// 追加オプション
			);
			// デバイスオブジェクト作成
			pDInput->CreateDevice(GUID_SysKeyboard, &pKeyDevice, nullptr);
			// デバイスの種類を指定
			pKeyDevice->SetDataFormat(&c_dfDIKeyboard);
			// 強調レベル設定(他の実行中アプリに対する優先度)
			pKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
		}

		// 毎フレームの入力更新
		void Update() {
			// 現在のキー状態をコピー
			memcpy(prevKeyState, keyState, sizeof(prevKeyState));
			// デバイスを操作可能状態にする
			pKeyDevice->Acquire();
			// 現在のキー状態を取得
			pKeyDevice->GetDeviceState(sizeof(keyState), &keyState);
		}

		// 指定キーが押されているか判定
		bool IsKey(int keyCode) {
			if (keyState[keyCode] & 0x80) {
				return true;
			}

			return false;
		}

		// 指定キーが押されたかどうか
		bool IsKeyDown(int keyCode) {
			// 今は押していて、前回は押していない
			if (IsKey(keyCode) && !(prevKeyState[keyCode] & 0x80)) {
				return true;
			}

			return false;
		}

		// 指定キーが押されたかどうか
		bool IsKeyUp(int keyCode) {
			// 今は押していなくて、前回は押している
			if (!IsKey(keyCode) && prevKeyState[keyCode] & 0x80) {
				return true;
			}

			return false;
		}

		// 解放
		void Relese() {
			SAFE_RELEASE(pKeyDevice);
			SAFE_RELEASE(pDInput);
		}

		// マウス位置取得
		XMVECTOR GetMousePosition() {
			return mousePosition;
		}

		// マウス位置をセット
		void SetMousePosition(int x, int y) {
			mousePosition = XMVectorSet((float)x, (float)y, 0, 0);
		}
	};
}