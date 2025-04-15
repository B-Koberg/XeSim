#include <G4Material.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Tubs.hh>
#include <G4Sphere.hh>
#include <G4Orb.hh>
#include <G4Polyhedra.hh>
#include <G4Trd.hh>
#include <G4Cons.hh>
#include <G4UnionSolid.hh>
#include <G4IntersectionSolid.hh>
#include <G4SubtractionSolid.hh>
#include "G4LogicalBorderSurface.hh"
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4PVParameterised.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4SDManager.hh>
#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>
#include <G4VisAttributes.hh>
#include <G4Colour.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4VPhysicalVolume.hh>
#include <G4GeometryManager.hh>
#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4UserLimits.hh>
#include <G4RunManager.hh>

#include <globals.hh>
#include <vector>
#include <numeric>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cassert>

using std::vector;
using std::stringstream;
using std::max;

#include "XeSimLXeSensitiveDetector.hh"
#include "XeSimPhotoDetSensitiveDetector.hh"
#include "templateDetectorConstruction.hh"
//#include "templateDetectorMessenger.hh"

map<G4String, G4double> templateDetectorConstruction::m_hGeometryParameters;

templateDetectorConstruction::templateDetectorConstruction() {
    // needs to be set for the AnalysisManager
    m_hGeometryParameters["NbPhotoDets"] = 1;
    //m_pDetectorMessenger = new templateDetectorMessenger(this);
}

templateDetectorConstruction::~templateDetectorConstruction() {
  //delete m_pDetectorMessenger;
}

G4VPhysicalVolume* templateDetectorConstruction::Construct() {
    DefineMaterials();
    
    DefineGeometryParameters();
    
    ConstructLaboratory();
    
    ConstructDetector();
    
    return m_pLabPhysicalVolume;
}

G4double templateDetectorConstruction::GetGeometryParameter(const char *szParameter) {
  return m_hGeometryParameters[szParameter];
}

