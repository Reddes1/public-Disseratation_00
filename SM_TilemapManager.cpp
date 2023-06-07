#include "SM_TilemapManager.h"
#include "Tiled_Utils.h"
#include "Game.h"		//Manager Access

using namespace rapidjson;

SM_TilemapManager::SM_TilemapManager()
{
	Init();
}

SM_TilemapManager::~SM_TilemapManager()
{
	Release();
}

void SM_TilemapManager::LoadMap(LoaderTypes loader, std::string& levelFP, std::string& tilesetName)
{
	//Load using specified loader, passing fp and name
	switch (loader)
	{
	case LoaderTypes::MAP_LOADER_00:
		MapLoaderTypeOne(levelFP, tilesetName);
		break;
	}

}

void SM_TilemapManager::Init()
{
	//Maps can be quite large, so calculate the biggest expected number of tiles,
	//and reserve that many nullptrs for space
	int res = 50 * 50;

	m_TileMap.reserve(res);
}

void SM_TilemapManager::InitTilesWithHighlightTexture(std::string& texName, unsigned index, unsigned spriteIndex, float alpha)
{
	for (auto a : m_TileMap)
	{
		a->GetSpriteAtIndex(spriteIndex).SetTexture(texName);
		a->GetSpriteAtIndex(spriteIndex).SetFrame(index);
		a->GetSpriteAtIndex(spriteIndex).SetColour(DirectX::XMVECTOR{1.0f, 1.0f, 1.0f, alpha});
		a->GetSpriteAtIndex(spriteIndex).SetOriginToCenter();
	}
}

void SM_TilemapManager::ClearAllParentTiles()
{
	for (auto& a : m_TileMap)
		a->GetAStarData().m_ParentTile = nullptr;
}

void SM_TilemapManager::Render(DirectX::SpriteBatch& sb, DirectX::DescriptorHeap* heap)
{
	for (auto& a : m_TileMap)
		a->Render(sb, heap);
}

void SM_TilemapManager::Update(float dTime)
{
	for (auto& a : m_TileMap)
	{
		a->Update(dTime);
	}
}

void SM_TilemapManager::Release()
{
	for (auto& a : m_TileMap)
	{
		delete a;
		a = nullptr;
	}
}

MT_Type1* SM_TilemapManager::GetMapTileByCoordinate(unsigned x, unsigned y)
{
	//Calculate an index
	unsigned index = (x + y) + (y * m_MapData.m_MapSize.x);
	msg_assert(index >= 0 && index <= m_TileMap.size(), "Tile search out of coordinate range.");

	return m_TileMap[index];
}

void SM_TilemapManager::SetupNeighbourTiles()
{
	//With all map setup done, find and assign all neighbour pointers
	DirectX::XMINT2& max = m_MapData.m_MapSize;
	for (auto& a : m_TileMap)
	{
		//Get tile coordinate
		int x = static_cast<int>(a->GetCoordinates().GetMapCoordinatesX());
		int y = static_cast<int>(a->GetCoordinates().GetMapCoordinatesY());

		//any non-zero tile (most common case)
		if ((x != 0 && x != max.x) && (y != 0 && y != max.y))
		{
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH_EAST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_EAST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH_EAST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH_WEST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_WEST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH_WEST, x, y, max.x)]);
		}
		//TL corner
		else if (x == 0 && y == 0)
		{
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_EAST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH_EAST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH, x, y, max.x)]);
		}
		//Along top (not corner)
		else if (y == 0 && (x != 0 && x != max.x))
		{
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_EAST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH_EAST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH_WEST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_WEST, x, y, max.x)]);
		}
		//TR corner
		else if (y == 0 && (x == max.x))
		{
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH_WEST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_WEST, x, y, max.x)]);
		}
		//Along left (no corner)
		else if (x == 0 && (y != 0 && y != max.y))
		{
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH_EAST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_EAST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH_EAST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH, x, y, max.x)]);
		}
		//Along right (no corner)
		else if (x == max.x && (y != 0 && y != max.y))
		{
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_SOUTH_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_SOUTH_WEST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_WEST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH_WEST, x, y, max.x)]);
		}
		//BL corner
		else if (x == 0 && y == max.y)
		{
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH_EAST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_EAST, x, y, max.x)]);
		}
		//Along bottom (no corner)
		else if ((x != 0 && x != max.x) && y == max.y)
		{
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH_EAST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_EAST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_EAST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_WEST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH_WEST, x, y, max.x)]);
		}
		//BR corner
		else if (x == max.x && y == max.y)
		{
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_WEST, x, y, max.x)]);
			a->SetNeighbourTileAtIndex(TNIndexes::TNI_NORTH_WEST, m_TileMap[GetIndexInDirection(TNIndexes::TNI_NORTH_WEST, x, y, max.x)]);;
		}
	}

}

