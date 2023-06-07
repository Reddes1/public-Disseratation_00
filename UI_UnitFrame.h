#pragma once

//Types
#include "UITypes.h"			//UI
#include "Entity_GameUnit.h"	//Game entity

/*
	Wrapped up behaviour for displaying unit information.
*/
class UI_UnitFrame
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////

	enum UnitFrameStyle : unsigned
	{
		BLUE_STYLE_00,
		RED_STYLE_00
	};

	enum StringDesc : unsigned
	{
		UNIT_NAME,
		HP,
		COUNT
	};

	////////////////////
	/// Constructors ///
	////////////////////

	UI_UnitFrame() { Init(); }
	~UI_UnitFrame() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Setups UI element with UI index
	void SetupUIElement(int id);

	/*
		Updates the unit frame with information from a given unit, and optional flag for control over
		how to update if required (later).
	*/
	void UpdateFrameWithUnit(Entity_GameUnit* const unit, TeamIndexes teamID = TeamIndexes::TEAM_00);

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
	void SetStyle(unsigned style);


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