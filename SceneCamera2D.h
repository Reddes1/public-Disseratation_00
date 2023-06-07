#pragma once

#include "D3DUtils.h"		//Types

/*
	Camera class that manipulates a matrix to achieve the effect of a moving camera.
	Use in combination with a cursor class to dictate how the camera should move and when.
*/
class SceneCamera_2D
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

	SceneCamera_2D() { Init(); }
	~SceneCamera_2D() { Release(); }

	//////////////////
	/// Operations ///
	//////////////////

	//Inits scene camera in one call
	void InitSceneCamera(DirectX::XMINT2 mapSize, DirectX::XMINT2 tileSize, DirectX::XMINT2 bufferRange, int windowX, int windowY);

	//If bool is flagged, will update the transformation (call this every frame regardless)
	void UpdateSceneCamera();

	/*
		Using coordinates and a direction, runs a check to see if the cursor position is within the
		boundary limit of the scene. If it is, then a check to determine if the scene requires moving or not.
	*/
	bool RunBoundaryCheck(DirectX::XMINT2 cursorCoords, Directional4Way dir);

	/*
		Syncs up the scene with a cursors position in the case where its been manually moved to a coordinate.
		This is done to fix the scene when the cursor would end of off screen, or inside the buffer zone when moved.
	*/
	void SyncSceneToCursor(DirectX::XMFLOAT2 cursorCoords);

	///////////
	/// Get ///
	///////////

	//Gets a copy of the scene transformation
	DirectX::XMMATRIX& GetSceneTransform() { return m_SceneTransform; }

	///////////
	/// Set ///
	///////////

	void SetMapSize(DirectX::XMINT2& mapSize) { m_MapSize = mapSize; }
	void SetMapSize(int x, int y) { m_MapSize.x = x; m_MapSize.y = y; }

	void SetTileSize(DirectX::XMINT2& tileSize) { m_TileSize = tileSize; }
	void SetTileSize(int x, int y) { m_TileSize.x = x; m_TileSize.y = y; }

	void SetSceneBuffer(DirectX::XMINT2& buffer) { m_SceneBuffer = buffer; }
	void SetSceneBuffer(int x, int y) { m_SceneBuffer.x = x; m_SceneBuffer.y = y; }

	//Calculates the number of tiles on scene, rounding down
	void CalculateSceneSize(int windowX, int windowY, DirectX::XMINT2 tileSize);


private:

	//////////////////
	/// Operations ///
	//////////////////

	void Init();
	void Release();
	
	////////////
	/// Data ///
	////////////

	//Scene manipulation matrix
	DirectX::XMMATRIX m_SceneTransform = DirectX::XMMatrixIdentity();
	//The translation, rotation and scale portions of the matrix
	DirectX::XMFLOAT2 m_SceneTranslation = { 0.f, 0.f };
	DirectX::XMFLOAT2 m_SceneScale = { 0.f, 0.f };
	float m_SceneRotation = 0.f;

	//Keep hold of the current number of tiles on the map
	DirectX::XMINT2 m_MapSize = { 40, 40 };
	//Keep hold of the tile size (for translation purposes)
	DirectX::XMINT2 m_TileSize = { 0, 0 };

	//This represents how many tiles fit inside the scene/window, aka scene size
	DirectX::XMINT2 m_SceneSize = { 20, 20 };
	//This is the buffer distance between the edge of the scene that tells us when we want to move the scene
	DirectX::XMINT2 m_SceneBuffer = { 2, 2 };

	//Tracking flag to update matrix if changes made
	bool m_UpdateFlag = false;


};