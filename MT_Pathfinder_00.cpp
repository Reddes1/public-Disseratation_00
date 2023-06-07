#include "MT_Pathfinder_00.h"
#include "GeneralUtils.h"	//Invert Check

#include <numeric>	//STL remove_if

void MT_Pathfinder_00::GeneratePathfinderGrid(std::vector<MT_Type1*>& tilemap, Entity_GameUnit* const unit, int mapLength, unsigned highlightFrameIndex)
{

	//
	// Setup
	//

	//Create two holding containers
	std::set<Node> newNodes;
	std::set<Node> tempNodes;

	//Grab unit coordinates as int
	DirectX::XMINT2 coords = { (int)unit->GetCoords().GetMapCoordinatesX(), (int)unit->GetCoords().GetMapCoordinatesY() };

	//Add root node and set it up
	Node rootNode;
	unsigned index = (unsigned)(coords.x + coords.y) + coords.y * mapLength;
	assert(index <= tilemap.size());
	rootNode.m_Tile = tilemap[index];
	rootNode.m_CostRemaining = unit->GetUnitData().m_MoveSpeed;


	//
	// Algorithm Pre-amble
	//

	//Check root node neighbours (N, E, S, W), and run part of the algorithm to get some explorable options
	for (unsigned i(0); i < (unsigned)Directional4Way::COUNT; ++i)
	{
		//Check for existing neighbour (We manipulate the index a little as neighbour we want are stored on odd indexes)
		if (rootNode.m_Tile->GetNeighbourTileAtIndex(i * 2))
		{
			//Run the validation policy			
			if ((this->*m_ValidationFunction)(rootNode, rootNode.m_Tile->GetNeighbourTileAtIndex(i * 2), unit))
			{
				//Now run validation check
				NodeValidationCheck(rootNode, newNodes, i * 2, highlightFrameIndex);
			}
		}
	}

	//Add the starting node to the manifest
	m_Manifest.insert(rootNode.m_Tile);
	//Add starting nodes to manifest
	for (auto& a : newNodes)
		m_Manifest.insert(a.m_Tile);

	
	//
	// Main Algorithm
	//

	//Using the starting nodes, run algorithm to explore and evaluate till all possible nodes have been found and evaluated
	while (newNodes.size() != 0)
	{
		//For each unresolved node, check for neighbours and validity
		for (auto& node : newNodes)
		{
			//Check root node neighbours (N, E, S, W), and run part of the algorithm to get some explorable options
			for (unsigned i(0); i < (unsigned)Directional4Way::COUNT; ++i)
			{
				//Check for existing neighbour (We manipulate the index a little as neighbour we want are stored on odd indexes)
				if (node.m_Tile->GetNeighbourTileAtIndex(i * 2))
				{
					//Run the validation policy			
					if ((this->*m_ValidationFunction)(node, node.m_Tile->GetNeighbourTileAtIndex(i * 2), unit))
					{
						//Now run validation check
						NodeValidationCheck(node, tempNodes, i * 2, highlightFrameIndex);
					}
				}
			}
		}

		//Store new nodes for next cycle (if any)
		newNodes = tempNodes;
		//Clear temp for next cycle
		tempNodes.clear();
		//Extract the tiles from the newly added nodes into the manifest
		for (auto& a : newNodes)
			m_Manifest.insert(a.m_Tile);

	}

	//
	// Post Cleanup
	//

	//For gameplay purposes, remove the root node
	rootNode.m_Tile->DisableHighlight();
	m_Manifest.erase(rootNode.m_Tile);

}

