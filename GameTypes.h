#pragma once

#include "GameConstants.h"

/*
	Specific Types for this game.
*/


/////////////////
/// Unit Data ///
/////////////////

//An individual units data
struct UnitData
{
	std::string m_UnitName = "No Name";
	float m_Health = 100.f;
	float m_MaxHealth = 100.f;
	float m_MoveSpeed = 4.f;
	float m_Damage = 50.f;
	short m_ID = -1;
	short m_UnitTypeID = -1;
	short m_TerrainID = 0;
	short m_AttackRange = 1;
	short m_VisionRange = 4;
	bool m_CanFly = false;
	bool m_CanSwim = false;
	bool m_Alive = true;
	bool m_HasDied = false;
};

//An individual matchup entry
struct MatchupData
{
	std::string m_UnitName = "No Name";
	float m_DamageDoneMod = 1.0f;
	short m_ID = -1;
};

//A units matchup data
struct UnitMatchupData
{
	UnitMatchupData()
	{
		//Reserve an amount equal to the game units
		m_Data.reserve(GameUnitIDs::GU_COUNT);
	}

	std::string m_UnitName;
	std::vector<MatchupData> m_Data;
};
