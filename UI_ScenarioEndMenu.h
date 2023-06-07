#pragma once

//Types
#include "UITypes.h"

/*
	Wrapped up behaviour for scenario end menu.
*/
class UI_ScenarioEndMenu
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////

	enum ActionDesc : unsigned
	{
		PLAY_AGAIN,
		MAIN_MENU,
		QUIT_GAME,
		NO_ACTION,
		ACTIONS_COUNT
	};

	////////////////////
	/// Constructors ///
	////////////////////

	UI_ScenarioEndMenu() { Init(); }
	~UI_ScenarioEndMenu() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Setups UI element with UI index
	void SetupUIElement(int id);

	//Updates the menu cursor with input given, and returns an output code
	unsigned ProcessInput(char key);


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

	FixedIndexNavUI* m_UIElement = nullptr;


};