void MT_Pathfinder_00::NodeValidationCheck(const Node& node, std::set<Node>& nodes, unsigned neighbourIndex, unsigned highlightFrameIndex)
{
	//Create a new node object
	Node newNode;

	//Setup with the minimum requirements before proceeding
	newNode.m_CostRemaining = node.m_CostRemaining - node.m_Tile->GetNeighbourTileAtIndex(neighbourIndex)->GetProperties().m_MoveCost;
	newNode.m_Tile = node.m_Tile->GetNeighbourTileAtIndex(neighbourIndex);

	/*
		As this algorithm doesn't evaluate on how it finds a tile, only that it deos, it may reach a tile
		with a less than optimal route in regards to cost, as well as overriding already determined routes,
		which may or may not be better. If a match is found, an evaluation is done to determine the better
		option of the two by comparing costs, keeping the higher remaining cost and discarding other.

		If the new node is better then the old is discarded, otherwise insert the node as normal.
	*/

	//Search for a tile match
	std::set<Node>::iterator it = nodes.find(newNode);
	if (it != nodes.end())
	{
		//Match found so determine which is better
		if (it->m_CostRemaining < newNode.m_CostRemaining)
		{
			//Discard the existing node
			nodes.erase(it);
			
			//Enable the tile's grid effect
			newNode.m_Tile->EnableHighlight(highlightFrameIndex);

			//Insert the new node
			nodes.insert(newNode);
		}
	}
	//No match, so this is the first time has been checked
	else
	{
		//Enable the tile's grid effect
		newNode.m_Tile->EnableHighlight(highlightFrameIndex);

		//Insert the new node
		nodes.insert(newNode);
	}
	
}

void MT_Pathfinder_00::ReleaseManifest()
{
	for (auto& a : m_Manifest)
	{
		//Disable grid
		a->DisableHighlight();
		a->GetAStarData().m_ParentTile = nullptr;
		
	}	
	m_Manifest.clear();
}


bool MT_Pathfinder_00::FindShortestRouteUnrestricted(MT_Type1* startTile, MT_Type1* endTile, unsigned highlightIndex, unsigned sprIndex, bool enableHighlight)
{
	//Hold unevaluated tiles here
	std::vector<MT_Type1*> unevaluated;
	//Hold evaluated tiles here
	std::vector<MT_Type1*> evaluated;

	//Setup starting node
	CalculateCosts(startTile,
		startTile->GetCoordinates().GetMapCoordinatesXY(),
		endTile->GetCoordinates().GetMapCoordinatesXY()
	);

	//Push starting node into unevaluated
	unevaluated.push_back(startTile);

	//Till path is found
	while (unevaluated.size() != 0)
	{
		//Create empty holding tile
		MT_Type1* currTile = nullptr;

		//Grab the lowest F cost tile
		FindLowestFCostTile(currTile, unevaluated);
		//Remove the current node from unevaluated
		RemoveTileFromContainer(currTile, unevaluated);

		//Store the tile under evaluated
		evaluated.push_back(currTile);

		//Check if the current note is the target node
		if ((currTile->GetCoordinates().GetMapCoordinatesX() == endTile->GetCoordinates().GetMapCoordinatesX()) &&
			(currTile->GetCoordinates().GetMapCoordinatesY() == endTile->GetCoordinates().GetMapCoordinatesY()))
		{
			//Store the end point for later
			m_AStarEndPointTile = currTile;
			//Enable the grid effect down the line on each tile in the path
			DrawPath(currTile, highlightIndex, sprIndex);

			//Exit algorithm
			return true;
		}
		//Start looking at the neighbouring tiles
		for (unsigned i(0); i < (unsigned)Directional4Way::COUNT; ++i)
		{
			/*
				The algorithm needs to check that tiles it evaluates aren't occupied so that it doesn't attempt
				to path through tiles that I cant. However, the target tile WILL be occupied and as this algorithm
				will only be called to path to a unit that is out of range, we can bypass the problem entirely.
				This pre-check ensures that the target tile isn't passed over because it fails the primary evaluation.
				Dev Note: Need to sit down and evaluate this whole process(ideally the same time that validation policies are properly
				introduced to replace the below the if statements).
			*/
			if (currTile->GetNeighbourTileAtIndex(i * 2) == endTile)
			{
				MT_Type1* newTile = currTile->GetNeighbourTileAtIndex(i * 2);
				//Store the end point for later
				m_AStarEndPointTile = newTile;
				newTile->GetAStarData().m_ParentTile = currTile;
				DrawPath(newTile, highlightIndex, sprIndex);

				//Path routed, so exit true
				return true;
			}


			//Validate if the neighbour requires evaluating or not
			if (currTile->GetNeighbourTileAtIndex(i * 2) &&									//Is there a neighbour?
				!currTile->GetNeighbourTileAtIndex(i * 2)->GetProperties().m_Impassable &&  //Is the tile not impassable
				!currTile->GetNeighbourTileAtIndex(i * 2)->GetProperties().m_IsOccupied &&	//Is the tile not occupied
				!IsTileInContainer(currTile->GetNeighbourTileAtIndex(i * 2), evaluated)		//Has the tile already been evaluated
				)
			{
				//Create a new tile and calculate information for the node
				MT_Type1* newTile = currTile->GetNeighbourTileAtIndex(i * 2);

				//Check to see if this tile is the target tile, and early out if yes
				if (newTile == endTile)
				{
					//Store the end point for later
					m_AStarEndPointTile = newTile;
					//Set parent for end point
					newTile->GetAStarData().m_ParentTile = currTile;
					//Enable the grid effect down the line on each tile in the path
					DrawPath(newTile, highlightIndex, sprIndex);

					//Exit algorithm
					return true;
				
				}

				CalculateCosts(newTile, startTile->GetCoordinates().GetMapCoordinatesXY(), endTile->GetCoordinates().GetMapCoordinatesXY());

				//Check if the H cost is lower OR not inside the open nodes container
				if (newTile->GetAStarData().m_HCost < currTile->GetAStarData().m_HCost ||
					!IsTileInContainer(newTile, unevaluated))
				{
					//Set the new tiles parent to the currently examined tile
					newTile->GetAStarData().m_ParentTile = currTile;

					//If the current node isn't the last in the container
					if (!IsTileInContainer(newTile, unevaluated))
						unevaluated.push_back(newTile);

				}
			}
		}
	}

	//No route was found, so return false
	return false;
}

