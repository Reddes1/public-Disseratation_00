#include "FL_Action_Module.h"

bool FL_Action_Module::Update(GameTimer& gt)
{
	//If the action cycle isn't complete
	if(!m_ActionCycleComplete)
	{
		//Update Timer
		m_Elapsed += gt.DeltaTime();

		//If timer has ticked over
		if (m_Elapsed >= m_ActionDelay)
		{
			//Reset Timer
			m_ActionDelay = 0.f;

			//Run algorithm based on current state
			switch (m_BehaviourState)
			{
			case BehaviourState::AGGRO:
				return RunAggroBehaviourLine();
				break;
			case BehaviourState::BALANCED:
				return RunBalancedBehaviourLine();
				break;
			case BehaviourState::DEFENSIVE:
				return RunDefensiveBehaviourLine();
				break;
			}
		}
	}
	else
	{
		//Actions can't be executed (likely because its complete, so return true to signal completion)
		return true;
	}

	return false;
}

bool FL_Action_Module::RunAggroBehaviourLine()
{

	return false;
}

bool FL_Action_Module::RunBalancedBehaviourLine()
{
	return false;
}

bool FL_Action_Module::RunDefensiveBehaviourLine()
{
	return false;
}
