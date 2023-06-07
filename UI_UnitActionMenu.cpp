#include "UI_UnitActionMenu.h"		//Owner

#include "Game.h"					//Manager Access
#include "TextureDescriptions.h"	//Texture indexes
#include "GameConstants.h"			//Game Info

void UI_UnitActionMenu::SetupUIElement(int id)
{
	Game::GetGame()->GetUIManager().GetFixedIndexNavUIElementByTypeID(m_UIElement, id);
}

unsigned UI_UnitActionMenu::ProcessInput(char key)
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

void UI_UnitActionMenu::UpdateMenuWithUnit(Entity_GameUnit* const unit, bool preClearStringColours)
{
	if (preClearStringColours)
		ResetStringColours();

	if (!unit->GetActionFlag())
		m_UIElement->SetColourAtIndex(DirectX::XMVECTOR(DirectX::Colors::DarkSlateGray), STR_ATTACK);
	if(!unit->GetMoveFlag())
		m_UIElement->SetColourAtIndex(DirectX::XMVECTOR(DirectX::Colors::DarkSlateGray), STR_MOVE);
}

void UI_UnitActionMenu::ResetStringColours(DirectX::XMVECTOR colour)
{
	for (unsigned i(0); i < m_UIElement->GetStringCount(); ++i)
	{
		m_UIElement->SetColourAtIndex(colour, i);
	}
}

void UI_UnitActionMenu::SetUIElement(FixedIndexNavUI* uiElement)
{
	assert(uiElement);

	m_UIElement = uiElement;
}

void UI_UnitActionMenu::Init()
{
}

void UI_UnitActionMenu::Release()
{
	//Let go of the UI element
	ReleaseUIElement(m_UIElement);
}

unsigned UI_UnitActionMenu::GetMenuCode()
{
	switch (m_UIElement->GetMenuIndex().y)
	{
		//Action: Attack
	case 0:
		return ActionDesc::ATTACK;
		break;
		//Action: Move
	case 1:
		return ActionDesc::MOVE;
		break;
		//Action: Pass
	case 2:
		return ActionDesc::PASS;
		break;
		//Action: Info
	case 3:
		return ActionDesc::INFORMATION;
		break;
	}

	//Shouldn't this, but just in case
	return ActionDesc::NO_ACTION;
}
