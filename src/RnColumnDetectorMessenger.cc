#include <G4SystemOfUnits.hh>
#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>
#include <G4ParticleTable.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWith3Vector.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithABool.hh>
#include <G4Tokenizer.hh>
#include <G4ios.hh>
#include <fstream>
#include <iomanip>

#include "RnColumnDetectorMessenger.hh"
#include "RnColumnDetectorConstruction.hh"

RnColumnDetectorMessenger::RnColumnDetectorMessenger(RnColumnDetectorConstruction *pXeDetector)
:m_pXeDetector(pXeDetector)
{ 
	m_pDetectorDir = new G4UIdirectory("/Xe/detector/");
	m_pDetectorDir->SetGuidance("detector control.");

	m_pNbTopPmtsCmd = new G4UIcmdWithAnInteger("/Xe/detector/setNbTopPmts", this);
	m_pNbTopPmtsCmd->SetGuidance("Define number of top PMTs.");
	m_pNbTopPmtsCmd->SetParameterName("NbTopPmts", false);
	m_pNbTopPmtsCmd->SetRange("NbTopPmts >= 0");
	m_pNbTopPmtsCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	m_pNbBottomPmtsCmd = new G4UIcmdWithAnInteger("/Xe/detector/setNbBottomPmts", this);
	m_pNbBottomPmtsCmd->SetGuidance("Define number of bottom PMTs.");
	m_pNbBottomPmtsCmd->SetParameterName("NbBottomPmts", false);
	m_pNbBottomPmtsCmd->SetRange("NbBottomPmts >= 0");
	m_pNbBottomPmtsCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	m_pSeparationPlateMaterialCmd = new G4UIcmdWithAString("/Xe/detector/setSeparationPlateMaterial", this);
	m_pSeparationPlateMaterialCmd->SetGuidance("Define material of the separation plate.");
	m_pSeparationPlateMaterialCmd->SetParameterName("choice", false);
	m_pSeparationPlateMaterialCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	m_pLXeLevelCmd = new G4UIcmdWithADoubleAndUnit("/Xe/detector/setLXeLevel", this);
	m_pLXeLevelCmd->SetGuidance("Define level of the liquid xenon - spread of the GXe.");
	m_pLXeLevelCmd->SetParameterName("LXeLevel", false);
	m_pLXeLevelCmd->SetRange("LXeLevel >= 0.");
	m_pLXeLevelCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	m_pSS304LSteelReflectivityCmd = new G4UIcmdWithADouble("/Xe/detector/setSSteelReflectivity", this);
	m_pSS304LSteelReflectivityCmd->SetGuidance("Define SS304L/SS316L Steel reflectivity.");
	m_pSS304LSteelReflectivityCmd->SetParameterName("R", false);
	m_pSS304LSteelReflectivityCmd->SetRange("R >= 0. && R <= 1.");
	m_pSS304LSteelReflectivityCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	m_pTeflonReflectivityCmd = new G4UIcmdWithADouble("/Xe/detector/setTeflonReflectivity", this);
	m_pTeflonReflectivityCmd->SetGuidance("Define teflon reflectivity.");
	m_pTeflonReflectivityCmd->SetParameterName("R", false);
	m_pTeflonReflectivityCmd->SetRange("R >= 0. && R <= 1.");
	m_pTeflonReflectivityCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	m_pGXeTeflonReflectivityCmd = new G4UIcmdWithADouble("/Xe/detector/setGXeTeflonReflectivity", this);
	m_pGXeTeflonReflectivityCmd->SetGuidance("Define teflon (into the GXe) reflectivity.");
	m_pGXeTeflonReflectivityCmd->SetParameterName("R", false);
	m_pGXeTeflonReflectivityCmd->SetRange("R >= 0. && R <= 1.");
	m_pGXeTeflonReflectivityCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	m_pLXeAbsorbtionLengthCmd = new G4UIcmdWithADoubleAndUnit("/Xe/detector/setLXeAbsorbtionLength", this);
	m_pLXeAbsorbtionLengthCmd->SetGuidance("Define LXe absorbtion length.");
	m_pLXeAbsorbtionLengthCmd->SetParameterName("AbsL", false);
	m_pLXeAbsorbtionLengthCmd->SetRange("AbsL >= 0.");
	m_pLXeAbsorbtionLengthCmd->SetUnitCategory("Length");
	m_pLXeAbsorbtionLengthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	m_pGXeAbsorbtionLengthCmd = new G4UIcmdWithADoubleAndUnit("/Xe/detector/setGXeAbsorbtionLength", this);
	m_pGXeAbsorbtionLengthCmd->SetGuidance("Define GXe absorbtion length.");
	m_pGXeAbsorbtionLengthCmd->SetParameterName("GAbsL", false);
	m_pGXeAbsorbtionLengthCmd->SetRange("GAbsL >= 0.");
	m_pGXeAbsorbtionLengthCmd->SetUnitCategory("Length");
	m_pGXeAbsorbtionLengthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	m_pLXeRayScatterLengthCmd = new G4UIcmdWithADoubleAndUnit("/Xe/detector/setLXeRayScatterLength", this);
	m_pLXeRayScatterLengthCmd->SetGuidance("Define LXe Rayleigh Scattering length.");
	m_pLXeRayScatterLengthCmd->SetParameterName("ScatL", false);
	m_pLXeRayScatterLengthCmd->SetRange("ScatL >= 0.");
	m_pLXeRayScatterLengthCmd->SetUnitCategory("Length");
	m_pLXeRayScatterLengthCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
	
	m_pLXeRefractionIndexCmd = new G4UIcmdWithADouble("/Xe/detector/setLXeRefractionIndex", this);
	m_pLXeRefractionIndexCmd->SetGuidance("Define LXe refraction index (MC: 1.63).");
	m_pLXeRefractionIndexCmd->SetParameterName("LXeR", false);
	m_pLXeRefractionIndexCmd->SetRange("LXeR >= 1.56 && LXeR <= 1.69");
	m_pLXeRefractionIndexCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

RnColumnDetectorMessenger::~RnColumnDetectorMessenger()
{
	delete m_pNbTopPmtsCmd;
	delete m_pNbBottomPmtsCmd;
	delete m_pSeparationPlateMaterialCmd;
	delete m_pLXeLevelCmd;
	delete m_pTeflonReflectivityCmd;
	delete m_pGXeTeflonReflectivityCmd;
	delete m_pLXeAbsorbtionLengthCmd;
	delete m_pGXeAbsorbtionLengthCmd;
	delete m_pLXeRayScatterLengthCmd;
	delete m_pLXeRefractionIndexCmd;
	delete m_pSS304LSteelReflectivityCmd;

	delete m_pDetectorDir;
}

void RnColumnDetectorMessenger::SetNewValue(G4UIcommand *pUIcommand, G4String hNewValue)
{
	if(pUIcommand == m_pNbTopPmtsCmd)
		m_pXeDetector->SetNbTopPmts(m_pNbTopPmtsCmd->GetNewIntValue(hNewValue));

	if(pUIcommand == m_pNbBottomPmtsCmd)
		m_pXeDetector->SetNbBottomPmts(m_pNbBottomPmtsCmd->GetNewIntValue(hNewValue));
	
	if(pUIcommand == m_pSeparationPlateMaterialCmd)
		m_pXeDetector->SetSeparationPlateMaterial(hNewValue);

	if(pUIcommand == m_pLXeLevelCmd)
		m_pXeDetector->SetLXeLevel(m_pLXeLevelCmd->GetNewDoubleValue(hNewValue));

	if(pUIcommand == m_pTeflonReflectivityCmd)
		m_pXeDetector->SetTeflonReflectivity(m_pTeflonReflectivityCmd->GetNewDoubleValue(hNewValue));
		
	if(pUIcommand == m_pGXeTeflonReflectivityCmd)
		m_pXeDetector->SetGXeTeflonReflectivity(m_pGXeTeflonReflectivityCmd->GetNewDoubleValue(hNewValue));
		
	if(pUIcommand == m_pSS304LSteelReflectivityCmd)
		m_pXeDetector->SetSSteelReflectivity(m_pSS304LSteelReflectivityCmd->GetNewDoubleValue(hNewValue));

	if(pUIcommand == m_pLXeAbsorbtionLengthCmd)
		m_pXeDetector->SetLXeAbsorbtionLength(m_pLXeAbsorbtionLengthCmd->GetNewDoubleValue(hNewValue));

	if(pUIcommand == m_pGXeAbsorbtionLengthCmd)
		m_pXeDetector->SetGXeAbsorbtionLength(m_pGXeAbsorbtionLengthCmd->GetNewDoubleValue(hNewValue));

	if(pUIcommand == m_pLXeRayScatterLengthCmd)
		m_pXeDetector->SetLXeRayScatterLength(m_pLXeRayScatterLengthCmd->GetNewDoubleValue(hNewValue));

	if(pUIcommand == m_pLXeRefractionIndexCmd)
		m_pXeDetector->SetLXeRefractionIndex(m_pLXeRefractionIndexCmd->GetNewDoubleValue(hNewValue));	  
}