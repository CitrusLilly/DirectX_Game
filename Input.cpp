#include "CoreEngine.h"
#include "Input.h"

using namespace DirectX;

namespace CoreEngine {

	namespace Input {

		LPDIRECTINPUT8 pDInput = nullptr;			// DirectInput本体のインターフェース
		
		// キーボード
		LPDIRECTINPUTDEVICE8 pKeyDevice = nullptr;	// キーボードデバイスオブジェクト
		BYTE keyState[256] = { 0 };					// 現在のキーの状態
		BYTE prevKeyState[256] = { 0 };				// 前フレームのキーの状態

		// マウス
		LPDIRECTINPUTDEVICE8 pMouseDevice = nullptr;// マウスデバイスオブジェクト
		DIMOUSESTATE mouseState_;					// マウスの状態
		DIMOUSESTATE prevMouseState_;				// 前フレームのマウスの状態
		POINT mousePosition_;						// マウスカーソル位置

		// コントローラー
		XINPUT_STATE controllerState_;				// コントローラーの状態
		XINPUT_STATE prevControllerState_;			// 前フレームのコントローラーの状態

		// 初期化
		void Initialize(HWND hWnd) {
			DirectInput8Create(
				GetModuleHandle(nullptr),	// アプリのインスタンス取得
				DIRECTINPUT_VERSION,		// バージョン指定
				IID_IDirectInput8,			// 生成するインターフェースID
				(void**)&pDInput,			// 作成されたDirectInput受け取り先
				nullptr						// 追加オプション
			);

			// キーボード
			// デバイスオブジェクト作成
			pDInput->CreateDevice(GUID_SysKeyboard, &pKeyDevice, nullptr);
			// デバイスの種類を指定
			pKeyDevice->SetDataFormat(&c_dfDIKeyboard);
			// 強調レベル設定(他の実行中アプリに対する優先度)
			pKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

			// マウス
			pDInput->CreateDevice(GUID_SysMouse, &pMouseDevice, nullptr);
			pMouseDevice->SetDataFormat(&c_dfDIMouse);
			pMouseDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
		}

		// 毎フレームの入力更新
		void Update() {
			// キーボード
			// デバイスを操作可能状態にする
			pKeyDevice->Acquire();
			// 現在のキー状態をコピー
			memcpy(prevKeyState, keyState, sizeof(prevKeyState));
			// 現在のキー状態を取得
			pKeyDevice->GetDeviceState(sizeof(keyState), &keyState);

			// マウス
			pMouseDevice->Acquire();
			memcpy(&prevMouseState_, &mouseState_, sizeof(mouseState_));
			pMouseDevice->GetDeviceState(sizeof(mouseState_), &mouseState_);

			// コントローラー
			memcpy(&prevControllerState_,&controllerState_,sizeof(controllerState_));
			XInputGetState(0,&controllerState_);
		}

		// 解放
		void Relese() {
			SAFE_RELEASE(pMouseDevice);
			SAFE_RELEASE(pKeyDevice);
			SAFE_RELEASE(pDInput);
		}

		//////////////////////////// キーボード ////////////////////////////
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

		//////////////////////////// マウス ////////////////////////////
		// マウスボタンが押されているか調べる
		bool IsMouseButton(int buttonCode) {
			if (mouseState_.rgbButtons[buttonCode] & 0x80) {
				return true;
			}
			return false;
		}
		// マウスのボタンを押したか調べる
		bool IsMouseButtonDown(int buttonCode) {
			// 今は押していて、前回は押していない
			if (IsMouseButton(buttonCode) && !(prevMouseState_.rgbButtons[buttonCode] & 0x80)) {
				return true;
			}
			return false;
		}
		// マウスのボタンを離したか調べる
		bool IsMouseButtonUp(int buttonCode) {
			// 今は押していなくて、前回は押している
			if (!IsMouseButton(buttonCode) && prevMouseState_.rgbButtons[buttonCode] & 0x80) {
				return true;
			}
			return false;
		}

		// マウス位置取得
		XMFLOAT3 GetMousePosition() {
			return XMFLOAT3((float)mousePosition_.x, (float)mousePosition_.y, 0);
		}
		// マウス位置をセット
		void SetMousePosition(int x, int y) {
			mousePosition_.x = x;
			mousePosition_.y = y;
		}
		// マウスの移動量を取得
		XMFLOAT3 GetMouseMove() {
			// lZはホイール回転量
			return XMFLOAT3((float)mouseState_.lX, (float)mouseState_.lY, (float)mouseState_.lZ);
		}

		//////////////////////////// コントローラー ////////////////////////////
		// コントローラーのボタンが押されているか調べる
		bool IsPadButton(int buttonCode, int padID) {
			if (controllerState_.Gamepad.wButtons & buttonCode) {
				return true;
			}
			return false;
		}
		// コントローラーのボタンを押したか調べる
		bool IsPadButtonDown(int buttonCode, int padID) {
			// 今は押していて、前回は押していない
			if (IsPadButton(buttonCode,padID) && !(prevControllerState_.Gamepad.wButtons & buttonCode)) {
				return true;
			}
			return false;
		}
		// コントローラーのボタンを離したか調べる
		bool IsPadButtonUp(int buttonCode, int padID) {
			// 今は押していなくて、前回は押している
			if (!IsPadButton(buttonCode,padID) && prevControllerState_.Gamepad.wButtons & buttonCode) {
				return true;
			}
			return false;
		}

		// 入力量を正規化してfloatで返す
		float GetAnalogValue(int raw, int max, int deadZone) {
			// デッドゾーン内なら0
			if (std::abs(raw) < deadZone) {
				return 0;
			}

			// デッドゾーンを差し引いた値
			float adjusted = 0;
			if (raw > 0) {
				adjusted = float(raw - deadZone);
			} else {
				adjusted = float(raw + deadZone);
			}

			// 正規化
			float range = float(max - deadZone);
			return adjusted / range;
		}

		// 左スティックの傾きを取得
		XMFLOAT3 GetPadStickL(int padID) {
			float x = GetAnalogValue(controllerState_.Gamepad.sThumbLX, 32767, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			float y = GetAnalogValue(controllerState_.Gamepad.sThumbLY, 32767, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			return XMFLOAT3(x, y, 0);
		}
		// 右スティックの傾きを取得
		XMFLOAT3 GetPadStickR(int padID) {
			float x = GetAnalogValue(controllerState_.Gamepad.sThumbRX, 32767, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
			float y = GetAnalogValue(controllerState_.Gamepad.sThumbRY, 32767, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
			return XMFLOAT3(x, y, 0);
		}

		// 左トリガーの押し込み具合を取得
		float GetPadTrrigerL(int padID) {
			return GetAnalogValue(controllerState_.Gamepad.bLeftTrigger, 255, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
		}
		// 右トリガーの押し込み具合を取得
		float GetPadTrrigerR(int padID) {
			return GetAnalogValue(controllerState_.Gamepad.bRightTrigger, 255, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
		}

		// 振動させる
		void SetPadVibration(int l, int r, int padID) {
			XINPUT_VIBRATION vibration;
			ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
			vibration.wLeftMotorSpeed = l;	// 左モーターの強さ
			vibration.wRightMotorSpeed = r; // 右モーターの強さ
			XInputSetState(padID, &vibration);
		}
	};
}