bool MT_Pathfinder_00::FindShortestRoute(MT_Type1* const startTile, MT_Type1* const endTile, unsigned highlightIndex, unsigned sprIndex)
{
	//Hold unevaluated tiles here
	std::vector<MT_Type1*> unevaluated;
	//Hold evaluated tiles here
	std::vector<MT_Type1*> evaluated;

	//Setup starting node
	CalculateCosts(startTile,
		startTile->GetCoordinates().GetMapCoordinatesXY(),
		endTile->GetCoordinates().GetMapCoordinatesXY()
	);

	//Push starting node into unevaluated
	unevaluated.push_back(startTile);

	//Till path is found
	while (unevaluated.size() != 0)
	{
		//Create empty holding tile
		MT_Type1* currTile = nullptr;

		//Grab the lowest F cost tile
		FindLowestFCostTile(currTile, unevaluated);
		//Remove the current node from unevaluated
		RemoveTileFromContainer(currTile, unevaluated);
		
		//Store the tile under evaluated
		evaluated.push_back(currTile);

		//Check if the current note is the target node
		if ((currTile->GetCoordinates().GetMapCoordinatesX() == endTile->GetCoordinates().GetMapCoordinatesX()) &&
			(currTile->GetCoordinates().GetMapCoordinatesY() == endTile->GetCoordinates().GetMapCoordinatesY()))
		{
			//Store the end point for later
			m_AStarEndPointTile = currTile;
			//Enable the grid effect down the line on each tile in the path
			DrawPath(currTile, highlightIndex, sprIndex);

			//Exit algorithm
			return true;
		}

		//Start looking at the neighbouring tiles
		for (unsigned i(0); i < (unsigned)Directional4Way::COUNT; ++i)
		{
			//Validate if the neighbour requires evaluating or not
			if (currTile->GetNeighbourTileAtIndex(i * 2) &&									//Is there a neighbour?
				IsTileInManifest(currTile->GetNeighbourTileAtIndex(i * 2)) &&				//Is the tile in the manifest
				!currTile->GetNeighbourTileAtIndex(i * 2)->GetProperties().m_Impassable &&  //Is the tile not impassable
				!IsTileInContainer(currTile->GetNeighbourTileAtIndex(i * 2), evaluated)		//Has the tile already been evaluated
				)
			{
				//Create a new tile and calculate information for the node
				MT_Type1* newTile = currTile->GetNeighbourTileAtIndex(i * 2);

				//Check to see if this tile is the target tile, and early out if yes
				if (newTile == endTile)
				{
					//Store the end point for later
					m_AStarEndPointTile = newTile;
					//Set parent for end point
					newTile->GetAStarData().m_ParentTile = currTile;
					//Enable the grid effect down the line on each tile in the path
					DrawPath(newTile, highlightIndex, sprIndex);

					//Exit algorithm
					return true;
				}

				CalculateCosts(newTile, startTile->GetCoordinates().GetMapCoordinatesXY(), endTile->GetCoordinates().GetMapCoordinatesXY());

				//Check if the H cost is lower OR not inside the open nodes container
				if (newTile->GetAStarData().m_HCost < currTile->GetAStarData().m_HCost ||
					!IsTileInContainer(newTile, unevaluated))
				{
					//Set the new tiles parent to the currently examined tile
					newTile->GetAStarData().m_ParentTile = currTile;

					//If the current node isn't the last in the container
					if (!IsTileInContainer(newTile, unevaluated))
						unevaluated.push_back(newTile);
					
				}
			}
		}
	}

	//No route was found, so return false
	return false;
}

