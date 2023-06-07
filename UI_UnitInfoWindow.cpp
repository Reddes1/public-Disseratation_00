#include "UI_UnitInfoWindow.h"	//Owner

#include "Game.h"				//Manager Access

void UI_UnitInfoWindow::SetupUIElement(int id)
{
	Game::GetGame()->GetUIManager().GetNonNavUIElementByTypeID(m_UIElement, id);
}

void UI_UnitInfoWindow::UpdateStrings(Entity_GameUnit* unit)
{
	if (unit)
	{
		//Clear strings
		m_UIElement->ResetAllStrings();
		std::stringstream ss;

		//Set Name
		ss << unit->GetUnitData().m_UnitName;
		m_UIElement->AppendStringAtIndex(ss, UNIT_NAME);
		std::stringstream().swap(ss);

		//Set HP
		ss << unit->GetUnitData().m_Health;
		m_UIElement->AppendStringAtIndex(ss, UNIT_HP);
		std::stringstream().swap(ss);

		//Set Movespeed
		ss << unit->GetUnitData().m_MoveSpeed;
		m_UIElement->AppendStringAtIndex(ss, UNIT_MOVESPEED);
		std::stringstream().swap(ss);

		//Set Matchups
		const UnitMatchupData* matchup = Game::GetGame()->GetGameplayManager().GetUnitDataManager().GetMatchupData(unit->GetUnitData().m_ID);

		//Strong matchups
		for (unsigned i(0); i < matchup->m_Data.size(); ++i)
		{
			if(matchup->m_Data[i].m_DamageDoneMod > 1.0f)
				ss << "- " << matchup->m_Data[i].m_UnitName << "\n";
		}
		m_UIElement->AppendStringAtIndex(ss, STRONG_MATCHUP_NAMES);
		std::stringstream().swap(ss);

		//Weak Matchups
		for (unsigned i(0); i < matchup->m_Data.size(); ++i)
		{
			if (matchup->m_Data[i].m_DamageDoneMod < 1.0f)
				ss << "- " << matchup->m_Data[i].m_UnitName << "\n";
		}
		m_UIElement->AppendStringAtIndex(ss, WEAK_MATCHUP_NAMES);

	}
}

void UI_UnitInfoWindow::Init()
{

}

void UI_UnitInfoWindow::Release()
{

}
