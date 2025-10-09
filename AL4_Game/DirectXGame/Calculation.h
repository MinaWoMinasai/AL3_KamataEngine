#pragma once
#define NOMINMAX
#include "Struct.h"

KamataEngine::Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// 直方体と直方体の当たり判定
bool IsCollision(const AABB& aabb1, const AABB& aabb2);