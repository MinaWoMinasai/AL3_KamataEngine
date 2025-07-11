#include "Enemy.h"
#include "BaseEnemyState.h"

class LeaveState : public BaseEnemyState {
public:
	void Update(Enemy& enemy) override;
};