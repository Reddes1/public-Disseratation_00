#include "UI_ScenarioEndMenu.h"

#include "Game.h"					//Manager Access
#include "TextureDescriptions.h"	//Texture indexes

void UI_ScenarioEndMenu::Init()
{

}

void UI_ScenarioEndMenu::Release()
{
	//Let go of the UI element
	ReleaseUIElement(m_UIElement);
}

void UI_ScenarioEndMenu::SetupUIElement(int id)
{
	Game::GetGame()->GetUIManager().GetFixedIndexNavUIElementByTypeID(m_UIElement, id);
}

unsigned UI_ScenarioEndMenu::ProcessInput(char key)
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
	}

	return ActionDesc::NO_ACTION;
}

unsigned UI_ScenarioEndMenu::GetMenuCode()
{
	switch (m_UIElement->GetMenuIndex().y)
	{
		//Action: End Turn
	case 0:
		return ActionDesc::PLAY_AGAIN;
		break;
		//Action: Main Menu
	case 1:
		return ActionDesc::MAIN_MENU;
		break;
		//Action: Quit Game
	case 2:
		return ActionDesc::QUIT_GAME;
		break;
	}

	//Shouldn't hit this, but just in case
	return ActionDesc::NO_ACTION;
}