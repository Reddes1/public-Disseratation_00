#include "Mode_MainGame.h"		//Owner

#include "Game.h"			//Manager Access

//Some common calls defined
#define SET_ACTIVE_GAME_STATE(state) m_GPManager->GetStateManager().SetActiveGameState(state) 
#define GET_INDEXED_TILE(index) m_GPManager->GetTileCursor().GetTileAtIndex(index)
#define GET_INDEXED_UNIT(index) m_GPManager->GetTileCursor().GetUnitAtIndex(index)

#define SET_ENABLED_STATE(ui, flag) ui.GetUIElement()->SetEnabledState(flag)
#define RENDER_UI(ui, sb, heap) ui.GetUIElement()->Render(sb, heap)

Mode_MainGame::Mode_MainGame(size_t id)
	:ModeInterface(id)
{
	Init();
}

void Mode_MainGame::Init()
{
	//Get Game
	Game* game = Game::GetGame();
	//Grab gameplay manager pointer
	m_GPManager = &game->GetGameplayManager();

	//Setup background
	m_Backgrounds.push_back(new EM_Sprite());
	m_Backgrounds[0]->SetTexture(DEFAULT_TEST_BG_TEXTURE_NAME, true);
	m_Backgrounds[0]->SetLayerDepth(DRAW_DEPTH_BACKGROUND_0);



	////////////
	/// Demo ///
	////////////

}

void Mode_MainGame::Release()
{
	ReleaseUIElement(m_MessageBox);

	//Release backgrounds
	for (auto& a : m_Backgrounds)
	{
		delete a;
		a = nullptr;
	}
}

void Mode_MainGame::Enter()
{
	if (m_RunOnceUIs)
	{
		//Setup Non-Navs
		m_Tooltip.SetupUIElement((int)NonNavUIDefinitions::TILE_TOOLTIP_00);
		m_UnitFrame.SetupUIElement((int)NonNavUIDefinitions::UNIT_FRAME_00);
		m_UnitInfoWindow.SetupUIElement((int)NonNavUIDefinitions::UNIT_INFO_WINDOW_00);

		//Setup Fixed-Navs
		m_UnitActionMenu.SetupUIElement((int)FixedIndexNavDefintions::UNIT_ACTION_MENU_00);
		m_MasterMenu.SetupUIElement((int)FixedIndexNavDefintions::GAME_MENU_MASTER_00);
		m_ScenarioEndMenu.SetupUIElement((int)FixedIndexNavDefintions::SCENARIO_OVER_MENU_00);

		//Setup Loose UI elements
		Game::GetGame()->GetUIManager().GetNonNavUIElementByTypeID(m_MessageBox, (int)NonNavUIDefinitions::MESSAGE_BOX_00);

		m_RunOnceUIs = false;
	}

	//Set UI Flags
	m_Tooltip.GetUIElement()->SetEnabledState(true);
	m_UnitFrame.GetUIElement()->SetEnabledState(false);
	m_UnitInfoWindow.GetUIElement()->SetEnabledState(false);
	m_MessageBox->SetEnabledState(false);

	m_UnitActionMenu.GetUIElement()->SetEnabledState(false);
	m_MasterMenu.GetUIElement()->SetEnabledState(false);
	m_ScenarioEndMenu.GetUIElement()->SetEnabledState(false);

	//Setup initial cursor state
	m_GPManager->GetTileCursor().MoveToCoordinate(DirectX::XMFLOAT2(25, 20));
	m_GPManager->GetSceneCamera().SyncSceneToCursor(DirectX::XMFLOAT2(25, 20));
	m_GPManager->GetTileCursor().ResetAndRescan();
	m_Tooltip.UpdateTooltipWithTile(GET_INDEXED_TILE(Entity_TileCursor::FIRST_TILE));
	m_UnitFrame.UpdateFrameWithUnit(GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET));

	//Set win condition for game
	m_GPManager->GetScenarioManager().SetActiveScenario(SM_ScenarioManager::ScenarioStates::ELIMINATION);
	//Set starting state
	SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::FREE_LOOK);
}

bool Mode_MainGame::Exit()
{
	return true;
}

