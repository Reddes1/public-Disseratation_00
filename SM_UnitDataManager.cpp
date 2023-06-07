#include "SM_UnitDataManager.h"

#include "RapidJSONLoaderUtils.h"

void SM_UnitDataManager::LoadUnitData(std::string& unitsFP)
{
	//Clears map just incase (should only have one unit roster loaded at one point)
	m_DataMap.clear();

	//Load doc
	rapidjson::Document doc;
	ParseNewJSONDocument(doc, unitsFP);

	//Load data from file
	for (unsigned i(0); i < doc["Units"].GetArray().Size(); ++i)
	{
		//Assert Members
		assert(doc["Units"][i].HasMember("Unit Name"));
		assert(doc["Units"][i].HasMember("Unit Unique ID"));
		assert(doc["Units"][i].HasMember("Unit Type ID"));
		assert(doc["Units"][i].HasMember("Terrain ID"));
		assert(doc["Units"][i].HasMember("Health"));
		assert(doc["Units"][i].HasMember("Movespeed"));
		assert(doc["Units"][i].HasMember("Attack Range"));
		assert(doc["Units"][i].HasMember("Vision Range"));
		assert(doc["Units"][i].HasMember("Damage"));
		assert(doc["Units"][i].HasMember("Can Fly"));
		assert(doc["Units"][i].HasMember("Can Swim"));
		assert(doc["Units"][i].HasMember("Unit Flavour Text"));
		
		//Create new pair
		std::pair<unsigned, UnitData> newPair;

		//Load ID into pair and data
		newPair.first = doc["Units"][i]["Unit Unique ID"].GetUint();
		newPair.second.m_ID = doc["Units"][i]["Unit Unique ID"].GetUint();
		
		//Load rest of the data
		newPair.second.m_UnitName = doc["Units"][i]["Unit Name"].GetString();
		newPair.second.m_UnitTypeID = doc["Units"][i]["Unit Type ID"].GetInt();
		newPair.second.m_TerrainID = doc["Units"][i]["Terrain ID"].GetInt();
		newPair.second.m_Health = doc["Units"][i]["Health"].GetFloat();
		newPair.second.m_MaxHealth = newPair.second.m_Health;
		newPair.second.m_MoveSpeed = doc["Units"][i]["Movespeed"].GetFloat();
		newPair.second.m_AttackRange = doc["Units"][i]["Attack Range"].GetInt();
		newPair.second.m_VisionRange = doc["Units"][i]["Vision Range"].GetInt();
		newPair.second.m_Damage = doc["Units"][i]["Damage"].GetFloat();
		newPair.second.m_CanFly = doc["Units"][i]["Can Fly"].GetBool();
		newPair.second.m_CanSwim = doc["Units"][i]["Can Swim"].GetBool();


		//Insert into container
		m_DataMap.insert(newPair);
	}

}

void SM_UnitDataManager::LoadMatchupData(std::string& matchesFP)
{
	//Clears map just incase (should only have one unit roster loaded at one point)
	m_MatchupDataMap.clear();

	//Load doc
	rapidjson::Document doc;
	ParseNewJSONDocument(doc, matchesFP);

	//Load data from file
	for (unsigned i(0); i < doc["Unit Matchups"].GetArray().Size(); ++i)
	{
		//Assert Members
		assert(doc["Unit Matchups"][i].HasMember("Unit Name"));
		assert(doc["Unit Matchups"][i].HasMember("Unit Unique ID"));
		assert(doc["Unit Matchups"][i].HasMember("Damage Modifications"));

		//Create new pair
		std::pair<unsigned, UnitMatchupData> newPair;

		//Load name and ID
		newPair.first = doc["Unit Matchups"][i]["Unit Unique ID"].GetInt();
		newPair.second.m_UnitName = doc["Unit Matchups"][i]["Unit Name"].GetString();

		//Cycle through each matchup and store them in vector
		for (unsigned j(0); j < doc["Unit Matchups"][i]["Damage Modifications"].GetArray().Size(); ++j)
		{
			//Create new data entry
			MatchupData newData;
			
			//Load Data
			newData.m_UnitName = doc["Unit Matchups"][i]["Damage Modifications"][j]["Unit Name"].GetString();
			newData.m_ID = doc["Unit Matchups"][i]["Damage Modifications"][j]["Unit Unique ID"].GetInt();
			newData.m_DamageDoneMod = doc["Unit Matchups"][i]["Damage Modifications"][j]["Damage Done Mod"].GetFloat();

			//Store it
			newPair.second.m_Data.push_back(newData);
		}

		//Store new pair
		m_MatchupDataMap.insert(newPair);

	}

}

const UnitData* SM_UnitDataManager::GetUnitDataByIDConst(unsigned index)
{
	// TODO: insert return statement here

	UnitDataMap::iterator it = m_DataMap.find(index);
	if (it != m_DataMap.end())
		return &it->second;
	else
		return nullptr;
}

UnitData SM_UnitDataManager::GetUnitDataByID(unsigned index)
{
	UnitDataMap::iterator it = m_DataMap.find(index);
	if (it != m_DataMap.end())
		return it->second;
	else
	{ 
		//This shouldn't be hit, so assert an error
		msg_assert(false, "No Unit Data Found");
		return UnitData();
	}
}

UnitMatchupData* const SM_UnitDataManager::GetMatchupData(unsigned index)
{
	MatchupDataMap::iterator it = m_MatchupDataMap.find(index);
	if (it != m_MatchupDataMap.end())
		return &it->second;
	else
	{
		//This shouldn't be hit, so assert an error
		msg_assert(false, "No Matchup Data Found");
		return nullptr;
	}
}

float SM_UnitDataManager::GetDamageModAgainstUnit(unsigned currentUnitIndex, unsigned targetUnitIndex)
{
	MatchupDataMap::iterator it = m_MatchupDataMap.find(currentUnitIndex);
	if (it != m_MatchupDataMap.end())
		return it->second.m_Data[targetUnitIndex].m_DamageDoneMod;
	else
	{
		//This shouldn't be hit, so assert an error
		msg_assert(false, "No Damage Mod Data Found");
		return 1.0f;
	}
}


void SM_UnitDataManager::Release()
{
	//Release containers
	m_DataMap.clear();
	m_MatchupDataMap.clear();
}