#ifndef __EnergyDistributionRock_H__
#define __EnergyDistributionRock_H__

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

class EnergyDistributionRock: public G4VUserDetectorConstruction {
public:
	EnergyDistributionRock();
	~EnergyDistributionRock();

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

	G4LogicalVolume *m_pRockLogicalVolume;
	G4VPhysicalVolume *m_pRockPhysicalVolume;

	G4LogicalVolume *m_pRockSpawnLogicalVolume;
	G4VPhysicalVolume *m_pRockSpawnPhysicalVolume;

	G4LogicalVolume *m_pConcreteLogicalVolume;
	G4VPhysicalVolume *m_pConcretePhysicalVolume;

	G4LogicalVolume *m_pConcreteSpawnLogicalVolume;
	G4VPhysicalVolume *m_pConcreteSpawnPhysicalVolume;

	G4LogicalVolume *m_pCountLogicalVolume;
	G4VPhysicalVolume *m_pCountPhysicalVolume;

	G4LogicalVolume *m_pControlLogicalVolume;
	G4VPhysicalVolume *m_pControlPhysicalVolume;


	G4LogicalVolume *m_pPhotoDetLogicalVolume;
	G4VPhysicalVolume *m_pPhotoDetPhysicalVolume;

	G4LogicalVolume *m_pLXeLogicalVolume;
	G4VPhysicalVolume *m_pLXePhysicalVolume;

	static map<G4String, G4double> m_hGeometryParameters;
	
	//NeutronShieldingMessenger *m_pDetectorMessenger;
};

#endif