void Mode_MainGame::Update(GameTimer& gt)
{
	//Check for a state change before updating frame
	if (m_GPManager->GetStateManager().CheckForActiveStateChange())
		//Check successful so manage transition
		ProcessStateChange();

	//Update team units
	m_GPManager->GetTeamsManager().UpdateAllTeams(gt);

	//Update AI for active team (If not in end state) - If returns true then AI cycle is complete so return true
	if(m_GPManager->GetStateManager().GetActiveGameState() != SM_GameStateManager::ActiveGameState::WIN_CON_ELIMINATION)
		if (m_GPManager->GetTeamsManager().GetTeamAtIndex(m_GPManager->GetTeamsManager().GetActiveTeamIndexAsNumber())->UpdateTeamAI(gt))
			EndTurn();

	//Update map
	m_GPManager->GetTileManager().Update(gt.DeltaTime());

	//Update scene
	m_GPManager->GetSceneCamera().UpdateSceneCamera();

	//Check for any scenario updates (and ending the game if conditions are met)
	m_GPManager->GetScenarioManager().UpdateScenarioManager();
}

void Mode_MainGame::Render(PassData& data)
{
	//Get Game
	Game* game = Game::GetGame();

	/////////////////////
	/// Render Pass 0 ///
	///  Backgrounds  ///
	/////////////////////

	//Start pass
	data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB]->Begin(
		game->GetCommandList().Get(),
		DirectX::SpriteSortMode_FrontToBack
	);

	//Render backgrounds
	for (auto& a : m_Backgrounds)
		a->Draw(*data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB], data.m_Heap);

	//End pass
	data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB]->End();

	/////////////////////
	/// Render Pass 1 ///
	///	    Scene     ///
	/////////////////////

	//Start pass
	data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB]->Begin(
		game->GetCommandList().Get(),
		DirectX::SpriteSortMode_FrontToBack,
		m_GPManager->GetSceneCamera().GetSceneTransform()
	);

	//Render game map
	m_GPManager->GetTileManager().Render(*data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB], data.m_Heap);
	//Render cursor
	m_GPManager->GetTileCursor().Render(*data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB], data.m_Heap);
	//Render team entities
	m_GPManager->GetTeamsManager().RenderAllTeams(*data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB], data.m_Heap);


	//End pass
	data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB]->End();

	/////////////////////
	/// Render Pass 2 ///
	///	     UIs      ///
	/////////////////////

	//Start pass
	data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB]->Begin(
		game->GetCommandList().Get(),
		DirectX::SpriteSortMode_FrontToBack
	);

	RENDER_UI(m_Tooltip, *data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB], data.m_Heap);
	RENDER_UI(m_UnitFrame, *data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB], data.m_Heap);
	RENDER_UI(m_UnitActionMenu, *data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB], data.m_Heap);
	RENDER_UI(m_UnitInfoWindow, *data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB], data.m_Heap);
	RENDER_UI(m_MasterMenu, *data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB], data.m_Heap);
	RENDER_UI(m_ScenarioEndMenu, *data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB], data.m_Heap);

	m_MessageBox->Render(*data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB], data.m_Heap);

	//End pass
	data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB]->End();

}

void Mode_MainGame::ProcessKey(char key)
{
	//Find out if the key is a registered action key, and get the index of that key and pass it where required
	unsigned option = m_GPManager->GetControlScheme().GetIndexOfMatchedKey(std::toupper(key));

	//Quick fix, converting OOR unsigned to char changes it to 0, causing false positives
	if (option == 0xFFFFFFFF)
		return;

	//Switch according to game state
	switch (m_GPManager->GetStateManager().GetActiveGameState())
	{
		///////////////////
		/// Game States ///
		///////////////////

	case SM_GameStateManager::ActiveGameState::WIN_CON_ELIMINATION:
		PK_ScenarioEnd(option);
		break;

		/////////////////////
		/// Cursor States ///
		/////////////////////

	case SM_GameStateManager::ActiveGameState::FREE_LOOK:
		PK_FreeLook(option);
		break;

		///////////////////
		/// Unit States ///
		///////////////////

	case SM_GameStateManager::ActiveGameState::UNIT_ACTION_MENU_OPEN:
		PK_UnitActionMenuOpen(option);
		break;
	case SM_GameStateManager::ActiveGameState::UNIT_INFO_WINDOW_OPEN:
		PK_UnitInfoWindowOpen(option);
		break;
	case SM_GameStateManager::ActiveGameState::GAME_MASTER_MENU_OPEN:
		PK_MasterMenuOpen(option);
		break;
	case SM_GameStateManager::ActiveGameState::UNIT_MOVING:
		PK_UnitMoveOpen(option);
		break;
	case SM_GameStateManager::ActiveGameState::UNIT_TARGETING:
		PK_UnitAttackOpen(option);
		break;
	}

}

