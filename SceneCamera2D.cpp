#include "SceneCamera2D.h"

void SceneCamera_2D::InitSceneCamera(DirectX::XMINT2 mapSize, DirectX::XMINT2 tileSize, DirectX::XMINT2 bufferRange, int windowX, int windowY)
{
	//Set values
	m_MapSize = mapSize;
	m_TileSize = tileSize;
	m_SceneBuffer = bufferRange;

	//Calculate scene size
	CalculateSceneSize(windowX, windowY, m_TileSize);
}

void SceneCamera_2D::UpdateSceneCamera()
{
	if (m_UpdateFlag)
	{
		m_SceneTransform = DirectX::XMMatrixTranslation(
			m_SceneTranslation.x * m_TileSize.x,
			m_SceneTranslation.y * m_TileSize.y,
			0.0f
		);
		m_UpdateFlag = false;
	}
}

bool SceneCamera_2D::RunBoundaryCheck(DirectX::XMINT2 cursorCoords, Directional4Way dir)
{
	switch (dir)
	{
	case Directional4Way::UP:
		//Check to see if the cursor is hitting/within the buffer area
		if (cursorCoords.y <= -m_SceneTranslation.y + m_SceneBuffer.y - 1)
		{
			//Check that the cursor is nearing the top of the scene, and that the scene has been moved
			if (m_SceneTranslation.y != 0 && cursorCoords.y < -m_SceneTranslation.y + m_SceneBuffer.y)
			{
				//Move scene and flag for update
				++m_SceneTranslation.y;
				m_UpdateFlag = true;
				return true;
			}
			else
			{
				//No move required, return false
				return false;
			}
		}
		break;

	case Directional4Way::DOWN:
		//Check to see if the cursor is hitting/within the buffer area
		if (cursorCoords.y >= (m_SceneSize.y + -m_SceneTranslation.y) - m_SceneBuffer.y)
		{
			//Check to see if the cursor position is below the end of the map
			if (cursorCoords.y <= m_MapSize.y - m_SceneBuffer.y)
			{
				//Move scene and flag for update
				--m_SceneTranslation.y;
				m_UpdateFlag = true;
				return true;
			}
			//Buffer was crossed, but is at the edge of the scene so 
			else
			{
				//No move required, return false
				return false;
			}
		}
		break;
	case Directional4Way::LEFT:
		//Check to see if the cursor is hitting/within the buffer area
		if (cursorCoords.x <= -m_SceneTranslation.x + m_SceneBuffer.x - 1)
		{
			//Check that the cursor is nearing the left of the scene, and that the scene has been moved
			if (m_SceneTranslation.x != 0 && cursorCoords.x < -m_SceneTranslation.x + m_SceneBuffer.x)
			{
				//Move scene and flag for update
				++m_SceneTranslation.x;
				m_UpdateFlag = true;
				return true;
			}
			else
			{
				//No move required, return false
				return false;
			}
		}
		break;

	case Directional4Way::RIGHT:
		//Check to see if the cursor is hitting/within the buffer area
		if (cursorCoords.x >= (m_SceneSize.x + -m_SceneTranslation.x) - m_SceneBuffer.x)
		{
			//Check to see if the cursor position is below the end of the map
			if (cursorCoords.x <= m_MapSize.x - m_SceneBuffer.x)
			{
				//Move scene and flag for update
				--m_SceneTranslation.x;
				m_UpdateFlag = true;
				return true;
			}
			else
			{
				//No move required, return false
				return false;
			}
		}
		break;

	}

	//Scene hasn't moved
	return false;
}

void SceneCamera_2D::SyncSceneToCursor(DirectX::XMFLOAT2 cursorCoords)
{
	//Calculate a scene position using current cursor position and half the scenes size
	//Always round down in the case of floating point
	int transX = -static_cast<int>(std::floor(cursorCoords.x - m_SceneSize.x * 0.5f));
	int transY = -static_cast<int>(std::floor(cursorCoords.y - m_SceneSize.y * 0.5f));

	//Check that the scene hasnt gone out of bounds and adjust the scene accordingly
	//Left Check
	if (transX > 0) 
		transX = 0;
	//Right Check
	else if (transX < m_SceneSize.x - m_MapSize.x - 1) 
		transX = m_SceneSize.x - m_MapSize.x - 1;
	//Top Check
	if (transY > 0)	
		transY = 0;
	//Bottom Check
	else if(transY < m_SceneSize.y - m_MapSize.y - 1)
		transY = m_SceneSize.y - m_MapSize.y - 1;

	//Set translation
	m_SceneTranslation.x = static_cast<float>(transX);
	m_SceneTranslation.y = static_cast<float>(transY);

	//Flag for update
	m_UpdateFlag = true;
}

void SceneCamera_2D::CalculateSceneSize(int windowX, int windowY, DirectX::XMINT2 tileSize)
{
	m_SceneSize.x = static_cast<int>(std::floor(windowX / tileSize.x));
	m_SceneSize.y = static_cast<int>(std::floor(windowY / tileSize.y));
}

void SceneCamera_2D::Init()
{

}

void SceneCamera_2D::Release()
{

}
