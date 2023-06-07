#pragma once
#include "ModeInterface.h"	//Parent

//Entities

//Managers
#include "GameplayManager.h"

//Uis
#include "UI_GameTooltip.h" 
#include "UI_UnitFrame.h"
#include "UI_UnitActionMenu.h"
#include "UI_UnitInfoWindow.h"
#include "UI_GameMasterMenu.h"
#include "UI_ScenarioEndMenu.h"

/*
	Main game state that the game will be primarily played in.
*/
class Mode_MainGame : public ModeInterface
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////

	enum FixedUIDesc
	{

	};

	enum NonFixedUIDesc
	{

	};

	////////////////////
	/// Constructors ///
	////////////////////

	Mode_MainGame(size_t id);
	~Mode_MainGame() { Release(); }

	/////////////////
	/// Overrides ///
	/////////////////

	void Enter() override;
	bool Exit() override;
	void Update(GameTimer& gt) override;
	void Render(PassData& data) override;
	void ProcessKey(char key) override;
	void Release() override;

	//////////////////
	/// Operations ///
	//////////////////


	///////////
	/// Get ///
	///////////


	///////////
	/// Set ///
	///////////


private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();


	//
	// Cursor Calls
	//

	//Wraps up moving the cursor, checking the scene and reading for tiles and units all in one place
	void MoveCursorUpdate(Directional4Way direction, Entity_TileCursor::EntityIndexes entitySlot, Entity_TileCursor::TileIndexes tileSlot);
	//Wraps up snapping the cursor back to a point, re-reading the tile and releasing excess information (2nd tile/unit etc)
	void SyncCursorToFirstUnit(bool releaseExtraObjects = true);
	//Wraps up determining if A* should run, and 
	void RunAStar();

	//
	// Composite Functions
	//


	////////////////////////
	/// State Operations ///
	////////////////////////

	//
	// State Changes
	//

	//Switch statements for 
	void ProcessStateChange();


	//
	// Process Key Updates
	//

	void PK_FreeLook(char key);
	void PK_UnitActionMenuOpen(char key);
	void PK_UnitMoveOpen(char key);
	void PK_UnitAttackOpen(char key);
	void PK_UnitInfoWindowOpen(char key);
	void PK_MasterMenuOpen(char key);
	void PK_ScenarioEnd(char key);


	//
	// State Composite Function Calls
	// 

	//Called for "Action" input in "Free Look"
	void ExecuteAction_FreeLook();

	//Called for "Move" return in "Unit Action Menu"
	void ExecuteAction_Menu_Move();
	//Called for "Attack" return in "Unit Action Menu"
	void ExecuteAction_Menu_Attack();

	//Called for "Action" input in "Unit Move"
	void ExecuteAction_UnitMove();

	//Called for "Action" input in "Unit Targeting"
	void ExecuteAction_UnitTargeting();

	//
	// Other
	//

	//Manages turn end tasks, switching teams etc
	void EndTurn();
	//Wraps up calls required for post manifest states (doesn't set a state)
	void ExitManifestStates();
	//Runs end of scenario setup
	void EndOfScenario();

	//Sets the game to a fresh playable state
	void FreshGameState();


	////////////
	/// Data ///
	////////////

	//Hold large, scene/background sprites here
	std::vector<EM_Sprite*> m_Backgrounds;

	//Hold reference to gameplay manager for quicker access
	GameplayManager* m_GPManager = nullptr;
	CS_Main_00* m_CS = nullptr;

	//////////
	// UIs ///
	//////////

	UI_GameMasterMenu m_MasterMenu;
	UI_GameTooltip m_Tooltip;
	UI_UnitFrame m_UnitFrame;
	UI_UnitActionMenu m_UnitActionMenu;
	UI_UnitInfoWindow m_UnitInfoWindow;
	UI_ScenarioEndMenu m_ScenarioEndMenu;

	NonNavUI* m_MessageBox = nullptr;


	///////////////
	/// Testing ///
	///////////////

	bool m_QuickKill = false;
	bool m_RunOnceUIs = true;

};