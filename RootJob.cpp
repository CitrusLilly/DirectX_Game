#include "RootJob.h"
#include "PlayScene.h"

// コンストラクタ
RootJob::RootJob() {

}
// デストラクタ
RootJob::~RootJob() {

}

// 初期化処理
void RootJob::Initialize() {
	// 最初のシーンを準備
	Instantiate<PlayScene>(this);
}
// 更新処理
void RootJob::Update() {

}
// 描画処理
void RootJob::Draw() {

}
// 解放
void RootJob::Release() {

}