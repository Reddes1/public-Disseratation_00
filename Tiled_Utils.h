#pragma once

#include <vector>
#include <unordered_map>
#include <rectypes.h>

#include "document.h"				//RapidJSON 

#include "SpriteBatch.h"			//SpriteEffect

/*
	Methods specialised to "Tiled" program JSON outputs. Host common support functions
	for frame generation, rotation flag resolution and member assertion.
*/

/*
	"Tiled" uses Flagged values attached to the TileID to denote rotated items.
	Theses are the values looked for.
*/

const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;
const unsigned ROTATED_HEXAGONAL_120_FLAG = 0x10000000;

/*
	Generate frame data for a singular tileset entry by index.
*/
void GenerateFrameData(rapidjson::Document& doc, int tilesetIndex, std::vector<RECT>& container);

/*
	Returns angle using TileID and any attached flag values. Return angles are 0, 90, 180, 270 in Radians.
	Will strip the flags (if present) after determining rotation type.
*/
float GetTileRotationValue(unsigned int& tileID);

/*
	Checks for flip flag, strips it and returns an effect to be applied to the tile (WIP)
*/
DirectX::SpriteEffects GetFlipValue(unsigned int& tileID);

/*
	Debug check that all basic members are present.
*/
void AssertBasicTilemapMembers(rapidjson::Document& doc);




