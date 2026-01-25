#include "Player.h"
#include "Fbx.h"
#include "CoreEngine.h"

using namespace CoreEngine;

// コンストラクタ
Player::Player(GameObject* parent)
	:GameObject(parent,"Player")
	,pFbx(nullptr)
{

}
// デストラクタ
Player::~Player() {

}

// 初期化処理
void Player::Initialize() {
	pFbx = new Fbx();
	pFbx->Load("Assets/Oden.fbx");
	this->transform_.scale_.x = 0.5f;
	this->transform_.scale_.y = 0.5f;
	this->transform_.scale_.z = 0.5f;
}
// 更新処理
void Player::Update() {

}
// 描画処理
void Player::Draw() {
	pFbx->Draw(transform_);
}
// 解放
void Player::Release() {
	pFbx->Release();
	SAFE_RELEASE(pFbx);
}
