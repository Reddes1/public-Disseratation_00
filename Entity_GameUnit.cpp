#include "Entity_GameUnit.h"

#include "GameConstants.h"			//Game info
#include "AnimationDefinitions.h"	//Animator info
#include "GeneralUtils.h"

#include "Game.h"					//Manager Access

void Entity_GameUnit::Update(GameTimer& gt)
{
	//Only update if enabled
	if (GetActiveState())
	{

	}

	//We dont want the animations desyncing, so update this regardless
	m_MainAnimator.Update(gt.DeltaTime());
}

void Entity_GameUnit::Render(DirectX::SpriteBatch& batch, DirectX::DescriptorHeap* heap)
{
	//Only render if enabled (this is set to default because this entity is likely created without a texture being set
	if (GetRenderState())
	{
		m_MainSprite.Draw(batch, heap);
	}

}

void Entity_GameUnit::EnableUnit()
{
	//Set Colour based on team ID
	switch (m_TeamID)
	{
	case static_cast<unsigned>(TeamIndexes::TEAM_00):
		m_MainSprite.SetColour(TEAM_00_COLOUR);

		break;
	case static_cast<unsigned>(TeamIndexes::TEAM_01):
		m_MainSprite.SetColour(TEAM_01_COLOUR);
		
		break;
	}

	//Set Animation based on class ID
	switch (m_Data.m_ID)
	{
	case GameUnitIDs::ASSASSIN:
		m_MainAnimator.SetAnimation((int)Assassin_00_Animations::IDLE_00);
		break;
	case GameUnitIDs::BARBARIAN:
		m_MainAnimator.SetAnimation((int)Barbarian_00_Animations::IDLE_00);
		break;
	case GameUnitIDs::ARCHER:
		m_MainAnimator.SetAnimation((int)Archer_00_Animations::IDLE_00);
		break;
	case GameUnitIDs::NECROMANCER:
		m_MainAnimator.SetAnimation((int)Necromancer_00_Animations::IDLE_00);
		break;
	case GameUnitIDs::SORCERESS:
		m_MainAnimator.SetAnimation((int)Sorceress_00_Animations::IDLE_00);
		break;
	case GameUnitIDs::WARRIOR:
		m_MainAnimator.SetAnimation((int)Warrior_00_Animations::IDLE_00);
		break;
	}


	//Enable flags
	m_CanAct = true;
	m_CanMove = true;
}

void Entity_GameUnit::DisableUnit()
{
	//Set Colour
	m_MainSprite.SetColour(UNIT_DISABLED_COLOUR_00);

	//Set Animation based on class ID
	switch (m_Data.m_ID)
	{
	case GameUnitIDs::ASSASSIN:
		m_MainAnimator.SetAnimation((int)Assassin_00_Animations::IDLE_00);
		break;
	case GameUnitIDs::BARBARIAN:
		m_MainAnimator.SetAnimation((int)Barbarian_00_Animations::IDLE_00);
		break;
	case GameUnitIDs::ARCHER:
		m_MainAnimator.SetAnimation((int)Archer_00_Animations::IDLE_00);
		break;
	case GameUnitIDs::NECROMANCER:
		m_MainAnimator.SetAnimation((int)Necromancer_00_Animations::IDLE_00);
		break;
	case GameUnitIDs::SORCERESS:
		m_MainAnimator.SetAnimation((int)Sorceress_00_Animations::IDLE_00);
		break;
	case GameUnitIDs::WARRIOR:
		m_MainAnimator.SetAnimation((int)Warrior_00_Animations::IDLE_00);
		break;
	}

	//Disable flags
	m_CanAct = false;
	m_CanMove = false;
}

void Entity_GameUnit::InitialiseFLModule(std::string aiFP, unsigned configIndex)
{
	if (m_FLModule == nullptr)
	{
		//Allocate Memory and flag enabled
		m_FLModule = new FL_Controller();
		m_AIPilotEnabled = true;

		//Load AI from file
		m_FLModule->InitFromFile(aiFP, configIndex);
	}
}

void Entity_GameUnit::InitialiseActionAIModule()
{
	if (m_ActionAlgorithms == nullptr)
	{
		m_ActionAlgorithms = new AI_Unit_Algorithms_00();
		m_ActionAlgorithms->BindToUnit(this);
	}
}

