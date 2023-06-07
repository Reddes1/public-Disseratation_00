#pragma once

#include "MT_Type1.h"		 //Primary Game Tile
#include "Entity_GameUnit.h" //Primary Game Unit	

/*
	Simple manager designed to generate and hold a manifest of tiles that represent a
	range of game unit.
*/
class MT_TargetingManager
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////

	enum TargetingTypes : unsigned
	{
		TYPE_DIAMOND_00,
		TYPE_DIAMOND_01,
		TYPE_COUNT
	};

	////////////////////
	/// Constructors ///
	////////////////////

	MT_TargetingManager() { Init(); }
	~MT_TargetingManager() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Runs a given algorithm to generate a manifest of tiles in a given range
	void RunTargetingAlgorithm(unsigned algorithmType, std::vector<MT_Type1*>& tiles, int mapLength,
		DirectX::XMFLOAT2 startingCoords, unsigned gridEffectID, int unitRange);

	//Check to see if a given tile is inside the current manifest
	bool IsTileInManifest(MT_Type1* const tile);
	
	//Clears the current manifest
	void ReleaseManifest();

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
	void Release();

	//
	// Targeting Types Types
	//

	void DiamondAlgorithm_00(std::vector<MT_Type1*>& tiles, int mapLength, DirectX::XMFLOAT2 startingCoords,
		 unsigned gridEffectID, int unitRange, bool excludeSelf = true);
	
	////////////
	/// Data ///
	////////////

	std::vector<MT_Type1*> m_Manifest;

};