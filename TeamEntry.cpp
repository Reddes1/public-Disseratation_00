#include "TeamEntry.h"

#include "AnimationDefinitions.h"	//Anim Data
#include "GeneralUtils.h"

#include "Game.h"		//Manager Access


TeamEntry::TeamEntry(short teamID)
	:m_TeamID(teamID)
{
	Init();
}

TeamEntry::TeamEntry(short teamID, int unitsCap)
	:m_TeamID(teamID), m_UnitsCap(unitsCap)
{
	Init();
}

bool TeamEntry::InitialiseNewUnit(unsigned unitTypeID, DirectX::XMFLOAT2 pos)
{
	//Cycle through each unit till a free unit is found, else return a false
	for (auto& a : m_Units)
	{
		//Use active flag to determine units availability
		if (!a->GetActiveState())
		{
			//Setup the unit by unit ID
			switch (unitTypeID)
			{
			case GameUnitIDs::ASSASSIN:
				//Setup Texture
				a->GetBaseSprite().SetTexture("Assassin_SS_00", true);
				//Set animation to idle
				a->GetBaseAnimator().SetAnimation(static_cast<unsigned short>(Assassin_00_Animations::IDLE_00));
				break;
			case GameUnitIDs::BARBARIAN:
				//Setup Texture
				a->GetBaseSprite().SetTexture("Barbarian_SS_00", true);
				//Set animation to idle
				a->GetBaseAnimator().SetAnimation(static_cast<unsigned short>(Barbarian_00_Animations::IDLE_00));
				break;
			case GameUnitIDs::ARCHER:
				//Setup Texture
				a->GetBaseSprite().SetTexture("Archer_SS_00", true);
				//Set animation to idle
				a->GetBaseAnimator().SetAnimation(static_cast<unsigned short>(Archer_00_Animations::IDLE_00));
				break;
			case GameUnitIDs::NECROMANCER:
				//Setup Texture
				a->GetBaseSprite().SetTexture("Necromancer_SS_00", true);
				//Set animation to idle
				a->GetBaseAnimator().SetAnimation(static_cast<unsigned short>(Necromancer_00_Animations::IDLE_00));
				break;
			case GameUnitIDs::SORCERESS:
				//Setup Texture
				a->GetBaseSprite().SetTexture("Sorceress_SS_00", true);
				//Set animation to idle
				a->GetBaseAnimator().SetAnimation(static_cast<unsigned short>(Sorceress_00_Animations::IDLE_00));
				break;
			case GameUnitIDs::WARRIOR:
				//Setup Texture
				a->GetBaseSprite().SetTexture("Warrior_SS_00", true);
				//Set animation to idle
				a->GetBaseAnimator().SetAnimation(static_cast<unsigned short>(Warrior_00_Animations::IDLE_00));
				break;
			}

			//Setup flags
			a->SetActiveState(true);
			a->SetRenderState(true);

			a->SetActionFlag(true);
			a->SetMoveFlag(true);
			
			//Add to alive count
			++m_UnitsAlive;

			msg_assert(static_cast<unsigned>(m_UnitsAlive) <= m_UnitsCap, "Unit alive count incorrect. Review addition/subtraction calls.");
		}
	}

	//Setup has failed if it hits this point (this shouldn't be called as another check should of been done prior to this)
	msg_assert(false, "Unit creation failed to find free unit.");
	return false;
}

bool TeamEntry::InitialiseFLModule(std::string aiFP, unsigned configIndex, bool enableOverride)
{
	if (m_FLModule == nullptr)
	{
		//Allocate Memory and flag enabled
		m_FLModule = new FL_Controller();

		//Load AI from file
		m_FLModule->InitFromFile(aiFP, configIndex);
		m_FLOverrideFlag = enableOverride;

		if (enableOverride)
			for (auto& a : m_Units)
				a->SetFLOverrideFlag(enableOverride);

		return true;
	}

	//FL Module already allocated for
	return false;
}