void Mode_MainGame::ProcessStateChange()
{
	//////////////////////
	/// Previous State ///
	//////////////////////

	switch (m_GPManager->GetStateManager().GetPreviousActiveGameState())
	{
	case SM_GameStateManager::ActiveGameState::FREE_LOOK:
		break;
	case SM_GameStateManager::ActiveGameState::UNIT_ACTION_MENU_OPEN:
		break;
	case SM_GameStateManager::ActiveGameState::UNIT_MOVING:
		break;
	}


	/////////////////
	/// New State ///
	/////////////////

	//Using this section to enable/disable any UIs that shouldn't be active
	switch (m_GPManager->GetStateManager().GetActiveGameState())
	{
	case SM_GameStateManager::ActiveGameState::FREE_LOOK:

		m_MasterMenu.GetUIElement()->SetEnabledState(false);
		m_UnitActionMenu.GetUIElement()->SetEnabledState(false);
		m_UnitInfoWindow.GetUIElement()->SetEnabledState(false);


		m_GPManager->GetTileCursor().ResetAndRescan();
		m_Tooltip.UpdateTooltipWithTile(GET_INDEXED_TILE(Entity_TileCursor::FIRST_TILE));
		m_UnitFrame.UpdateFrameWithUnit(GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET));


		break;
	case SM_GameStateManager::ActiveGameState::UNIT_ACTION_MENU_OPEN:

		m_MasterMenu.GetUIElement()->SetEnabledState(false);
		m_UnitActionMenu.GetUIElement()->SetEnabledState(true);
		m_UnitInfoWindow.GetUIElement()->SetEnabledState(false);

		m_GPManager->GetTileCursor().ResetAndRescan();
		m_Tooltip.UpdateTooltipWithTile(GET_INDEXED_TILE(Entity_TileCursor::FIRST_TILE));
		m_UnitFrame.UpdateFrameWithUnit(GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET));

		m_UnitActionMenu.UpdateMenuWithUnit(GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET), true);

		break;
	case SM_GameStateManager::ActiveGameState::UNIT_INFO_WINDOW_OPEN:

		m_MasterMenu.GetUIElement()->SetEnabledState(false);
		m_UnitActionMenu.GetUIElement()->SetEnabledState(false);
		m_UnitInfoWindow.GetUIElement()->SetEnabledState(true);

		break;
	case SM_GameStateManager::ActiveGameState::GAME_MASTER_MENU_OPEN:

		m_MasterMenu.GetUIElement()->SetEnabledState(true);
		m_UnitActionMenu.GetUIElement()->SetEnabledState(false);
		m_UnitInfoWindow.GetUIElement()->SetEnabledState(false);

		break;
	case SM_GameStateManager::ActiveGameState::BUILDING_ACTION_MENU_OPEN:
		break;
	case SM_GameStateManager::ActiveGameState::UNIT_MOVING:

		m_MasterMenu.GetUIElement()->SetEnabledState(false);
		m_UnitActionMenu.GetUIElement()->SetEnabledState(false);
		m_UnitInfoWindow.GetUIElement()->SetEnabledState(false);

		m_GPManager->GetTileCursor().ResetAndRescan();
		m_Tooltip.UpdateTooltipWithTile(GET_INDEXED_TILE(Entity_TileCursor::FIRST_TILE));
		m_UnitFrame.UpdateFrameWithUnit(GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET));

		break;
	case SM_GameStateManager::ActiveGameState::UNIT_TARGETING:

		m_MasterMenu.GetUIElement()->SetEnabledState(false);
		m_UnitActionMenu.GetUIElement()->SetEnabledState(false);
		m_UnitInfoWindow.GetUIElement()->SetEnabledState(false);

		m_GPManager->GetTileCursor().ResetAndRescan();
		m_Tooltip.UpdateTooltipWithTile(GET_INDEXED_TILE(Entity_TileCursor::FIRST_TILE));
		m_UnitFrame.UpdateFrameWithUnit(GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET));

		break;
	case SM_GameStateManager::ActiveGameState::UNIT_COMBAT_START:
		break;
	case SM_GameStateManager::ActiveGameState::UNIT_COMBAT_END:
		break;


		///////////////////
		/// End of Game ///
		///////////////////


	case SM_GameStateManager::ActiveGameState::WIN_CON_ELIMINATION:
		EndOfScenario();
		break;
	}

	//Disable change flags
	m_GPManager->GetStateManager().DisabledActiveStateChangeFlag();
	m_GPManager->GetStateManager().DisablePrepStateChangeFlag();

}

