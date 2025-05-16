#ifndef __NeutronShieldingConstruction_H__
#define __NeutronShieldingConstruction_H__

#include <globals.hh>
 
#include <vector>
#include <map>

using std::vector; 
using std::map;

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4UserLimits;
//class NeutronShieldingMessenger;

#include <G4VUserDetectorConstruction.hh>
#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>

class NeutronShieldingConstruction: public G4VUserDetectorConstruction {
public:
	NeutronShieldingConstruction();
	~NeutronShieldingConstruction();

	G4VPhysicalVolume* Construct();

	static G4double GetGeometryParameter(const char *szParameter);

private:
	void DefineMaterials();
	void DefineGeometryParameters();

	void ConstructLaboratory();
	void ConstructDetector();
    
private:
	G4LogicalVolume *m_pLabLogicalVolume;
	G4VPhysicalVolume *m_pLabPhysicalVolume;

	G4LogicalVolume *m_pBuildLogicalVolume;
	G4VPhysicalVolume *m_pBuildPhysicalVolume;

	G4LogicalVolume *m_pBuildAirLogicalVolume;
	G4VPhysicalVolume *m_pBuildAirPhysicalVolume;

	G4LogicalVolume *m_pAbsorberLogicalVolume;
	G4VPhysicalVolume *m_pAbsorberPhysicalVolume;

	G4LogicalVolume *m_pAbsorberAirLogicalVolume;
	G4VPhysicalVolume *m_pAbsorberAirPhysicalVolume;

	G4LogicalVolume *m_pLXeContainerLogicalVolume;
    G4VPhysicalVolume *m_pLXeContainerPhysicalVolume;

    G4LogicalVolume *m_pLXeLogicalVolume;
    G4VPhysicalVolume *m_pLXePhysicalVolume;


	G4LogicalVolume *m_pRockLogicalVolume;
	G4VPhysicalVolume *m_pRockPhysicalVolume;

	G4LogicalVolume *m_pConcreteLogicalVolume;
	G4VPhysicalVolume *m_pConcretePhysicalVolume;

	G4LogicalVolume *m_pSpawnLogicalVolume;
	G4VPhysicalVolume *m_pSpawnPhysicalVolume;

	G4LogicalVolume *m_pAirLogicalVolume;
	G4VPhysicalVolume *m_pAirPhysicalVolume;


	G4LogicalVolume *m_pWaterLogicalVolume;
	G4VPhysicalVolume *m_pWaterPhysicalVolume;

	G4LogicalVolume *m_pWaterContainerLogicalVolume;
	G4VPhysicalVolume *m_pWaterContainerPhysicalVolume;


	G4LogicalVolume *m_pPhotoDetLogicalVolume;
    G4VPhysicalVolume *m_pPhotoDetPhysicalVolume;
	
    
	static map<G4String, G4double> m_hGeometryParameters;
	
	//NeutronShieldingMessenger *m_pDetectorMessenger;
};

#endif

