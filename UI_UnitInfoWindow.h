#pragma once

//Types
#include "UITypes.h"
#include "Entity_GameUnit.h"

/*
	Wrapped up behaviour for Unit Info Window
*/
class UI_UnitInfoWindow
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////

	enum StringDesc : unsigned
	{
		UNIT_NAME,
		UNIT_HP,
		UNIT_MOVESPEED,
		STRONG_MATCHUP_TITLE,
		WEAK_MATCHUP_TITLE,
		STRONG_MATCHUP_NAMES,
		WEAK_MATCHUP_NAMES,
		COUNT
	};

	////////////////////
	/// Constructors ///
	////////////////////

	UI_UnitInfoWindow() { Init(); }
	~UI_UnitInfoWindow() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Setups UI element with UI index
	void SetupUIElement(int id);

	void UpdateStrings(Entity_GameUnit* unit);

	///////////
	/// Get ///
	///////////

	NonNavUI*& GetUIElement() { return m_UIElement; };

	///////////
	/// Set ///
	///////////

	void SetUIElement(NonNavUI* uiElement) { m_UIElement = uiElement; }

private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();
	void Release();

	
	////////////
	/// Data ///
	////////////

	NonNavUI* m_UIElement = nullptr;

};