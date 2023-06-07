#pragma once


#include "MT_Type1.h"		 //Primary Game Tile
#include "Entity_GameUnit.h" //Primary Game Unit	


/*
	Dedicated combat simulation manager.
*/
class SM_CombatManager
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////


	////////////////////
	/// Constructors ///
	////////////////////

	SM_CombatManager() { }
	~SM_CombatManager() { }

	//////////////////
	/// Operations ///
	//////////////////

	//Runs complete combat simulation
	bool RunCombatSimulation(Entity_GameUnit* unitOne, Entity_GameUnit* unitTwo, MT_Type1* tileOne, MT_Type1* tileTwo);


	///////////
	/// Get ///
	///////////

	//Gets a sample damage calculation (no variance) using units and tiles they're on.
	float GetSampleDamageCalculation(Entity_GameUnit* unitOne, Entity_GameUnit* unitTwo, MT_Type1* tileOne, MT_Type1* tileTwo);


	///////////
	/// Set ///
	///////////


private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();
	void Release();
	
	////////////
	/// Data ///
	////////////


};