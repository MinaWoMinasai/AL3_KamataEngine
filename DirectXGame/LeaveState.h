#include "Enemy.h"
#include "BaseEnemyState.h"

class LeaveState : public BaseEnemyState {
public:

	void Initialize(Enemy& enemy) override;
	void Update(Enemy& enemy) override;
};