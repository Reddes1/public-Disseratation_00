#pragma once

//Types
#include "UITypes.h"

/*
	Wrapped up behavior for games master menu.
*/
class UI_GameMasterMenu
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
		END_TURN,
		QUIT_GAME,
		BACK,
		NO_ACTION,
		ACTIONS_COUNT
	};

	////////////////////
	/// Constructors ///
	////////////////////

	UI_GameMasterMenu() { Init(); }
	~UI_GameMasterMenu() { Release(); }

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