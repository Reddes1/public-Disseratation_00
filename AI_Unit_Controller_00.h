#pragma once

#include "GameConstants.h"		//Global Data
#include "AI_Definitions.h"		//Condition codes

//Forward Dec
class Entity_GameUnit;
class MT_Type1;

/*
	Functionality for autonomous AI actions + data storage for algorithm requirements.
	Uses information through Gameplay Manager + Sub-Managers for data and decision making.
*/
class AI_Unit_Algorithms_00
{
public:

	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////

	//Search Algorithms
	enum class SearchAlgorithms : unsigned
	{
		FIND_ALL_ENEMIES,
		FIND_ALL_ADJACENT_ENEMIES,
		FIND_FIRST_ENEMY,
		FIND_BEST_ENEMY_MATCH_UP,
		FIND_ALL_ENEMIES_OOR,
		COUNT
	};

	//Movement Algorithms
	enum class MovementAlgorithms : unsigned
	{
		BEST_DEFENSIVE_TILE,
		BEST_MOVE_COSTED_TILE,
		RANDOMIZED,
		RANDOM_ADJACENT_TILE_TARGET,
		BEST_DEFENSIVE_ADJACENT,
		MOVE_CLOSER_TO_TARGET,
		MOVE_TO_FIRST_RANGE_TILE,
		COUNT
	};

	//Attack Algorithms
	enum class AttackAlgorithms : unsigned
	{
		ATTACK_FIRST_ADJACENT,
		ATTACK_TARGET_ADJACENT,
		ATTACK_FIRST_VALID_ENEMY,
		ATTACK_UNIT,
		COUNT
	};

	//Target Selection Algorithms
	enum class TargetAlgorithms : unsigned
	{
		RANDOM_ENEMY,
		LOWEST_HP_ENEMY,
		HIGHEST_HP_ENEMY,
		LOWEST_DEFENSE_ENEMY,
		HIGHEST_DEFENSE_ENEMY,
		CLOSEST_ENEMY,
		FURTHEST_ENEMY,
		COUNT
	};

	//Behaviour Tree Orientated Algorithms
	enum class BTSupportAlgorithms : unsigned
	{
		WAIT_TIMER_00,
		RANGE_GREATER_THAN_ONE,
		COUNT
	};

	////////////////////
	/// Constructors ///
	////////////////////

	AI_Unit_Algorithms_00() { Init(); }
	AI_Unit_Algorithms_00(Entity_GameUnit* unit);
	~AI_Unit_Algorithms_00() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Run designated search algorithm (stored in container, specific targets accessible by index)
	//Returns true if algorithm successful. Clears any stored targets on function call).
	ConditionCode RunSearchAlgorithm(SearchAlgorithms algorithmID);

	//Run designated movement algorithm. Returns true if algorithm successfully moves the unit.
	//Disables units "CanMove" Flag
	ConditionCode RunMovementAlgorithm(MovementAlgorithms algorithmID);

	//Runs designated attack algorithm, using search algorithm target(s) as parameters.
	//Returns true if attack successfully executed. 
	ConditionCode RunAttackAlgorithm(AttackAlgorithms algorithmID);

	//Runs designated target algorithm, selecting a primary target using algorithm parameters
	//Selects from pool of targets found with Search Algorithm(s).
	//Returns false if no targets are available for sourcing (No search has been done)
	ConditionCode RunTargetAlgorithm(TargetAlgorithms algorithmID);

	//Runs designated BT support algorithm, providing support for BT algorithm (such as timers). 
	ConditionCode RunBTSupportAlgorithm(BTSupportAlgorithms algorithmID);

	//Clears the current target manifest
	void ClearTargetManifest() { m_TargetManifest.clear(); }
	//Clear 


	///////////
	/// Get ///
	///////////

	unsigned GetPrimaryTargetIndexValue()					 { return m_PrimaryTargetIndex; }
	Entity_GameUnit* GetPrimaryTargetUnit()					 { return m_TargetManifest[m_PrimaryTargetIndex]; }
	std::vector<Entity_GameUnit*>& GetTargetManifest()		 { return m_TargetManifest; }
	MT_Type1* GetTargetTile()								 { return m_TargetTile; }

