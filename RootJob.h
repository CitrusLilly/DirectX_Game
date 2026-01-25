#pragma once

#include "GameObject.h"

// 全てのゲームオブジェクトのトップに来るオブジェクト
class RootJob 
	: public GameObject {
public:
	// コンストラクタ
	RootJob();
	// デストラクタ
	~RootJob();

	// 初期化処理
	void Initialize();
	// 更新処理
	void Update();
	// 描画処理
	void Draw();
	// 解放
	void Release();
};