void MT_Pathfinder_00::CalculateCosts(MT_Type1* tile, DirectX::XMFLOAT2& origin, DirectX::XMFLOAT2& target)
{
	//
	// Calc Distance From Origin
	//

	float distValAdj = 1.0f;
	float moveValAdj = 1.0f;

	//Get the X/Y distance difference
	float xDist = origin.x - tile->GetCoordinates().GetMapCoordinatesX();
	float yDist = origin.y - tile->GetCoordinates().GetMapCoordinatesY();
	
	//Flip any negatives for the sake of the algorithm
	InvertIfNegative(xDist);
	InvertIfNegative(yDist);



	//Set G Cost
	tile->GetAStarData().m_GCost = ((xDist * distValAdj) + (yDist * distValAdj)) * (tile->GetProperties().m_MoveCost) * moveValAdj;

	//
	// Calc Distance From Target
	//

	//Get the X/Y distance difference
	xDist = target.x - tile->GetCoordinates().GetMapCoordinatesX();
	yDist = target.y - tile->GetCoordinates().GetMapCoordinatesY();

	//Flip any negatives for the sake of the algorithm
	InvertIfNegative(xDist);
	InvertIfNegative(yDist);

	//Set H Cost
	tile->GetAStarData().m_HCost = xDist + yDist;

	//Set F Cost
	tile->GetAStarData().UpdateFCost();
}

void MT_Pathfinder_00::FindLowestFCostTile(MT_Type1*& tile, std::vector<MT_Type1*>& unevaluated)
{
	//Set tile to first tile in container
	tile = unevaluated[0];

	//Cycle through each tile
	for (auto& a : unevaluated)
	{
		//Run some checks, and set tile to unevaluated if checks pass
		if (a->GetAStarData().m_FCost < tile->GetAStarData().m_FCost ||		//Is unevaluated F cost lower?
		   (a->GetAStarData().m_FCost == tile->GetAStarData().m_FCost &&    //Is unevaluated F cost equal & H cost lower
			a->GetAStarData().m_HCost < tile->GetAStarData().m_HCost))
		{
			tile = a;
		}
	}
}

void MT_Pathfinder_00::RemoveTileFromContainer(MT_Type1*& tile, std::vector<MT_Type1*>& container)
{
	for (unsigned i(0); i < container.size(); ++i)
		if (tile == container[i])
			container.erase(container.begin() + i);
}

void MT_Pathfinder_00::DrawPath(MT_Type1*& endTile, unsigned highlightIndex, unsigned sprIndex)
{
	//Setup
	bool pathComplete = false;
	MT_Type1* currTile = endTile;

	//Till all the tiles in the path has been seen
	while (!pathComplete)
	{
		//Set the effect if flagged
		currTile->GetSpriteAtIndex(sprIndex).SetFrame(highlightIndex);

		//Check if the tile has a parent
		if (currTile->GetAStarData().m_ParentTile)
			currTile = currTile->GetAStarData().m_ParentTile;
		//Origin point found, so end
		else
			pathComplete = true;
	}

}

bool MT_Pathfinder_00::DisablePath(bool useStoredEndpoint, MT_Type1* endTile)
{
	//Setup
	bool done = false;
	MT_Type1* currTile = nullptr;

	if (useStoredEndpoint)
		currTile = m_AStarEndPointTile;
	else
	{
		assert(endTile);
		currTile = endTile;
	}
	
	//If no assigned tile, not path was generated so exit
	if (currTile == nullptr)
		return false;

	//While A* path is still active
	while (!done)
	{
		//Disable tile highlight effect
		currTile->DisableHighlight();

		//If the current tile has a parent tile (i.e another link in the chain)
		if (currTile->GetAStarData().m_ParentTile)
		{
			//Temp hold the current tile
			MT_Type1* temp = currTile;
			//Switch evaluation tile
			currTile = currTile->GetAStarData().m_ParentTile;
			//Clear previous tiles parent tile
			temp->GetAStarData().m_ParentTile = false;
		}
		else
			done = true;
	}

	//Clear stored tile
	m_AStarEndPointTile = nullptr;

	return true;
}

