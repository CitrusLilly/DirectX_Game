#include "Transform.h"

// コンストラクタ
Transform::Transform():
	matTranslate_(XMMatrixIdentity()),
	matRotate_(XMMatrixIdentity()),
	matScale_(XMMatrixIdentity()),
	position_(XMFLOAT3(0,0,0)),
	rotate_(XMFLOAT3(0, 0, 0)),
	scale_(XMFLOAT3(1, 1, 1))
{
}

// デストラクタ
Transform::~Transform() {
}

// 各行列の計算
void Transform::Calclation() {
	// 位置計算
	matTranslate_ = XMMatrixTranslation(position_.x, position_.y, position_.z);

	// 回転計算
	XMMATRIX rotateX,rotateY, rotateZ;
	rotateX = XMMatrixRotationX(XMConvertToRadians(rotate_.x));
	rotateY = XMMatrixRotationX(XMConvertToRadians(rotate_.y));
	rotateZ = XMMatrixRotationX(XMConvertToRadians(rotate_.z));
	matRotate_ = rotateX * rotateY * rotateZ;

	// 拡大率計算
	matScale_ = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
}

// ワールド行列を取得
XMMATRIX Transform::GetWorldMatrix() {
	return matTranslate_ * matRotate_ * matScale_;
}

// 法線変形用行列を取得
XMMATRIX Transform::GetNormalMatrix() {
	return matRotate_ * XMMatrixInverse(nullptr, matScale_);
}