void Mode_MainGame::PK_FreeLook(char key)
{
	switch (key)
	{
		//Action: Move Up
	case 0:
		MoveCursorUpdate(Directional4Way::UP,
			Entity_TileCursor::FIRST_TARGET,
			Entity_TileCursor::FIRST_TILE
		);
		break;
		//Action: Move Down
	case 1:
		MoveCursorUpdate(Directional4Way::DOWN,
			Entity_TileCursor::FIRST_TARGET,
			Entity_TileCursor::FIRST_TILE
		);
		break;
		//Action: Move Left
	case 2:
		MoveCursorUpdate(Directional4Way::LEFT,
			Entity_TileCursor::FIRST_TARGET,
			Entity_TileCursor::FIRST_TILE
		);
		break;
		//Action: Move Right
	case 3:
		MoveCursorUpdate(Directional4Way::RIGHT,
			Entity_TileCursor::FIRST_TARGET,
			Entity_TileCursor::FIRST_TILE
		);
		break;
		//Action: Confirm
	case 4:
		ExecuteAction_FreeLook();
		break;
		//Action: Back
	case 5:
		//Change State
		SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::GAME_MASTER_MENU_OPEN);
		break;
	}
}

void Mode_MainGame::PK_UnitActionMenuOpen(char key)
{
	//Menu control is done internally, but actionable data can come back so determine what to do
	switch (m_UnitActionMenu.ProcessInput(key))
	{
		//Action: Do Nothing
	case UI_UnitActionMenu::NO_ACTION:
		break;

	case UI_UnitActionMenu::ATTACK:
		ExecuteAction_Menu_Attack();
		break;

	case UI_UnitActionMenu::MOVE:
		ExecuteAction_Menu_Move();
		break;

	case UI_UnitActionMenu::PASS:
		GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->DisableUnit();
		m_UnitActionMenu.GetUIElement()->SetEnabledState(false);
		SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::FREE_LOOK);

		break;

	case UI_UnitActionMenu::INFORMATION:
		//Set UI flags
		m_UnitInfoWindow.GetUIElement()->SetEnabledState(true);
		m_UnitInfoWindow.UpdateStrings(GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET));
		m_UnitActionMenu.GetUIElement()->SetEnabledState(false);
		//Change State
		SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::UNIT_INFO_WINDOW_OPEN);
		break;

	case UI_UnitActionMenu::BACK:
		//Set UI flags
		m_UnitActionMenu.GetUIElement()->SetEnabledState(false);

		//Change State
		SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::FREE_LOOK);
		break;
	}

}

void Mode_MainGame::PK_UnitMoveOpen(char key)
{
	switch (key)
	{
		//Action: Move Up
	case 0:
		MoveCursorUpdate(Directional4Way::UP,
			Entity_TileCursor::SECOND_TARGET,
			Entity_TileCursor::SECOND_TILE
		);
		RunAStar();
		break;
		//Action: Move Down
	case 1:
		MoveCursorUpdate(Directional4Way::DOWN,
			Entity_TileCursor::SECOND_TARGET,
			Entity_TileCursor::SECOND_TILE
		);
		RunAStar();
		break;
		//Action: Move Left
	case 2:
		MoveCursorUpdate(Directional4Way::LEFT,
			Entity_TileCursor::SECOND_TARGET,
			Entity_TileCursor::SECOND_TILE
		);
		RunAStar();
		break;
		//Action: Move Right
	case 3:
		MoveCursorUpdate(Directional4Way::RIGHT,
			Entity_TileCursor::SECOND_TARGET,
			Entity_TileCursor::SECOND_TILE
		);
		RunAStar();
		break;
		//Action: Confirm
	case 4:
		//Attempt to move the held unit
		ExecuteAction_UnitMove();
		break;
		//Action: Back
	case 5:
		ExitManifestStates();
		SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::UNIT_ACTION_MENU_OPEN);

		break;
	}
}

