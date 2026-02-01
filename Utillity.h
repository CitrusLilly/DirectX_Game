#pragma once

// 色構造体
struct Color {
	float Red;
	float Green;
	float Blue;
	float Alpha;

	Color(float red, float green, float blue, float alpha) {
		Red = red;
		Green = green;
		Blue = blue;
		Alpha = alpha;
	}

	Color() {
		Red = Green = Blue = Alpha = 1.0f;
	}
};

// 三次元構造体
struct Vector3 {
	float X;
	float Y;
	float Z;

	Vector3(float x, float y, float z) {
		X = x;
		Y = y;
		Z = z;
	}

	Vector3() {
		X = Y = Z = 0.0f;
	}
};

// 二次元構造体
struct Vector2 {
	float X;
	float Y;

	Vector2(float x, float y) {
		X = x;
		Y = y;
	}

	Vector2() {
		X = Y = 0.0f;
	}
};

// 頂点情報
struct CustomVERTEX {
	Vector3 position;	// 頂点座標
	Vector3 normal;		// 法線ベクトル
	Vector2 uv;			// UV
	Color color;		// 頂点カラー
};