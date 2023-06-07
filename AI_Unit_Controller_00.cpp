#include "AI_Unit_Controller_00.h"	//Owner

#include "Game.h"			 //Manager Access
#include "Entity_GameUnit.h" //Game Unit


#include "GeneralUtils.h"	//Math Help
#include <algorithm>		//std::find
#include <random>			//std::random

AI_Unit_Algorithms_00::AI_Unit_Algorithms_00(Entity_GameUnit* unit)
{
	//Bind unit to AI module
	assert(unit);
	m_Unit = unit;

	Init();
}

void AI_Unit_Algorithms_00::BindToUnit(Entity_GameUnit* unit)
{
	assert(unit);
	if (m_Unit)
		msg_assert(false, "Attempting to bind AI module to another unit, intended?");
	m_Unit = unit;
}

void AI_Unit_Algorithms_00::Init()
{
	m_TargetManifest.reserve(10);
}

void AI_Unit_Algorithms_00::Release()
{
	m_TargetManifest.clear();
}


///////////////////////
/// Algorithm Trees ///
///////////////////////

ConditionCode AI_Unit_Algorithms_00::RunSearchAlgorithm(SearchAlgorithms algorithmID)
{
	//Pre-check unit is valid
	assert(m_Unit);

	//Clear Targets Manifest
	m_TargetManifest.clear();

	switch (algorithmID)
	{
	case SearchAlgorithms::FIND_ALL_ENEMIES:
		return FindAllEnemyTargets();
		break;

	case SearchAlgorithms::FIND_ALL_ADJACENT_ENEMIES:
		return FindAllAdjacentEnemyTargets();
		break;

	case SearchAlgorithms::FIND_FIRST_ENEMY:
		return ConditionCode::FAILED;
		break;
	case SearchAlgorithms::FIND_BEST_ENEMY_MATCH_UP:
		return ConditionCode::FAILED;
		break;

	case SearchAlgorithms::FIND_ALL_ENEMIES_OOR:
		return FindAllEnemyUnitsOOR();
		break;

	}

	//Switch didn't find algorithm to run
	msg_assert(false, "Search algorithm switch failed to find algorithm.");
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::RunMovementAlgorithm(MovementAlgorithms algorithmID)
{
	//Pre-check unit is valid
	assert(m_Unit);

	switch (algorithmID)
	{
	case MovementAlgorithms::BEST_DEFENSIVE_TILE:
		return MoveToBestDefensiveTile();
		break;

	case MovementAlgorithms::BEST_MOVE_COSTED_TILE:
		return ConditionCode::FAILED;
		break;

	case MovementAlgorithms::RANDOMIZED:
		return MoveToRandomTile();
		break;

	case MovementAlgorithms::RANDOM_ADJACENT_TILE_TARGET:
		return MoveToAnyAdjacentTile();
		break;

	case MovementAlgorithms::BEST_DEFENSIVE_ADJACENT:
		return MoveToBestDefensiveAdjacentTile();
		break;

	case MovementAlgorithms::MOVE_CLOSER_TO_TARGET:
		return MoveCloserToTarget();
		break;

	case MovementAlgorithms::MOVE_TO_FIRST_RANGE_TILE:
		return MoveToTargetFirstRangedTile();
		break;
	}

	//Incorrect or In-complete algorithm ran, return false
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::RunAttackAlgorithm(AttackAlgorithms algorithmID)
{
	//Pre-check unit is valid
	assert(m_Unit);

	switch (algorithmID)
	{
	case AttackAlgorithms::ATTACK_FIRST_ADJACENT:
		return AttackFirstAdjacentEnemy();
		break;

	case AttackAlgorithms::ATTACK_TARGET_ADJACENT:
		return AttackAdjacentTargetEnemy();
		break;

	case AttackAlgorithms::ATTACK_FIRST_VALID_ENEMY:
		return AttackFirstValidEnemy();
		break;

	case AttackAlgorithms::ATTACK_UNIT:
		return AttackUnit();
		break;
	}

	//Incorrect or In-complete algorithm ran, return false
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::RunTargetAlgorithm(TargetAlgorithms algorithmID)
{
	//Pre-check that there are targets to select from
	if (m_TargetManifest.size() == 0)
	{
		msg_assert(false, "ActionAI: No targets to source from. Intended?");
		return ConditionCode::FAILED;
	}

	switch (algorithmID)
	{
	case TargetAlgorithms::RANDOM_ENEMY:
		return SelectRandomTarget();
		break;

	case TargetAlgorithms::CLOSEST_ENEMY:
		return SelectClosestTarget();
		break;

	case TargetAlgorithms::FURTHEST_ENEMY:
		return SelectFurthestTarget();
		break;
	}


	//Incorrect or in-complete algorithm ran, return false
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::RunBTSupportAlgorithm(BTSupportAlgorithms algorithmID)
{
	switch (algorithmID)
	{
	case BTSupportAlgorithms::WAIT_TIMER_00:
		return RunTimer00();
		break;

	case BTSupportAlgorithms::RANGE_GREATER_THAN_ONE:
		return RangeGreaterOne();
		break;
	}

	//Should not hit this
	return ConditionCode::NO_OPERATION;
}


/////////////////////////
/// Search Algorithms ///
/////////////////////////

ConditionCode AI_Unit_Algorithms_00::FindAllEnemyTargets()
{
	//Get required managers for brevity
	GameplayManager& gm = Game::GetGame()->GetGameplayManager();
	MT_Pathfinder_00& pf = gm.GetPathfinder();

	//Run edge case check for immediately adjacent enemies first
	FindAllAdjacentEnemyTargets();

	//Run pathfinder algorith to get edge of effective range
	pf.GeneratePathfinderGrid(
		gm.GetTileManager().GetTileMap(),
		m_Unit,
		gm.GetTileManager().GetMapData().m_MapSize.x,
		BK_UI_Bundle_00_Desc::BK_UI_BLUE_TILE_HIGHLIGHT_00_48x48
	);

	//Setup iterator (due to manifest data format)
	auto it = pf.GetManifest().begin();

	//For each tile in the manifest
	for (unsigned i(0); i < pf.GetManifestSize(); ++i)
	{
		//For each neighbour tile on each manifest entry
		for (unsigned j(0); j < TNIndexes::TNI_COUNT / 2; ++j)
		{
			//Grab current neighbour
			auto nb = (*it)->GetNeighbourTileAtIndex(j * 2);

			//Does neighbour exist, is it not in the manifest, and is it occupied
			if (nb && !pf.IsTileInManifest(nb) && nb->GetProperties().m_IsOccupied)
			{
				//Find the unit at coordinates
				auto unit = gm.GetTeamsManager().SearchAllTeamsForUnitAtCoordinate(
					nb->GetCoordinates().GetMapCoordinatesXi(),
					nb->GetCoordinates().GetMapCoordinatesYi(),
					true
				);

				assert(unit);

				//Check unit affiliation + manifest status
				AddEnemyToManifestCheck(unit);
			}
		}

		//Advance iterator
		std::advance(it, 1);
	}

	//Release pathfinder manifest
	pf.ReleaseManifest();

	//Were any enemies found?
	if (m_TargetManifest.empty())
	{
		return ConditionCode::FAILED;
	}
	else
	{
		//Set primary target to first index for safety (run target algorithm)
		m_PrimaryTargetIndex = 0;
		return ConditionCode::SUCCESS;
	}

	//This shouldn't be hit.
	assert(false);
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::FindAllAdjacentEnemyTargets()
{
	//Get required managers for brevity
	GameplayManager& gm = Game::GetGame()->GetGameplayManager();

	//Get units tile
	auto ut = gm.GetTileManager().GetMapTileByCoordinate(
		m_Unit->GetCoords().GetMapCoordinatesXi(),
		m_Unit->GetCoords().GetMapCoordinatesYi()
	);

	for (unsigned i(0); i < TNIndexes::TNI_COUNT / 2; ++i)
	{
		//Get current neighbour
		auto nb = ut->GetNeighbourTileAtIndex(i * 2);

		if (nb && nb->GetProperties().m_IsOccupied)
		{
			//Find the unit at coordinates
			auto unit = gm.GetTeamsManager().SearchAllTeamsForUnitAtCoordinate(
				nb->GetCoordinates().GetMapCoordinatesXi(),
				nb->GetCoordinates().GetMapCoordinatesYi(),
				true
			);

			assert(unit);

			//Check unit affiliation + manifest status
			AddEnemyToManifestCheck(unit);
		}
	}

	//Were any enemies found?
	if (m_TargetManifest.empty())
	{
		return ConditionCode::FAILED;
	}
	else
	{
		//Set primary target to first index for safety (run target algorithm)
		m_PrimaryTargetIndex = 0;
		return ConditionCode::SUCCESS;
	}

	//This shouldn't be hit.
	assert(false);
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::FindAllEnemyUnitsOOR()
{
	//Get required managers for brevity
	GameplayManager& gm = Game::GetGame()->GetGameplayManager();
	SM_TeamsManager& tm = Game::GetGame()->GetGameplayManager().GetTeamsManager();

	//Cycle through each enemy team
	for (unsigned i(0); i < tm.GetNumberOfTeams(); ++i)
	{
		//Is this team an enemy team?
		if (tm.GetTeamAtIndex(i)->GetTeamID() != m_Unit->GetTeamID())
		{
			for (unsigned j(0); j < tm.GetTeamAtIndex(i)->GetUnitContainerSize(); ++j)
			{
				if (tm.GetTeamAtIndex(i)->GetUnitAtIndex(j)->GetActiveState() &&
					tm.GetTeamAtIndex(i)->GetUnitAtIndex(j)->GetUnitData().m_Alive)
				{
					m_TargetManifest.push_back(tm.GetTeamAtIndex(i)->GetUnitAtIndex(j));
				}
			}
		}
	}
	
	//Flag tile rememberance
	m_RememberTargetTile = true;

	//Were any enemies found?
	if (m_TargetManifest.empty())
	{
		//No enemy team was found, which is highly unlikely and shouldn't happen
		msg_assert(m_TargetManifest.empty(), "FindAllEnemyUnitsOOR() Failed: No Enemy Teams Found");
		return ConditionCode::FAILED;
	}
	else
	{
		//Set primary target to first index for safety (run target algorithm)
		m_PrimaryTargetIndex = 0;
		return ConditionCode::SUCCESS;
	}		

	//This shouldn't be hit.
	assert(false);
	return ConditionCode::FAILED;
}


///////////////////////////
/// Movement Algorithms ///
///////////////////////////

ConditionCode AI_Unit_Algorithms_00::MoveToRandomTile()
{
	//Get required managers for brevity
	GameplayManager& gm = Game::GetGame()->GetGameplayManager();
	MT_Pathfinder_00& pf = gm.GetPathfinder();

	//Run pathfinder algorith to get edge of effective range
	pf.GeneratePathfinderGrid(
		gm.GetTileManager().GetTileMap(),
		m_Unit,
		gm.GetTileManager().GetMapData().m_MapSize.x,
		BK_UI_Bundle_00_Desc::BK_UI_BLUE_TILE_HIGHLIGHT_00_48x48
	);

	//Move Unit
	if (gm.MoveUnitToTile(
		m_Unit,
		pf.GetRandomTileInManifest(),
		gm.GetTileManager().GetMapTileByCoordinate(
			m_Unit->GetCoords().GetMapCoordinatesXi(),
			m_Unit->GetCoords().GetMapCoordinatesYi())
	))
	{
		//Release pathfinder manifest
		pf.ReleaseManifest();
		m_Unit->SetMoveFlag(false);
		//Unit moved, return true
		return ConditionCode::SUCCESS;
	}
	else
	{
		//Release pathfinder manifest
		pf.ReleaseManifest();
		//Unit unable to move
		return ConditionCode::FAILED;
	}

	//This shouldn't be hit.
	assert(false);
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::MoveToBestDefensiveTile()
{
	//Get required managers for brevity
	GameplayManager& gm = Game::GetGame()->GetGameplayManager();
	MT_Pathfinder_00& pf = gm.GetPathfinder();

	//Run pathfinder algorith to find full range of movement
	pf.GeneratePathfinderGrid(
		gm.GetTileManager().GetTileMap(),
		m_Unit,
		gm.GetTileManager().GetMapData().m_MapSize.x,
		BK_UI_Bundle_00_Desc::BK_UI_BLUE_TILE_HIGHLIGHT_00_48x48
	);

	//Setup iterator (due to manifest data format)
	auto it = pf.GetManifest().begin();

	//Hold the first tile in the manifest as the base line tile to compare against
	auto tile = *it;

	for (unsigned i(0); i < pf.GetManifestSize(); ++i)
	{
		//Compare current tiles defensive value against new tile, capturing new tile if better
		if ((*it)->GetProperties().m_DefensiveValue > tile->GetProperties().m_DefensiveValue)
			tile = *it;

		//Advance iterator
		std::advance(it, 1);
	}

	//Move Unit
	if (gm.MoveUnitToTile(
		m_Unit,
		tile,
		gm.GetTileManager().GetMapTileByCoordinate(
			m_Unit->GetCoords().GetMapCoordinatesXi(),
			m_Unit->GetCoords().GetMapCoordinatesYi())
	))
	{
		//Release pathfinder manifest
		pf.ReleaseManifest();
		//Unit moved, return true
		return ConditionCode::SUCCESS;
	}
	else
	{
		//Release pathfinder manifest
		pf.ReleaseManifest();
		//Unit unable to move
		return ConditionCode::FAILED;
	}

	//This shouldn't be hit.
	assert(false);
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::MoveToAnyAdjacentTile()
{
	//Get required managers for brevity
	GameplayManager& gm = Game::GetGame()->GetGameplayManager();
	MT_Pathfinder_00& pf = gm.GetPathfinder();

	//Run pathfinder algorith to find full range of movement
	pf.GeneratePathfinderGrid(
		gm.GetTileManager().GetTileMap(),
		m_Unit,
		gm.GetTileManager().GetMapData().m_MapSize.x,
		BK_UI_Bundle_00_Desc::BK_UI_BLUE_TILE_HIGHLIGHT_00_48x48
	);

	//Find primary targets tile
	auto et = gm.GetTileManager().GetMapTileByCoordinate(
		m_TargetManifest[m_PrimaryTargetIndex]->GetCoords().GetMapCoordinatesXi(),
		m_TargetManifest[m_PrimaryTargetIndex]->GetCoords().GetMapCoordinatesYi()
	);

	//Edge case check to see if already on adjacent tile
	auto ut = gm.GetTileManager().GetMapTileByCoordinate(
		m_Unit->GetCoords().GetMapCoordinatesXi(),
		m_Unit->GetCoords().GetMapCoordinatesYi()
	);

	//Start main algorithm

	//Create container to store possible candidates 
	std::vector<MT_Type1*> candidates;
	candidates.reserve(TNIndexes::TNI_COUNT / 2);
	//Gather candidates
	for (unsigned i(0); i < TNIndexes::TNI_COUNT / 2; ++i)
	{
		//Get neighbour
		auto nb = et->GetNeighbourTileAtIndex(i * 2);

		//If the neighbour exists, is in the manifest, move unit to new adjacent tile
		if (nb && pf.IsTileInManifest(nb))
		{
			//Store the candidate
			candidates.push_back(nb);
		}
	}

	//Edge case check, if only one tile in container then run move attempt on it
	if (candidates.size() == 1)
	{
		//Move Unit
		if (gm.MoveUnitToTile(
			m_Unit,
			candidates[0],
			gm.GetTileManager().GetMapTileByCoordinate(
				m_Unit->GetCoords().GetMapCoordinatesXi(),
				m_Unit->GetCoords().GetMapCoordinatesYi())
		))
		{
			//Release pathfinder manifest
			pf.ReleaseManifest();
			//Unit moved, return true
			return ConditionCode::SUCCESS;
		}
		else
		{
			//Release pathfinder manifest
			pf.ReleaseManifest();
			//Unit unable to move
			return ConditionCode::FAILED;
		}
	}
	//If candidates found, attempt to move to one of them
	else if(!candidates.empty())
	{
		//Random sort container
		auto rng = std::default_random_engine{};
		std::shuffle(std::begin(candidates), std::end(candidates), rng);

		//Cycle through each candidate and attempt to move
		for (auto& a : candidates)
		{
			//Attempt to move unit
			if (gm.MoveUnitToTile(
				m_Unit,
				a,
				gm.GetTileManager().GetMapTileByCoordinate(
					m_Unit->GetCoords().GetMapCoordinatesXi(),
					m_Unit->GetCoords().GetMapCoordinatesYi())
			))
			{
				//Release pathfinder manifest
				pf.ReleaseManifest();
				//Unit moved, return true
				return ConditionCode::SUCCESS;
			}
		}
	}

	//No candidate tiles found, exit out
	pf.ReleaseManifest();
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::MoveCloserToTarget()
{
	//Get required managers for brevity
	GameplayManager& gm = Game::GetGame()->GetGameplayManager();
	MT_Pathfinder_00& pf = gm.GetPathfinder();

	//Get units and enemy tile
	auto ut = gm.GetTileManager().GetMapTileByCoordinate(
		m_Unit->GetCoords().GetMapCoordinatesXi(),
		m_Unit->GetCoords().GetMapCoordinatesYi()
	);
	auto et = gm.GetTileManager().GetMapTileByCoordinate(
		m_TargetManifest[m_PrimaryTargetIndex]->GetCoords().GetMapCoordinatesXi(),
		m_TargetManifest[m_PrimaryTargetIndex]->GetCoords().GetMapCoordinatesYi()
	);

	msg_assert(ut, "MoveCloserToTarget(): Unit Tile Not Found");
	msg_assert(et, "MoveCloserToTarget(): Enemy Tile Not Found");

	//Run main pathfinder
	pf.GeneratePathfinderGrid(
		gm.GetTileManager().GetTileMap(),
		m_Unit,
		gm.GetTileManager().GetMapData().m_MapSize.x,
		BK_UI_Bundle_00_Desc::BK_UI_BLUE_TILE_HIGHLIGHT_00_48x48
	);

	//If manifest is empty, then unit cannot move so just exit
	if (pf.GetManifest().empty())
		return ConditionCode::FAILED;

	//Run A* algorithm to build path between target and unit tiles
	pf.FindShortestRouteUnrestricted(
		ut,
		et,
		BK_UI_Bundle_00_Desc::BK_UI_RED_TILE_HIGHLIGHT_00_48x48,
		TileSpriteIndexes::TS_HIGHLIGHT_TILE_00,
		true
	);

	//Get first tile in A* route the overlaps with manifest
	auto nt = pf.GetFirstOverlappingTile();

	//Clear manifest and pathfinder grid
	pf.ReleaseManifest();
	pf.DisablePath();

	//If a tile was found
	if (nt)
	{
		//Return true if move to tile was successful
		if (gm.MoveUnitToTile(m_Unit, nt, ut))
			return ConditionCode::SUCCESS;
		else
		{
			msg_assert(false, "MoveToCloserTarget Failed: Found Tile But Failed To Move");
			return ConditionCode::FAILED;
		}
	}

	//Unable to find path or tile 
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::MoveToTargetFirstRangedTile()
{
	//Get required managers for brevity
	GameplayManager& gm = Game::GetGame()->GetGameplayManager();
	MT_Pathfinder_00& pf = gm.GetPathfinder();

	//Get units and enemy tile
	auto ut = gm.GetTileManager().GetMapTileByCoordinate(
		m_Unit->GetCoords().GetMapCoordinatesXi(),
		m_Unit->GetCoords().GetMapCoordinatesYi()
	);
	auto et = gm.GetTileManager().GetMapTileByCoordinate(
		m_TargetManifest[m_PrimaryTargetIndex]->GetCoords().GetMapCoordinatesXi(),
		m_TargetManifest[m_PrimaryTargetIndex]->GetCoords().GetMapCoordinatesYi()
	);

	msg_assert(ut, "MoveToTargetRanged(): Unit Tile Not Found");
	msg_assert(et, "MoveToTargetRanged(): Enemy Tile Not Found");

	//Run main pathfinder
	pf.GeneratePathfinderGrid(
		gm.GetTileManager().GetTileMap(),
		m_Unit,
		gm.GetTileManager().GetMapData().m_MapSize.x,
		BK_UI_Bundle_00_Desc::BK_UI_BLUE_TILE_HIGHLIGHT_00_48x48
	);
	
	//Search through pathfinder manifest and find a tile that is at range from target tile

	//Setup iterator (due to manifest data format)
	auto it = pf.GetManifest().begin();

	for (unsigned i(0); i < pf.GetManifest().size(); ++i)
	{
		//Calculate distance between target unit and manifest tile being tested
		int dist = GetDistanceBetweenCoordinates(
			(*it)->GetCoordinates().GetMapCoordinatesXi(),
			(*it)->GetCoordinates().GetMapCoordinatesYi(),
			et->GetCoordinates().GetMapCoordinatesXi(),
			et->GetCoordinates().GetMapCoordinatesYi()
		);

		//If the tile not adjacent to the unit, and not out of range
		if (dist > 1 && dist <= m_Unit->GetUnitData().m_AttackRange)
		{
			//Move unit to target tile
			if (gm.MoveUnitToTile(
				m_Unit,
				(*it),
				gm.GetTileManager().GetMapTileByCoordinate(
					m_Unit->GetCoords().GetMapCoordinatesXi(),
					m_Unit->GetCoords().GetMapCoordinatesYi())
			))
			{
				//Release pathfinder manifest
				pf.ReleaseManifest();
				//Unit moved, return true
				return ConditionCode::SUCCESS;
			}
			else
			{
				//Release pathfinder manifest
				pf.ReleaseManifest();
				//Unit failed to move, return false
				return ConditionCode::FAILED;
			}
		}
		else
		{
			//Advance iterator
			std::advance(it, 1);
		}
	}

	pf.ReleaseManifest();
	//Failed to find any appropriate tile, return false
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::MoveToBestDefensiveAdjacentTile()
{
	//Get required managers for brevity
	GameplayManager& gm = Game::GetGame()->GetGameplayManager();
	MT_Pathfinder_00& pf = gm.GetPathfinder();

	//Run pathfinder algorith to find full range of movement
	pf.GeneratePathfinderGrid(
		gm.GetTileManager().GetTileMap(),
		m_Unit,
		gm.GetTileManager().GetMapData().m_MapSize.x,
		BK_UI_Bundle_00_Desc::BK_UI_BLUE_TILE_HIGHLIGHT_00_48x48
	);

	//Find primary targets tile
	auto et = gm.GetTileManager().GetMapTileByCoordinate(
		m_TargetManifest[m_PrimaryTargetIndex]->GetCoords().GetMapCoordinatesXi(),
		m_TargetManifest[m_PrimaryTargetIndex]->GetCoords().GetMapCoordinatesYi()
	);

	//Edge case check to see if already on adjacent tile
	auto ut = gm.GetTileManager().GetMapTileByCoordinate(
		m_Unit->GetCoords().GetMapCoordinatesXi(),
		m_Unit->GetCoords().GetMapCoordinatesYi()
	);

	for (unsigned i(0); i < TNIndexes::TNI_COUNT / 2; ++i)
	{
		//Is this neighbour tile the enemy tile?
		if (ut->GetNeighbourTileAtIndex(i * 2) == et)
		{
			//Release pathfinder manifest
			pf.ReleaseManifest();
			//Skip out early
			return ConditionCode::SUCCESS;
		}
	}

	//Start main algorithm
	 
	//Create container to store possible candidates 
	std::vector<MT_Type1*> candidates;
	candidates.reserve(TNIndexes::TNI_COUNT / 2);
	//Gather candidates
	for (unsigned i(0); i < TNIndexes::TNI_COUNT / 2; ++i)
	{
		//Get neighbour
		auto nb = et->GetNeighbourTileAtIndex(i * 2);

		//If the neighbour exists, is in the manifest, move unit to new adjacent tile
		if (nb && pf.IsTileInManifest(nb))
		{
			//Store the candidate
			candidates.push_back(nb);
		}
	}

	//Edge case check, if only one tile in container then run move attempt on it
	if (candidates.size() == 1)
	{
		//Move Unit
		if (gm.MoveUnitToTile(
			m_Unit,
			candidates[0],
			gm.GetTileManager().GetMapTileByCoordinate(
				m_Unit->GetCoords().GetMapCoordinatesXi(),
				m_Unit->GetCoords().GetMapCoordinatesYi())
		))
		{
			//Release pathfinder manifest
			pf.ReleaseManifest();
			//Unit moved, return true
			return ConditionCode::SUCCESS;
		}
		else
		{
			//Release pathfinder manifest
			pf.ReleaseManifest();
			//Unit unable to move
			return ConditionCode::FAILED;
		}
	}
	//Store container by best defensive tile value and then attempt to move to each till successful or container exhausted
	else
	{
		//Sort container
		std::sort(candidates.begin(), candidates.end(), [](MT_Type1*& lhs, MT_Type1*& rhs)->bool
		{
			return lhs->GetProperties().m_DefensiveValue > rhs->GetProperties().m_DefensiveValue;
		});

		//Cycle through each candidate and attempt to move
		for (auto& a : candidates)
		{
			//Attempt to move unit
			if (gm.MoveUnitToTile(
				m_Unit,
				a,
				gm.GetTileManager().GetMapTileByCoordinate(
					m_Unit->GetCoords().GetMapCoordinatesXi(),
					m_Unit->GetCoords().GetMapCoordinatesYi())
			))
			{
				//Release pathfinder manifest
				pf.ReleaseManifest();
				//Unit moved, return true
				return ConditionCode::SUCCESS;
			}
		}

		//Move attempts failed
		pf.ReleaseManifest();
		//Unit unable to move
		return ConditionCode::FAILED;
	}

	//Shouldn't be hit
	assert(false);
	return ConditionCode::CONDITION_ERROR;
}


/////////////////////////
/// Attack Algorithms ///
/////////////////////////

ConditionCode AI_Unit_Algorithms_00::AttackAdjacentTargetEnemy()
{
	//Get the distance between target and unit
	int dist = GetDistanceToUnit(m_TargetManifest[m_PrimaryTargetIndex]);

	//If the dist is not equal to 1, then the unit is too far away (not adjacent), or somehow on top of the unit (which shouldn't happen)
	if (dist == 1)
	{
		//Get gameplay manager for brevity
		GameplayManager& gm = Game::GetGame()->GetGameplayManager();

		//Run combat simulation
		gm.GetCombatManager().RunCombatSimulation(
			m_Unit,
			m_TargetManifest[m_PrimaryTargetIndex],
			gm.GetTileManager().GetMapTileByCoordinate(
				m_Unit->GetCoords().GetMapCoordinatesXi(),
				m_Unit->GetCoords().GetMapCoordinatesYi()),
			gm.GetTileManager().GetMapTileByCoordinate(
				m_TargetManifest[m_PrimaryTargetIndex]->GetCoords().GetMapCoordinatesXi(),
				m_TargetManifest[m_PrimaryTargetIndex]->GetCoords().GetMapCoordinatesYi())
		);

		//Return true as combat resolved correctly
		return ConditionCode::SUCCESS;
	}

	//Distance was not equal to 1, so attack could not occur
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::AttackFirstAdjacentEnemy()
{
	//Get gameplay manager for brevity
	GameplayManager& gm = Game::GetGame()->GetGameplayManager();

	//Get the units tile
	auto ut = gm.GetTileManager().GetMapTileByCoordinate(
		m_Unit->GetCoords().GetMapCoordinatesXi(),
		m_Unit->GetCoords().GetMapCoordinatesYi()
	);

	//For each neighbour the units tile has
	for (unsigned i(0); i < TNIndexes::TNI_COUNT / 2; ++i)
	{
		//Get current neighbour
		auto nb = ut->GetNeighbourTileAtIndex(i * 2);

		//Is there a neighbour, and is it occupied
		if (nb && nb->GetProperties().m_IsOccupied)
		{
			//Find the unit at coordinates
			auto unit = gm.GetTeamsManager().SearchAllTeamsForUnitAtCoordinate(
				nb->GetCoordinates().GetMapCoordinatesXi(),
				nb->GetCoordinates().GetMapCoordinatesYi(),
				true
			);

			//If the unit isn't friendly, run attack
			if (unit->GetTeamID() != m_Unit->GetTeamID())
			{
				//Run combat simulation
				gm.GetCombatManager().RunCombatSimulation(
					m_Unit,
					unit,
					ut,
					nb
				);

				//Unit was attacked, return true
				return ConditionCode::SUCCESS;
			}
		}
	}

	//No valid unit was found to attack, return false
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::AttackUnit()
{
	//Get gameplay manager for brevity
	GameplayManager& gm = Game::GetGame()->GetGameplayManager();

	//Get the unit and enemy tile
	auto ut = gm.GetTileManager().GetMapTileByCoordinate(
		m_Unit->GetCoords().GetMapCoordinatesXi(),
		m_Unit->GetCoords().GetMapCoordinatesYi()
	);
	auto et = gm.GetTileManager().GetMapTileByCoordinate(
		m_TargetManifest[m_PrimaryTargetIndex]->GetCoords().GetMapCoordinatesXi(),
		m_TargetManifest[m_PrimaryTargetIndex]->GetCoords().GetMapCoordinatesYi()
	);

	//Run targeting algorithm
	gm.GetTargetingSystem().RunTargetingAlgorithm(
		MT_TargetingManager::TYPE_DIAMOND_00,
		gm.GetTileManager().GetTileMap(),
		gm.GetTileManager().GetMapData().m_MapSize.x,
		m_Unit->GetCoords().GetMapCoordinatesXY(),
		BK_UI_Bundle_00_Desc::BK_UI_RED_TILE_HIGHLIGHT_00_48x48,
		m_Unit->GetUnitData().m_AttackRange
	);

	//Check that target is in range
	if (gm.GetTargetingSystem().IsTileInManifest(et))
	{
		//Run combat simulation
		gm.GetCombatManager().RunCombatSimulation(
			m_Unit,
			m_TargetManifest[m_PrimaryTargetIndex],
			ut,
			et
		);

		//Clear target manifest
		gm.GetTargetingSystem().ReleaseManifest();

		//Unit was attacked, return true
		return ConditionCode::SUCCESS;
	}

	//Clear target manifest
	gm.GetTargetingSystem().ReleaseManifest();

	//Unit was out of range, return false
	return ConditionCode::FAILED;
}

ConditionCode AI_Unit_Algorithms_00::AttackFirstValidEnemy()
{
	//Get gameplay manager for brevity
	GameplayManager& gm = Game::GetGame()->GetGameplayManager();

	//Get the units tile
	auto ut = gm.GetTileManager().GetMapTileByCoordinate(
		m_Unit->GetCoords().GetMapCoordinatesXi(),
		m_Unit->GetCoords().GetMapCoordinatesYi()
	);

	//Run targeting algorithm
	gm.GetTargetingSystem().RunTargetingAlgorithm(
		MT_TargetingManager::TYPE_DIAMOND_00,
		gm.GetTileManager().GetTileMap(),
		gm.GetTileManager().GetMapData().m_MapSize.x,
		m_Unit->GetCoords().GetMapCoordinatesXY(),
		BK_UI_Bundle_00_Desc::BK_UI_RED_TILE_HIGHLIGHT_00_48x48,
		m_Unit->GetUnitData().m_AttackRange
	);

	//Cross reference each known unit against target manifest and attack the first valid unit (if found)
	for (auto& a : m_TargetManifest)
	{
		//Get tile for current index
		auto et = gm.GetTileManager().GetMapTileByCoordinate(
			a->GetCoords().GetMapCoordinatesXi(),
			a->GetCoords().GetMapCoordinatesYi()
		);

		if (gm.GetTargetingSystem().IsTileInManifest(et) && a->GetTeamID() != m_Unit->GetTeamID())
		{
			//Run combat simulation
			gm.GetCombatManager().RunCombatSimulation(
				m_Unit,
				a,
				ut,
				et
			);

			//Clear target manifest
			gm.GetTargetingSystem().ReleaseManifest();

			//Unit was attacked, return true
			return ConditionCode::SUCCESS;
		}
	}

	//Clear target manifest
	gm.GetTargetingSystem().ReleaseManifest();

	//No units in range
	return ConditionCode::FAILED;
}

/////////////////////////
/// Target Algorithms ///
/////////////////////////

ConditionCode AI_Unit_Algorithms_00::SelectRandomTarget()
{
	//Set random index from range of targets
	std::srand((unsigned)time(NULL));
	m_PrimaryTargetIndex = std::rand() % m_TargetManifest.size();

	return ConditionCode::SUCCESS;
}

ConditionCode AI_Unit_Algorithms_00::SelectClosestTarget()
{
	//Use first index as basis
	m_PrimaryTargetIndex = 0;

	//Calculate total distance in tiles and keep track of the best indexed unit that meets criteria
	for (unsigned i(1); i < m_TargetManifest.size(); ++i)
	{
		//Get distances between the unit and current target index and index being evaluated
		int d1 = GetDistanceToUnit(m_TargetManifest[m_PrimaryTargetIndex]);
		int d2 = GetDistanceToUnit(m_TargetManifest[i]);

		//If the new distance is less than the currently held distance, set new index
		if (d2 < d1)
			m_PrimaryTargetIndex = i;
	}
	
	return ConditionCode::SUCCESS;
}

ConditionCode AI_Unit_Algorithms_00::SelectFurthestTarget()
{
	//Use first index as basis
	m_PrimaryTargetIndex = 0;

	//Calculate total distance in tiles and keep track of the best indexed unit that meets criteria
	for (unsigned i(1); i < m_TargetManifest.size(); ++i)
	{
		//Get distances between the unit and current target index and index being evaluated
		int d1 = GetDistanceToUnit(m_TargetManifest[m_PrimaryTargetIndex]);
		int d2 = GetDistanceToUnit(m_TargetManifest[i]);

		//If the new distance is greater than the currently held distance, set new index
		if (d2 > d1)
			m_PrimaryTargetIndex = i;
	}

	return ConditionCode::SUCCESS;
}

/////////////////////////////
/// BT Support Algorithms ///
/////////////////////////////

ConditionCode AI_Unit_Algorithms_00::RunTimer00()
{
	//Increment timer
	m_Elapsed += Game::GetGame()->GetGameTime().DeltaTime();

	if (m_Elapsed >= m_TargetTime)
	{
		//Reset clock and return success
		m_Elapsed = 0.f;
		return ConditionCode::SUCCESS;
	}
	else
		return ConditionCode::RUNNING;

	//Shouldn't hit this
	return ConditionCode();
}

ConditionCode AI_Unit_Algorithms_00::RangeGreaterOne()
{
	if (m_Unit->GetUnitData().m_AttackRange > 1)
		return ConditionCode::SUCCESS;
	else
		return ConditionCode::FAILED;

	return ConditionCode();
}


////////////////////////////////
/// Algorithm Supports/Other ///
////////////////////////////////

ConditionCode AI_Unit_Algorithms_00::AddEnemyToManifestCheck(Entity_GameUnit*& unit)
{
	if ((unit->GetTeamID() != m_Unit->GetTeamID()) && (!(std::find(m_TargetManifest.begin(), m_TargetManifest.end(), unit) != m_TargetManifest.end())))
	{
		//Add unit and return true
		m_TargetManifest.push_back(unit);
		return ConditionCode::SUCCESS;
	}

	//Unit not added, return false
	return ConditionCode::FAILED;
}

int AI_Unit_Algorithms_00::GetDistanceToUnit(Entity_GameUnit* unit)
{
	//Get distance between current, best index and unit
	int dX = m_Unit->GetCoords().GetMapCoordinatesXi() - unit->GetCoords().GetMapCoordinatesXi();
	int dY = m_Unit->GetCoords().GetMapCoordinatesYi() - unit->GetCoords().GetMapCoordinatesYi();

	//Flip values to positive if required
	InvertIfNegative(dX);
	InvertIfNegative(dY);

	return (dX + dY);
}

int AI_Unit_Algorithms_00::GetDistanceBetweenCoordinates(int x1, int y1, int x2, int y2)
{
	int dX = x1 - x2;
	int dY = y1 - y2;

	//Flip values to positive if required
	InvertIfNegative(dX);
	InvertIfNegative(dY);

	return (dX + dY);
}


