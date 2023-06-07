#include "SM_ScenarioManager.h"

#include "Game.h"		//Manager Access

bool SM_ScenarioManager::UpdateScenarioManager()
{
	if (m_ScenarioConditionCheck)
	{
		//Disable flag
		m_ScenarioConditionCheck = false;

		//Check what scenario to check against
		switch (m_Scenario)
		{
		case ScenarioStates::CAPTURE_HQ:			
			return CaptureHQConditionCheck();
			break;

		case ScenarioStates::ELIMINATION:			
			return EliminationConditionCheck();
			break;
		
		default:
			DBOUT("No Scenario Active, Is This Intended?")
		}
	}

	//Scenario check not active
	return false;
}

void SM_ScenarioManager::Init()
{

}

void SM_ScenarioManager::Release()
{

}

bool SM_ScenarioManager::CaptureHQConditionCheck()
{
	return false;
}

bool SM_ScenarioManager::EliminationConditionCheck()
{
	//Get Gameplay Manager for brevity
	SM_TeamsManager& tm = Game::GetGame()->GetGameplayManager().GetTeamsManager();

	//Create some holding flags tracking if a team has been eliminated
	std::vector<bool> elimFlags;
	elimFlags.insert(elimFlags.begin(), tm.GetNumberOfTeams(), false);

	//Check team one units for any alive units
	for (unsigned i(0); i < tm.GetNumberOfTeams(); ++i)
	{
		//If the team has no alive units
		if (tm.GetTeamAtIndex(i)->GetUnitsAliveCount() <= 0)
		{
			elimFlags[i] = true;
			tm.GetTeamAtIndex(i)->SetEliminationFlag(true);
		}
	}

	//Check now if any team was flagged as eliminated and return true if any was eliminated
	for (unsigned i(0); i < elimFlags.size(); ++i)
	{
		if (elimFlags[i])
		{
			//Update Game State appropriately
			Game::GetGame()->GetGameplayManager().GetStateManager().SetActiveGameState(SM_GameStateManager::ActiveGameState::WIN_CON_ELIMINATION);

			int count = 0;
			for (auto& a : Game::GetGame()->GetGameplayManager().GetTileManager().GetTileMap())
			{
				if (a->GetProperties().m_IsOccupied)
					++count;
			}
			DBOUT("Occupied Tile Count At End of Game: " << count);
			
			return true;
		}
	}

	//No teams were eliminated, so return false
	return false;
}