void Entity_GameUnit::InitialiseBTAIModule()
{
	if (m_BTModule == nullptr)
	{
		m_BTModule = new AI_BehaviourTree_Interface();
		m_AIPilotEnabled = true;

		m_BTModule->BindUnitController(m_ActionAlgorithms);
	}
}


bool Entity_GameUnit::AnyActionsAvailable()
{
	return m_CanAct || m_CanMove;
}

void Entity_GameUnit::Init()
{
	//Pair base sprite and animator
	m_MainAnimator.SetSpritePointer(&m_MainSprite);

	//Disable flags & ensure certain updates dont occur
	SetActiveState(false);
	SetRenderState(false);

	m_MainAnimator.SetPlay(false);

}

void Entity_GameUnit::Release()
{
	if (m_FLModule)
	{
		delete m_FLModule;
		m_FLModule = nullptr;
	}
	if (m_ActionAlgorithms)
	{
		delete m_ActionAlgorithms;
		m_ActionAlgorithms = nullptr;
	}
	if (m_BTModule)
	{
		delete m_BTModule;
		m_BTModule = nullptr;
	}
}

ConditionCode Entity_GameUnit::UpdateBehaviourAI(GameTimer& gt)
{
	//Only ever run the AI if the pilot flag is enabled, and that the unit is alive
	if (!m_Data.m_Alive || !m_AIPilotEnabled)
	{
		//Return AI not in operation code
		return ConditionCode::AI_NOT_OPERABLE;
	}

	//If the control flag is enabled
	if (m_UpdateAI)
	{
		//Update by hierarchy
		if (m_BTModule)
			return UpdateBehaviourBT();
		else if (m_FLModule)
			return UpdateBehaviourFL(gt);
	}

	return ConditionCode::AI_NOT_OPERABLE;
}

ConditionCode Entity_GameUnit::UpdateBehaviourBT()
{
	//Get run condition
	ConditionCode ret = m_BTModule->RunAlgorithm();
	//If the root has reset, then tree has run a full cycle, so disable update flag and return
	if (ret == ConditionCode::ROOT_RESET)
	{
		m_UpdateAI = false;
		return ConditionCode::AI_CYCLE_COMPLETE;
	}
	//Algorithm not complete yet
	else
		return ConditionCode::RUNNING;

	//Should not hit this
	assert(false);
	return ConditionCode();
}

ConditionCode Entity_GameUnit::UpdateBehaviourFL(GameTimer& gt)
{
	//If the flagged, run fresh cycle function
	if (m_FLFreshCycleFlag)
	{
		StartFreshCycleFL();
		return ConditionCode::RUNNING;
	}

	m_Elapsed += gt.DeltaTime();

	//If timer elapsed, run action
	if (m_Elapsed >= m_ActionDelay)
	{
		//Reset timer
		m_Elapsed = 0.f;

		//Based on override state, determine what state to use
		FuzzyBehaviourStates state = FuzzyBehaviourStates::COUNT;
		if (m_FLOverrideFlag)
			state = m_OverrideState;
		else
			state = static_cast<FuzzyBehaviourStates>(m_FLModule->GetRuleset().GetActiveState());

		//Action State should be determined, so run behaviour logic based on the current state
		switch (state)
		{
		case FuzzyBehaviourStates::AGGRO:
			return RunFLBehaviour_Aggro();
			break;
		case FuzzyBehaviourStates::BALANCED:
			return RunFLBehaviour_Balanced();
			break;
		case FuzzyBehaviourStates::DEFENSIVE:
			return RunFLBehaviour_Defensive();
			break;
		}

		msg_assert(false, "UpdateBehaviourFL(): Unknown State/No Function.");
	}

	return ConditionCode::RUNNING;
}

