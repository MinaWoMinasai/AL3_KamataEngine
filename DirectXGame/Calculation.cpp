#include "Calculation.h"
using namespace KamataEngine;
using namespace MathUtility;

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {

	Vector3 result{
	    v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
	    v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
	    v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2],
	};

	return result;
}

KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t) { 
	return (1.0f - t) * v1 + t * v2; 
}


KamataEngine::Vector3 Slerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t) {

	// 正規化
	Vector3 from = Normalize(const_cast<Vector3&>(v1));
	Vector3 to = Normalize(const_cast<Vector3&>(v2));

	// 内積（cosθ）
	float dot = Dot(from, to);

	// クランプ（数値誤差でacosの範囲外に出るのを防ぐ）
	dot = std::clamp(dot, -1.0f, 1.0f);

	// θを求める
	float theta = std::acos(dot);

	// θがほぼ0の場合はLerpで近似（0除算対策）
	if (std::abs(theta) < 1e-5f) {
		return Lerp(from, to, t);
	}

	// 球面補間係数
	float sinTheta = std::sin(theta);
	float scale1 = std::sin((1.0f - t) * theta) / sinTheta;
	float scale2 = std::sin(t * theta) / sinTheta;

	return scale1 * from + scale2 * to;
}

KamataEngine::Vector3 CatmullRomInterpolation(const KamataEngine::Vector3& p0, const KamataEngine::Vector3& p1, const KamataEngine::Vector3& p2, const KamataEngine::Vector3& p3, float t) {
	
	const float s = 0.5f;

	float t2 = t * t;
	float t3 = t2 * t;

	Vector3 e3 = -p0 + 3 * p1 - 3 * p2 + p3;
	Vector3 e2 = 2 * p0 - 5 * p1 + 4 * p2 - p3;
	Vector3 e1 = -p0 + p2;
	Vector3 e0 = 2 * p1;

	return s * (e3 * t3 + e2 * t2 + e1 * t + e0);
}

KamataEngine::Vector3 CatmullRomPosition(const std::vector<KamataEngine::Vector3>& points, float t) { 

	// 区間点は制御店の数-1
	size_t division = points.size() - 1;
	// 1区間の長さ(全体を1.0とした割合)
	float areaWidth = 1.0f / division;

	// 区間点の始点を0.0f, 終点を1.0fとしたときの現在位置
	float t_2 = std::fmod(t, areaWidth) * division;
	// 下限(0.0f)と上限(1.0f)の範囲に収める
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	// 区間番号
	size_t index = static_cast<size_t>(t / areaWidth);
	// 区間番号が上限を超えないように収める
	index = std::clamp(index, size_t(0u), division - 1);

	// 四点分のインデックス
	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	// 最初の区間のp0はp1を重複使用する
	if (index == 0) {
		index0 = index1;
	}
	// 最後の区間のp3はp2を重複使用する
	if (index3 >= points.size()) {
		index3 = index2;
	}

	// 4点の座標
	const Vector3& p0 = points[index0];
	const Vector3& p1 = points[index1];
	const Vector3& p2 = points[index2];
	const Vector3& p3 = points[index3];

	// 四点を指定してCatmull-Rom補間
	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
}
