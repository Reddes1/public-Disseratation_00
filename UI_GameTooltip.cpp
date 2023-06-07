#include "UI_GameTooltip.h"			//Owner

#include "Game.h"					//Manager Access
#include "TextureDescriptions.h"	//Texture indexes

void UI_GameTooltip::SetupUIElement(int id)
{
	Game::GetGame()->GetUIManager().GetNonNavUIElementByTypeID(m_UIElement, id);
}

void UI_GameTooltip::UpdateTooltipWithTile(MT_Type1* const tile, TeamIndexes teamID)
{
	if (tile)
	{
		///////////////////
		/// Set Strings ///
		///////////////////


		//Reset strings
		m_UIElement->ResetAllStrings();

		//Update Tile Title
		m_UIElement->SetStringAtIndex(tile->GetProperties().m_Name, StringDesc::TILE_NAME);

		//Update cost
		std::stringstream ss;
		ss << tile->GetProperties().m_MoveCost;
		m_UIElement->AppendStringAtIndex(ss, StringDesc::MOVE_COST);

		//Update defensive value
		std::stringstream().swap(ss);
		ss << tile->GetProperties().m_DefensiveValue;
		m_UIElement->AppendStringAtIndex(ss, StringDesc::DEFENSIVE_VALUE);


		////////////
		/// Post ///
		////////////


		//Flag visability
		m_UIElement->SetEnabledState(true);
		
	}
	else
	{
		m_UIElement->SetEnabledState(false);
	}


}

void UI_GameTooltip::SetUIElement(NonNavUI* uiElement)
{
	assert(uiElement);

	m_UIElement = uiElement;
}

void UI_GameTooltip::SetStyle(TooltipStyles style)
{
	//Setup a frame style as required
	switch (style)
	{
	case TooltipStyles::BLUE_STYLE_00:
		m_UIElement->GetSprite().SetFrame(BK_UI_Bundle_00_Desc::BK_UI_BLUE_00_SQ_128x128);
		break;

	case TooltipStyles::RED_STYLE_00:
		m_UIElement->GetSprite().SetFrame(BK_UI_Bundle_00_Desc::BK_UI_RED_00_SQ_128x128);
		break;
	}
}

void UI_GameTooltip::Init()
{

}

void UI_GameTooltip::Release()
{
	//Let go of the UI element
	ReleaseUIElement(m_UIElement);
}
