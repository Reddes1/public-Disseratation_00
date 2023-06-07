#pragma once
#include "ModeInterface.h"	//Parent Class

#include "Game.h"			//Manager Access + Utils

class Mode_MainMenu : public ModeInterface
{
public:


private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();

	////////////
	/// Data ///
	////////////

	//Splash screens
	std::vector<EM_Sprite*> m_Backgrounds;

	//Main Menu UI 
	FixedIndexNavUI* m_MainMenu;

};