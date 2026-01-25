#include "GameObject.h"

// コンストラクタ
GameObject::GameObject()
	:pParent_(nullptr)
{
}
GameObject::GameObject(GameObject* parent, const std::string& name)
	:pParent_(nullptr)
{
}

// デストラクタ
GameObject::~GameObject() {

}

// 自分の更新後、全ての子の更新を行う
void GameObject::UpdateSub() {
	// 自分の更新処理
	Update();
	// 全ての子の更新処理
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++) {
		(*itr)->UpdateSub();
	}
}

// 自分の描画後、全ての子の描画を行う
void GameObject::DrawSub() {
	// 自分の描画処理
	Draw();
	// 全ての子の描画処理
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++) {
		(*itr)->DrawSub();
	}
}

// 自分の解放後、全ての子の解放を行う
void GameObject::ReleaseSub() {
	// 自分の解放
	Release();
	// 全ての子の解放
	for (auto itr = childList_.begin(); itr != childList_.end(); itr++) {
		(*itr)->ReleaseSub();
	}
}