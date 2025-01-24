#ifndef __RnColumnDetectorConstruction_H__
#define __RnColumnDetectorConstruction_H__

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

class RnColumnDetectorConstruction: public G4VUserDetectorConstruction {
public:
	RnColumnDetectorConstruction();
	~RnColumnDetectorConstruction();

	G4VPhysicalVolume* Construct();

	static G4double GetGeometryParameter(const char *szParameter);

private:
	void DefineMaterials();
	void DefineGeometryParameters();

	void ConstructLaboratory();
	void ConstructReboiler();
    void ConstructPMTs();
    
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
    
    G4LogicalVolume *m_pReboilerTubeLogicalVolume;
    G4VPhysicalVolume *m_pReboilerTubePhysicalVolume;

    G4LogicalVolume *m_pReboilerTopFlangeLogicalVolume;
    G4VPhysicalVolume *m_pReboilerTopFlangePhysicalVolume;

    G4LogicalVolume *m_pReboilerBottomFlangeLogicalVolume;
    G4VPhysicalVolume *m_pReboilerBottomFlangePhysicalVolume;

    G4LogicalVolume *m_pLXeLogicalVolume;
    G4VPhysicalVolume *m_pLXePhysicalVolume;

    G4LogicalVolume *m_pGXeLogicalVolume;
    G4VPhysicalVolume *m_pGXePhysicalVolume;

    G4LogicalVolume *m_pPmtWindowLogicalVolume;
	G4LogicalVolume *m_pPmtCasingLogicalVolume;
	G4LogicalVolume *m_pPmtInteriorLogicalVolume;
	G4LogicalVolume *m_pPmtPhotoCathodeLogicalVolume;
	G4LogicalVolume *m_pPmtBaseLogicalVolume;

    vector<G4VPhysicalVolume *> m_hPmtWindowPhysicalVolumes;
	vector<G4VPhysicalVolume *> m_hPmtCasingPhysicalVolumes;
	G4VPhysicalVolume *m_pPmtInteriorPhysicalVolume;
	G4VPhysicalVolume *m_pPmtPhotoCathodePhysicalVolume;
	vector<G4VPhysicalVolume *> m_hPmtBasePhysicalVolumes;

    G4LogicalVolume * m_pPTFEPMTSeparationLogicalVolume;
    G4VPhysicalVolume * m_pPTFEPMTSeparationPhysicalVolume;
    
	static map<G4String, G4double> m_hGeometryParameters;
	
	//templateDetectorMessenger *m_pDetectorMessenger;
};

#endif

