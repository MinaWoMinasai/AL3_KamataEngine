#pragma once
#include <KamataEngine.h>
#include <algorithm>

// 線形補間
KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t);
// 球面線形補間
KamataEngine::Vector3 Slerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t);

// Catmull-Rom 補間
KamataEngine::Vector3 CatmullRomInterpolation(const KamataEngine::Vector3& p0, const KamataEngine::Vector3& p1, const KamataEngine::Vector3& p2, const KamataEngine::Vector3& p3, float t);

// Catmull-Romスプライン曲線上の座標を得る
KamataEngine::Vector3 CatmullRomPosition(const std::vector<KamataEngine::Vector3>& points, float t);

KamataEngine::Vector3 VectorToRotation(const KamataEngine::Vector3& dir);

// ビューポート変換行列 
KamataEngine::Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);