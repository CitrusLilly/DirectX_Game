#pragma once
#include "Direct3D.h"
#include <DirectXMath.h>

using namespace DirectX;

namespace CoreEngine {

	// カメラ
	namespace Camera {
		// 初期化 (プロジェクション行列作成)
		void Initialize();

		// 更新 (ビュー行列作成)
		void Update();

		// 視点(カメラ位置)を設定
		void SetPosition(XMVECTOR position);
		void SetPosition(XMFLOAT3 position);

		// 焦点(見る位置)を設定
		void SetTarget(XMVECTOR target);
		void SetTarget(XMFLOAT3 target);

		// ビュー行列取得
		XMMATRIX GetViewMatrix();

		// プロジェクション行列取得
		XMMATRIX GetProjectionMatrix();
	};
}