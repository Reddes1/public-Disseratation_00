#include "SM_TeamsManager.h"

#include "Game.h"			//Manager Access
#include "GeneralUtils.h"	//Math support

size_t SM_TeamsManager::AddNewTeam()
{
	m_Teams.reserve(m_Teams.size());

	//Insert new team and give it an ID
	TeamEntry newTeam(static_cast<short>(m_Teams.size()));

	//Put into container
	m_Teams.push_back(new TeamEntry(static_cast<short>(m_Teams.size())));

	//Pass back an index for use (if required)
	return m_Teams.size() - 1;
}

void SM_TeamsManager::UpdateAllTeams(GameTimer& gt)
{
	for (auto& a : m_Teams)
		a->UpdateUnits(gt);
}

void SM_TeamsManager::RenderAllTeams(DirectX::SpriteBatch& sb, DirectX::DescriptorHeap* heap)
{
	for (auto& a : m_Teams)
		a->RenderUnits(sb, heap);
}

Entity_GameUnit* SM_TeamsManager::SearchAllTeamsForUnitAtCoordinate(int x, int y, bool mustBeActive)
{
	//Cycle through each team
	for (auto& a : m_Teams)
	{
		//Search team for a unit at coords
		Entity_GameUnit* unit = a->GetUnitByCoordinate(x, y, mustBeActive);

		//If a unit is found, early out and return
		if (unit)
			return unit;
	}

	//No units found among all teams so return nullptr
	return nullptr;
}

void SM_TeamsManager::ProcessUnitDeath(Entity_GameUnit* deadUnit)
{
	unsigned index = 0;
	if (m_Teams[deadUnit->GetTeamID()]->FindUnitIndexByPointer(index, deadUnit))
	{
		//Disable Unit
		deadUnit->SetActiveState(false);
		deadUnit->SetRenderState(false);

		//Flag dead
		deadUnit->GetUnitData().m_Alive = false;

		//Update Team Stats
		m_Teams[deadUnit->GetTeamID()]->IncrementUnitsAlive(-1);

		//Update Tile
		Game::GetGame()->GetGameplayManager().GetTileManager().GetMapTileByCoordinate(
			static_cast<unsigned>(deadUnit->GetCoords().GetMapCoordinatesX()),
			static_cast<unsigned>(deadUnit->GetCoords().GetMapCoordinatesY())
		)->GetProperties().m_IsOccupied = false;

	}
	//Unit not found given its team index (this shouldn't happen)
	else
	{
		msg_assert(false, "Unit has team index but doesn't exist in team container");
	}
}

void SM_TeamsManager::EnableAllTeamsAllUnits()
{
	for (auto& a : m_Teams)
	{
		a->EnableAllUnits();
	}
}

void SM_TeamsManager::DisableAllTeamsAllUnits()
{
	for (auto& a : m_Teams)
	{
		a->DisableAllUnits();
	}
}

void SM_TeamsManager::ResetAllTeams()
{
	for (auto& a : m_Teams)
	{
		a->DisableAllUnits();
		a->ClearUnitsAliveCount();
	}
}

void SM_TeamsManager::SetTeamToNextTeam()
{
	//Get current current team as number and increment
	unsigned team = static_cast<unsigned>(m_ActiveTeam);
	++team;

	//Set to first team index if gone beyond team count, else set the new incremented index
	if (team >= static_cast<unsigned>(TeamIndexes::TEAM_COUNT))
		m_ActiveTeam = TeamIndexes::TEAM_00;
	else
		m_ActiveTeam = static_cast<TeamIndexes>(team);

}

void SM_TeamsManager::GameplayScenarioSetup_00()
{
	//Get Gameplay manager
	GameplayManager& gp = Game::GetGame()->GetGameplayManager();

	ResetAllTeams();

	//Place order: Assassin, Barbarian, Archer, Necromancer, Sorc, Warrior
	DirectX::XMINT2 coords[] = {
	//{7, 11}, {8, 14}, {12, 20}, {10, 31}, {17, 30}, {17, 20},
	//{40, 29}, {37, 26}, {38, 20}, {39, 12}, {34, 11}, {32, 20}
	{11, 21}, {11, 18}, {11, 19}, {8, 18}, {8, 21}, {13, 19},
	{38, 18}, {38, 21}, {38, 20}, {41, 21}, {41, 18}, {36, 20},
	};

	//Spawn new units in line with coordinates above
	for (unsigned i(0); i < GameUnitIDs::GU_COUNT; ++i)
	{
		gp.SpawnNewUnit(0, i, gp.GetTileManager().GetMapTileByCoordinate(coords[i].x, coords[i].y));
		gp.SpawnNewUnit(1, i, gp.GetTileManager().GetMapTileByCoordinate(coords[i + 6].x, coords[i + 6].y));
	}

	//Init AI For team one (Team Fuzzy Logic)
	m_Teams[0]->SetAIPilotFlag(true);
	for (unsigned i(0); i < m_Teams[0]->GetUnitContainerSize(); ++i)
	{
		//m_Teams[0]->GetUnitAtIndex(i)->InitialiseFLModule(AI_FP_00, 0);
		//m_Teams[0]->InitialiseFLModule(AI_FP_00, 1, false);
		
		m_Teams[0]->GetUnitAtIndex(i)->InitialiseActionAIModule();

		m_Teams[0]->GetUnitAtIndex(i)->InitialiseBTAIModule();
		m_Teams[0]->GetUnitAtIndex(i)->GetBTController()->CompileAlgorithm_00();
	}

	//Init AI for team two (Team Behaviour Trees)
	m_Teams[1]->SetAIPilotFlag(true);
	for (unsigned i(0); i < m_Teams[1]->GetUnitContainerSize(); ++i)
	{
		//m_Teams[1]->GetUnitAtIndex(i)->InitialiseFLModule(AI_FP_00, 0);
		//m_Teams[1]->InitialiseFLModule(AI_FP_00, 1, false);

		m_Teams[1]->GetUnitAtIndex(i)->InitialiseActionAIModule();

		m_Teams[1]->GetUnitAtIndex(i)->InitialiseBTAIModule();
		m_Teams[1]->GetUnitAtIndex(i)->GetBTController()->CompileAlgorithm_00();
	}
}

void SM_TeamsManager::Init()
{
	//Reserve space
	m_Teams.reserve(BASE_TEAM_COUNT);
	
	//Setup team data to starting specs
	for (unsigned i(0); i < BASE_TEAM_COUNT; ++i)
	{	
		m_Teams.push_back(new TeamEntry(i));
	}

}

void SM_TeamsManager::Release()
{
	for (auto& a : m_Teams)
	{
		delete a;
		a = nullptr;
	}

	m_Teams.clear();
}
