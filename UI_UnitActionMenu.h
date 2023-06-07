#pragma once

//Types
#include "UITypes.h"
#include "Entity_GameUnit.h" //Main Game Unit

/*
	Wrapped up behaviour for unit action menu.
	Contigent on compatible UI element being used with behaviour.
*/
class UI_UnitActionMenu
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
		STR_ATTACK,
		STR_MOVE,
		STR_PASS,
		STR_INFORMATION,
		STR_COUNT
	};


	enum ActionDesc : unsigned
	{
		ATTACK,
		MOVE,
		PASS,
		INFORMATION,
		BACK,
		NO_ACTION,
		CANNOT_ACT,
		ACTIONS_COUNT
	};

	////////////////////
	/// Constructors ///
	////////////////////

	UI_UnitActionMenu() { Init(); }
	~UI_UnitActionMenu() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Setups UI element with UI index
	void SetupUIElement(int id);

	//Updates the menu cursor with input given, and returns an output code
	unsigned ProcessInput(char key);

	//Updates the menus strings to reflect units possible actions
	void UpdateMenuWithUnit(Entity_GameUnit* const unit, bool preClearStringColours = true);

	//Resets all strings to a set colour
	void ResetStringColours(DirectX::XMVECTOR colour = DirectX::Colors::White);


	///////////
	/// Get ///
	///////////

	FixedIndexNavUI*& GetUIElement() { return m_UIElement; }

	///////////
	/// Set ///
	///////////

	//Sets the UI element required for use
	void SetUIElement(FixedIndexNavUI* uiElement);


private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();
	void Release();

	//Return a menu code based on current menu index
	unsigned GetMenuCode();
	
	////////////
	/// Data ///
	////////////

	//Store unavailable menu indexes here
	std::vector<DirectX::XMINT2> m_UnavailableIndexes;


	FixedIndexNavUI* m_UIElement = nullptr;

};