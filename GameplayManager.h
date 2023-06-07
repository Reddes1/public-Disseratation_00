#pragma once

//Sub-Managers
#include "SM_TilemapManager.h"
#include "SM_UnitDataManager.h"
#include "SM_TeamsManager.h"
#include "SM_GameStateManager.h"
#include "SM_CombatManager.h"
#include "SM_ScenarioManager.h"

//Entities
#include "Entity_TileCursor.h"

//Other
#include "SceneCamera2D.h"
#include "CS_Main_00.h"				//Main Game Control Scheme
#include "MT_Pathfinder_00.h"		//Pathfinding functionality
#include "MT_TargetingManager.h"	//Target grid functionality
#include "SM_CombatLog.h"			//Output information to file




/*
	Centralises all gameplay orientated behaviours and modules together.
	If possible, any behaviour that can be wrapped up into tidy classes should be added here,
	and getters added to access them. This will effectively act as a hub type class, but
	specifically for gameplay related tasks and actions, without cluttering this hub.
*/
class GameplayManager
{
public:

	////////////
	/// Data ///
	////////////


	////////////////////
	/// Constructors ///
	////////////////////

	GameplayManager() {}
	~GameplayManager() {}

	//////////////////
	/// Operations ///
	//////////////////

	//
	// Composite Functions
	//

	/*
		Calls a number of member manager functions to load a level. Uses pre-loaded assets (found by name)
		and level file to construct level. Sets cursor to coordinates given, and configures camera accordingly.
	*/
	void LoadLevel(SM_TilemapManager::LoaderTypes loaderType, std::string& levelFP, std::string& tilesetName,
		DirectX::XMFLOAT2& cursorStartCoords, DirectX::XMINT2& sceneBuffer, int windowX, int windowY);
	/*
		Spawns a game unit at a given location, using team id and unit id to assign to the correct team
		and to spawn the correct type of unit as required. Pass the desired tile as well to update the 
		status of the tile.
	*/
	void SpawnNewUnit(unsigned teamID, unsigned unitID, MT_Type1* tile);
	/*
		Moves a game unit from one tile to another, updating each tile as required.
		This does not update the cursor position, nor sync the scene camera.
	*/
	bool MoveUnitToTile(Entity_GameUnit* unit, MT_Type1* newTile, MT_Type1* oldTile);
	
	//Syncs scene and cursor to a given unit.
	void SyncToUnit(Entity_GameUnit* const unit);


	///////////
	/// Get ///
	///////////

	SM_TilemapManager& GetTileManager()			 { return m_TileManager; }
	SM_UnitDataManager& GetUnitDataManager()	 { return m_UnitDataManager; }
	SM_TeamsManager& GetTeamsManager()			 { return m_TeamsManager; }
	SM_GameStateManager& GetStateManager()		 { return m_StateManager; }
	SM_CombatManager& GetCombatManager()		 { return m_CombatManager; }
	SM_ScenarioManager& GetScenarioManager()	 { return m_ScenarioManager; }

	Entity_TileCursor& GetTileCursor()			 { return m_TileCursor; }
	SceneCamera_2D& GetSceneCamera()			 { return m_SceneCamera; }
	CS_Interface& GetControlScheme()			 { return m_ControlScheme; }

	MT_Pathfinder_00& GetPathfinder()			 { return m_Pathfinder; }
	MT_TargetingManager& GetTargetingSystem()	 { return m_TargetingManager; }

	SM_CombatLog& GetCombatLog()				 { return m_CombatLog; }

	///////////
	/// Set ///
	///////////


private:

	//////////////////
	/// Operations ///
	//////////////////



	////////////
	/// Data ///
	////////////

	//Responsible for managing games tilemap
	SM_TilemapManager m_TileManager;
	
	//Responsible for loading and storing unit data
	SM_UnitDataManager m_UnitDataManager;

	//Responsible for managing the teams involved in the game.
	SM_TeamsManager m_TeamsManager;

	//Responsible for tracking and managing program/game state for use elsewhere
	SM_GameStateManager m_StateManager;

	//Responsible for manage damage calculations, and setting unit states post combat
	SM_CombatManager m_CombatManager;

	//Responsible for managing scenario requirements
	SM_ScenarioManager m_ScenarioManager;

	//Manages the movement of the game camera
	SceneCamera_2D m_SceneCamera;

	//Tile based cursor for use with tile & unit manager
	Entity_TileCursor m_TileCursor;

	//Main control scheme
	CS_Main_00 m_ControlScheme;

	//Used with TileManager to produce pathfinder information
	MT_Pathfinder_00 m_Pathfinder;

	//Responsible calculating and storing targetable area for units
	MT_TargetingManager m_TargetingManager;

	//Responsible for holding file open, outputing formatted message to file
	SM_CombatLog m_CombatLog;

};


