#include "MT_TargetingManager.h"

void MT_TargetingManager::RunTargetingAlgorithm(unsigned algorithmType, std::vector<MT_Type1*>& tiles, int mapLength,  DirectX::XMFLOAT2 startingCoords, unsigned gridEffectID, int unitRange)
{
	switch (algorithmType)
	{
	case TargetingTypes::TYPE_DIAMOND_00:
		DiamondAlgorithm_00(tiles, mapLength, startingCoords, gridEffectID, unitRange, true);
		break;
	case TargetingTypes::TYPE_DIAMOND_01:
		DiamondAlgorithm_00(tiles, mapLength, startingCoords, gridEffectID, unitRange, false);
		break;
	}
}

bool MT_TargetingManager::IsTileInManifest(MT_Type1* const tile)
{
	//Attempt to find a match for the given tile and return true if found
	if (std::find(m_Manifest.begin(), m_Manifest.end(), tile) != m_Manifest.end())
		return true;
	else
		return false;
}

void MT_TargetingManager::ReleaseManifest()
{
	for (auto& a : m_Manifest)
	{
		a->DisableHighlight();
	}
	m_Manifest.clear();
}

void MT_TargetingManager::Init()
{
	//Safe reserve amount
	m_Manifest.reserve(100);
}

void MT_TargetingManager::Release()
{
	//Let go of pointers
	for (auto& a : m_Manifest)
		a = nullptr;
	m_Manifest.clear();
}

void MT_TargetingManager::DiamondAlgorithm_00(std::vector<MT_Type1*>& tiles, int mapLength,
	DirectX::XMFLOAT2 startingCoords, unsigned gridEffectID, int unitRange, bool excludeSelf)
{
	//Clear manifest before beginning

	for (int i(0); i < unitRange + 1; ++i)
	{
		//Topside + Origin Row
		for (int j(-i - 1); j < i; ++j)
		{
			//Get tile at index
			int xAdj = static_cast<int>(startingCoords.x) - j - 1;
			int yAdj = static_cast<int>(startingCoords.y) - unitRange + i;
			int index = (xAdj + yAdj) + (yAdj * mapLength);

			//Prevent invalid index access and drawing on tiles that would be drawn due to screen wrapping.
			if (!(index < 0 || index > tiles.size()) && !(xAdj < 0 || xAdj > mapLength))
			{
				MT_Type1* tile = tiles[index];

				if (!tile->GetProperties().m_Impassable)
				{
					tile->GetSpriteAtIndex(TileSpriteIndexes::TS_HIGHLIGHT_TILE_00).SetFrame(gridEffectID);
					tile->EnableHighlight();
					m_Manifest.push_back(tile);
				}
				else
					tile = nullptr;
			}

			//Botside
			for (int j = unitRange - 1 - i; j > -unitRange + i; --j)
			{
				//Grab tile at index
				int xAdj = static_cast<int>(startingCoords.x) - j;
				int yAdj = static_cast<int>(startingCoords.y) + i + 1;
				int index = (xAdj + yAdj) + (yAdj * mapLength);

				//Prevent invalid index access and drawing on tiles that would be drawn due to screen wrapping.
				if (!(index < 0 || index > tiles.size()) && !(xAdj < 0 || xAdj > mapLength))
				{
					MT_Type1* tile = tiles[index];

					if (!tile->GetProperties().m_Impassable)
					{
						tile->GetSpriteAtIndex(TileSpriteIndexes::TS_HIGHLIGHT_TILE_00).SetFrame(gridEffectID);
						tile->EnableHighlight();
						m_Manifest.push_back(tile);
					}
					else
						tile = nullptr;
				}
			}
		}
	}

	//If flag true
	if (excludeSelf)
	{
		int index = (static_cast<int>(startingCoords.x) + static_cast<int>(startingCoords.y)) + (static_cast<int>(startingCoords.y) * mapLength);
		assert(index <= tiles.size());
		MT_Type1* tile = tiles[index];
				
		for (unsigned i(0); i < m_Manifest.size(); ++i)
		{
			if (m_Manifest[i] == tile)
			{
				m_Manifest[i]->DisableHighlight();
				m_Manifest.erase(m_Manifest.begin() + i);
				break;
			}
		}
	}
}
