#include "Calculation.h"
#include <algorithm>
using namespace KamataEngine;
using namespace MathUtility;

KamataEngine::Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {

	Matrix4x4 result;

	result.m[0][0] = width / 2.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = -height / 2.0f;
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[2][3] = 0.0f;

	result.m[3][0] = left + width / 2.0f;
	result.m[3][1] = top + height / 2.0f;
	result.m[3][2] = minDepth;
	result.m[3][3] = 1.0f;

	return result;
}

bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
		return true;
	}
	return false;
}

Vector3 GetOverlap(const AABB& a, const AABB& b) {
	Vector3 overlap = {std::min(a.max.x, b.max.x) - std::max(a.min.x, b.min.x), std::min(a.max.y, b.max.y) - std::max(a.min.y, b.min.y), std::min(a.max.z, b.max.z) - std::max(a.min.z, b.min.z)};
	return overlap;
}

bool IsCollision(const Sphere& sphere, const Plane& plane) {

	float distance = Dot(plane.normal, sphere.center) - plane.distance;
	if (distance < 0.0f) {
		distance *= -1.0f;
	}
	return distance < sphere.radius;
}

bool IsCollision(const Segment& segment, const Plane& plane) {

	float dot = Dot(plane.normal, segment.diff);

	if (dot == 0.0f) {
		return false;
	}
	float t = (plane.distance - Dot(plane.normal, segment.origin)) / dot;

	// tの値と線の種類によって衝突しているかを判断する
	return (0.0f <= t && t <= 1.0f);
}

bool IsCollision(const Segment& segment, const Triangle& triangle) {
	// 三角形の点を結んだベクトルを作る
	const Vector3 v01 = triangle.vertex[1] - triangle.vertex[0];
	const Vector3 v12 = triangle.vertex[2] - triangle.vertex[1];
	const Vector3 v20 = triangle.vertex[0] - triangle.vertex[2];

	Vector3 dir = segment.diff - segment.origin; // 線分の方向ベクトル
	Vector3 normal = Cross(v01, v12);
    normal = Normalize(normal); // 三角形の法線

	float d = Dot(normal, triangle.vertex[0]);
	float denom = Dot(normal, dir);

	// 平行チェック
	if (fabs(denom) < 1e-6f)
		return false;

	// 線分と平面の交点を t で求める（segment.origin + t * dir）
	float t = (d - Dot(normal, segment.origin)) / denom;

	// t が [0,1] の範囲外なら、交点は線分外にある
	if (t < 0.0f || t > 1.0f)
		return false;

	Vector3 intersection = segment.origin + t * dir;

	// 頂点と衝突点pを結んだベクトルを作る
	Vector3 v0p = intersection - triangle.vertex[0];
	Vector3 v1p = intersection - triangle.vertex[1];
	Vector3 v2p = intersection - triangle.vertex[2];

	// これらのベクトルのクロス積を取る
	Vector3 cross01 = Cross(v01, v1p);
	Vector3 cross12 = Cross(v12, v2p);
	Vector3 cross20 = Cross(v20, v0p);

	// すべての小三角形のクロス積と法線が同じ方向を向いていたら衝突
	if (Dot(cross01, normal) >= 0.0f && Dot(cross12, normal) >= 0.0f && Dot(cross20, normal) >= 0.0f) {
		return true;
	}
	return false;
}

bool IsCollision(const AABB& aabb, const Sphere& sphere) {
	Vector3 closetpoint{
	    std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
	    std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
	    std::clamp(sphere.center.z, aabb.min.z, aabb.max.z),
	};
	// 最近接点と球の中心との距離を求める
	float distance = Length(closetpoint - sphere.center);
	// 距離が半径よりも小さければ衝突
	if (distance <= sphere.radius) {
		// 衝突
		return true;
	}

	return false;
}

bool IsCollision(const AABB& aabb, const Segment& segment) {
	Vector3 dir = segment.diff; // 方向ベクトル（差分）

	float tmin = 0.0f; // 線分始点
	float tmax = 1.0f; // 線分終点

	for (int i = 0; i < 3; ++i) {
		float origin = (i == 0) ? segment.origin.x : (i == 1) ? segment.origin.y : segment.origin.z;
		float direction = (i == 0) ? dir.x : (i == 1) ? dir.y : dir.z;
		float slabMin = (i == 0) ? aabb.min.x : (i == 1) ? aabb.min.y : aabb.min.z;
		float slabMax = (i == 0) ? aabb.max.x : (i == 1) ? aabb.max.y : aabb.max.z;

		if (fabsf(direction) < 1e-6f) {
			// 平行な場合、始点がAABB内にないと交差しない
			if (origin < slabMin || origin > slabMax) {
				return false;
			}
		} else {
			float t1 = (slabMin - origin) / direction;
			float t2 = (slabMax - origin) / direction;

			if (t1 > t2) {
				float temp = t1;
				t1 = t2;
				t2 = temp;
			}

			tmin = (t1 > tmin) ? t1 : tmin;
			tmax = (t2 < tmax) ? t2 : tmax;

			if (tmin > tmax) {
				return false;
			}
		}
	}

	// すべて当たっていたら
	return true;
}
