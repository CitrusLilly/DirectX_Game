#pragma once

#include <list>
#include <string>
#include "Transform.h"

using std::string;
using std::list;

class GameObject {
protected:
	// 子オブジェクトのリスト
	list<GameObject*>		childList_;
	// 位置、向き、スケールを管理
	CoreEngine::Transform	transform_;
	// 親オブジェクトへの参照(ルートの場合はnullptr)
	GameObject*				pParent_;
	// オブジェクトの名前
	string					objectName;

public:
	// コンストラクタ
	GameObject();
	GameObject(GameObject* parent, const std::string& name);
	
	// デストラクタ
	~GameObject();

	// 仮想定義
	// 初期化処理
	virtual void Initialize() = 0;
	// 更新処理
	virtual void Update() = 0;
	// 描画処理
	virtual void Draw() = 0;
	// 解放
	virtual void Release() = 0;
	
	// 自分の処理後に全ての子の処理を行う
	void UpdateSub();
	void DrawSub();
	void ReleaseSub();

	// シーン準備用テンプレート
	template <class T>
	void Instantiate(GameObject* parent) {
		// シーンを準備
		T* pObject;
		// シーンの親にルートを登録
		pObject = new T(parent);
		pObject->Initialize();
		// シーンを子として追加
		parent->childList_.push_back(pObject);
	}
};
