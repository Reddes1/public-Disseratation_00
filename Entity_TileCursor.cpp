#include "Entity_TileCursor.h"		//Parent

#include "Game.h"					//Manager Access


void Entity_TileCursor::Update(GameTimer& gt)
{

}

void Entity_TileCursor::Render(DirectX::SpriteBatch& batch, DirectX::DescriptorHeap* heap)
{
	m_MainSprite.Draw(batch, heap);
}

void Entity_TileCursor::InitialiseCursor(std::string& textureFP, int frameIndex, float depthLayer)
{
	//Set Texture
	m_MainSprite.SetTexture(textureFP, true);
	//Set Frame
	m_MainSprite.SetFrame(frameIndex);
	//Set Draw Depth
	m_MainSprite.SetLayerDepth(depthLayer);
	//Center origin
	m_MainSprite.SetOriginToCenter();
}

void Entity_TileCursor::Move(Directional4Way direction)
{
	switch (direction)
	{
	case Directional4Way::UP:
		//Set new coordinate
		GetCoords().IncrementYCoordinate(-1.f, static_cast<float>(m_ClampCoords.y), true);
		//Set the position of the cursor relative to the new coordinate
		m_MainSprite.SetPosition(m_MainSprite.GetPosition().x, (GetCoords().GetMapCoordinatesY() * m_TileSize.y) + m_TileSize.y * 0.5f);
		break;

	case Directional4Way::DOWN:
		//Set new coordinate
		GetCoords().IncrementYCoordinate(1.f, static_cast<float>(m_ClampCoords.y), true);
		//Set the position of the cursor relative to the new coordinate
		m_MainSprite.SetPosition(m_MainSprite.GetPosition().x, (GetCoords().GetMapCoordinatesY() * m_TileSize.y) + m_TileSize.y * 0.5f);
		break;

	case Directional4Way::LEFT:
		//Set new coordinate
		GetCoords().IncrementXCoordinate(-1.f, static_cast<float>(m_ClampCoords.x), true);
		//Set the position of the cursor relative to the new coordinate
		m_MainSprite.SetPosition((GetCoords().GetMapCoordinatesX() * m_TileSize.x) + m_TileSize.x * 0.5f, m_MainSprite.GetPosition().y);
		break;

	case Directional4Way::RIGHT:
		//Set new coordinate
		GetCoords().IncrementXCoordinate(1.f, static_cast<float>(m_ClampCoords.x), true);
		//Set the position of the cursor relative to the new coordinate
		m_MainSprite.SetPosition((GetCoords().GetMapCoordinatesX() * m_TileSize.x) + m_TileSize.x * 0.5f, m_MainSprite.GetPosition().y);
		break;
	}
}

void Entity_TileCursor::MoveToCoordinate(DirectX::XMFLOAT2& newCoordinates)
{
	//Check these coordinates are valid for the map
	msg_assert(newCoordinates.x >= 0 && newCoordinates.x <= m_ClampCoords.x && newCoordinates.y >= 0 && newCoordinates.y <= m_ClampCoords.y,
		"Coordinates are outside of map");
	//Set new coordinates
	GetCoords().SetMapCoordinatesXY(newCoordinates.x, newCoordinates.y);
	//Adjust position to match
	m_MainSprite.SetPosition(
		(GetCoords().GetMapCoordinatesX() * m_TileSize.x) + m_TileSize.x * 0.5f,
		(GetCoords().GetMapCoordinatesY() * m_TileSize.y) + m_TileSize.y * 0.5f
	);
}

void Entity_TileCursor::ReleaseUnitAtIndex(EntityIndexes index)
{
	assert(index >= 0 && index <= m_UnitEntities.size());

	m_UnitEntities[index] = nullptr;
}

void Entity_TileCursor::ReleaseTileAtIndex(TileIndexes index)
{
	assert(index >= 0 && index <= m_Tiles.size());

	m_Tiles[index] = nullptr;
}

void Entity_TileCursor::ResetAndRescan(EntityIndexes unitIndex, TileIndexes tileIndex)
{
	//Clear held pointers
	for (auto& a : m_UnitEntities)
		a = nullptr;
	for (auto& a : m_Tiles)
		a = nullptr;

	//Rescan
	SearchForUnitAtCoordinates(unitIndex);
	SearchForTileObjectAtCoordinates(tileIndex);
}


Entity_GameUnit* Entity_TileCursor::GetUnitAtIndex(EntityIndexes index)
{
	assert(index >= 0 && index <= m_UnitEntities.size());

	return m_UnitEntities[index];
}

MT_Type1* Entity_TileCursor::GetTileAtIndex(TileIndexes index)
{
	assert(index >= 0 && index <= m_Tiles.size());

	return m_Tiles[index];
}

bool Entity_TileCursor::SearchForUnitAtCoordinates(EntityIndexes index)
{
	assert(index >= 0 && index <= m_UnitEntities.size());

	//Get the team manager
	SM_TeamsManager& tm = Game::GetGame()->GetGameplayManager().GetTeamsManager();

	Entity_GameUnit* unit = tm.SearchAllTeamsForUnitAtCoordinate(
		GetCoords().GetMapCoordinatesXi(),
		GetCoords().GetMapCoordinatesYi(),
		true
	);

	//If a unit was found
	if (unit)
	{
		//Null unit there currently and set new ptr
		m_UnitEntities[index] = nullptr;
		m_UnitEntities[index] = unit;

		//Unit found and set so return true
		return true;
	}

	//No unit found 
	return false;
}

bool Entity_TileCursor::SearchForTileObjectAtCoordinates(TileIndexes index)
{
	assert(index >= 0 && index <= m_Tiles.size());

	//Get map manager
	SM_TilemapManager& tm = Game::GetGame()->GetGameplayManager().GetTileManager();

	//Gets the tile at the given coordinates
	MT_Type1* tile = tm.GetMapTileByCoordinate(
		static_cast<unsigned>(GetCoords().GetMapCoordinatesXi()),
		static_cast<unsigned>(GetCoords().GetMapCoordinatesYi())
	);

	if (tile)
	{
		//Tile found so clear index and set
		m_Tiles[index] = nullptr;
		m_Tiles[index] = tile;

		//Tile found and set so return true
		return true;
	}

	msg_assert(false, "Cursor attempted to check outside of map range");
	//No tile found (this should never really be hit unless cursor is outside of the map for w/e reason)
	return false;
}

void Entity_TileCursor::SetUnitToIndex(EntityIndexes index, Entity_GameUnit* ptr)
{
	assert(index >= 0 && index <= m_UnitEntities.size());

	m_UnitEntities[index] = ptr;
}

void Entity_TileCursor::SetTileToIndex(TileIndexes index, MT_Type1* ptr)
{
	assert(index >= 0 && index <= m_Tiles.size());

	m_Tiles[index] = ptr;
}

void Entity_TileCursor::Init()
{
	//Reserve space equal to counts
	m_UnitEntities.reserve(EntityIndexes::SLOT_COUNT);
	m_Tiles.reserve(TileIndexes::TILE_COUNT);

	//Create a number of null pointers for use later
	for (unsigned i(0); i < EntityIndexes::SLOT_COUNT; ++i)
	{
		m_UnitEntities.push_back(nullptr);
	}
	for (unsigned i(0); i < TileIndexes::TILE_COUNT; ++i)
	{
		m_Tiles.push_back(nullptr);
	}
}

void Entity_TileCursor::Release()
{
	//Just null the pointers, releasing should be handled else where for these pointers
	for (auto& a : m_UnitEntities)
		a = nullptr;
	for (auto& a : m_Tiles)
		a = nullptr;
}