bool Entity_GameUnit::StartFreshCycleFL()
{
	//Run search and targeting algorithm to target for FL measurement

	//If in-range search fails, get all known enemies instead
	if (m_ActionAlgorithms->RunSearchAlgorithm(AI_Unit_Algorithms_00::SearchAlgorithms::FIND_ALL_ENEMIES) == ConditionCode::FAILED)
		m_ActionAlgorithms->RunSearchAlgorithm(AI_Unit_Algorithms_00::SearchAlgorithms::FIND_ALL_ENEMIES_OOR);

	//Now find closest enemy
	m_ActionAlgorithms->RunTargetAlgorithm(AI_Unit_Algorithms_00::TargetAlgorithms::CLOSEST_ENEMY);

	//If not being override by team state, determine own state
	if (!m_FLOverrideFlag)
	{
		//Get target for brevity
		Entity_GameUnit* const target = m_ActionAlgorithms->GetPrimaryTargetUnit();
		//Build data bundle
		std::vector<float> flValues(3, 0);

		//Store this units hp, and targets hp
		flValues[0] = m_Data.m_Health;
		flValues[1] = target->GetUnitData().m_Health;
		//Calculate the distance between the units and store it
		flValues[2] = (float)m_ActionAlgorithms->GetDistanceToUnit(target);

		//Run FL logic algorithm
		m_FLModule->RunLogicAlgorithm(flValues);
		//Update combat log with FL state
		Game::GetGame()->GetGameplayManager().GetCombatLog().LogUnitFL_00(this, target);
	}

	//Disable flags and reset step count for next action cycle
	m_CurrentActionStep = 0;
	m_Elapsed = 0.f;
	m_FLFreshCycleFlag = false;

	return true;
}

ConditionCode Entity_GameUnit::RunFLBehaviour_Aggro()
{
	//Use current action step to define action
	switch (m_CurrentActionStep)
	{
		//Move Unit
	case 0:
		//Check to see if this unit is ranged or not to dictate move type
		if (m_Data.m_AttackRange > 1)
		{
			//If no ranged tile free, attempt to move adjacently, else move closer (was out of range)
			if (m_ActionAlgorithms->RunMovementAlgorithm(AI_Unit_Algorithms_00::MovementAlgorithms::MOVE_TO_FIRST_RANGE_TILE) == ConditionCode::FAILED)
				if (m_ActionAlgorithms->RunMovementAlgorithm(AI_Unit_Algorithms_00::MovementAlgorithms::RANDOM_ADJACENT_TILE_TARGET) == ConditionCode::FAILED)
					m_ActionAlgorithms->RunMovementAlgorithm(AI_Unit_Algorithms_00::MovementAlgorithms::MOVE_CLOSER_TO_TARGET);
		}
		//Unit not ranged
		else
		{
			//Attempt to move to adjacent to target, else move closer (was out of range)
			if (m_ActionAlgorithms->RunMovementAlgorithm(AI_Unit_Algorithms_00::MovementAlgorithms::RANDOM_ADJACENT_TILE_TARGET) == ConditionCode::FAILED)
				m_ActionAlgorithms->RunMovementAlgorithm(AI_Unit_Algorithms_00::MovementAlgorithms::MOVE_CLOSER_TO_TARGET);
		}
		
		//Increment count and return
		++m_CurrentActionStep;
		return ConditionCode::RUNNING;
		break;

		//Attack Target
	case 1:

		//Attempt to attack target
		m_ActionAlgorithms->RunAttackAlgorithm(AI_Unit_Algorithms_00::AttackAlgorithms::ATTACK_UNIT);

		//Flag algorithm completion
		m_UpdateAI = false;
		m_FLFreshCycleFlag = true;
		return ConditionCode::AI_CYCLE_COMPLETE;
	}

	//Edge case check
	if (m_CurrentActionStep > 1)
	{
		msg_assert(false, "RunFLBehaviour_Aggro(): Step count OOR of algorithm");
		//For safety, flag algorithm done so that cycle can attempt to cycle properly
		m_UpdateAI = false;
		m_FLFreshCycleFlag = true;
		return ConditionCode::AI_CYCLE_COMPLETE;
	}

	//Shouldn't be hit
	assert(false);
	return ConditionCode();
}