size_t SM_TilemapManager::GetIndexInDirection(int dir, size_t x, size_t y, size_t maxX)
{
	switch (dir)
	{
	case TNIndexes::TNI_NORTH:
		return (x + (y - 1)) + ((y - 1) * maxX);
		break;
	case TNIndexes::TNI_NORTH_EAST:
		return (x + 1 + (y - 1)) + ((y - 1) * maxX);
		break;
	case TNIndexes::TNI_EAST:
		return (x + 1 + y) + (y * maxX);
		break;
	case TNIndexes::TNI_SOUTH_EAST:
		return (x + 1 + (y + 1)) + ((y + 1) * maxX);
		break;
	case TNIndexes::TNI_SOUTH:
		return (x + (y + 1)) + ((y + 1) * maxX);
		break;
	case TNIndexes::TNI_SOUTH_WEST:
		return (x - 1 + (y + 1)) + ((y + 1) * maxX);
		break;
	case TNIndexes::TNI_WEST:
		return (x - 1 + y) + (y * maxX);
		break;
	case TNIndexes::TNI_NORTH_WEST:
		return (x - 1 + (y - 1)) + ((y - 1) * maxX);
		break;
	}

	//Should never hit this
	assert(false);
	return size_t(0);
}

void SM_TilemapManager::MapLoaderTypeOne(std::string& levelFP, std::string& tilesetName)
{
	//Get Game
	Game* game = Game::GetGame();

	//Parse out level document
	rapidjson::Document levelDoc;
	ParseNewJSONDocument(levelDoc, levelFP);

	//Store the seperate layers, we need to access them at different points
	const Value& terrainLayer1 = levelDoc["layers"][0];
	const Value& terrainLayer2 = levelDoc["layers"][1];
	const Value& decorationLayer1 = levelDoc["layers"][2];
	const Value& objectLayer1 = levelDoc["layers"][3];

	//Release the current array
	Release();

	//Get some map information for the building purposes
	int mapRows = levelDoc["height"].GetInt();
	int mapCols = levelDoc["width"].GetInt();
	int tileWidth = levelDoc["tilewidth"].GetInt();
	int tileHeight = levelDoc["tileheight"].GetInt();

	//Get hold of the related textures
	auto tilemapTex = game->GetTextureManager().GetSpriteTextureData(
		std::string(levelDoc["tilesets"][0]["name"].GetString()));

	//Get hold of the map hold information on this tileset
	std::unordered_map<int, TMM_Properties_Type1>* const tileMap = game->GetTilesetManager().GetTileMapByName(tilesetName);
	assert(tileMap);

	//Counter for data sub-array
	int count = 0;
	//Start building the map (L->R, T->B)
	for (int y(0); y < mapRows; ++y)
	{
		for (int x(0); x < mapCols; ++x)
		{

			//Get the data gathering out of the way here
			//Get all the layer ID, and iteratations done
			unsigned int layer1ID = terrainLayer1["data"][count].GetUint();
			unsigned int layer2ID = terrainLayer2["data"][count].GetUint();
			unsigned int layer3ID = decorationLayer1["data"][count].GetUint();


			//
			// General Work
			//
			
			//Before anything, check layer 1 is valid (should never be 0), and align it with map
			assert(layer1ID != 0);
			--layer1ID;

			//Create a new tile
			MT_Type1* newTile = new MT_Type1();

			//Store worlds space coordinates for this tile
			newTile->GetCoordinates().SetMapCoordinatesXY((float)x, (float)y);

			//Get the general work done for each sprite
			for (int i(0); i < TileSpriteIndexes::TS_COUNT; ++i)
			{
				newTile->GetSpriteAtIndex(i).SetTexture(tilemapTex);
				newTile->GetSpriteAtIndex(i).SetRect(RECT{ 0, 0, 0, 0 });
				newTile->GetSpriteAtIndex(i).SetPosition(
					static_cast<float>((tileWidth * x) + tileWidth * 0.5f),
					static_cast<float>((tileWidth * y) + tileHeight * 0.5f)
				);
			}

			//Set initial layer depths
			newTile->GetSpriteAtIndex(0).SetLayerDepth(DRAW_DEPTH_FOREGROUND_0);
			newTile->GetSpriteAtIndex(1).SetLayerDepth(DRAW_DEPTH_FOREGROUND_0);
			newTile->GetSpriteAtIndex(2).SetLayerDepth(DRAW_DEPTH_FOREGROUND_0);
			newTile->GetSpriteAtIndex(3).SetLayerDepth(DRAW_DEPTH_FOREGROUND_1);


			//
			//1st Layer Sprite
			//
			
			//Strip rotation flag and apply rotation
			newTile->GetSpriteAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_00).SetRotation(GetTileRotationValue(layer1ID));

			//Find the details for this tile from map
			std::unordered_map<int, TMM_Properties_Type1>::iterator layer1Iter = tileMap->find(layer1ID);

			//Check tile properties found (should never assert false)
			if (layer1Iter != tileMap->end())
			{
				//Set Animation & Rect
				newTile->GetAnimatorAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_00).SetAnimArray(layer1Iter->second.m_Animations);
				newTile->GetSpriteAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_00).SetRect(layer1Iter->second.m_Animations[0].m_FrameRect);

				//Check if more than 1 frame found, enabled animation component
				if (layer1Iter->second.m_Animations.size() > 1)
					newTile->GetAnimatorAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_00).SetPlayState(true);
				else
					newTile->GetAnimatorAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_00).SetPlayState(false);

				//Center orgin and enable draw/update for sprite 0
				newTile->GetSpriteAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_00).SetOriginToCenter();
				newTile->SetEnabledAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_00, true);
			}
			else
				assert(false);


			//
			//Properties & 2nd Layer Sprite
			//

			//Check if there is a tile present
			if (layer2ID != 0)
			{
				//Align to map
				--layer2ID;
				//Strip rotation flag and apply rotation
				newTile->GetSpriteAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_01).SetRotation(GetTileRotationValue(layer2ID));
				//Find properites
				std::unordered_map<int, TMM_Properties_Type1>::iterator layer2Iter = tileMap->find(layer2ID);

				//Check tile properties found (should never assert false)
				if (layer2Iter != tileMap->end())
				{
					//Set Animation & Rect
					newTile->GetAnimatorAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_01).SetAnimArray(layer2Iter->second.m_Animations);
					newTile->GetSpriteAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_01).SetRect(layer2Iter->second.m_Animations[0].m_FrameRect);

					//Check if more than 1 frame found, enabled animation component
					if (layer2Iter->second.m_Animations.size() > 1)
						newTile->GetAnimatorAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_01).SetPlayState(true);
					else
						newTile->GetAnimatorAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_01).SetPlayState(false);

					//Center orgin and enable draw/update for sprite 1
					newTile->GetSpriteAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_01).SetOriginToCenter();
					newTile->SetEnabledAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_01, true);

					//If this layer has priority (it usually should if on this layer), then apply this tiles 
					if (layer2Iter->second.m_LoadedProperties.m_Priority)
						newTile->ExtractPropertiesFromMapTileProperties(layer2Iter->second);
					else
						newTile->ExtractPropertiesFromMapTileProperties(layer1Iter->second);

				}
				else
					assert(false);
			}
			//No 2nd layer tile found, so just set properties as normal & disable sprite 1
			else
			{
				newTile->ExtractPropertiesFromMapTileProperties(layer1Iter->second);
				newTile->SetEnabledAtIndex(TileSpriteIndexes::TS_TERRAIN_TILE_01, false);
			}


			//
			//3rd Layer Sprite & Finalization
			// 

			//Check if there is a tile present
			if (layer3ID != 0)
			{
				//Align to map
				--layer3ID;
				//Strip rotation flag and apply rotation
				newTile->GetSpriteAtIndex(TileSpriteIndexes::TS_DECORATION_TILE_00).SetRotation(GetTileRotationValue(layer3ID));
				//Find properites
				std::unordered_map<int, TMM_Properties_Type1>::iterator layer3Iter = tileMap->find(layer3ID);

				//Check tile properties found (should never assert false)
				if (layer3Iter != tileMap->end())
				{
					//Get Animations and then rect
					newTile->GetAnimatorAtIndex(TileSpriteIndexes::TS_DECORATION_TILE_00).SetAnimArray(layer3Iter->second.m_Animations);
					newTile->GetSpriteAtIndex(TileSpriteIndexes::TS_DECORATION_TILE_00).SetRect(layer3Iter->second.m_Animations[0].m_FrameRect);

					//Check if more than 1 frame found, enabled animation component
					if (layer3Iter->second.m_Animations.size() > 1)
						newTile->GetAnimatorAtIndex(TileSpriteIndexes::TS_DECORATION_TILE_00).SetPlayState(true);
					else
						newTile->GetAnimatorAtIndex(TileSpriteIndexes::TS_DECORATION_TILE_00).SetPlayState(false);

					//Center orgin and enable draw/update for sprite 2
					newTile->GetSpriteAtIndex(TileSpriteIndexes::TS_DECORATION_TILE_00).SetOriginToCenter();
					newTile->SetEnabledAtIndex(TileSpriteIndexes::TS_DECORATION_TILE_00, true);
				}
				else
					assert(false);
			}
			//No 3rd layer tile found, disable sprite 2
			else
			{
				//Disable draw/update
				newTile->SetEnabledAtIndex(TileSpriteIndexes::TS_DECORATION_TILE_00, false);
			}


			//Store new tile
			m_TileMap.push_back(newTile);

			//Increment loop
			++count;
		}
	}

	//Store some key map details before finishing up
	if (levelDoc.HasMember("properties"))
		m_MapData.m_MapName = levelDoc["properties"][0]["value"].GetString();
	else
		m_MapData.m_MapName = "Name Not Set";

	m_MapData.m_MapName = tilesetName;
	m_MapData.m_MapSize.x = mapCols - 1;
	m_MapData.m_MapSize.y = mapRows - 1;
	m_MapData.m_TileSize.x = tileWidth;
	m_MapData.m_TileSize.y = tileHeight;

	//Lastly, setup the neighbour pointers
	SetupNeighbourTiles();
}