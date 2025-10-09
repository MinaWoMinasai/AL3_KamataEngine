#pragma once
#include "KamataEngine.h"
#include <Windows.h>

struct AABB {
	KamataEngine::Vector3 min;
	KamataEngine::Vector3 max;
};
