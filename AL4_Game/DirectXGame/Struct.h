#pragma once
#include "KamataEngine.h"
#include <Windows.h>

// 球
struct Sphere {
	KamataEngine::Vector3 center; // 中心点
	float radius;   // 半径
};

struct Line {
	KamataEngine::Vector3 origin; // 始点
	KamataEngine::Vector3 diff;   // 終点への差分ベクトル
};

struct Ray {
	KamataEngine::Vector3 origin; // 始点
	KamataEngine::Vector3 diff;   // 終点への差分ベクトル
};

struct Segment {
	KamataEngine::Vector3 origin; // 始点
	KamataEngine::Vector3 diff;   // 終点への差分ベクトル
};

struct Capsule {
	Segment segment;
	float radius;
};

struct Plane {
	KamataEngine::Vector3 normal; // 法線
	float distance; // 距離
};

struct Triangle {
	KamataEngine::Vector3 vertex[3]; // 頂点
};

struct AABB {
	KamataEngine::Vector3 min; // 最小値
	KamataEngine::Vector3 max; // 最大値
};

struct Spring {
	// アンカー。固定された端の位置
	KamataEngine::Vector3 anchor;
	float naturalLength;      // 自然長
	float stiffness;          // 剛性。ばね定数k
	float dampingCoefficient; // 減衰係数
};

struct Ball {
	KamataEngine::Vector3 position;
	KamataEngine::Vector3 velocity;
	KamataEngine::Vector3 acceleration;
	float mass;
	float radius;
	unsigned int color;
};

struct Pendulum {
	KamataEngine::Vector3 anchor;
	float length;
	float angle;
	float anglerVelocity;
	float angularAcceleration;
};

struct ConicalPendulum {
	KamataEngine::Vector3 anchor;
	float length;
	float halfApexAngle;
	float angle;
	float angularVelocity;
};
