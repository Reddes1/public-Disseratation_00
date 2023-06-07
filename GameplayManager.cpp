#include "GameplayManager.h"

#include "AnimationDefinitions.h"	//Anim enums
#include "GeneralUtils.h"			//Math
#include "EngineDefinitions.h"		//Layer Depth info
#include "GameConstants.h"			//Game info

void GameplayManager::LoadLevel(SM_TilemapManager::LoaderTypes loaderType, std::string& levelFP, std::string& tilesetName,
	DirectX::XMFLOAT2& cursorStartCoords, DirectX::XMINT2& sceneBuffer, int windowX, int windowY)
{
	//Call map loader function
	m_TileManager.LoadMap(loaderType, levelFP, tilesetName);

	//Configure cursor
	m_TileCursor.SetClampingValues(DirectX::XMINT2(m_TileManager.GetMapData().m_MapSize));
	m_TileCursor.SetTileSize(DirectX::XMINT2(m_TileManager.GetMapData().m_TileSize));
	m_TileCursor.MoveToCoordinate(cursorStartCoords);
	
	//Configure scene camera
	m_SceneCamera.InitSceneCamera(
		m_TileManager.GetMapData().m_MapSize,
		m_TileManager.GetMapData().m_TileSize,
		sceneBuffer,
		windowX,
		windowY
	);

	//Sync the scene to the cursor position
	m_SceneCamera.SyncSceneToCursor(m_TileCursor.GetCoords().GetMapCoordinatesXY());
}

void GameplayManager::SpawnNewUnit(unsigned teamID, unsigned unitID, MT_Type1* tile)
{
	//Find a free unit slot
	Entity_GameUnit* unit = m_TeamsManager.GetTeamAtIndex(teamID)->FindFreeUnit();
	
	assert(unit);
	if (unit)
	{
		////////////////////////
		/// Individual Setup ///
		////////////////////////

		//Use unitID to determine how to configure the unit
		switch (unitID)
		{
		case GameUnitIDs::ASSASSIN:
			//Set Texture
			unit->GetBaseSprite().SetTexture("Assassin_SS_00");
			//Set Animation
			unit->GetBaseAnimator().SetAnimation((int)Assassin_00_Animations::IDLE_00);
			//Set Class Data
			unit->GetUnitData() = m_UnitDataManager.GetUnitDataByID(GameUnitIDs::ASSASSIN);
			break;

		case GameUnitIDs::BARBARIAN:
			//Set Texture
			unit->GetBaseSprite().SetTexture("Barbarian_SS_00");
			//Set Animation
			unit->GetBaseAnimator().SetAnimation((int)Barbarian_00_Animations::IDLE_00);
			//Set Class Data
			unit->GetUnitData() = m_UnitDataManager.GetUnitDataByID(GameUnitIDs::BARBARIAN);
			break;


		case GameUnitIDs::ARCHER:
			//Set Texture
			unit->GetBaseSprite().SetTexture("Archer_SS_00");
			//Set Animation
			unit->GetBaseAnimator().SetAnimation((int)Archer_00_Animations::IDLE_00);
			//Set Class Data
			unit->GetUnitData() = m_UnitDataManager.GetUnitDataByID(GameUnitIDs::ARCHER);
			break;

		case GameUnitIDs::NECROMANCER:
			//Set Texture
			unit->GetBaseSprite().SetTexture("Necromancer_SS_00");
			//Set Animation
			unit->GetBaseAnimator().SetAnimation((int)Necromancer_00_Animations::IDLE_00);
			//Set Class Data
			unit->GetUnitData() = m_UnitDataManager.GetUnitDataByID(GameUnitIDs::NECROMANCER);
			break;


		case GameUnitIDs::SORCERESS:
			//Set Texture
			unit->GetBaseSprite().SetTexture("Sorceress_SS_00");
			//Set Animation
			unit->GetBaseAnimator().SetAnimation((int)Sorceress_00_Animations::IDLE_00);
			//Set Class Data
			unit->GetUnitData() = m_UnitDataManager.GetUnitDataByID(GameUnitIDs::SORCERESS);
			break;

		case GameUnitIDs::WARRIOR:
			//Set Texture
			unit->GetBaseSprite().SetTexture("Warrior_SS_00");
			//Set Animation
			unit->GetBaseAnimator().SetAnimation((int)Warrior_00_Animations::IDLE_00);
			//Set Class Data
			unit->GetUnitData() = m_UnitDataManager.GetUnitDataByID(GameUnitIDs::WARRIOR);
			break;
		}

		////////////////////
		/// Shared Setup ///
		////////////////////
		
		//Calculate a scale to fix the unit to the tile properly
		float xScale = GetPercentageOfNumber(
			static_cast<float>(m_TileManager.GetMapData().m_TileSize.x),
			static_cast<float>(unit->GetBaseSprite().GetTexturePtr()->m_Frames[0].right)
		);
		float yScale = GetPercentageOfNumber(
			static_cast<float>(m_TileManager.GetMapData().m_TileSize.x),
			static_cast<float>(unit->GetBaseSprite().GetTexturePtr()->m_Frames[0].bottom)
		);

		//Set Origin
		unit->GetBaseSprite().SetOriginToCenter();

		//Apply scale (with minor adjustment)
		unit->GetBaseSprite().SetScale(xScale * 1.25f, yScale * 1.25f);

		//Set Position
		unit->GetBaseSprite().SetPosition(
			(tile->GetCoordinates().GetMapCoordinatesX() * m_TileManager.GetMapData().m_TileSize.x) + m_TileManager.GetMapData().m_TileSize.x * 0.5f,
			(tile->GetCoordinates().GetMapCoordinatesY() * m_TileManager.GetMapData().m_TileSize.y) + m_TileManager.GetMapData().m_TileSize.y * 0.5f
		);

		//Set Coordinates
		unit->GetCoords().SetMapCoordinatesXY(
			tile->GetCoordinates().GetMapCoordinatesX(),
			tile->GetCoordinates().GetMapCoordinatesY()
		);

		//Set Draw Depth
		unit->GetBaseSprite().SetLayerDepth(DRAW_DEPTH_ENTITY_0);

		//Set flags
		unit->SetActiveState(true);
		unit->SetRenderState(true);
		unit->SetMoveFlag(true);
		unit->SetActionFlag(true);

		//Set team ID
		unit->SetTeamID(teamID);

		//Flip the sprite for the 2nd team
		if (teamID == (unsigned)TeamIndexes::TEAM_01)
			unit->GetBaseSprite().SetEffect(DirectX::SpriteEffects::SpriteEffects_FlipHorizontally);
		else
			unit->GetBaseSprite().SetEffect(DirectX::SpriteEffects::SpriteEffects_None);


		//Conditional Setup
		switch (teamID)
		{
		case (unsigned)TeamIndexes::TEAM_00:

			//Set Sprite Effect
			unit->GetBaseSprite().SetEffect(DirectX::SpriteEffects::SpriteEffects_None);
			//Set Colour
			unit->GetBaseSprite().SetColour(TEAM_00_COLOUR);

			break;

		case (unsigned)TeamIndexes::TEAM_01:

			//Set Sprite Effect
			unit->GetBaseSprite().SetEffect(DirectX::SpriteEffects::SpriteEffects_FlipHorizontally);
			//Set Colour
			unit->GetBaseSprite().SetColour(TEAM_01_COLOUR);
			break;
		}

		//Flag tile as occupied
		tile->GetProperties().m_IsOccupied = true;

		//Increment units alive count for team
		m_TeamsManager.GetTeamAtIndex(teamID)->IncrementUnitsAlive(1);

	}
}

