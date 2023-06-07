#include "FL_Controller.h"

#include <iostream>

FL_Controller::~FL_Controller()
{
	Release();
}

void FL_Controller::InitFromFile(std::string fp, unsigned configIndex)
{
	//Release any current sets
	Release();

	//Load doc
	rapidjson::Document doc;
	ParseNewJSONDocument(doc, fp);

	//Grab array for brevity
	const rapidjson::Value& arr = doc["Fuzzy Logic Configurations"].GetArray();

	//Setup Controller
	m_Name = arr[configIndex]["Configuration Name"].GetString();

	//Initialise each input fuzzy set
	std::vector<FL_Member_Interface::MemberTypes> setTypes;
	for (unsigned i(0); i < arr[configIndex]["Input Sets"].Size(); ++i)
	{
		//Allocate new set
		FL_Set* set = new FL_Set();
		set->SetName(arr[configIndex]["Input Sets"][i]["Set Name"].GetString());
		set->SetID(i);
		
		//Get all member types
		std::vector<FL_Member_Interface::MemberTypes> types;
		//Store shape types for each set
		for (unsigned j(0); j < arr[configIndex]["Input Sets"][i]["Members"].Size(); ++j)
			types.push_back(static_cast<FL_Member_Interface::MemberTypes>(arr[configIndex]["Input Sets"][i]["Members"][j]["Member Shape ID"].GetUint()));

		//Initialise set members
		set->InitSet(arr[configIndex]["Input Sets"][i]["Member Count"].GetUint(), types);
		
		//Init each set member
		for (unsigned j(0); j < arr[configIndex]["Input Sets"][i]["Members"].Size(); ++j)
		{
			//Load members values and store
			std::vector<float> values;

			//Set member value
			for (unsigned k(0); k < arr[configIndex]["Input Sets"][i]["Members"][j]["Values"].GetArray().Size(); ++k)
				values.push_back(arr[configIndex]["Input Sets"][i]["Members"][j]["Values"][k].GetFloat());

			//Init Member
			set->GetMemberAtIndex(j)->InitMember(
				arr[configIndex]["Input Sets"][i]["Members"][j]["Member Name"].GetString(),
				values
			);
		}

		//Store new set
		m_Sets.push_back(set);

	}

	//Setup ruleset
	
	//Set Name
	m_Rules.SetOutputSetName(arr[configIndex]["Output Set Name"].GetString());

	//Initialise the container detials and store keys
	m_Rules.InitRulesContainer(arr[configIndex]["Max Key Depth"].GetUint(), arr[configIndex]["Max Key Length"].GetUint());
	for (unsigned i(0); i < arr[configIndex]["Rule Keys"].GetArray().Size(); ++i)
		m_Rules.SetValueAtKey(arr[configIndex]["Rule Keys"][i].GetString(), arr[configIndex]["Rule Outputs"][i].GetUint());

	//Store output state names
	for (unsigned i(0); i < arr[configIndex]["Output Set"].Size(); ++i)
		m_Rules.InitIndividualName(arr[configIndex]["Output Set"][i]["Output Name"].GetString());

}

unsigned FL_Controller::RunLogicAlgorithm(std::vector<float>& values)
{
	//Pass values into each respective set and each of its members, then extract the highest member into key
	std::string key;
	for (unsigned i(0); i < m_Sets.size(); ++i)
	{
		m_Sets[i]->CalculateMemberships(values[i]);
		key += std::to_string(m_Sets[i]->GetHighestMember()->GetMemberID());
	}

	//Use key to find a state return (and store)
	return m_Rules.GetStateViaKey(key);
}

void FL_Controller::RunLogicSimulationCMD(std::vector<float>& values, std::vector<unsigned>& saveContainer)
{
	///////////////////////////
	/// Simulation & Saving ///
	///////////////////////////
	
	//Call the regular algorithm call
	RunLogicAlgorithm(values);

	//Store Input Set Member IDs
	for (auto& a : m_Sets)
		saveContainer.push_back(a->GetHighestMember()->GetMemberID());
	//Store output
	saveContainer.push_back(m_Rules.GetActiveState());

	//////////////////
	/// CMD Output ///
	//////////////////

	std::cout << "=============> Logic Output <=============\n" << std::endl;
	for (unsigned i(0); i < m_Sets.size(); ++i)
	{
		std::cout << "Set Name & ID: " << m_Sets[i]->GetSetName()
			<< ", " << m_Sets[i]->GetSetID() << std::endl;
		std::cout << "Input Value Was: " << values[i] << std::endl;
		std::cout << "Highest Member Name & ID: " << m_Sets[i]->GetHighestMember()->GetName()
			<< ", " << m_Sets[i]->GetHighestMember()->GetMemberID() << "\n\n" << std::endl;
	}

	std::cout << "Access Key: " << m_Rules.GetLastRuleKey() << std::endl;
	std::cout << "Output State Name & ID: " << m_Rules.GetActiveStateName()
		<< ", " << m_Rules.GetActiveState() << "\n" << std::endl;

	std::cout << "==============> Logic End! <==============\n\n" << std::endl;

}

FL_Set* FL_Controller::GetSetAtIndex(unsigned index)
{
	assert(index <= m_Sets.size());
	return m_Sets[index];
}

void FL_Controller::Release()
{
	//Release Sets
	for (auto& a : m_Sets)
	{
		if (a != nullptr)
		{
			delete a;
			a = nullptr;
		}
	}
}