void Mode_MainGame::PK_UnitAttackOpen(char key)
{
	switch (key)
	{
		//Action: Move Up
	case 0:
		MoveCursorUpdate(Directional4Way::UP,
			Entity_TileCursor::SECOND_TARGET,
			Entity_TileCursor::SECOND_TILE
		);
		break;
		//Action: Move Down
	case 1:
		MoveCursorUpdate(Directional4Way::DOWN,
			Entity_TileCursor::SECOND_TARGET,
			Entity_TileCursor::SECOND_TILE
		);
		break;
		//Action: Move Left
	case 2:
		MoveCursorUpdate(Directional4Way::LEFT,
			Entity_TileCursor::SECOND_TARGET,
			Entity_TileCursor::SECOND_TILE
		);
		break;
		//Action: Move Right
	case 3:
		MoveCursorUpdate(Directional4Way::RIGHT,
			Entity_TileCursor::SECOND_TARGET,
			Entity_TileCursor::SECOND_TILE
		);
		break;
		//Action: Confirm
	case 4:
		ExecuteAction_UnitTargeting();
		break;
		//Action: Back
	case 5:
		ExitManifestStates();
		SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::UNIT_ACTION_MENU_OPEN);
		break;
	}
}

void Mode_MainGame::PK_UnitInfoWindowOpen(char key)
{
	switch (key)
	{
		//Action: Back
	case 5:
		ExitManifestStates();
		m_UnitInfoWindow.GetUIElement()->SetEnabledState(false);
		SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::UNIT_ACTION_MENU_OPEN);
		break;
	}
}

void Mode_MainGame::PK_MasterMenuOpen(char key)
{
	switch (m_MasterMenu.ProcessInput(key))
	{
	case UI_GameMasterMenu::END_TURN:
		EndTurn();
		break;
	case UI_GameMasterMenu::QUIT_GAME:
		PostQuitMessage(0);
		break;
	case UI_GameMasterMenu::BACK:
		//Change State
		SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::FREE_LOOK);
		break;
	}
}

void Mode_MainGame::PK_ScenarioEnd(char key)
{
	switch (m_ScenarioEndMenu.ProcessInput(key))
	{
		//Action: Play Again
	case 0:
		FreshGameState();
		break;
		//Action: Main Menu
	case 1:
		//State NYI
		break;
		//Action: Quit Game
	case 2:
		PostQuitMessage(0);
		break;
	}
}

void Mode_MainGame::MoveCursorUpdate(Directional4Way direction, Entity_TileCursor::EntityIndexes entitySlot,
	Entity_TileCursor::TileIndexes tileSlot)
{
	//Call cursor move
	m_GPManager->GetTileCursor().Move(direction);

	//Call scene check
	m_GPManager->GetSceneCamera().RunBoundaryCheck(
		DirectX::XMINT2(
			static_cast<int>(m_GPManager->GetTileCursor().GetCoords().GetMapCoordinatesX()),
			static_cast<int>(m_GPManager->GetTileCursor().GetCoords().GetMapCoordinatesY())),
		direction
	);

	//If unit found, flag update on cursor, else disable the draw flag as it isn't needed
	if (m_GPManager->GetTileCursor().SearchForUnitAtCoordinates(entitySlot))
	{
		m_UnitFrame.UpdateFrameWithUnit(GET_INDEXED_UNIT(entitySlot));


		if (GET_INDEXED_UNIT(entitySlot)->GetTeamID() ==
			m_GPManager->GetTeamsManager().GetActiveTeamIndexAsNumber())
			m_UnitFrame.SetStyle(UI_UnitFrame::BLUE_STYLE_00);
		else
			m_UnitFrame.SetStyle(UI_UnitFrame::RED_STYLE_00);
	}
	else
	{
		m_UnitFrame.GetUIElement()->SetEnabledState(false);
		m_GPManager->GetTileCursor().ReleaseUnitAtIndex(entitySlot);
	}

	//Check for tile at coordinates
	if (m_GPManager->GetTileCursor().SearchForTileObjectAtCoordinates(tileSlot))
	{
		//Update UI to reflect
		m_Tooltip.UpdateTooltipWithTile(GET_INDEXED_TILE(tileSlot));
	}

}

