#pragma once

#include "GameObject.h"
#include "Fbx.h"

class Player
	: public GameObject
{
	CoreEngine::Fbx* pFbx;
public:
	// コンストラクタ
	Player(GameObject* parent);
	// デストラクタ
	~Player();

	// 初期化処理
	void Initialize() override;
	// 更新処理
	void Update() override;
	// 描画処理
	void Draw() override;
	// 解放
	void Release() override;
};