ConditionCode Entity_GameUnit::RunFLBehaviour_Balanced()
{
	//Use current action step to define action
	switch (m_CurrentActionStep)
	{
		//Move Unit
	case 0:
		//Check to see if this unit is ranged or not to dictate move type
		if (m_Data.m_AttackRange > 1)
		{
			//If no ranged tile free, attempt to move adjacently, else move closer (was out of range)
			if (m_ActionAlgorithms->RunMovementAlgorithm(AI_Unit_Algorithms_00::MovementAlgorithms::MOVE_TO_FIRST_RANGE_TILE) == ConditionCode::FAILED)
				if (m_ActionAlgorithms->RunMovementAlgorithm(AI_Unit_Algorithms_00::MovementAlgorithms::BEST_DEFENSIVE_ADJACENT) == ConditionCode::FAILED)
					m_ActionAlgorithms->RunMovementAlgorithm(AI_Unit_Algorithms_00::MovementAlgorithms::MOVE_CLOSER_TO_TARGET);
		}
		//Unit not ranged
		else
		{
			//Attempt to move to adjacent to target, else move closer (was out of range)
			if (m_ActionAlgorithms->RunMovementAlgorithm(AI_Unit_Algorithms_00::MovementAlgorithms::BEST_DEFENSIVE_ADJACENT) == ConditionCode::FAILED)
				m_ActionAlgorithms->RunMovementAlgorithm(AI_Unit_Algorithms_00::MovementAlgorithms::MOVE_CLOSER_TO_TARGET);
		}

		//Increment count and return
		++m_CurrentActionStep;
		return ConditionCode::RUNNING;
		break;

		//Attack Target
	case 1:

		//Attempt to attack target
		m_ActionAlgorithms->RunAttackAlgorithm(AI_Unit_Algorithms_00::AttackAlgorithms::ATTACK_UNIT);

		//Flag algorithm completion
		m_UpdateAI = false;
		m_FLFreshCycleFlag = true;
		return ConditionCode::AI_CYCLE_COMPLETE;
	}

	//Edge case check
	if (m_CurrentActionStep > 1)
	{
		msg_assert(false, "RunFLBehaviour_Aggro(): Step count OOR of algorithm");
		//For safety, flag algorithm done so that cycle can attempt to cycle properly
		m_UpdateAI = false;
		m_FLFreshCycleFlag = true;
		return ConditionCode::AI_CYCLE_COMPLETE;
	}


	//Shouldn't be hit
	assert(false);
	return ConditionCode();
}

ConditionCode Entity_GameUnit::RunFLBehaviour_Defensive()
{
	//Use current action step to define action
	switch (m_CurrentActionStep)
	{
		//Pre-Move Attack
	case 0:
		//Attempt to attack in range target, or any target after if possible before moving
		if (m_ActionAlgorithms->RunAttackAlgorithm(AI_Unit_Algorithms_00::AttackAlgorithms::ATTACK_UNIT) == ConditionCode::FAILED)
			if (m_ActionAlgorithms->RunAttackAlgorithm(AI_Unit_Algorithms_00::AttackAlgorithms::ATTACK_UNIT) == ConditionCode::SUCCESS)
				m_CanAct = false;

		//Increment count and return
		++m_CurrentActionStep;
		return ConditionCode::RUNNING;
		break;

		//Move Unit
	case 1:
		//Attempt to find the highest defensive tile in range and move to it
		m_ActionAlgorithms->RunMovementAlgorithm(AI_Unit_Algorithms_00::MovementAlgorithms::BEST_DEFENSIVE_TILE);

		//Increment count and return
		++m_CurrentActionStep;
		return ConditionCode::RUNNING;
		break;
	
		//Post-Move Attack
	case 2:

		//If pre-move failed
		if (m_CanAct)
		{
			//Attempt to attack target if in range, else attack anything else in range
			if (m_ActionAlgorithms->RunAttackAlgorithm(AI_Unit_Algorithms_00::AttackAlgorithms::ATTACK_UNIT) == ConditionCode::FAILED)
				m_ActionAlgorithms->RunAttackAlgorithm(AI_Unit_Algorithms_00::AttackAlgorithms::ATTACK_FIRST_VALID_ENEMY);
		}

		//Flag algorithm completion
		m_UpdateAI = false;
		m_FLFreshCycleFlag = true;
		return ConditionCode::AI_CYCLE_COMPLETE;
		break;
	}

	//Edge case check
	if (m_CurrentActionStep > 2)
	{
		msg_assert(false, "RunFLBehaviour_Defensive(): Step count OOR of algorithm");
		//For safety, flag algorithm done so that cycle can attempt to cycle properly
		m_UpdateAI = false;
		m_FLFreshCycleFlag = true;

		return ConditionCode::AI_CYCLE_COMPLETE;
	}


	//Shouldn't be hit
	assert(false);
	return ConditionCode();
}