MT_Type1* MT_Pathfinder_00::GetFirstOverlappingTile(bool useStoredEndpoint, MT_Type1* endTile)
{
	assert(!m_Manifest.empty());

	//Setup
	bool done = false;
	MT_Type1* currTile = nullptr;

	if (useStoredEndpoint)
	{
		//If no end point was stored, then no valid tile was found around the target
		if (m_AStarEndPointTile == nullptr)
			return nullptr;
		else
		{
			//Set new tile and clear old
			currTile = m_AStarEndPointTile;
		}
			
	}
	else if (endTile)
	{
		currTile = endTile;
	}
	else
	{
		//No tile able to be used
		return false;
	}


	//While A* path not fully navigated
	while (!done)
	{
		//Check the current tile against the manifest, return the tile if true
		if (IsTileInManifest(currTile))
		{
			return currTile;
		}
		//Cycle to the next tile (if there is one)
		else
		{
			if (currTile->GetAStarData().m_ParentTile)
			{
				currTile = currTile->GetAStarData().m_ParentTile;
			}
			else
				done = true;
		}
	}

	//No tile found, return false
	return nullptr;
}

MT_Type1* MT_Pathfinder_00::GetTileViaAlgorithm(FindTileAlgorithms algorithmID)
{
	assert(!m_Manifest.empty());

	switch (algorithmID)
	{
	case FindTileAlgorithms::FIND_MIN_X:
		return false;
		break;

	case FindTileAlgorithms::FIND_MAX_X:
		return false;
		break;

	case FindTileAlgorithms::FIND_MIN_Y:
		return false;
		break;

	case FindTileAlgorithms::FIND_MAX_Y:
		return false;
		break;

	}

	return nullptr;
}

bool MT_Pathfinder_00::IsTileInManifest(MT_Type1* const tile)
{
	//Attempt to find a match for the given tile and return true if found
	if (std::find(m_Manifest.begin(), m_Manifest.end(), tile) != m_Manifest.end())
		return true;
	else
		return false;
}

bool MT_Pathfinder_00::IsTileInContainer(MT_Type1* tile, std::vector<MT_Type1*>& container)
{
	//Cycle through container and return true if match found
	for (auto& a : container)
		if (a == tile)
			return true;

	//No tile found, return false
	return false;
}

void MT_Pathfinder_00::SetManifestEffect(unsigned frameIndex, unsigned sprIndex)
{
	for (auto& a : m_Manifest)
		a->GetSpriteAtIndex(sprIndex).SetFrame(frameIndex);
}

bool MT_Pathfinder_00::ValidationPolicy_00(const Node& node, MT_Type1* const tile, Entity_GameUnit* const unit)
{
	MT_Properties_Type1& tileProps = tile->GetProperties();

	return ((node.m_CostRemaining - tileProps.m_MoveCost >= 0) &&			//Is there enough cost remaining?
			(tileProps.m_IsOccupied == false) &&							//Is the tile occupied?
			(tileProps.m_Impassable == false) &&							//Is the tile impassable?
			(unit->GetUnitData().m_TerrainID == tileProps.m_TileTypeID)		//Is the tile type compatible with the unit (land =/= land)
		);
}

MT_Type1* MT_Pathfinder_00::GetRandomTileInManifest()
{
	unsigned offset = std::rand() % (m_Manifest.size() - 1);

	auto it = m_Manifest.cbegin();
	std::advance(it, offset);

	return *it;
}

void MT_Pathfinder_00::SetValidationPolicy(ValidationPolicies policyIndex)
{
	switch (policyIndex)
	{
	case ValidationPolicies::BASIC_POLICY_00:
		m_ValidationFunction = &MT_Pathfinder_00::ValidationPolicy_00;
		break;
	}
}

void MT_Pathfinder_00::Init()
{
	//Set default validation policy
	m_ValidationFunction = &MT_Pathfinder_00::ValidationPolicy_00;
}

void MT_Pathfinder_00::Release()
{
	m_Manifest.clear();
}