void templateDetectorConstruction::DefineMaterials() {
  G4Element *Xe = new G4Element("Xenon",     "Xe", 54., 131.293*g/mole);
  G4Element *H  = new G4Element("Hydrogen",  "H",  1.,  1.0079*g/mole);
  G4Element *C  = new G4Element("Carbon",    "C",  6.,  12.011*g/mole);
  G4Element *N  = new G4Element("Nitrogen",  "N",  7.,  14.007*g/mole);
  G4Element *O  = new G4Element("Oxygen",    "O",  8.,  15.999*g/mole);
  G4Element *F  = new G4Element("Fluorine",  "F",  9.,  18.998*g/mole);
  G4Element *Al = new G4Element("Aluminium", "Al", 13., 26.982*g/mole);
  G4Element *Si = new G4Element("Silicon",   "Si", 14., 28.086*g/mole);
  G4Element *Cr = new G4Element("Chromium",  "Cr", 24., 51.996*g/mole);
  G4Element *Mn = new G4Element("Manganese", "Mn", 25., 54.938*g/mole);
  G4Element *Fe = new G4Element("Iron",      "Fe", 26., 55.85*g/mole);
  G4Element *Ni = new G4Element("Nickel",    "Ni", 28., 58.693*g/mole);
  G4Element *Cu = new G4Element("Copper",    "Cu", 29., 63.546*g/mole);
  G4Element *Pb = new G4Element("Lead",      "Pb", 82., 207.2*g/mole);
  G4Element *Mo = new G4Element("Molybdenum","Mo", 42., 95.96*g/mole);
  
  G4NistManager* pNistManager = G4NistManager::Instance();
  pNistManager->FindOrBuildMaterial("G4_AIR");
  
  G4Material *Vacuum = new G4Material("Vacuum", 1.e-20*g/cm3, 2, kStateGas);
  Vacuum->AddElement(N, 0.755);
  Vacuum->AddElement(O, 0.245);
  
  //-------------------------------- liquid xenon ---------------------------------
  G4Material *LXe = new G4Material("LXe", 2.9172*g/cm3, 1, kStateLiquid, 168.15*kelvin, 1.5*atmosphere);
  LXe->AddElement(Xe, 1);

  const G4int iNbEntries = 3;

  G4double pdLXePhotonMomentum[]   = {6.91*eV, 6.98*eV, 7.05*eV}; //178nm 
  G4double pdLXeScintillation[]    = {0.1,     1.0,     0.1};
  G4double pdLXeRefractiveIndex[]  = {1.63,    1.61,    1.58};
  //measured at some point in the past:
  G4double pdLXeAbsorbtionLength[] = {100.*cm, 100.*cm, 100.*cm}; 
  G4double pdLXeScatteringLength[] = {30.*cm,  30.*cm,  30.*cm};
		
  G4MaterialPropertiesTable *pLXePropertiesTable = new G4MaterialPropertiesTable();
	
  pLXePropertiesTable->AddProperty("SCINTILLATIONCOMPONENT1", pdLXePhotonMomentum, pdLXeScintillation, iNbEntries);
  pLXePropertiesTable->AddProperty("SCINTILLATIONCOMPONENT2", pdLXePhotonMomentum, pdLXeScintillation, iNbEntries);
  pLXePropertiesTable->AddProperty("RINDEX", pdLXePhotonMomentum, pdLXeRefractiveIndex, iNbEntries);
  pLXePropertiesTable->AddProperty("ABSLENGTH", pdLXePhotonMomentum, pdLXeAbsorbtionLength, iNbEntries);
  pLXePropertiesTable->AddProperty("RAYLEIGH", pdLXePhotonMomentum, pdLXeScatteringLength, iNbEntries);
	
  // Set to 0./21.6 to turn off scintillation
  pLXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD", 1./(21.6*eV));
  pLXePropertiesTable->AddConstProperty("RESOLUTIONSCALE", 0);
  pLXePropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 3.*ns);
  pLXePropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 27.*ns);
  pLXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD1", 1.0);//ratio btw fast time constant and slow time constant
	
  LXe->SetMaterialPropertiesTable(pLXePropertiesTable);

  //-------------------------------- gaseous xenon --------------------------------
  G4Material *GXe = new G4Material("GXe", 0.005887*g/cm3, 1, kStateGas, 173.15*kelvin, 1.5*atmosphere);
  GXe->AddElement(Xe, 1);

  G4double pdGXePhotonMomentum[]   = {6.91*eV, 6.98*eV, 7.05*eV};
  G4double pdGXeScintillation[]    = {0.1,     1.0,     0.1};
  G4double pdGXeRefractiveIndex[]  = {1.00,    1.00,    1.00};
  G4double pdGXeAbsorbtionLength[] = {100*m,   100*m,   100*m};
  G4double pdGXeScatteringLength[] = {100*m,   100*m,   100*m};

  G4MaterialPropertiesTable *pGXePropertiesTable = new G4MaterialPropertiesTable();

  pGXePropertiesTable->AddProperty("SCINTILLATIONCOMPONENT1", pdGXePhotonMomentum, pdGXeScintillation, iNbEntries);
  pGXePropertiesTable->AddProperty("SCINTILLATIONCOMPONENT2", pdGXePhotonMomentum, pdGXeScintillation, iNbEntries);
  pGXePropertiesTable->AddProperty("RINDEX", pdGXePhotonMomentum, pdGXeRefractiveIndex, iNbEntries);
  pGXePropertiesTable->AddProperty("ABSLENGTH", pdGXePhotonMomentum, pdGXeAbsorbtionLength, iNbEntries);
  pGXePropertiesTable->AddProperty("RAYLEIGH", pdGXePhotonMomentum, pdGXeScatteringLength, iNbEntries);

  // Set to 0./21.6 to turn off scintillation
  pGXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD", 1./(21.6*eV));
  pGXePropertiesTable->AddConstProperty("RESOLUTIONSCALE", 0);
  pGXePropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 3.*ns);
  pGXePropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 27.*ns);
  pGXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD1", 1.0);

  GXe->SetMaterialPropertiesTable(pGXePropertiesTable);
  
  //----------------------------------- quartz ------------------------------------
  // ref: http://www.sciner.com/Opticsland/FS.htm
  G4Material *Quartz = new G4Material("Quartz", 2.201*g/cm3, 2, kStateSolid, 168.15*kelvin, 1.5*atmosphere);
  Quartz->AddElement(Si, 1);
  Quartz->AddElement(O, 2);

  G4double pdQuartzPhotonMomentum[]   = {6.91*eV, 6.98*eV, 7.05*eV};
  G4double pdQuartzRefractiveIndex[]  = {1.50,    1.56,    1.60};
  G4double pdQuartzAbsorbtionLength[] = {30*m,    30*m,    30*m};

  G4MaterialPropertiesTable *pQuartzPropertiesTable = new G4MaterialPropertiesTable();

  pQuartzPropertiesTable->AddProperty("RINDEX", pdQuartzPhotonMomentum, pdQuartzRefractiveIndex, iNbEntries);
  pQuartzPropertiesTable->AddProperty("ABSLENGTH", pdQuartzPhotonMomentum, pdQuartzAbsorbtionLength, iNbEntries);

  Quartz->SetMaterialPropertiesTable(pQuartzPropertiesTable);

  // //------------------------------- stainless steel -------------------------------
  G4Material *SS304LSteel = new G4Material("SS304LSteel", 8.00*g/cm3, 5, kStateSolid);
  SS304LSteel->AddElement(Fe, 0.65);
  SS304LSteel->AddElement(Cr, 0.20);
  SS304LSteel->AddElement(Ni, 0.12);
  SS304LSteel->AddElement(Mn, 0.02);
  SS304LSteel->AddElement(Si, 0.01);

  G4double pdSteelPhotonMomentum[] = {6.91*eV, 6.98*eV, 7.05*eV};
  G4double pdSteelReflectivity[]   = {0.15,    0.2,     0.15};
  G4MaterialPropertiesTable *pSteelPropertiesTable = new G4MaterialPropertiesTable();

  pSteelPropertiesTable->AddProperty("REFLECTIVITY", pdSteelPhotonMomentum, pdSteelReflectivity, iNbEntries);
  SS304LSteel->SetMaterialPropertiesTable(pSteelPropertiesTable);

  //------------------------------- stainless steel -------------------------------
  G4Material *SS316LSteel = new G4Material("SS316LSteel", 8.00*g/cm3, 6, kStateSolid);
  SS316LSteel->AddElement(Fe, 0.682);
  SS316LSteel->AddElement(Cr, 0.172);
  SS316LSteel->AddElement(Ni, 0.109);
  SS316LSteel->AddElement(Mn, 0.016);
  SS316LSteel->AddElement(C, 0.0002);
  SS316LSteel->AddElement(Mo, 0.021);

  //As defined above:
  //G4double pdSteelPhotonMomentum[] = {6.91*eV, 6.98*eV, 7.05*eV};
  //G4double pdSteelReflectivity[]   = {0.15,    0.2,     0.15};
  //G4MaterialPropertiesTable *pSteelPropertiesTable = new G4MaterialPropertiesTable();

  pSteelPropertiesTable->AddProperty("REFLECTIVITY", pdSteelPhotonMomentum, pdSteelReflectivity, iNbEntries);
  SS316LSteel->SetMaterialPropertiesTable(pSteelPropertiesTable);

  //---------------------------------- aluminium ----------------------------------
  pNistManager->FindOrBuildMaterial("G4_Al");

  //---------------------------- photocathode aluminium ---------------------------
  G4Material *PhotoCathodeAluminium = new G4Material("PhotoCathodeAluminium", 8.00*g/cm3, 1, kStateSolid);
  PhotoCathodeAluminium->AddElement(Al, 1);

  G4double pdPhotoCathodePhotonMomentum[]   = {1.*eV, 6.91*eV, 6.98*eV, 7.05*eV};
  G4double pdPhotoCathodeRefractiveIndex[]  = {1.50, 1.50, 1.56, 1.60};
  G4double pdPhotoCathodeAbsorbtionLength[] = {1.*nm, 1.*nm, 1.*nm, 1.*nm};

  G4MaterialPropertiesTable *pPhotoCathodePropertiesTable = new G4MaterialPropertiesTable();

  pPhotoCathodePropertiesTable->AddProperty("RINDEX", pdPhotoCathodePhotonMomentum, pdPhotoCathodeRefractiveIndex, iNbEntries);
  pPhotoCathodePropertiesTable->AddProperty("ABSLENGTH", pdPhotoCathodePhotonMomentum, pdPhotoCathodeAbsorbtionLength, iNbEntries);

  PhotoCathodeAluminium->SetMaterialPropertiesTable(pPhotoCathodePropertiesTable);

  //------------------------------------ PTFE -----------------------------------
  G4Material* PTFE = new G4Material("LXePTFE", 2.2*g/cm3, 2, kStateSolid);
  PTFE->AddElement(C, 0.240183);
  PTFE->AddElement(F, 0.759817);

  G4double pdPTFEPhotonMomentum[]  = {6.91*eV, 6.98*eV, 7.05*eV};
  G4double pdPTFERefractiveIndex[] = {1.63,    1.61,    1.58};
  G4double pdPTFEReflectivity[]    = {0.95,    0.95,    0.95};
  G4double pdPTFESpecularLobe[]    = {0.01,    0.01,    0.01};
  G4double pdPTFESpecularSpike[]   = {0.01,    0.01,    0.01};
  G4double pdPTFEBackscatter[]     = {0.01,    0.01,    0.01};
  G4double pdPTFEEfficiency[]      = {1.0,     1.0,     1.0};
  G4MaterialPropertiesTable *pPTFEPropertiesTable = new G4MaterialPropertiesTable();

  pPTFEPropertiesTable->AddProperty("RINDEX", pdPTFEPhotonMomentum, pdPTFERefractiveIndex, iNbEntries);
  pPTFEPropertiesTable->AddProperty("REFLECTIVITY", pdPTFEPhotonMomentum, pdPTFEReflectivity, iNbEntries);
  pPTFEPropertiesTable->AddProperty("SPECULARLOBECONSTANT", pdPTFEPhotonMomentum, pdPTFESpecularLobe, iNbEntries);
  pPTFEPropertiesTable->AddProperty("SPECULARSPIKECONSTANT", pdPTFEPhotonMomentum, pdPTFESpecularSpike, iNbEntries);
  pPTFEPropertiesTable->AddProperty("BACKSCATTERCONSTANT", pdPTFEPhotonMomentum, pdPTFEBackscatter, iNbEntries);
  pPTFEPropertiesTable->AddProperty("EFFICIENCY", pdPTFEPhotonMomentum, pdPTFEEfficiency, iNbEntries);
  PTFE->SetMaterialPropertiesTable(pPTFEPropertiesTable);

  //------------------------------------ GXe PTFE -----------------------------------
  G4Material* GXePTFE = new G4Material("GXePTFE", 2.2*g/cm3, 2, kStateSolid);
  GXePTFE->AddElement(C, 0.240183);
  GXePTFE->AddElement(F, 0.759817);
  
  G4double pdGXePTFEPhotonMomentum[iNbEntries]  = {6.91*eV, 6.98*eV, 7.05*eV};
  G4double pdGXePTFERefractiveIndex[iNbEntries] = {1.63,    1.61,    1.58};
  G4double pdGXePTFEReflectivity[iNbEntries]    = {0.95,    0.95,    0.95};
  G4double pdGXePTFESpecularLobe[iNbEntries]    = {0.01,    0.01,    0.01};
  G4double pdGXePTFESpecularSpike[iNbEntries]   = {0.01,    0.01,    0.01};
  G4double pdGXePTFEBackscatter[iNbEntries]     = {0.01,    0.01,    0.01};
  G4double pdGXePTFEEfficiency[iNbEntries]      = {1.0,     1.0,     1.0};
  
  G4MaterialPropertiesTable *pGXePTFEPropertiesTable = new G4MaterialPropertiesTable();
  
  pGXePTFEPropertiesTable->AddProperty("RINDEX", pdGXePTFEPhotonMomentum, pdGXePTFERefractiveIndex, iNbEntries);
  pGXePTFEPropertiesTable->AddProperty("REFLECTIVITY", pdGXePTFEPhotonMomentum, pdGXePTFEReflectivity, iNbEntries);
  pGXePTFEPropertiesTable->AddProperty("SPECULARLOBECONSTANT", pdGXePTFEPhotonMomentum, pdGXePTFESpecularLobe, iNbEntries);
  pGXePTFEPropertiesTable->AddProperty("SPECULARSPIKECONSTANT", pdGXePTFEPhotonMomentum, pdGXePTFESpecularSpike, iNbEntries);
  pGXePTFEPropertiesTable->AddProperty("BACKSCATTERCONSTANT", pdGXePTFEPhotonMomentum, pdGXePTFEBackscatter, iNbEntries);
  pGXePTFEPropertiesTable->AddProperty("EFFICIENCY", pdGXePTFEPhotonMomentum, pdGXePTFEEfficiency, iNbEntries);
  
  GXePTFE->SetMaterialPropertiesTable(pGXePTFEPropertiesTable);
}

