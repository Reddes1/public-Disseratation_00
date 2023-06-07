#pragma once

#include "D3DUtils.h"

//EMs
#include "EM_Sprite.h"
#include "EM_EntityInterface.h"
#include "EM_Coordinates.h"


#include "MT_Type1.h"				//Main Type Type
#include "Entity_GameUnit.h"		//Main Game Unit


/*
	Fixed distance cursor class. Built for use in navigated a tiled based environment and displaying
	tile information aswell as unit information. Has operational capacity to store pointers to tiles or
	units found for personal use or mechanical use.
*/
class Entity_TileCursor : public EM_EntityInterface2D
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////

	/*
		Class will reserve and create a number of null pointers upto the count.
		Add defintions as required
	*/

	enum EntityIndexes : unsigned
	{
		FIRST_TARGET,
		SECOND_TARGET,
		SLOT_COUNT
	};

	enum TileIndexes : unsigned
	{
		FIRST_TILE,
		SECOND_TILE,
		TILE_COUNT
	};

	////////////////////
	/// Constructors ///
	////////////////////

	Entity_TileCursor() { Init(); }
	~Entity_TileCursor() { Release(); }

	/////////////////
	/// Overrides ///
	/////////////////
		
	void Update(GameTimer& gt) override;
	void Render(DirectX::SpriteBatch& batch, DirectX::DescriptorHeap* heap) override;

	//////////////////
	/// Operations ///
	//////////////////

	//Initialise cursor with basics it needs to work
	void InitialiseCursor(std::string& textureFP, int frameIndex, float depthLayer);

	//Moves the cursor in a given direction
	void Move(Directional4Way direction);
	//Moves the cursor to a location via coordinate
	void MoveToCoordinate(DirectX::XMFLOAT2& newCoordinates);

	//Releases the unit held at a given index
	void ReleaseUnitAtIndex(EntityIndexes index);
	//Releases the tile held at a given index
	void ReleaseTileAtIndex(TileIndexes index);

	//Clears all held pointers and re-scans to the indexes given
	void ResetAndRescan(EntityIndexes unitIndex = EntityIndexes::FIRST_TARGET, TileIndexes tileIndex = TileIndexes::FIRST_TILE);

	///////////
	/// Get ///
	///////////

	Entity_GameUnit* GetUnitAtIndex(EntityIndexes index);
	MT_Type1* GetTileAtIndex(TileIndexes index);

	EM_Sprite& GetMainSprite() { return m_MainSprite; }

	/*
		Searches the team manager for a unit at the current coordinates
		Optional flag to assign to position in the container (defaults to index 0)
		False returns indicate no unit was found at the coordinates.
	*/
	bool SearchForUnitAtCoordinates(EntityIndexes index = EntityIndexes::FIRST_TARGET);

	/*
		Searches the tile manager for a tile matching the current coordinates.
		Optional flag to assign to position in the container (defaults to index 0).
		False returns indicates the cursor is outside of the field of play.
	*/
	bool SearchForTileObjectAtCoordinates(TileIndexes index = TileIndexes::FIRST_TILE);


	///////////
	/// Set ///
	///////////

	void SetUnitToIndex(EntityIndexes index, Entity_GameUnit* ptr);
	void SetTileToIndex(TileIndexes index, MT_Type1* ptr);
	void SetClampingValues(DirectX::XMINT2& clamp) { m_ClampCoords = clamp; }
	void SetClampingValues(int x, int y) { m_ClampCoords.x = x; m_ClampCoords.y = y; }
	void SetTileSize(DirectX::XMINT2& tileSize) { m_TileSize = tileSize; }
	void SetTileSize(int x, int y) { m_TileSize.x = x; m_TileSize.y = y; }


private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();
	void Release();

	////////////
	/// Data ///
	////////////

	//Cursor Sprite
	EM_Sprite m_MainSprite;

	//Holds a number of entity pointers for easy access
	std::vector<Entity_GameUnit*> m_UnitEntities;
	//Holds a number of tile pointers for easy access
	std::vector<MT_Type1*> m_Tiles;

	//Stores map size of clamping purposes
	DirectX::XMINT2 m_ClampCoords = { 25, 25 };
	//Stores the tile size for positional alignment purposes
	DirectX::XMINT2 m_TileSize = { 0, 0 };

};


