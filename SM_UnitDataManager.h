#pragma once

#include "GameConstants.h"		//Unit Information
#include "GameTypes.h"			//Data Types

#include <string>
#include <unordered_map>

/*
	Manager class for loading and storing game unit information for use in game.
	Stores upto ONE set of units and matchup data for access, as well as access options for the data.
	Index options assume uniform ID system amongst units (use enum).
*/
class SM_UnitDataManager
{
public:


	////////////
	/// Data ///
	////////////


	//////////////////
	/// Defintions ///
	//////////////////

	typedef std::unordered_map<unsigned, UnitData> UnitDataMap;
	typedef std::unordered_map<unsigned, UnitMatchupData> MatchupDataMap;

	////////////////////
	/// Constructors ///
	////////////////////

	SM_UnitDataManager() {}
	~SM_UnitDataManager() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	void LoadUnitData(std::string& unitsFP);
	void LoadMatchupData(std::string& matchesFP);

	///////////
	/// Get ///
	///////////

	//Gets read-only access to a units data
	const UnitData* GetUnitDataByIDConst(unsigned index);
	//Gets copy of a units data
	UnitData GetUnitDataByID(unsigned index);

	//Gets read-only access to a units matchup data
	UnitMatchupData* const GetMatchupData(unsigned index);
	//Gets copy on specific multiplier that one unit has against another
	float GetDamageModAgainstUnit(unsigned currentUnitIndex, unsigned targetUnitIndex);

	///////////
	/// Set ///
	///////////



private:

	//////////////////
	/// Operations ///
	//////////////////

	void Release();

	////////////
	/// Data ///
	////////////

	UnitDataMap m_DataMap;
	MatchupDataMap m_MatchupDataMap;

};