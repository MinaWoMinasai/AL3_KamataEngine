#pragma once
#include "KamataEngine.h"
#include <Windows.h>
#include <vector>

KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);

void WorldTransformUpdate(KamataEngine::WorldTransform& worldTransform);