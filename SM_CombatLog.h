#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "Entity_GameUnit.h"	//Primary Game Unit
#include "TeamEntry.h"

/*
	Manager keeps file handle open and takes strings to output to file. Can be passed any gameplay related
	outputs such as moving, attacking, deaths etcs
*/
class SM_CombatLog
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////

	enum class EventCodes : unsigned
	{
		UNIT_DEATH_00,
		UNIT_DEATH_01,
		UNIT_DAMAGED_00,
		UNIT_DAMAGED_01,
		FL_STATE_00,
		FL_STATE_01
	};

	////////////////////
	/// Constructors ///
	////////////////////

	~SM_CombatLog() { ReleaseHandle(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Start combat logging process (opens new file called "CombatLog_DataHere")
	bool StartCombatLog();
	//Starts combat logging process, but using existing file
	bool StartExistingCombatLog(std::string fp);
	//Release File Handle (Call only when done, or when starting new combatlog)
	bool ReleaseHandle();


	//////////////////
	/// Log Events ///
	//////////////////

	//Logs a units death
	bool LogUnitDeath_00(Entity_GameUnit* const deadUnit);
	bool LogUnitDeath_01(Entity_GameUnit* const attackingUnit, Entity_GameUnit* const deadUnit);

	bool LogDamageDealt_00(Entity_GameUnit* const attackingUnit, Entity_GameUnit* const defendingUnit, float dmg);
	bool LogUnitFL_00(Entity_GameUnit* const unit, Entity_GameUnit* const targetUnit);
	bool LogTeamFL_00(TeamEntry* const team);

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

	//Shortcut some common string appends here
	void AppendEventCode(std::stringstream& ss, EventCodes code);
	void AppendNameTeamID(std::stringstream& ss, Entity_GameUnit* const unit);
	void AppendQuickFLDataUnit(std::stringstream& ss, Entity_GameUnit* const unit);
	void AppendQuickFLDataTeam(std::stringstream& ss, TeamEntry* const team);


	////////////
	/// Data ///
	////////////

	//File Handle
	std::ofstream m_FileHandle;

	//Store current filepath
	std::string m_FileHandleFP = "N/A";
	//File Location
	std::string m_FileLocation = "data/logs/";
	//Default filepath append name
	std::string m_FileAppendName = "CombatLog_";
	//File format
	std::string m_FileFormat = ".txt";

};