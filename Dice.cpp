#include "CoreEngine.h"
#include "Dice.h"

// 頂点情報の初期化
void Dice::InitVertexData() {
	// 頂点データ作成
	vertices_ = {
		// 前(1)
		{XMVectorSet(-1.0f,1.0f,-1.0f,0.0f), XMVectorSet(0.0f,0.0f,0.0f,0.0f), XMVectorSet(0.0f,0.0f,-1.0f,0.0f)}, // 0
		{XMVectorSet(1.0f,1.0f,-1.0f,0.0f),  XMVectorSet(0.25f,0.0f,0.0f,0.0f), XMVectorSet(0.0f,0.0f,-1.0f,0.0f)}, // 1
		{XMVectorSet(1.0f,-1.0f,-1.0f,0.0f), XMVectorSet(0.25f,0.5f,0.0f,0.0f), XMVectorSet(0.0f,0.0f,-1.0f,0.0f)}, // 2
		{XMVectorSet(-1.0f,-1.0f,-1.0f,0.0f),XMVectorSet(0.0f,0.5f,0.0f,0.0f), XMVectorSet(0.0f,0.0f,-1.0f,0.0f)}, // 3
		// 右(2)
		{XMVectorSet(1.0f,1.0f,-1.0f,0.0f),  XMVectorSet(0.25f,0.0f,0.0f,0.0f), XMVectorSet(1.0f,0.0f,0.0f,0.0f)}, // 4
		{XMVectorSet(1.0f,1.0f,1.0f,0.0f),   XMVectorSet(0.5f,0.0f,0.0f,0.0f),  XMVectorSet(1.0f,0.0f,0.0f,0.0f)}, // 5
		{XMVectorSet(1.0f,-1.0f,1.0f,0.0f),  XMVectorSet(0.5f,0.5f,0.0f,0.0f),  XMVectorSet(1.0f,0.0f,0.0f,0.0f)}, // 6
		{XMVectorSet(1.0f,-1.0f,-1.0f,0.0f), XMVectorSet(0.25f,0.5f,0.0f,0.0f), XMVectorSet(1.0f,0.0f,0.0f,0.0f)}, // 7
		// 左(3)
		{XMVectorSet(-1.0f,1.0f,1.0f,0.0f),  XMVectorSet(0.5f,0.0f,0.0f,0.0f), XMVectorSet(-1.0f,0.0f,0.0f,0.0f)}, // 8
		{XMVectorSet(-1.0f,1.0f,-1.0f,0.0f), XMVectorSet(0.75f,0.0f,0.0f,0.0f),XMVectorSet(-1.0f,0.0f,0.0f,0.0f)}, // 9
		{XMVectorSet(-1.0f,-1.0f,-1.0f,0.0f),XMVectorSet(0.75f,0.5f,0.0f,0.0f),XMVectorSet(-1.0f,0.0f,0.0f,0.0f)}, // 10
		{XMVectorSet(-1.0f,-1.0f,1.0f,0.0f), XMVectorSet(0.5f,0.5f,0.0f,0.0f), XMVectorSet(-1.0f,0.0f,0.0f,0.0f)}, // 11
		// 奥(4)
		{XMVectorSet(1.0f,1.0f,1.0f,0.0f),   XMVectorSet(0.75f,0.0f,0.0f,0.0f), XMVectorSet(0.0f,0.0f,1.0f,0.0f)}, // 12
		{XMVectorSet(-1.0f,1.0f,1.0f,0.0f),  XMVectorSet(1.0f,0.0f,0.0f,0.0f),  XMVectorSet(0.0f,0.0f,1.0f,0.0f)}, // 13
		{XMVectorSet(-1.0f,-1.0f,1.0f,0.0f), XMVectorSet(1.0f,0.5f,0.0f,0.0f),  XMVectorSet(0.0f,0.0f,1.0f,0.0f)}, // 14
		{XMVectorSet(1.0f,-1.0f,1.0f,0.0f),  XMVectorSet(0.75f,0.5f,0.0f,0.0f), XMVectorSet(0.0f,0.0f,1.0f,0.0f)}, // 15
		// 上(5)
		{XMVectorSet(-1.0f,1.0f,1.0f,0.0f),  XMVectorSet(0.0f,0.5f,0.0f,0.0f), XMVectorSet(0.0f,1.0f,0.0f,0.0f)}, // 16
		{XMVectorSet(1.0f,1.0f,1.0f,0.0f),   XMVectorSet(0.25f,0.5f,0.0f,0.0f),XMVectorSet(0.0f,1.0f,0.0f,0.0f)}, // 17
		{XMVectorSet(1.0f,1.0f,-1.0f,0.0f),  XMVectorSet(0.25f,1.0f,0.0f,0.0f),XMVectorSet(0.0f,1.0f,0.0f,0.0f)}, // 18
		{XMVectorSet(-1.0f,1.0f,-1.0f,0.0f), XMVectorSet(0.0f,1.0f,0.0f,0.0f), XMVectorSet(0.0f,1.0f,0.0f,0.0f)}, // 19
		// 下(6)
		{XMVectorSet(-1.0f,-1.0f,-1.0f,0.0f),XMVectorSet(0.25f,0.5f,0.0f,0.0f), XMVectorSet(0.0f,-1.0f,0.0f,0.0f)}, // 20
		{XMVectorSet(1.0f,-1.0f,-1.0f,0.0f), XMVectorSet(0.5f,0.5f,0.0f,0.0f),  XMVectorSet(0.0f,-1.0f,0.0f,0.0f)}, // 21
		{XMVectorSet(1.0f,-1.0f,1.0f,0.0f),  XMVectorSet(0.5f,1.0f,0.0f,0.0f),  XMVectorSet(0.0f,-1.0f,0.0f,0.0f)}, // 22
		{XMVectorSet(-1.0f,-1.0f,1.0f,0.0f), XMVectorSet(0.25f,1.0f,0.0f,0.0f), XMVectorSet(0.0f,-1.0f,0.0f,0.0f)}  // 23
	};

	// 頂点の数
	vertexNum_ = vertices_.size();
}

// インデックス情報の初期化
void Dice::InitIndexData() {
	index_ = {
		0,1,2,
		0,2,3,
		4,5,6,
		4,6,7,
		8,9,10,
		8,10,11,
		12,13,14,
		12,14,15,
		16,17,18,
		16,18,19,
		20,21,22,
		20,22,23,
	};

	// インデックスの数
	indexNum_ = index_.size();
}