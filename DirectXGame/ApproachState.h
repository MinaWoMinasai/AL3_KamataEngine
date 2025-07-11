#pragma once
#include "BaseEnemyState.h"
#include "Enemy.h"

class ApproachState : public BaseEnemyState {
public:
	void Update(Enemy& enemy) override;
};
