#pragma once

#include <set>			//Container
#include <functional>	//Function ptrs

//Types
#include "MT_Type1.h"			//Primary tile
#include "Entity_GameUnit.h"	//Main game unit

/*
	Specialised pathfinder that uses a Monte-carlo based AI model for exploring viable tiles and highlighting them,
	and A* for displaying the path taken from point A to point B.
*/
class MT_Pathfinder_00
{
public:

	////////////
	/// Data ///
	////////////

	//Support type for use in pathfinder algorithm
	struct Node
	{
		Node() {}
		~Node() { m_Tile = nullptr; }

		MT_Type1* m_Tile = nullptr;
		float m_CostRemaining = -1.f;

		//A* Data
		//Distance from origin point
		float m_GCost = 0.f;
		//Distance to target
		float m_HCost = 0.f;
		//Sum of G+H
		float m_FCost = 0.f;
	
		//Comparison operator for set container
		bool operator<(const Node& ptr) const
		{
			//Grab and use tile coordinates as comparitor
			DirectX::XMFLOAT2 rCoords = m_Tile->GetCoordinates().GetMapCoordinatesXY();
			DirectX::XMFLOAT2 lCoords = ptr.m_Tile->GetCoordinates().GetMapCoordinatesXY();

			//Compare
			return (rCoords.x < lCoords.x) || ((!(lCoords.x < rCoords.x)) && (rCoords.y < lCoords.y));
		}

	};

	///////////////////
	/// Definitions ///
	///////////////////

	//Define the polices found in "MT_Pathfinder_Policies"
	enum class ValidationPolicies
	{
		BASIC_POLICY_00,
		POLICY_COUNT
	};

	//Algorithms defintions for finding tiles with certain parameters
	enum class FindTileAlgorithms : unsigned
	{
		FIND_MIN_X,
		FIND_MAX_X,
		FIND_MIN_Y,
		FIND_MAX_Y,
		COUNT
	};


	////////////////////
	/// Constructors ///
	////////////////////

	MT_Pathfinder_00() { Init(); }
	~MT_Pathfinder_00() { Release(); }


	//////////////////
	/// Operations ///
	//////////////////

	//Determine if the given tile is inside the manifest
	bool IsTileInManifest(MT_Type1* const tile);
	//Determine if tile is inside the given container
	bool IsTileInContainer(MT_Type1* tile, std::vector<MT_Type1*>& container);

	//Sets all manifest tiles to a frame index
	void SetManifestEffect(unsigned frameIndex, unsigned sprIndex);

	//
	// Monte-Carlo
	//

	//Using a tilemap, game unit and map length, explore the units surroundings, storing and highlighting all reachable tiles from the unit.
	void GeneratePathfinderGrid(std::vector<MT_Type1*>& tilemap , Entity_GameUnit* const unit , int mapLength, unsigned highlightFrameIndex);
	//Runs a validation check to discard worse outcomes (we only want the best possible paths)
	void NodeValidationCheck(const Node& node, std::set<Node>& nodes, unsigned neighbourIndex, unsigned highlightFrameIndex);
	//Release the currently held manifest, and disable any effects on the release
	void ReleaseManifest();


	//
	// A* 
	//

	//Runs unrestricted A* algorithm (doesn't require MC manifest) between start tile and end tile, with optional highlight
	bool FindShortestRouteUnrestricted(MT_Type1* startTile, MT_Type1* endTile, unsigned highlightIndex, unsigned sprIndex, bool enableHighlight = false);



	//Displays shortest route between the start coordinates and the target coordinates
	bool FindShortestRoute(MT_Type1* const startTile, MT_Type1* const endTile, unsigned highlightIndex, unsigned sprIndex);
	//Calculates the given tiles A* costs using an origin point and target point
	void CalculateCosts(MT_Type1* tile, DirectX::XMFLOAT2& origin, DirectX::XMFLOAT2& target);

	//Finds the lowest F cost tile in current unevaluated set
	void FindLowestFCostTile(MT_Type1*& emptyTile, std::vector<MT_Type1*>& unevaluated);
	//Remove matching tile from container
	void RemoveTileFromContainer(MT_Type1*& tile, std::vector<MT_Type1*>& container);

	//Enables the grid effect on each tile in the path
	void DrawPath(MT_Type1*& endTile, unsigned highlightIndex, unsigned sprIndex);
	//Disables A* path highlights by following string of linked tiles (uses stored tile if told)
	bool DisablePath(bool useStoredEndpoint = true, MT_Type1* endTile = nullptr);

	//Finds first tile from end point of A* algorithm that overlaps with MC manifest
	MT_Type1* GetFirstOverlappingTile(bool useStoredEndpoint = true, MT_Type1* endTile = nullptr);

	//
	// Utility
	//

	//Returns tile in manifest that matches algorithm call
	MT_Type1* GetTileViaAlgorithm(FindTileAlgorithms algorithmID);


	///////////
	/// Get ///
	///////////

	std::set<MT_Type1*>& GetManifest() { return m_Manifest; }
	size_t GetManifestSize() { return m_Manifest.size(); }
	MT_Type1* GetRandomTileInManifest();

	///////////
	/// Set ///
	///////////

	//Sets the validation function to an internally hosted function
	void SetValidationPolicy(ValidationPolicies policyIndex);
	

private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();
	void Release();

	//
	// Validation Policies
	//

	//Basic, ground only unit check
	bool ValidationPolicy_00(const Node& node, MT_Type1* const tile, Entity_GameUnit* const unit);

	//
	// Utilities
	//

	//Returns tile with lowest X
	
	////////////
	/// Data ///
	////////////

	//Holds a manifest of tiles known to the pathfinder through algorithm
	std::set<MT_Type1*> m_Manifest;

	//Hold current end tile for last run A* algorithm
	MT_Type1* m_AStarEndPointTile = nullptr;

	//This holds and runs the currently set validation process for determining what constitutes a valid tile
	bool(MT_Pathfinder_00::* m_ValidationFunction)(const Node&, MT_Type1* const, Entity_GameUnit* const);
};