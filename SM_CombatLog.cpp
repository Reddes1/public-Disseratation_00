#include "SM_CombatLog.h"

#include <sstream>

bool SM_CombatLog::StartCombatLog()
{
	//Construct filepath using date time
	time_t tm = time(NULL);
	struct tm* currTime = localtime(&tm);
	std::string str(asctime(currTime));

	//Scrub unwanted/invalid symbols
	str.erase(std::remove(str.begin(), str.end(), ':'), str.end());
	str.pop_back();

	//Construct filepath
	m_FileHandleFP = m_FileLocation + m_FileAppendName + str + m_FileFormat;

	//Open filepath
	m_FileHandle.open(m_FileHandleFP);

	//Error Checking
	msg_assert(m_FileHandle, "CombatLog Failed to Initialise");
	if (m_FileHandle)
		return true;
	else
		return false;

	return false;
}

bool SM_CombatLog::StartExistingCombatLog(std::string fp)
{
	//Open filepath
	m_FileHandle.open(fp);

	//Error Checking
	msg_assert(m_FileHandle, "CombatLog Failed to Initialise");
	if (m_FileHandle)
		return true;
	else
		return false;

	return false;
}

bool SM_CombatLog::ReleaseHandle()
{
	m_FileHandle.close();
	return true;
}

bool SM_CombatLog::LogUnitDeath_00(Entity_GameUnit* const deadUnit)
{
	//Construct String
	std::stringstream ss;
	AppendEventCode(ss, EventCodes::UNIT_DEATH_00);

	ss << deadUnit->GetUnitData().m_UnitName << "[Team " << deadUnit->GetTeamID() << "] has died!\n";

	m_FileHandle << ss.str();

	return true;
}

bool SM_CombatLog::LogUnitDeath_01(Entity_GameUnit* const attackingUnit, Entity_GameUnit* const deadUnit)
{
	//Construct String
	std::stringstream ss;

	AppendEventCode(ss, EventCodes::UNIT_DEATH_01);
	AppendNameTeamID(ss, attackingUnit);
	ss << " has killed ";
	AppendNameTeamID(ss, deadUnit);
	ss << ".\n";

	m_FileHandle << ss.str();

	return true;
}

bool SM_CombatLog::LogDamageDealt_00(Entity_GameUnit* const attackingUnit, Entity_GameUnit* const defendingUnit, float dmg)
{
	//Construct String
	std::stringstream ss;

	AppendEventCode(ss, EventCodes::UNIT_DAMAGED_00);
	AppendNameTeamID(ss, attackingUnit);
	ss << " attacked ";
	AppendNameTeamID(ss, defendingUnit);
	ss << " for " << dmg << " damage.\n";

	m_FileHandle << ss.str();

	return true;
}

bool SM_CombatLog::LogUnitFL_00(Entity_GameUnit* const unit, Entity_GameUnit* const targetUnit)
{
	//Construct String
	std::stringstream ss;

	AppendEventCode(ss, EventCodes::FL_STATE_00);
	AppendNameTeamID(ss, unit);
	ss << " fuzzy logic module resolved state as ";
	ss << "[State Name: " << unit->GetFLController()->GetRuleset().GetActiveStateName() << "]";
	ss << "[State ID: " << unit->GetFLController()->GetRuleset().GetActiveState() << "]";
	ss << " against target ";
	AppendNameTeamID(ss, targetUnit);

	//Attach data as bundle to the end of the string
	AppendQuickFLDataUnit(ss, unit);

	ss << "\n";

	m_FileHandle << ss.str();

	return true;
}

bool SM_CombatLog::LogTeamFL_00(TeamEntry* const team)
{
	//Construct String
	std::stringstream ss;

	AppendEventCode(ss, EventCodes::FL_STATE_00);
	ss << "[Team " << team->GetTeamID() << "]" ;
	ss << "fuzzy logic module resolved state as ";
	ss << "[State Name: " << team->GetFLController()->GetRuleset().GetActiveStateName() << "]";
	ss << "[State ID: " << team->GetFLController()->GetRuleset().GetActiveState() << "].";

	//Attach data as bundle to the end of the string
	AppendQuickFLDataTeam(ss, team);

	ss << "\n";
	m_FileHandle << ss.str();

	return true;
}

void SM_CombatLog::AppendEventCode(std::stringstream& ss, EventCodes code)
{
	ss << "(EC:" << static_cast<unsigned>(code) << ")";
}

void SM_CombatLog::AppendNameTeamID(std::stringstream& ss, Entity_GameUnit* const unit)
{
	ss << "[Team " << unit->GetTeamID() << "][" << unit->GetUnitData().m_UnitName << "]";
}

void SM_CombatLog::AppendQuickFLDataUnit(std::stringstream& ss, Entity_GameUnit* const unit)
{
	//Append Delim Part 1
	ss << ",";
	
	//Append highest members
	for (unsigned i(0); i < unit->GetFLController()->GetSetCount(); ++i)
		ss << unit->GetFLController()->GetSetAtIndex(i)->GetHighestMember()->GetMemberID() << ",";
	//Append rule outcome
	ss << unit->GetFLController()->GetRuleset().GetActiveState();

}

void SM_CombatLog::AppendQuickFLDataTeam(std::stringstream& ss, TeamEntry* const team)
{
	//Append Delim Part 1
	ss << ",";

	//Append highest members
	for (unsigned i(0); i < team->GetFLController()->GetSetCount(); ++i)
		ss << team->GetFLController()->GetSetAtIndex(i)->GetHighestMember()->GetMemberID() << ",";
	//Append rule outcome
	ss << team->GetFLController()->GetRuleset().GetActiveState();

}