void templateDetectorConstruction::DefineGeometryParameters() {
    m_hGeometryParameters["PTFECylinderHeight"] = 100.00*mm;
    m_hGeometryParameters["PTFECylinderInnerRadius"] = 40.00*mm;
    m_hGeometryParameters["PTFECylinderOuterRadius"] = 50.00*mm;
    
    m_hGeometryParameters["PTFECylinderTopEndHeight"] = 10.00*mm;
    m_hGeometryParameters["PTFECylinderBottomEndHeight"] = 10.00*mm;
    
    m_hGeometryParameters["LXeHeight"] = 70.00*mm;
}

void templateDetectorConstruction::ConstructLaboratory() {
  const G4double dLabHalfX = 1.5*m;
  const G4double dLabHalfY = 1.5*m;
  const G4double dLabHalfZ = 1.5*m;

  G4Box *pLabBox = new G4Box("LabBox", dLabHalfX, dLabHalfY, dLabHalfZ);
	
  G4Material *Air = G4Material::GetMaterial("G4_AIR");

  m_pLabLogicalVolume = new G4LogicalVolume(pLabBox, Air, "LabLogicalVolume", 0, 0, 0);

  G4Colour LabColor(1.0,0.0,0.0,1.0);
  G4VisAttributes *pLabVisAtt = new G4VisAttributes(LabColor);
  pLabVisAtt->SetVisibility(false);
  m_pLabLogicalVolume->SetVisAttributes(pLabVisAtt);

  m_pLabPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(), m_pLabLogicalVolume, "Lab", 0, false, 0);
}