	///////////
	/// Set ///
	///////////

	//Set unit pointer for AI to work with
	void BindToUnit(Entity_GameUnit* unit);


	////////////////////////////////
	/// Algorithm Supports/Other ///
	////////////////////////////////

	//Get the distance from the held unit and target unit
	int GetDistanceToUnit(Entity_GameUnit* unit);
	//Generalised distance function, get the distance (in tiles) between two locations
	int GetDistanceBetweenCoordinates(int x1, int y1, int x2, int y2);

private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();
	void Release();
	
	/////////////////////////
	/// Search Algorithms ///
	/////////////////////////

	//Finds all enemy units (units not sharing team ID) and stores them
	ConditionCode FindAllEnemyTargets();
	//Check units adjacent tiles for enemies (also serves as edge case function for other algorithms)
	ConditionCode FindAllAdjacentEnemyTargets();
	//Store each known enemy unit (regardless of range)
	ConditionCode FindAllEnemyUnitsOOR();

	///////////////////////////
	/// Movement Algorithms ///
	///////////////////////////
	
	//Moves to any random tile in range of unit
	ConditionCode MoveToRandomTile();
	//Moves the best tile in range the unit (comparison keeps current tile in tie-breakers)
	ConditionCode MoveToBestDefensiveTile();
	//Moves to the first free tile that is adjacent to primary target
	ConditionCode MoveToAnyAdjacentTile();
	//Moves closer to the target by running A* algorithm to find path, then backtracking the path
	//till tile inside the manifest is found.
	ConditionCode MoveCloserToTarget();
	//Moves the first found tile that is at range from the target (relative to the units max range)
	ConditionCode MoveToTargetFirstRangedTile();
	//Moves the best defensive tile next to target unit if possible, otherwise chosing first valid tile if only one found
	ConditionCode MoveToBestDefensiveAdjacentTile();

	/////////////////////////
	/// Attack Algorithms ///
	/////////////////////////

	//Attacks primary target enemy if adjacent
	ConditionCode AttackAdjacentTargetEnemy();
	//Attacks any adjacent enemy, regardless of target priority (N/E/S/W order check)
	ConditionCode AttackFirstAdjacentEnemy();
	//Attack the target unit if it is in range
	ConditionCode AttackUnit();
	//Attack first valid enemy unit in range of the unit (if able)
	ConditionCode AttackFirstValidEnemy();


	/////////////////////////
	/// Target Algorithms ///
	/////////////////////////

	//Select random target from manifest
	ConditionCode SelectRandomTarget();
	//Select closest target in manifest (tie breaks are determined by sort algorithm)
	ConditionCode SelectClosestTarget();
	//Select furthest target in manifest (tie breaks are determined by sort algorithm)
	ConditionCode SelectFurthestTarget();

	/////////////////////////////
	/// BT Support Algorithms ///
	/////////////////////////////

	ConditionCode RunTimer00();
	ConditionCode RangeGreaterOne();


	////////////////////////////////
	/// Algorithm Supports/Other ///
	////////////////////////////////

	//Checks unit isn't friendly, whether its in the manifest or not and adds if its not
	ConditionCode AddEnemyToManifestCheck(Entity_GameUnit*& unit);

	


	////////////
	/// Data ///
	////////////

	//Stores any valid targets found during search algorithms
	std::vector<Entity_GameUnit*> m_TargetManifest;

	//Stores the index of the "best" target found by search algorithm (lowest, best match-up etc)
	unsigned m_PrimaryTargetIndex = 0;
	//Keep track of tile location for distance targets (needed to disable A* path)
	MT_Type1* m_TargetTile = nullptr;

	//Bind this controller to a game unit
	Entity_GameUnit* m_Unit = nullptr;

	//Hold elapsed time and target time
	float m_Elapsed = 0.f;
	float m_TargetTime = 0.15f;

	//Uses in algorithms to flag that it needs to remember the target units tile
	bool m_RememberTargetTile = false;

};