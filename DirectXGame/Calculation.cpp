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
