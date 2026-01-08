#pragma once
#include "Quad.h"

class Dice : public Quad {
public:
	// 頂点情報の初期化
	void InitVertexData()	override;
	// インデックス情報の初期化
	void InitIndexData()	override;
};