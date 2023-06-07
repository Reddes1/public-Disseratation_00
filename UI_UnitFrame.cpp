#include "UI_UnitFrame.h"		//Owner

#include "Game.h"					//Manager Access
#include "TextureDescriptions.h"	//Texture indexes

void UI_UnitFrame::Init()
{
}

void UI_UnitFrame::Release()
{
	//Let go of the UI element
	ReleaseUIElement(m_UIElement);
}

void UI_UnitFrame::SetupUIElement(int id)
{
	Game::GetGame()->GetUIManager().GetNonNavUIElementByTypeID(m_UIElement, id);
}

void UI_UnitFrame::UpdateFrameWithUnit(Entity_GameUnit* const unit, TeamIndexes teamID)
{
	if (unit)
	{
		//Get Game manager
		GameplayManager& gp = Game::GetGame()->GetGameplayManager();


		//////////////////
		/// Set String ///
		//////////////////

		//Reset strings
		m_UIElement->ResetAllStrings();

		//Update unit name
		m_UIElement->SetStringAtIndex(unit->GetUnitData().m_UnitName, StringDesc::UNIT_NAME);

		//Update HP
		std::stringstream ss;
		ss << unit->GetUnitData().m_Health;
		m_UIElement->AppendStringAtIndex(ss, StringDesc::HP);


		///////////////
		/// Styling ///
		///////////////
		
		//Check the team ID against currently active team
		if (gp.GetTileCursor().GetUnitAtIndex(Entity_TileCursor::FIRST_TARGET)->GetTeamID() ==
			gp.GetTeamsManager().GetActiveTeamIndexAsNumber())
		{
			SetStyle(BLUE_STYLE_00);
		}
		else
		{
			SetStyle(RED_STYLE_00);
		}

			
		////////////
		/// Post ///
		////////////

		//Flag visability
		m_UIElement->SetEnabledState(true);

	}
	else
	{
		//Disable visibility
		m_UIElement->SetEnabledState(false);
	}
		
}

void UI_UnitFrame::SetUIElement(NonNavUI* uiElement)
{
	assert(uiElement);

	m_UIElement = uiElement;
}

void UI_UnitFrame::SetStyle(unsigned style)
{
	//Setup a frame style as required
	switch (style)
	{
	case UnitFrameStyle::BLUE_STYLE_00:
		m_UIElement->GetSprite().SetFrame(BK_UI_Bundle_00_Desc::BK_UI_BLUE_00_SQ_128x64);
		break;

	case UnitFrameStyle::RED_STYLE_00:
		m_UIElement->GetSprite().SetFrame(BK_UI_Bundle_00_Desc::BK_UI_RED_00_SQ_128x64);
		break;
	}
}
