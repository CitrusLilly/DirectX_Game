#pragma once
#include <DirectXMath.h>

using namespace DirectX;

namespace CoreEngine {

	// 位置、向き、拡大率などを管理するクラス
	class Transform {
		XMMATRIX matTranslate_;	// 移動行列
		XMMATRIX matRotate_;	// 回転行列
		XMMATRIX matScale_;		// 拡大行列

	public:
		XMFLOAT3 position_;	// 位置
		XMFLOAT3 rotate_;	// 向き
		XMFLOAT3 scale_;	// 拡大率

		Transform();				// コンストラクタ
		~Transform();				// デストラクタ
		void Calclation();			// 各行列の計算
		XMMATRIX GetWorldMatrix();	// ワールド行列を取得
		XMMATRIX GetNormalMatrix();	// 法線変形用行列を取得
	};
}