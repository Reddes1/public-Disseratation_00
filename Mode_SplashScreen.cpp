#include "Mode_SplashScreen.h"	//Owner

#include "Game.h"		//Manager Acccess

void Mode_SplashScreen::Enter()
{
}

void Mode_SplashScreen::Update(GameTimer& gt)
{
}

void Mode_SplashScreen::Render(PassData& data)
{
	//Start Render Pass
	data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB]->Begin(
		Game::GetGame()->GetCommandList().Get(),
		DirectX::SpriteSortMode_FrontToBack
	);




	//End Render Pass
	data.m_Spritebatches[SpritebatchDefinitions::MAIN_SB]->End();
}

void Mode_SplashScreen::ProcessKey(char key)
{
}

void Mode_SplashScreen::Release()
{
}

void Mode_SplashScreen::Init()
{
}