void Mode_MainGame::SyncCursorToFirstUnit(bool releaseExtraObjects)
{
	//Snap the cursor position back to the first unit held
	m_GPManager->GetTileCursor().MoveToCoordinate(GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->GetCoords().GetMapCoordinatesXY());

	//Sync the scene to the cursor position
	m_GPManager->GetSceneCamera().SyncSceneToCursor(m_GPManager->GetTileCursor().GetCoords().GetMapCoordinatesXY());

	//Read the tile and unit to UI elements
	m_UnitFrame.UpdateFrameWithUnit(GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET));
	m_Tooltip.UpdateTooltipWithTile(GET_INDEXED_TILE(Entity_TileCursor::FIRST_TILE));

}

void Mode_MainGame::RunAStar()
{
	//Check to see if the current tile is in the current pathfinder manifest
	if (m_GPManager->GetPathfinder().IsTileInManifest(GET_INDEXED_TILE(Entity_TileCursor::SECOND_TILE)))
	{
		//Clear the manifest to a given frame
		m_GPManager->GetPathfinder().SetManifestEffect(
			BK_UI_Bundle_00_Desc::BK_UI_BLUE_TILE_HIGHLIGHT_00_48x48,
			TileSpriteIndexes::TS_HIGHLIGHT_TILE_00
		);

		//Call A* algorithm
		m_GPManager->GetPathfinder().FindShortestRoute(
			GET_INDEXED_TILE(Entity_TileCursor::FIRST_TILE),
			GET_INDEXED_TILE(Entity_TileCursor::SECOND_TILE),
			BK_UI_Bundle_00_Desc::BK_UI_RED_TILE_HIGHLIGHT_00_48x48,
			TileSpriteIndexes::TS_HIGHLIGHT_TILE_00
		);

	}
	//The cursor has left, or isn't in the manifests known area (somehow), so remove any previous A* calls
	else
	{
		//Clear the manifest to a given frame
		m_GPManager->GetPathfinder().SetManifestEffect(
			BK_UI_Bundle_00_Desc::BK_UI_BLUE_TILE_HIGHLIGHT_00_48x48,
			TileSpriteIndexes::TS_HIGHLIGHT_TILE_00
		);
	}
}

void Mode_MainGame::ExecuteAction_FreeLook()
{
	//Is there a unit at the cursor
	if (GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET) &&
		GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->AnyActionsAvailable() &&
		GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->GetTeamID() ==
		m_GPManager->GetTeamsManager().GetActiveTeamIndexAsNumber())
	{
		//Check if the unit is AI enabled, and flag AI update if so
		if (GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->GetAIPilotFlag())
			GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->SetAIUpdateFlag(true);

		//Snap camera to unit
		m_GPManager->SyncToUnit(GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET));
		//Switch State
		SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::UNIT_ACTION_MENU_OPEN);
	}
}

void Mode_MainGame::ExecuteAction_Menu_Move()
{
	//Check if the unit can attack
	if (GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->GetMoveFlag())
	{
		//Call Pathfinder
		m_GPManager->GetPathfinder().GeneratePathfinderGrid(
			m_GPManager->GetTileManager().GetTileMap(),
			GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET),
			m_GPManager->GetTileManager().GetMapData().m_MapSize.x,
			BK_UI_Bundle_00_Desc::BK_UI_BLUE_TILE_HIGHLIGHT_00_48x48
		);

		//Change State
		SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::UNIT_MOVING);
	}
	else
	{
		DBOUT("Already Moved, Cannot Use Action!")
	}
}

