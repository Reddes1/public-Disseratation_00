#include "SM_CombatManager.h"		//Owner
#include "Game.h"					//Manager access

bool SM_CombatManager::RunCombatSimulation(Entity_GameUnit* unitOne, Entity_GameUnit* unitTwo, MT_Type1* tileOne, MT_Type1* tileTwo)
{
	//Get game 
	Game* game = Game::GetGame();

	//Get the damage modification for this exchange
	float unitOneDmgMod = game->GetGameplayManager().GetUnitDataManager().
		GetMatchupData(unitOne->GetUnitData().m_ID)->m_Data[unitTwo->GetUnitData().m_ID].m_DamageDoneMod;

	//Calculate damage
	float dmg = (unitOne->GetUnitData().m_Damage * unitOneDmgMod) * (1.f - ((tileTwo->GetProperties().m_DefensiveValue * DEF_PER_POINT_VALUE) / 100.f));

	//Apply damage to unit
	unitTwo->GetUnitData().m_Health -= dmg;

	//Update combatlog
	game->GetGameplayManager().GetCombatLog().LogDamageDealt_00(unitOne, unitTwo, dmg);

	//Check for health
	if (unitTwo->GetUnitData().m_Health <= 0.f)
	{
		//Contact teams manager with unit that has died
		game->GetGameplayManager().GetTeamsManager().ProcessUnitDeath(unitTwo);

		//Trigger update flag for scenario conditions
		game->GetGameplayManager().GetScenarioManager().SetConditionCheckFlag(true);

		//Update combatlog
		game->GetGameplayManager().GetCombatLog().LogUnitDeath_01(unitOne, unitTwo);

		//Return true for unit death
		return true;
	}

	//The unit didn't die, so return false
	return false;
}

float SM_CombatManager::GetSampleDamageCalculation(Entity_GameUnit* unitOne, Entity_GameUnit* unitTwo, MT_Type1* tileOne, MT_Type1* tileTwo)
{
	//Get unit match up data for this exchange
	float unitOneDmgMod = Game::GetGame()->GetGameplayManager().GetUnitDataManager().GetMatchupData(unitOne->GetUnitData().m_ID)
		->m_Data[unitTwo->GetUnitData().m_ID].m_DamageDoneMod;

	//Calculate damage
	float dmg = (unitOne->GetUnitData().m_Damage * unitOneDmgMod) * (1.f - ((tileTwo->GetProperties().m_DefensiveValue * DEF_PER_POINT_VALUE) / 100.f));

	return dmg;
}

void SM_CombatManager::Init()
{

}

void SM_CombatManager::Release()
{

}
