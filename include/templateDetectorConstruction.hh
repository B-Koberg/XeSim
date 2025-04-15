#ifndef __templateDetectorConstruction_H__
#define __templateDetectorConstruction_H__

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

class templateDetectorConstruction: public G4VUserDetectorConstruction {
public:
	templateDetectorConstruction();
	~templateDetectorConstruction();

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

    G4LogicalVolume *m_pLXeLogicalVolume;
    G4VPhysicalVolume *m_pLXePhysicalVolume;
    
	G4LogicalVolume *m_pGXeLogicalVolume;
    G4VPhysicalVolume *m_pGXePhysicalVolume;
    
    G4LogicalVolume *m_pLXePTFELogicalVolume;
    G4VPhysicalVolume *m_pLXePTFEPhysicalVolume;
    
    G4LogicalVolume *m_pGXePTFELogicalVolume;
    G4VPhysicalVolume *m_pGXePTFEPhysicalVolume;
    
    G4LogicalVolume *m_pPTFETopLogicalVolume;
    G4VPhysicalVolume *m_pPTFETopPhysicalVolume;
    
    G4LogicalVolume *m_pPTFEBottomLogicalVolume;
    G4VPhysicalVolume *m_pPTFEBottomPhysicalVolume;
    
    G4LogicalVolume *m_pPhotoDetLogicalVolume;
    G4VPhysicalVolume *m_pPhotoDetPhysicalVolume;
    
	static map<G4String, G4double> m_hGeometryParameters;
	
	//templateDetectorMessenger *m_pDetectorMessenger;
};

#endif

