#pragma once

#include "GameConstants.h"	

//Types
#include "UITypes.h"
#include "MT_Type1.h"

/*
	Wrapped up behaviour for game tooltip. Uses specific UI element for display purposes.
	Contigent on compatible UI element being used with behaviour.
*/
class UI_GameTooltip
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////

	enum class TooltipStyles : unsigned short
	{
		BLUE_STYLE_00,
		RED_STYLE_00		
	};

	enum StringDesc : unsigned short
	{
		TILE_NAME,
		MOVE_COST,
		DEFENSIVE_VALUE
	};

	////////////////////
	/// Constructors ///
	////////////////////

	UI_GameTooltip() { Init(); }
	~UI_GameTooltip() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Setups UI element with UI index
	void SetupUIElement(int id);

	/*
		Updates the tooltip wiht a given tiles information, with optional flag to control how the tooltip
		updates if required (later).
	*/
	void UpdateTooltipWithTile(MT_Type1* const tile , TeamIndexes teamID = TeamIndexes::TEAM_00);


	///////////
	/// Get ///
	///////////

	NonNavUI*& GetUIElement() { return m_UIElement; }


	///////////
	/// Set ///
	///////////

	//Sets the UI element required for use
	void SetUIElement(NonNavUI* uiElement);

	//Sets the tooltip to a specific stype
	void SetStyle(TooltipStyles style);

private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();
	void Release();
	
	////////////
	/// Data ///
	////////////

	//UI element this class is orientated about
	NonNavUI* m_UIElement = nullptr;
};