void TeamEntry::UpdateUnits(GameTimer& gt)
{
	for (auto& a : m_Units)
		a->Update(gt);
}

void TeamEntry::RenderUnits(DirectX::SpriteBatch& sb, DirectX::DescriptorHeap* heap)
{
	for (auto& a : m_Units)
		a->Render(sb, heap);
}

Entity_GameUnit* TeamEntry::GetUnitByCoordinate(int x, int y, bool retOnlyIfActive)
{
	//Cycle through container and try find a unit
	for (auto& a : m_Units)
	{
		//Check if coords match
		if (x == a->GetCoords().GetMapCoordinatesXi() && y == a->GetCoords().GetMapCoordinatesYi())
		{
			//If the flag is active, but the unit isnt, then return nullptr
			if (retOnlyIfActive && !a->GetActiveState())
			{
				//Do Nothing, Found A Dead Unit
			}
			//else always return the unit
			else
				return a;
		}
	}
	//No Unit Found
	return nullptr;
}

void TeamEntry::EnableAllUnits()
{
	for (auto& a : m_Units)
	{
		if (a->GetActiveState())
		{
			a->EnableUnit();
		}
	}
}

void TeamEntry::DisableAllUnits()
{
	for (auto& a : m_Units)
	{
		//Disable
		a->SetActiveState(false);
	}
}

bool TeamEntry::UpdateTeamAI(GameTimer& gt)
{
	//If AI flag not enabled, do nothing
	if (!m_AIPilotFlag)
		return false;
	
	//If allocated and flagged correctly, run state update and pass this to each unit
	if (m_FLModule && m_FLOverrideFlag && !m_FLRunOnceDone)
		return FLOverrideUpdate(gt);

	//Run update and get condition flag
	ConditionCode ret = GetUnitAtIndex(m_CurrentUnitAIIndex)->UpdateBehaviourAI(gt);
	//If the AI has complete, or not operable for any reason (like unit dead)
	if (ret == ConditionCode::AI_CYCLE_COMPLETE || ret == ConditionCode::AI_NOT_OPERABLE)
	{

		//Run while loop to find next operable unit
		bool done = false;
		while (!done)
		{
			//Inc current index
			++m_CurrentUnitAIIndex;

			//If pass container limit, no further units to be able to update so prep for end of turn
			if (m_CurrentUnitAIIndex >= m_Units.size())
				return EndOfAITurnCleanUp();

			//Run validation check on the index and end loop if it passes
			done = UnitIndexValidation();
		}
	}

	//Team has not expended all unit actions, so return false
	return false;
}

bool TeamEntry::FLOverrideUpdate(GameTimer& gt)
{
	//Dirty pre-amble to get the opposite teams ID
	unsigned index = 0;
	if (m_TeamID == 0)
		index = 1;

	//Build data bundle (Arranged teams avg % hp [0, 1], teams alive unit count [2, 3])
	std::vector<float> flValues(4, 0);
	//Get opposing team
	TeamEntry* const eTeam = Game::GetGame()->GetGameplayManager().GetTeamsManager().GetTeamAtIndex(index);

	//Get the average hp % for each team
	flValues[0] = GetTeamsAverageHP();
	flValues[1] = eTeam->GetTeamsAverageHP();
	flValues[2] = (float)m_UnitsAlive;
	flValues[3] = (float)eTeam->GetUnitsAliveCount();

	//Run FL logic algorithm
	m_FLModule->RunLogicAlgorithm(flValues);

	//Impart state to each unit
	for (auto& a : m_Units)
		a->SetFLOverrideState(static_cast<FuzzyBehaviourStates>(m_FLModule->GetRuleset().GetActiveState()));

	//Update CombatLog
	Game::GetGame()->GetGameplayManager().GetCombatLog().LogTeamFL_00(this);

	//Flag run once done
	m_FLRunOnceDone = true;
	return false;
}

void TeamEntry::ResetTeamAIData()
{
	EndOfAITurnCleanUp();
}

