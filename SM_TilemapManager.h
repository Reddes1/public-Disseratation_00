#pragma once
#include "D3DUtils.h"
#include "RapidJSONLoaderUtils.h" //File Loading

#include "MT_Type1.h"

/*
	Sub-Manager that handles map loading, management and access for modes
*/

class SM_TilemapManager
{
public:

	////////////
	/// Data ///
	////////////

	enum TileLayerDefintions
	{
		TLD_TERRAIN_LAYER_00,
		TLD_TERRAIN_LAYER_01,
		TLD_DECORATIONS_LAYER_02,
		TLD_HIGHLIGHT_LAYER_03
	};

	//Store current relevant map details
	struct MapData
	{
		std::string m_MapName = "";
		DirectX::XMINT2 m_MapSize;
		DirectX::XMINT2 m_TileSize;
	};

	//////////////////
	/// Defintions ///
	//////////////////

	enum class LoaderTypes : unsigned short
	{
		MAP_LOADER_00
	};

	////////////////////
	/// Constructors ///
	////////////////////

	SM_TilemapManager();
	~SM_TilemapManager();

	//////////////////
	/// Operations ///
	//////////////////

	//Loads a map using a configuration file, and the texture name of a already loaded tileset.
	//Define what type of loader to use (specific to the map being loaded).
	void LoadMap(LoaderTypes loader, std::string& levelFP, std::string& tilesetName);
	//Renders each individual tile
	void Render(DirectX::SpriteBatch& sb, DirectX::DescriptorHeap* heap);
	//Updates each individual tile
	void Update(float dTime);
	//Releases current map array
	void Release();

	/*
		Highlight texture isn't setup by default, so load texture, frame & sprite index,
		and optional alpha value to apply to colour (if highlight not transparent by default)
	*/
	void InitTilesWithHighlightTexture(std::string& texName, unsigned frameIndex, unsigned spriteIndex, float alpha = 1.0f);

	//Purge all parent tile knowledge from each tile in the current map (can cause loop bugs)
	void ClearAllParentTiles();

	///////////
	/// Get ///
	///////////

	//Get the data on the currently loaded map
	const MapData& GetMapData() { return m_MapData; }

	//Finds a given tile using x/y coordinates to find its relative index in the container
	MT_Type1* GetMapTileByCoordinate(unsigned x, unsigned y);

	//Get direct look at map
	std::vector<MT_Type1*>& GetTileMap() { return m_TileMap; }

	///////////
	/// Set ///
	///////////

private:

	//////////////////
	/// Operations ///
	//////////////////
	
	void Init();

	//
	//Map Loading
	//

	//Type One Functions
	void MapLoaderTypeOne(std::string& levelFP, std::string& tilesetName);


	//
	//Utils
	//

	//Setups up pointers for a tiles neighbours
	void SetupNeighbourTiles();
	//Returns the container index of a tile relative to its direction from another tile
	size_t GetIndexInDirection(int dir, size_t x, size_t y, size_t maxX);


	////////////
	/// Data ///
	////////////

	//Retain information on the currently loaded maps tiles
	//so that live changes can occur if required
	//NYI
	
	//Map Container
	std::vector<MT_Type1*> m_TileMap;

	//Currently active map data
	MapData m_MapData;
	//Identifier for what mode currently owns the map
	int m_OwnerModeID = -1;
};
