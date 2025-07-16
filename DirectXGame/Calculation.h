#pragma once
#include <KamataEngine.h>
#include <algorithm>

KamataEngine::Vector3 TransformNormal(const KamataEngine::Vector3& v, const KamataEngine::Matrix4x4& m);

// 線形補間
KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t);
// 球面線形補間
KamataEngine::Vector3 Slerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t);
