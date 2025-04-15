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

#include "muensterTPCDetectorMessenger.hh"
#include "muensterTPCDetectorConstruction.hh"

muensterTPCDetectorMessenger::muensterTPCDetectorMessenger(muensterTPCDetectorConstruction *pXeDetector)
:m_pXeDetector(pXeDetector)
{ 
	m_pDetectorDir = new G4UIdirectory("/Xe/detector/");
	m_pDetectorDir->SetGuidance("detector control.");

	// Not working - just a template
	m_pLXeLevelCmd = new G4UIcmdWithADoubleAndUnit("/Xe/detector/setLXeLevel", this);
	m_pLXeLevelCmd->SetGuidance("Define level of the liquid xenon - spread of the GXe.");
	m_pLXeLevelCmd->SetParameterName("LXeLevel", false);
	m_pLXeLevelCmd->SetRange("LXeLevel >= 0.");
	m_pLXeLevelCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	m_pMaterCmd = new G4UIcmdWithAString("/Xe/detector/setMat",this);
	m_pMaterCmd->SetGuidance("Select material of the LXe volume.");
	m_pMaterCmd->SetParameterName("choice",false);
	m_pMaterCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	m_pLXeMeshMaterialCmd = new G4UIcmdWithAString("/Xe/detector/setLXeMeshMaterial",this);
	m_pLXeMeshMaterialCmd->SetGuidance("Select material of the LXe Meshes.");
	m_pLXeMeshMaterialCmd->SetParameterName("choice",false);
	m_pLXeMeshMaterialCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	m_pGXeMeshMaterialCmd = new G4UIcmdWithAString("/Xe/detector/setGXeMeshMaterial",this);
	m_pGXeMeshMaterialCmd->SetGuidance("Select material of the GXe Meshes.");
	m_pGXeMeshMaterialCmd->SetParameterName("choice",false);
	m_pGXeMeshMaterialCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

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

	m_pLXeScintillationCmd = new G4UIcmdWithABool("/Xe/detector/setLXeScintillation", this);
	m_pLXeScintillationCmd->SetGuidance("Switch on/off LXe scintillation in the sensitive volume.");
	m_pLXeScintillationCmd->SetParameterName("LXeScint", false); 
	m_pLXeScintillationCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

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
	
	m_pLXeMeshTransparencyCmd = new G4UIcmdWithADouble("/Xe/detector/setLXeMeshTransparency", this);
	m_pLXeMeshTransparencyCmd->SetGuidance("Define LXe mesh transparency.");
	m_pLXeMeshTransparencyCmd->SetParameterName("Transpa", false);
	m_pLXeMeshTransparencyCmd->SetRange("Transpa >= 0. && Transpa <= 1.");
	m_pLXeMeshTransparencyCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	m_pGXeMeshTransparencyCmd = new G4UIcmdWithADouble("/Xe/detector/setGXeMeshTransparency", this);
	m_pGXeMeshTransparencyCmd->SetGuidance("Define GXe mesh transparency.");
	m_pGXeMeshTransparencyCmd->SetParameterName("Transpa", false);
	m_pGXeMeshTransparencyCmd->SetRange("Transpa >= 0. && Transpa <= 1.");
	m_pGXeMeshTransparencyCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

	m_pLXeRefractionIndexCmd = new G4UIcmdWithADouble("/Xe/detector/setLXeRefractionIndex", this);
    m_pLXeRefractionIndexCmd->SetGuidance("Define LXe refraction index (MC: 1.63).");
    m_pLXeRefractionIndexCmd->SetParameterName("LXeR", false);
    m_pLXeRefractionIndexCmd->SetRange("LXeR >= 1.56 && LXeR <= 1.69");
    m_pLXeRefractionIndexCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

}

muensterTPCDetectorMessenger::~muensterTPCDetectorMessenger()
{
	delete m_pLXeLevelCmd;
	delete m_pMaterCmd;
	delete m_pLXeMeshMaterialCmd;
	delete m_pGXeMeshMaterialCmd;
	delete m_pTeflonReflectivityCmd;
	delete m_pGXeTeflonReflectivityCmd;
	delete m_pLXeScintillationCmd;
	delete m_pLXeAbsorbtionLengthCmd;
	delete m_pGXeAbsorbtionLengthCmd;
	delete m_pLXeRayScatterLengthCmd;
	delete m_pLXeRefractionIndexCmd;
	delete m_pLXeMeshTransparencyCmd;
	delete m_pGXeMeshTransparencyCmd;

	delete m_pDetectorDir;
}

void muensterTPCDetectorMessenger::SetNewValue(G4UIcommand *pUIcommand, G4String hNewValue)
{

	if(pUIcommand == m_pLXeLevelCmd)
		m_pXeDetector->SetLXeLevel(m_pLXeLevelCmd->GetNewDoubleValue(hNewValue));

	if(pUIcommand == m_pMaterCmd )
		m_pXeDetector->SetMaterial(hNewValue);

	if(pUIcommand == m_pLXeMeshMaterialCmd )
		m_pXeDetector->SetLXeMeshMaterial(hNewValue);

	if(pUIcommand == m_pGXeMeshMaterialCmd )
		m_pXeDetector->SetGXeMeshMaterial(hNewValue);

	if(pUIcommand == m_pLXeScintillationCmd)
		m_pXeDetector->SetLXeScintillation(m_pLXeScintillationCmd->GetNewBoolValue(hNewValue));

	if(pUIcommand == m_pTeflonReflectivityCmd)
		m_pXeDetector->SetTeflonReflectivity(m_pTeflonReflectivityCmd->GetNewDoubleValue(hNewValue));		
		
	if(pUIcommand == m_pGXeTeflonReflectivityCmd)
      m_pXeDetector->SetGXeTeflonReflectivity(m_pGXeTeflonReflectivityCmd->GetNewDoubleValue(hNewValue));		
		
	if(pUIcommand == m_pLXeAbsorbtionLengthCmd)
		m_pXeDetector->SetLXeAbsorbtionLength(m_pLXeAbsorbtionLengthCmd->GetNewDoubleValue(hNewValue));

	if(pUIcommand == m_pGXeAbsorbtionLengthCmd)
		m_pXeDetector->SetGXeAbsorbtionLength(m_pGXeAbsorbtionLengthCmd->GetNewDoubleValue(hNewValue));

	if(pUIcommand == m_pLXeRayScatterLengthCmd)
		m_pXeDetector->SetLXeRayScatterLength(m_pLXeRayScatterLengthCmd->GetNewDoubleValue(hNewValue));

	if(pUIcommand == m_pLXeMeshTransparencyCmd)
		m_pXeDetector->SetLXeMeshTransparency(m_pLXeMeshTransparencyCmd->GetNewDoubleValue(hNewValue));

	if(pUIcommand == m_pGXeMeshTransparencyCmd)
		m_pXeDetector->SetGXeMeshTransparency(m_pGXeMeshTransparencyCmd->GetNewDoubleValue(hNewValue));

	if(pUIcommand == m_pLXeRefractionIndexCmd)
      m_pXeDetector->SetLXeRefractionIndex(m_pLXeRefractionIndexCmd->GetNewDoubleValue(hNewValue));	  
}


