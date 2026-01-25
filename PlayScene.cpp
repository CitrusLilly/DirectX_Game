#include "PlayScene.h"
#include "Fbx.h"
#include "Player.h"

// コンストラクタ
// 引数: parent 親オブジェクト(SecenManager)
PlayScene::PlayScene(GameObject* parent)
	:GameObject(parent,"PlayScene")
{
}

// 初期化処理
void PlayScene::Initialize() {
	Instantiate<Player>(this);
}
// 更新処理
void PlayScene::Update() {
	this->transform_.rotate_.y += 0.01f;
}
// 描画処理
void PlayScene::Draw() {

}
// 解放
void PlayScene::Release() {

}