void templateDetectorConstruction::ConstructDetector() {
  G4SDManager *pSDManager = G4SDManager::GetSDMpointer();
  
  G4Material *LXe = G4Material::GetMaterial("LXe");
  G4Material *GXe = G4Material::GetMaterial("GXe");
  G4Material *LXePTFE = G4Material::GetMaterial("LXePTFE");
  G4Material *GXePTFE = G4Material::GetMaterial("GXePTFE");
  G4Material *PhotoDetAluminium = G4Material::GetMaterial("PhotoCathodeAluminium");
  
  const G4double dLXeHalfZ = 0.5*GetGeometryParameter("PTFECylinderHeight")+0.5*GetGeometryParameter("PTFECylinderTopEndHeight")+0.5*GetGeometryParameter("PTFECylinderBottomEndHeight");
  const G4double dLXeOffsetZ = 0.5*GetGeometryParameter("PTFECylinderHeight")-GetGeometryParameter("LXeHeight");
  
  G4Tubs *pLXeTubs = new G4Tubs("LXeTubs", 0., GetGeometryParameter("PTFECylinderOuterRadius")-0.01, 
                                           dLXeHalfZ, 0.*deg, 360.*deg);
  m_pLXeLogicalVolume = new G4LogicalVolume(pLXeTubs, LXe, "LXeVolume", 0, 0, 0);
  m_pLXePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., dLXeOffsetZ), m_pLXeLogicalVolume,
                                              "LXe", m_pLabLogicalVolume, false, 0);
  
  const G4double dGXeHalfZ = 0.5*(GetGeometryParameter("PTFECylinderHeight")-GetGeometryParameter("LXeHeight"))+0.5*GetGeometryParameter("PTFECylinderTopEndHeight");
  const G4double dGXeOffsetZ = dGXeHalfZ+(GetGeometryParameter("LXeHeight")-0.5*GetGeometryParameter("PTFECylinderHeight"));
  
  G4Tubs *pGXeTubs = new G4Tubs("GXeTubs", 0., GetGeometryParameter("PTFECylinderOuterRadius")-0.01, 
                                           dGXeHalfZ, 0.*deg, 360.*deg);
  m_pGXeLogicalVolume = new G4LogicalVolume(pGXeTubs, GXe, "GXeVolume", 0, 0, 0);
  m_pGXePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., dGXeOffsetZ), m_pGXeLogicalVolume, 
                                              "GXe", m_pLXeLogicalVolume, false, 0);


  XeSimLXeSensitiveDetector *pLXeSD = new XeSimLXeSensitiveDetector("template/LXeSD");
  pSDManager->AddNewDetector(pLXeSD);
  m_pLXeLogicalVolume->SetSensitiveDetector(pLXeSD);
  m_pGXeLogicalVolume->SetSensitiveDetector(pLXeSD);
  
  const G4double dLXePTFEOffsetZ = -0.5*(GetGeometryParameter("PTFECylinderHeight")-GetGeometryParameter("LXeHeight"));

  G4Tubs *pLXePTFETubs = new G4Tubs("LXePTFETubs", GetGeometryParameter("PTFECylinderInnerRadius"), 
                                             GetGeometryParameter("PTFECylinderOuterRadius"), 
                                             0.5*GetGeometryParameter("LXeHeight"), 
                                             0.*deg, 360.*deg);
  m_pLXePTFELogicalVolume = new G4LogicalVolume(pLXePTFETubs, LXePTFE, "LXePTFEVolume", 0, 0, 0);
  m_pLXePTFEPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., dLXePTFEOffsetZ), m_pLXePTFELogicalVolume, 
                                            "LXePTFECylinder", m_pLXeLogicalVolume, false, 0);
       
  const G4double dGXePTFEHalfZ = 0.5*(GetGeometryParameter("PTFECylinderHeight")-GetGeometryParameter("LXeHeight"));
  
  G4Tubs *pGXePTFETubs = new G4Tubs("GXePTFETubs", GetGeometryParameter("PTFECylinderInnerRadius"), 
                                             GetGeometryParameter("PTFECylinderOuterRadius"), 
                                             dGXePTFEHalfZ, 0.*deg, 360.*deg);
  m_pGXePTFELogicalVolume = new G4LogicalVolume(pGXePTFETubs, GXePTFE, "GXePTFEVolume", 0, 0, 0);
  m_pGXePTFEPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., -0.5*GetGeometryParameter("PTFECylinderTopEndHeight")), m_pGXePTFELogicalVolume, 
                                            "GXePTFECylinder", m_pGXeLogicalVolume, false, 0);
          
  const G4double dPTFETopOffsetZ = dGXeHalfZ-0.5*GetGeometryParameter("PTFECylinderTopEndHeight");
          
  G4Tubs *pPTFETopTubs = new G4Tubs("PTFETopTubs", 0., GetGeometryParameter("PTFECylinderOuterRadius"), 
                                             0.5*GetGeometryParameter("PTFECylinderTopEndHeight"), 
                                             0.*deg, 360.*deg);
  m_pPTFETopLogicalVolume = new G4LogicalVolume(pPTFETopTubs, GXePTFE, "PTFETopVolume", 0, 0, 0);
  m_pPTFETopPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., dPTFETopOffsetZ), m_pPTFETopLogicalVolume, 
                                            "GXePTFETop", m_pGXeLogicalVolume, false, 0);
                                                    
  const G4double dPhotoDetHeight = 1.;
  const G4double dPhotoDetOffsetZ = -dLXeHalfZ+GetGeometryParameter("PTFECylinderBottomEndHeight")-dPhotoDetHeight;
  G4Box *pPhotoDetBox = new G4Box("PhotoDetBox", 5, 5, dPhotoDetHeight);
  m_pPhotoDetLogicalVolume = new G4LogicalVolume(pPhotoDetBox, PhotoDetAluminium, "PmtPhotoCathodeLogicalVolume", 0, 0, 0);
  m_pPhotoDetPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., dPhotoDetOffsetZ),
						       m_pPhotoDetLogicalVolume, "PmtPhotoCathode", m_pLXeLogicalVolume, false, 0);

  XeSimPhotoDetSensitiveDetector *pPmtSD = new XeSimPhotoDetSensitiveDetector("template/PhotoDetSD");
  pSDManager->AddNewDetector(pPmtSD);
  m_pPhotoDetLogicalVolume->SetSensitiveDetector(pPmtSD);        
          
  const G4double dPTFEBottomOffsetZ = -dLXeHalfZ+0.5*GetGeometryParameter("PTFECylinderBottomEndHeight");
  const G4double dPhotoDetOffsetSubtr = 0.5*GetGeometryParameter("PTFECylinderBottomEndHeight")-dPhotoDetHeight;
  
  G4Tubs *pPTFEBottomTubs = new G4Tubs("PTFEBottomTubs", 0., GetGeometryParameter("PTFECylinderOuterRadius"), 
                                             0.5*GetGeometryParameter("PTFECylinderBottomEndHeight"), 
                                             0.*deg, 360.*deg);
  G4SubtractionSolid *pPTFEBottomTubs_cut = new G4SubtractionSolid("PTFEBottomTubs_cut", 
                      pPTFEBottomTubs, pPhotoDetBox, 0, G4ThreeVector(0., 0., dPhotoDetOffsetSubtr));
  
  m_pPTFEBottomLogicalVolume = new G4LogicalVolume(pPTFEBottomTubs_cut, LXePTFE, "PTFEBottomVolume", 0, 0, 0);
  m_pPTFEBottomPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., dPTFEBottomOffsetZ), m_pPTFEBottomLogicalVolume, 
                                            "LXePTFEBottom", m_pLXeLogicalVolume, false, 0);


  G4double dSigmaAlpha = 0.1;
  G4OpticalSurface *pLXePTFEOpticalSurface = new G4OpticalSurface("LXePTFEOpticalSurface",
  	unified, groundbackpainted, dielectric_dielectric, dSigmaAlpha);
  	
  G4OpticalSurface *pGXePTFEOpticalSurface = new G4OpticalSurface("GXePTFEOpticalSurface", 
  	unified, groundbackpainted, dielectric_dielectric, dSigmaAlpha);
  	
  pLXePTFEOpticalSurface->SetMaterialPropertiesTable(LXePTFE->GetMaterialPropertiesTable());
  pGXePTFEOpticalSurface->SetMaterialPropertiesTable(GXePTFE->GetMaterialPropertiesTable());
  
  new G4LogicalBorderSurface("LXePTFECylinderLogicalBorderSurface",
  	m_pLXePhysicalVolume, m_pLXePTFEPhysicalVolume, pLXePTFEOpticalSurface);
    
  new G4LogicalBorderSurface("GXePTFECylinderLogicalBorderSurface",
  	m_pGXePhysicalVolume, m_pGXePTFEPhysicalVolume, pGXePTFEOpticalSurface);
  
  new G4LogicalBorderSurface("GXePTFETopCylinderLogicalBorderSurface",
  	m_pGXePhysicalVolume, m_pPTFETopPhysicalVolume, pGXePTFEOpticalSurface);
  
  new G4LogicalBorderSurface("LXePTFEBottomCylinderLogicalBorderSurface",
  	m_pLXePhysicalVolume, m_pPTFEBottomPhysicalVolume, pLXePTFEOpticalSurface);
	

  G4Colour hPTFEColor(1., 0., 1., 1.);
  G4Colour hLXeColor(0.0,0.0,1.0,0.25);
  G4Colour hGXeColor(0.0,1.0,1.0,0.25);

  G4VisAttributes *pLXeVisAtt = new G4VisAttributes(hLXeColor);
  pLXeVisAtt->SetVisibility(true);
  m_pLXeLogicalVolume->SetVisAttributes(pLXeVisAtt);
	
  G4VisAttributes *pGXeVisAtt = new G4VisAttributes(hGXeColor);
  pGXeVisAtt->SetVisibility(true);
  m_pGXeLogicalVolume->SetVisAttributes(pGXeVisAtt);
  
  G4VisAttributes *pLXePTFEVisAtt = new G4VisAttributes(hPTFEColor);
  pLXePTFEVisAtt->SetVisibility(true);
  m_pLXePTFELogicalVolume->SetVisAttributes(pLXePTFEVisAtt);
  m_pGXePTFELogicalVolume->SetVisAttributes(pLXePTFEVisAtt);
  m_pPTFETopLogicalVolume->SetVisAttributes(pLXePTFEVisAtt);
  m_pPTFEBottomLogicalVolume->SetVisAttributes(pLXePTFEVisAtt);
}