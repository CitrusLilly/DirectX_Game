#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

namespace CoreEngine {

	using std::wstring;
	using std::wstring_view;
	using std::unique_ptr;
	using std::shared_ptr;
	using std::make_unique;
	using std::make_shared;
	using Microsoft::WRL::ComPtr;

	using namespace DirectX;

	// ”’l‚©‚ç•¶š—ñ‚Ö‚Ì•ÏŠ·
	template<typename T>
	inline std::wstring ToString(const T& v) { return std::to_wstring(v); }
	inline std::wstring ToString(const wstring& v) { return v; }
	inline std::wstring ToString(const wstring_view& v) { return std::wstring(v); }

	inline std::wstring ToString(const DirectX::XMFLOAT2& v) {
		std::wostringstream ss;
		ss << std::fixed << std::setprecision(3) << L"(" << v.x << L"," << v.y << "L)";
		return ss.str();
	}
	inline std::wstring ToString(const DirectX::XMFLOAT3& v) {
		std::wostringstream ss;
		ss << std::fixed << std::setprecision(3) << L"(" << v.x << L"," << v.y << L"," << v.z << "L)";
		return ss.str();
	}
	inline std::wstring ToString(const DirectX::XMFLOAT4& v) {
		std::wostringstream ss;
		ss << std::fixed << std::setprecision(3) << L"(" << v.x << L"," << v.y << L"," << v.z << L"," << v.w << "L)";
		return ss.str();
	}
	inline std::wstring ToString(const DirectX::XMVECTOR& v) {
		XMFLOAT4 f;
		XMStoreFloat4(&f, v); // ˆê’UXMFLOAT4‚É•ÏŠ·

		std::wostringstream ss;
		ss << std::fixed << std::setprecision(3) << L"(" << f.x << L"," << f.y << L"," << f.z << L"," << f.w << "L)";
		return ss.str();
	}

}