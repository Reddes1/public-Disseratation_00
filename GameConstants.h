#pragma once
#include "D3DUtils.h"

/*
	Any high level constants, definitions or enumerations that the game or entities may need.
*/

///////////////////
/// Definitions ///
///////////////////

#define EXAMPLE_DEF 0
#define AI_FP_00 "data/files/AI_Configs_00.json"


/////////////////
/// Constants ///
/////////////////

//
//Game Details
//

const int TEAM_STARTING_FUNDS = 10000;
const int TEAM_MAX_FUNDS = 15000;


//
//Team Details
//

const int BASE_TEAM_COUNT = 2;
const unsigned MAX_TEAM_SIZE = 10;
const DirectX::XMVECTOR TEAM_00_COLOUR = DirectX::Colors::Cyan;
const DirectX::XMVECTOR TEAM_01_COLOUR = DirectX::Colors::PaleVioletRed;
const DirectX::XMVECTOR UNIT_DISABLED_COLOUR_00 = DirectX::Colors::Gray;

//
//Unit Details
//

const float SPRITE_SCALE_ADJ_X = 0.25f;
const float SPRITE_SCALE_ADJ_Y = 0.25f;

//
//Other
//

const int SCENE_BUFFER_X = 2;
const int SCENE_BUFFER_Y = 2;

const float DEF_PER_POINT_VALUE = 10.f;

////////////////////
/// Enumerations ///
////////////////////

/*
	Name defintions, as well ID values, for game units.
*/
enum GameUnitIDs : unsigned short
{
	ASSASSIN,
	BARBARIAN,
	ARCHER,
	NECROMANCER,
	SORCERESS,
	WARRIOR,
	GU_COUNT
};

/*
	IDs for different type tiles, as it relates to pathfinding concerns
*/
enum GameTileTypeID : unsigned short
{
	GTT_LAND,
	GTT_LAND_N_SEA,
	GTT_SEA,
	GTT_IMPASSABLE
};

/*
	Team index descriptions
*/
enum class TeamIndexes : unsigned
{
	TEAM_00,
	TEAM_01,
	TEAM_COUNT
};

//////////
/// AI ///
//////////

enum class ActionStates : unsigned
{
	AGGRO,
	BALANCED,
	DEFENSIVE,
	UNDEFINED
};