bool TeamEntry::EndOfAITurnCleanUp()
{
	//Re-enable AI update flags for next cycle
	for (auto& a : m_Units)
		a->SetAIUpdateFlag(true);

	m_CurrentUnitAIIndex = 0;
	m_FLRunOnceDone = false;

	return true;
}

bool TeamEntry::UnitIndexValidation()
{
	if (GetUnitAtIndex(m_CurrentUnitAIIndex)->GetUnitData().m_Alive &&
		GetUnitAtIndex(m_CurrentUnitAIIndex)->GetActiveState())
		return true;
	
	return false;
}

bool TeamEntry::EndTeamTurn()
{
	return false;
}



Entity_GameUnit* TeamEntry::FindFreeUnit()
{
	for (auto& a : m_Units)
	{
		//If the unit isnt active
		if (!a->GetActiveState())
			return a;
	}

	//Failed to find a unit
	return nullptr;
}

bool TeamEntry::FindUnitIndexByPointer(unsigned& index, Entity_GameUnit* unit)
{
	for (unsigned i(0); i < m_Units.size(); ++i)
	{
		if (m_Units[i] == unit)
		{
			//Unit found, set index and ret
			index = i;
			return true;
		}
	}

	//No unit found, no change to index value
	return false;
}

float TeamEntry::GetTeamsAverageHP(unsigned avgType)
{
	switch (avgType)
	{
	case 0:
		return TeamAverageType00();
		break;
	}

	return 0;
}

void TeamEntry::SetAIOverrideFlagForUnits(bool overrideFlag)
{
	for (auto& a : m_Units)
		a->SetFLOverrideFlag(overrideFlag);

	m_FLOverrideFlag = overrideFlag;
}

void TeamEntry::IncrementFunds(int newFunds, bool clampToCap, bool clampToZero, bool breakCapRule, bool breakZeroRule)
{
	//Run an early check to determine if funds already beyond the cap, and that this is flagged as allowed.
	if (m_Funds > m_FundsCap && breakCapRule)
		return;

	//Increment Funds
	m_Funds += newFunds;

	//If cap clamp enabled and above cap
	if (clampToCap && m_Funds > m_FundsCap)
		m_Funds = m_FundsCap;

	//Run an early out check to see if values below zero are allowed
	if (m_Funds < 0 && breakZeroRule)
		return;
	//If zero clamp enabled and below zero
	else if (clampToZero && m_Funds < 0)
		m_Funds = 0;
}

void TeamEntry::IncrementUnitsCap(int newCap)
{
	//Increment
	m_UnitsCap += newCap;

	//Check if below zero
	if (m_UnitsCap < 0)
		m_UnitsCap = 0;
}

void TeamEntry::IncrementUnitsAlive(int count)
{
	//Increment
	m_UnitsAlive += count;
	//Should never go below zero
	assert(m_UnitsAlive > -1);

	//It's not invalid that the units be above the cap, but inform debug that this happened
	if(m_UnitsAlive > static_cast<int>(m_UnitsCap))
		DBOUT("Units Alive over Unit Cap. Is this intended?")
}

void TeamEntry::Init()
{
	//Reserve container space
	m_Units.reserve(m_UnitsCap);

	//Initialise container with blank units
	for (unsigned i(0); i < m_UnitsCap; ++i)
		m_Units.push_back(new Entity_GameUnit());
}

void TeamEntry::Release()
{
	for (auto& a : m_Units)
	{
		delete a;
		a = nullptr;
	}
	m_Units.clear();
	
	if (m_FLModule)
	{
		delete m_FLModule;
		m_FLModule = nullptr;
	}

}

float TeamEntry::TeamAverageType00()
{
	//Store sum of hps, and number of alive units
	float sum = 0;
	float maxSum = 0;

	for (auto& a : m_Units)
	{
		if (a->GetActiveState() && a->GetUnitData().m_Alive)
		{
			sum += a->GetUnitData().m_Health;
			maxSum += a->GetUnitData().m_MaxHealth;
		}
	}

	return GetPercentageOfNumber(sum, maxSum, true);
}
