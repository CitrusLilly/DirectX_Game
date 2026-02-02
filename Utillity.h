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

// マテリアル
struct Material {
	float Ambient[4];	// 環境光
	float Diffuse[4];	// 拡散光
	float Specular[4];	// 鏡面光
	float Alpha;		// 透明度(1.0 = 不透明,0.0 = 透明)

	Material() {
		for (int i = 0; i < 4; i++) {
			Ambient[i] = 1.0f;
			Diffuse[i] = 1.0f;
			Specular[i] = 1.0f;
		}
		Alpha = 1.0f;
	}

	// 環境光設定
	void SetAmbient(float r, float g, float b, float factor) {
		Ambient[0] = r;
		Ambient[1] = g;
		Ambient[2] = b;
		Ambient[3] = factor;
	}

	// 拡散光設定
	void SetDiffuse(float r, float g, float b, float factor) {
		Diffuse[0] = r;
		Diffuse[1] = g;
		Diffuse[2] = b;
		Diffuse[3] = factor;
	}

	// 鏡面光設定
	void SetSpecular(float r, float g, float b, float factor) {
		Specular[0] = r;
		Specular[1] = g;
		Specular[2] = b;
		Specular[3] = factor;
	}
};