bool GameplayManager::MoveUnitToTile(Entity_GameUnit* unit, MT_Type1* newTile, MT_Type1* oldTile)
{
	msg_assert(unit, "MoveUnitToTile(): No Valid Unit Pointer Passed");
	msg_assert(newTile, "MoveUnitToTile(): No Valid New Tile Pointer Passed");
	msg_assert(oldTile, "MoveUnitToTile(): No Valid Old Tile Pointer Passed");

	//Check that the tile isnt already occupied
	if (!newTile->GetProperties().m_IsOccupied)
	{
		//Set New Position
		unit->GetBaseSprite().SetPosition(
			(newTile->GetCoordinates().GetMapCoordinatesX() * m_TileManager.GetMapData().m_TileSize.x) + m_TileManager.GetMapData().m_TileSize.x * 0.5f,
			(newTile->GetCoordinates().GetMapCoordinatesY() * m_TileManager.GetMapData().m_TileSize.y) + m_TileManager.GetMapData().m_TileSize.y * 0.5f
		);

		//Set Coordinates
		unit->GetCoords().SetMapCoordinatesXY(
			newTile->GetCoordinates().GetMapCoordinatesX(),
			newTile->GetCoordinates().GetMapCoordinatesY()
		);
		
		//Flag tile as occupied
		newTile->GetProperties().m_IsOccupied = true;
		//Disable occupied at old tile if passed
		if(oldTile)
			oldTile->GetProperties().m_IsOccupied = false;		
		return true;
	}
	//Cant move unit, return false
	else
		return false;
}

void GameplayManager::SyncToUnit(Entity_GameUnit* const unit)
{
	if (unit)
	{
		//Move cursor
		m_TileCursor.MoveToCoordinate(unit->GetCoords().GetMapCoordinatesXY());
		//Sync scene
		m_SceneCamera.SyncSceneToCursor(m_TileCursor.GetCoords().GetMapCoordinatesXY());
	}
	else
	{
		msg_assert(false, "Sync To Unit Failed, No Unit Found");
	}
}
