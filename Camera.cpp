#include "CoreEngine.h"
#include "Camera.h"

namespace CoreEngine {

	namespace Camera {
		// 変数
		XMVECTOR position_;			// カメラの位置(視点)
		XMVECTOR target_;			// 見る位置(焦点)
		XMMATRIX viewMatrix_;		// ビュー行列
		XMMATRIX projMatrix_;		// プロジェクション行列


		// 初期化
		void Camera::Initialize() {
			position_ = XMVectorSet(0.0f, 3.0f, -10.0f, 0.0f);	// カメラの位置
			target_ = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);		// カメラの焦点

			// プロジェクション行列作成
			projMatrix_ = XMMatrixPerspectiveFovLH(
				XM_PIDIV4,			// 視野角(縦方向)
				800.0f / 600.0f,	// アスペクト比
				0.1f,				// 前端距離 (ゼロに近いほど近くまで描画)ニア
				100.0f				// 後端距離 (遠くまで描画)ファー
			);
		}

		// 更新
		void Camera::Update() {
			// ビュー行列作成
			viewMatrix_ = XMMatrixLookAtLH(
				position_,				// カメラの位置
				target_,				// カメラの焦点
				XMVectorSet(0, 1, 0, 0) // 上方向ベクトル
			);
		}

		// 位置を設定
		void Camera::SetPosition(XMVECTOR position) {
			position_ = position;
		}

		void Camera::SetPosition(XMFLOAT3 position) {
			SetPosition(XMLoadFloat3(&position));
		}

		// 焦点を設定
		void Camera::SetTarget(XMVECTOR target) {
			target_ = target;
		}
		void Camera::SetTarget(XMFLOAT3 target) {
			SetTarget(XMLoadFloat3(&target));
		}

		// ビュー行列取得
		XMMATRIX Camera::GetViewMatrix() {
			return viewMatrix_;
		}

		// プロジェクション行列取得
		XMMATRIX Camera::GetProjectionMatrix() {
			return projMatrix_;
		}
	}
}