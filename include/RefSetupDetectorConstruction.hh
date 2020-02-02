#ifndef __RefSetupDetectorConstruction_H__
#define __RefSetupDetectorConstruction_H__

#include <globals.hh>

#include <vector>
#include <map>

using std::vector;
using std::map;

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class G4UserLimits;
//class templateDetectorMessenger;

#include <G4VUserDetectorConstruction.hh>
#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>

class RefSetupDetectorConstruction: public G4VUserDetectorConstruction {
public:
	RefSetupDetectorConstruction();
	~RefSetupDetectorConstruction();

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

    G4LogicalVolume *m_pVacuumLogicalVolume;
    G4VPhysicalVolume *m_pVacuumPhysicalVolume;
    
	G4LogicalVolume *m_pMainVacuumChamberLogicalVolume;
    G4VPhysicalVolume *m_pMainVacuumChamberPhysicalVolume;
    
    G4LogicalVolume *m_pMainVacuumChamberFlangeLogicalVolume;
    G4VPhysicalVolume *m_pMainVacuumChamberFlangeTopPhysicalVolume;
    G4VPhysicalVolume *m_pMainVacuumChamberFlangeBottomPhysicalVolume;
    
    G4LogicalVolume *m_pPMTScreenLogicalVolume;
    G4VPhysicalVolume *m_pPMTScreenPhysicalVolume;
    
    G4LogicalVolume *m_pSampleHolderFlangeLogicalVolume;
    G4VPhysicalVolume *m_pSampleHolderFlangePhysicalVolume;
    
    G4LogicalVolume *m_pPTFEHolderLogicalVolume;
    G4VPhysicalVolume *m_pPTFEHolderPhysicalVolume;
    
    G4LogicalVolume *m_pPTFELogicalVolume;
    G4VPhysicalVolume *m_pPTFEPhysicalVolume;
    
    G4LogicalVolume *m_pPhotonsLogicalVolume;
    G4VPhysicalVolume *m_pPhotonsPhysicalVolume;
    
	static map<G4String, G4double> m_hGeometryParameters;
	
	//templateDetectorMessenger *m_pDetectorMessenger;
};

#endif

