#pragma once
#define NOMINMAX
#include "Struct.h"

KamataEngine::Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

// 球と平面の衝突判定
bool IsCollision(const Sphere& sphere, const Plane& plane);

// 球と線分の衝突判定
bool IsCollision(const Segment& segment, const Plane& plane);

// 三角形と線のあたり判定
bool IsCollision(const Segment& segment, const Triangle& triangle);

// 直方体と直方体の当たり判定
bool IsCollision(const AABB& aabb1, const AABB& aabb2);

// 球と直方体のあたり判定
bool IsCollision(const AABB& aabb, const Sphere& sphere);

// 直方体と線の当たり判定
bool IsCollision(const AABB& aabb, const Segment& segmrnt);
