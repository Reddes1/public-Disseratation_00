#include "FL_RuleModule.h"

#include <assert.h>
#include <sstream>

void FL_RuleModule::InitRulesContainer(unsigned keyDepth, unsigned keyLength)
{
	assert(keyDepth <= 9);

	//Create a string equal to required key depth (i.e 3 = "012")
	unsigned short key = 0;
	std::string str;

	for (unsigned i(0); i < keyDepth; ++i)
	{
		str += std::to_string(key++);
	}
	
	//Get str as char arr for algorithm
	char* charStr = &str[0];

	//Run recursive algorithm to calculate
	GetPermutations(charStr, keyDepth, keyLength);
}

void FL_RuleModule::InitStateNames(std::vector<std::string>& names)
{
	//Clear and reserve space
	ClearNames();
	m_StateNames.reserve(names.size());

	//Insert names
	for (unsigned i(0); i < names.size(); ++i)
		m_StateNames.push_back(names[i]);

}

bool FL_RuleModule::SetValueAtKey(std::string key, unsigned value)
{
	std::unordered_map<std::string, unsigned>::iterator entry = m_RulePermutations.find(key);

	if (entry != m_RulePermutations.end())
	{
		entry->second = value;
		return true;
	}
	else
		assert(false);

	return false;
}

bool FL_RuleModule::SetNumerousValues(std::vector<std::string>& keys, std::vector<unsigned>& values)
{
	assert(keys.size() <= values.size());

	for (unsigned i(0); i < keys.size(); ++i)
		SetValueAtKey(keys[i], values[i]);

	return true;
}

unsigned FL_RuleModule::GetStateViaKey(std::string key)
{
	std::unordered_map<std::string, unsigned>::iterator entry = m_RulePermutations.find(key);

	if (entry != m_RulePermutations.end())
	{
		//Store key and active state before returning value
		m_LastKey = key;
		m_ActiveState = entry->second;
		return entry->second;
	}
	else
	{
		//No entry found, likely a bug
		assert(false);
	}

	//Return large and likely unusable value (shouldn't hit this)
	return 1000000;
}

std::string FL_RuleModule::GetStateNameAtIndex(unsigned index)
{
	assert(index >=  m_StateNames.size());

	return m_StateNames[index];
}

void FL_RuleModule::GetPermutations(char* str, unsigned keyDepth, unsigned strLength)
{
	//Temporary array for use in generated permutations
	char* tempArr = new char[strLength];
	tempArr[strLength] = '\0';

	//Start recursion
	StorePermutationR(str, tempArr, keyDepth, strLength, 0);
}

void FL_RuleModule::StorePermutationR(char* str, char* tempStr, unsigned keyDepth, unsigned strLength, unsigned index)
{
	for (unsigned i(0); i < keyDepth; ++i)
	{
		tempStr[index] = str[i];

		if (index == strLength - 1)
			m_RulePermutations.insert(std::pair<std::string, int>(tempStr, 0));
		else
			StorePermutationR(str, tempStr, keyDepth, strLength, index + 1);
	}
}

void FL_RuleModule::SetActiveState(std::string& key)
{

}

void FL_RuleModule::Release()
{
	m_RulePermutations.clear();
	m_StateNames.clear();
}
