#pragma once
#include "D3DUtils.h"
#include "GameTimer.h"		//Delta & Gametime

#include "EM_Coordinates.h" //World space tracking

/*
	Parent class designed to easy to interact with a high level (Updates, renders etc)
	Parent class for use in creating game entities. Allows for high level access and calls for updating, rendering and basic flags.
	Child classes can be composed of modular functionality classes, each providing key behaviour functions (movement, sprites, animation, actions etc)
*/

class EM_EntityInterface2D
{
public:

	////////////
	/// Data ///
	////////////

	struct EntityFlags
	{
		EntityFlags()
			:m_IsActive(true), m_Renderable(true)
		{}

		//Is the entity updatable/interactable
		bool m_IsActive : 1;
		//Is the entity draw enabled
		bool m_Renderable : 1;
	};

	////////////////////
	/// Constructors ///
	////////////////////

	EM_EntityInterface2D() {}
	~EM_EntityInterface2D() {}


	////////////////
	/// Virtuals ///
	////////////////

	//
	//Operations
	//
	
	//Use this call to update any behaviours for the object
	virtual void Update(GameTimer& gt) = 0;
	//Use this call to draw any sprites (EM_Sprite etc) owned by the object
	virtual void Render(DirectX::SpriteBatch& batch, DirectX::DescriptorHeap* heap) = 0;

	//
	//EM_Accessors
	//

	//Dev Note: This section is intended to be filled with basic accessor virtuals for a numerous types of EM_Modules
	//The viabilty, or need for this will be assessed at a later point as need arises


	//
	//Utilities
	//
	
	//Optional defaulter function so that any entity can be reset to a definable state
	virtual void ResetEntityToDefaults() {}
	
	///////////
	/// Get ///
	///////////

	EM_Coordinates& GetCoords()			   { return m_WorldCoordinates; }
	bool GetActiveState()				   { return m_Flags.m_IsActive; }
	bool GetRenderState()				   { return m_Flags.m_Renderable; }
										   
	///////////							   
	/// Set ///							   
	///////////							   
										   
	void SetActiveState(bool isActive)     { m_Flags.m_IsActive = isActive; }
	void SetRenderState(bool isRenderable) { m_Flags.m_Renderable = isRenderable; }

private:

	//Almost everything needs world space coordinates, so add this to the baseline
	EM_Coordinates m_WorldCoordinates;
	//Some basic entity flags the child classes can use for control purposes
	EntityFlags m_Flags;

};