void Mode_MainGame::ExecuteAction_Menu_Attack()
{
	//Check if the unit can attack
	if (GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->GetActionFlag())
	{
		m_GPManager->GetTargetingSystem().RunTargetingAlgorithm(
			MT_TargetingManager::TYPE_DIAMOND_00,
			m_GPManager->GetTileManager().GetTileMap(),
			m_GPManager->GetTileManager().GetMapData().m_MapSize.x,
			m_GPManager->GetTileCursor().GetCoords().GetMapCoordinatesXY(),
			BK_UI_Bundle_00_Desc::BK_UI_RED_TILE_HIGHLIGHT_00_48x48,
			GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->GetUnitData().m_AttackRange
		);

		//Change State
		SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::UNIT_TARGETING);
	}
	else
	{
		DBOUT("Already Attacked, Cannot Use Action!")
	}

}

void Mode_MainGame::ExecuteAction_UnitMove()
{
	//Check that the tile is inside the manifest
	if (m_GPManager->GetPathfinder().IsTileInManifest(GET_INDEXED_TILE(Entity_TileCursor::SECOND_TILE)))
	{
		//Confirmed viability, so move the unit
		m_GPManager->MoveUnitToTile(
			GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET),
			GET_INDEXED_TILE(Entity_TileCursor::SECOND_TILE),
			GET_INDEXED_TILE(Entity_TileCursor::FIRST_TILE)
		);

		//Clear up manifest
		ExitManifestStates();

		//Disable Move Flag
		GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->SetMoveFlag(false);

		//Check if the unit can still perform any action and direct state accordingly
		if (GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->AnyActionsAvailable())
		{
			//Change State
			SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::UNIT_ACTION_MENU_OPEN);
		}
		//No more actions available, so disable the unit
		else
		{
			//Disable Unit
			GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->DisableUnit();
			//Change State
			SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::FREE_LOOK);
		}
	}
}

void Mode_MainGame::ExecuteAction_UnitTargeting()
{
	//Get cursor for brevity
	Entity_TileCursor& cursor = m_GPManager->GetTileCursor();

	if (GET_INDEXED_UNIT(Entity_TileCursor::SECOND_TARGET) && GET_INDEXED_TILE(Entity_TileCursor::SECOND_TILE) &&
		GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET) != GET_INDEXED_UNIT(Entity_TileCursor::SECOND_TARGET) &&
		m_GPManager->GetTargetingSystem().IsTileInManifest(GET_INDEXED_TILE(Entity_TileCursor::SECOND_TILE)))
	{

		if (m_QuickKill)
		{
			//Kill Unit
			Game::GetGame()->GetGameplayManager().GetTeamsManager().ProcessUnitDeath(GET_INDEXED_UNIT(Entity_TileCursor::SECOND_TARGET));

			//Trigger update flag for scenario conditions
			Game::GetGame()->GetGameplayManager().GetScenarioManager().SetConditionCheckFlag(true);
		}

		else
			m_GPManager->GetCombatManager().RunCombatSimulation(
				GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET),
				GET_INDEXED_UNIT(Entity_TileCursor::SECOND_TARGET),
				GET_INDEXED_TILE(Entity_TileCursor::FIRST_TILE),
				GET_INDEXED_TILE(Entity_TileCursor::SECOND_TILE)
			);

		//Disable action flag
		GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->SetActionFlag(false);

		//Check if the unit can still perform any action and direct state accordingly
		if (GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->AnyActionsAvailable())
		{
			//Clear up manifest
			ExitManifestStates();

			//Change State
			SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::UNIT_ACTION_MENU_OPEN);
		}
		//No actions left
		else
		{
			//Clear up manifest
			ExitManifestStates();
			//Disable Unit
			GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET)->DisableUnit();

			//Change State
			SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::FREE_LOOK);
		}
	}
}

void Mode_MainGame::EndTurn()
{
	//Change the active team ID
	m_GPManager->GetTeamsManager().SetTeamToNextTeam();
	//Reset all unit states
	m_GPManager->GetTeamsManager().EnableAllTeamsAllUnits();
	//Sync and reset the cursor held pointers
	m_GPManager->GetTileCursor().ResetAndRescan();

	//Change State
	SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::FREE_LOOK);

}

