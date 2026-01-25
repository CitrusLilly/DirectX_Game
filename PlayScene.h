#pragma once

#include "GameObject.h"

// 前方宣言
class Fbx;

class PlayScene : public GameObject {
public:
	// コンストラクタ
	// 引数: parent 親オブジェクト(SecenManager)
	PlayScene(GameObject* parent);

	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update() override;
	// 描画処理
	void Draw() override;
	// 解放
	void Release() override;
};
