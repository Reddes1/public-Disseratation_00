#include "UI_GameMasterMenu.h"	//Owner

#include "Game.h"					//Manager Access
#include "TextureDescriptions.h"	//Texture indexes

void UI_GameMasterMenu::SetupUIElement(int id)
{
	Game::GetGame()->GetUIManager().GetFixedIndexNavUIElementByTypeID(m_UIElement, id);
}

unsigned UI_GameMasterMenu::ProcessInput(char key)
{
	Game* game = Game::GetGame();

	switch (key)
	{
		//Action: Move Up
	case 0:
		m_UIElement->MoveCursorUp();
		break;
		//Action: Move Down
	case 1:
		m_UIElement->MoveCursorDown();
		break;
		//Action: Move Left
	case 2:
		m_UIElement->MoveCursorLeft();
		break;
		//Action: Move Right
	case 3:
		m_UIElement->MoveCursorRight();
		break;
		//Action: Confirm
	case 4:
		return GetMenuCode();
		break;
		//Action: Back
	case 5:
		return ActionDesc::BACK;
		break;
	}

	return ActionDesc::NO_ACTION;
}

void UI_GameMasterMenu::SetUIElement(FixedIndexNavUI* uiElement)
{
	assert(uiElement);

	m_UIElement = uiElement;
}

void UI_GameMasterMenu::Init()
{

}

void UI_GameMasterMenu::Release()
{
	//Let go of the UI element
	ReleaseUIElement(m_UIElement);
}

unsigned UI_GameMasterMenu::GetMenuCode()
{
	switch (m_UIElement->GetMenuIndex().y)
	{
		//Action: End Turn
	case 0:
		return ActionDesc::END_TURN;
		break;
		//Action: Quit Game
	case 1:
		return ActionDesc::QUIT_GAME;
		break;
	}

	//Shouldn't hit this, but just in case
	return ActionDesc::NO_ACTION;
}