void Mode_MainGame::ExitManifestStates()
{
	//Release any active manifests
	m_GPManager->GetPathfinder().ReleaseManifest();
	m_GPManager->GetTargetingSystem().ReleaseManifest();

	//Sync the cursor back to original unit & reset and rescan cursor
	m_GPManager->SyncToUnit(GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET));
	m_GPManager->GetTileCursor().ResetAndRescan();

	//Update UIs
	m_Tooltip.UpdateTooltipWithTile(GET_INDEXED_TILE(Entity_TileCursor::FIRST_TILE));
	m_UnitFrame.UpdateFrameWithUnit(GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET));
}

void Mode_MainGame::EndOfScenario()
{
	//Disable/Enable UIs
	SET_ENABLED_STATE(m_MasterMenu, false);
	SET_ENABLED_STATE(m_Tooltip, false);
	SET_ENABLED_STATE(m_UnitFrame, false);
	SET_ENABLED_STATE(m_UnitActionMenu, false);
	SET_ENABLED_STATE(m_UnitInfoWindow, false);

	SET_ENABLED_STATE(m_ScenarioEndMenu, true);
	m_MessageBox->SetEnabledState(true);

	//Check through each team and get the elimination status of them
	for (unsigned i(0); i < m_GPManager->GetTeamsManager().GetNumberOfTeams(); ++i)
	{
		if (m_GPManager->GetTeamsManager().GetTeamAtIndex(i)->GetEliminatedFlag())
		{
			std::stringstream ss;
			ss << "Team " << m_GPManager->GetTeamsManager().GetTeamAtIndex(i)->GetTeamID() + 1 << " Loses!";

			DBOUT(ss.str());
			m_MessageBox->SetStringAtIndex(ss.str(), 0);
		}
	}
}

void Mode_MainGame::FreshGameState()
{
	//Disable all units (so they are available for spawning)
	m_GPManager->GetTeamsManager().DisableAllTeamsAllUnits();

	//Clear all tiles to usable state
	for (auto& a : m_GPManager->GetTileManager().GetTileMap())
	{
		a->GetProperties().m_IsOccupied = false;
		a->GetAStarData().m_ParentTile = nullptr;
	}

	//Call current init function
	m_GPManager->GetTeamsManager().GameplayScenarioSetup_00();

	//Setup initial cursor state
	m_GPManager->GetTileCursor().MoveToCoordinate(DirectX::XMFLOAT2(25, 20));
	m_GPManager->GetSceneCamera().SyncSceneToCursor(DirectX::XMFLOAT2(25, 20));
	m_GPManager->GetTileCursor().ResetAndRescan();
	m_Tooltip.UpdateTooltipWithTile(GET_INDEXED_TILE(Entity_TileCursor::FIRST_TILE));
	m_UnitFrame.UpdateFrameWithUnit(GET_INDEXED_UNIT(Entity_TileCursor::FIRST_TARGET));

	//Set UI Flags
	m_Tooltip.GetUIElement()->SetEnabledState(true);
	m_UnitFrame.GetUIElement()->SetEnabledState(false);
	m_UnitInfoWindow.GetUIElement()->SetEnabledState(false);
	m_MessageBox->SetEnabledState(false);

	m_UnitActionMenu.GetUIElement()->SetEnabledState(false);
	m_MasterMenu.GetUIElement()->SetEnabledState(false);
	m_ScenarioEndMenu.GetUIElement()->SetEnabledState(false);

	//Set win condition for game
	m_GPManager->GetScenarioManager().SetActiveScenario(SM_ScenarioManager::ScenarioStates::ELIMINATION);

	//Set Active Team
	m_GPManager->GetTeamsManager().SetActiveTeam(TeamIndexes::TEAM_00);

	//Reset AI Data for each team
	for (unsigned i(0); i < m_GPManager->GetTeamsManager().GetNumberOfTeams(); ++i)
		m_GPManager->GetTeamsManager().GetTeamAtIndex(i)->ResetTeamAIData();

	//Reseed RNG for run
	std::srand(time(NULL));

	//Set New State
	SET_ACTIVE_GAME_STATE(SM_GameStateManager::ActiveGameState::FREE_LOOK);
}
