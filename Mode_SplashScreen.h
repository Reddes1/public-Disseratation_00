#pragma once
#include "ModeInterface.h"	//Parent

#include "EM_Sprite.h"

/*
	Any intro logos go here!
*/
class Mode_SplashScreen : public ModeInterface
{
public:


	////////////
	/// Data ///
	////////////


	///////////////////
	/// Definitions ///
	///////////////////


	////////////////////
	/// Constructors ///
	////////////////////

	Mode_SplashScreen(size_t id) :ModeInterface(id) { Init(); }
	~Mode_SplashScreen() { Release(); }

	/////////////////
	/// Overrides ///
	/////////////////

	void Enter() override;
	bool Exit() override { return true; }
	void Update(GameTimer& gt) override;
	void Render(PassData& data) override;
	void ProcessKey(char key) override;
	void Release() override;

	//////////////////
	/// Operations ///
	//////////////////


	///////////
	/// Get ///
	///////////


	///////////
	/// Set ///
	///////////



private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();


	////////////
	/// Data ///
	////////////

	//Any Background Sprite
	EM_Sprite m_Background;

};