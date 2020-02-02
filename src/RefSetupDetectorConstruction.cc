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
#include "RefSetupDetectorConstruction.hh"
//#include "templateDetectorMessenger.hh"

map<G4String, G4double> RefSetupDetectorConstruction::m_hGeometryParameters;

RefSetupDetectorConstruction::RefSetupDetectorConstruction() {
    // needs to be set for the AnalysisManager
    m_hGeometryParameters["NbPhotoDets"] = 1;
    //m_pDetectorMessenger = new templateDetectorMessenger(this);
}

RefSetupDetectorConstruction::~RefSetupDetectorConstruction() {
  //delete m_pDetectorMessenger;
}

G4VPhysicalVolume* RefSetupDetectorConstruction::Construct() {
    DefineMaterials();
    
    DefineGeometryParameters();
    
    ConstructLaboratory();
    
    ConstructDetector();
    
    return m_pLabPhysicalVolume;
}

G4double RefSetupDetectorConstruction::GetGeometryParameter(const char *szParameter) {
  return m_hGeometryParameters[szParameter];
}

void RefSetupDetectorConstruction::DefineMaterials() {
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
  
  const G4int iNbEntries = 3;
  
  G4Material *Vacuum = new G4Material("Vacuum", 1.e-20*g/cm3, 2, kStateGas);
  Vacuum->AddElement(N, 0.755);
  Vacuum->AddElement(O, 0.245);
  
  G4MaterialPropertiesTable *pVacuumPropertiesTable = new G4MaterialPropertiesTable();
  G4double pdVacuumPhotonMomentum[]   = {6.91*eV, 6.98*eV, 7.05*eV}; //178nm 
  G4double pdVacuumAbsorbtionLength[] = {300.*cm, 300.*cm, 300.*cm}; 
  G4double pdVacuumRefractiveIndex[]  = {1.,    1.,    1.};

  pVacuumPropertiesTable->AddProperty("RINDEX", pdVacuumPhotonMomentum, pdVacuumRefractiveIndex, iNbEntries);
  pVacuumPropertiesTable->AddProperty("ABSLENGTH", pdVacuumPhotonMomentum, pdVacuumAbsorbtionLength, iNbEntries);

  Vacuum->SetMaterialPropertiesTable(pVacuumPropertiesTable);
  
  //-------------------------------- liquid xenon ---------------------------------
  G4Material *LXe = new G4Material("LXe", 2.9172*g/cm3, 1, kStateLiquid, 168.15*kelvin, 1.5*atmosphere);
  LXe->AddElement(Xe, 1);

  G4double pdLXePhotonMomentum[]   = {6.91*eV, 6.98*eV, 7.05*eV}; //178nm 
  G4double pdLXeScintillation[]    = {0.1,     1.0,     0.1};
  G4double pdLXeRefractiveIndex[]  = {1.63,    1.61,    1.58};
  //measured at some point in the past:
  G4double pdLXeAbsorbtionLength[] = {100.*cm, 100.*cm, 100.*cm}; 
  G4double pdLXeScatteringLength[] = {30.*cm,  30.*cm,  30.*cm};
		
  G4MaterialPropertiesTable *pLXePropertiesTable = new G4MaterialPropertiesTable();
	
  pLXePropertiesTable->AddProperty("FASTCOMPONENT", pdLXePhotonMomentum, pdLXeScintillation, iNbEntries);
  pLXePropertiesTable->AddProperty("SLOWCOMPONENT", pdLXePhotonMomentum, pdLXeScintillation, iNbEntries);
  pLXePropertiesTable->AddProperty("RINDEX", pdLXePhotonMomentum, pdLXeRefractiveIndex, iNbEntries);
  pLXePropertiesTable->AddProperty("ABSLENGTH", pdLXePhotonMomentum, pdLXeAbsorbtionLength, iNbEntries);
  pLXePropertiesTable->AddProperty("RAYLEIGH", pdLXePhotonMomentum, pdLXeScatteringLength, iNbEntries);
	
  pLXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD", 0./(21.6*eV));
  pLXePropertiesTable->AddConstProperty("RESOLUTIONSCALE", 0);
  pLXePropertiesTable->AddConstProperty("FASTTIMECONSTANT", 3.*ns);
  pLXePropertiesTable->AddConstProperty("SLOWTIMECONSTANT", 27.*ns);
  pLXePropertiesTable->AddConstProperty("YIELDRATIO", 1.0);//ratio btw fast time constant and slow time constant
	
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

  pGXePropertiesTable->AddProperty("FASTCOMPONENT", pdGXePhotonMomentum, pdGXeScintillation, iNbEntries);
  pGXePropertiesTable->AddProperty("SLOWCOMPONENT", pdGXePhotonMomentum, pdGXeScintillation, iNbEntries);
  pGXePropertiesTable->AddProperty("RINDEX", pdGXePhotonMomentum, pdGXeRefractiveIndex, iNbEntries);
  pGXePropertiesTable->AddProperty("ABSLENGTH", pdGXePhotonMomentum, pdGXeAbsorbtionLength, iNbEntries);
  pGXePropertiesTable->AddProperty("RAYLEIGH", pdGXePhotonMomentum, pdGXeScatteringLength, iNbEntries);

  pGXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD", 0./(21.6*eV));
  pGXePropertiesTable->AddConstProperty("RESOLUTIONSCALE", 0);
  pGXePropertiesTable->AddConstProperty("FASTTIMECONSTANT", 3.*ns);
  pGXePropertiesTable->AddConstProperty("SLOWTIMECONSTANT", 27.*ns);
  pGXePropertiesTable->AddConstProperty("YIELDRATIO", 1.0);

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
  G4double pdSteelReflectivity[]   = {0.05, 0.05, 0.05}; //{0.15,    0.2,     0.15};
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

void RefSetupDetectorConstruction::DefineGeometryParameters() {
    m_hGeometryParameters["MainVacuumChamberOuterRadius"] = 330.00*mm;
    m_hGeometryParameters["MainVacuumChamberInnerRadius"] = 325.00*mm;
    m_hGeometryParameters["MainVacuumChamberHalfZ"] = 150.00*mm;
    m_hGeometryParameters["MainVacuumChamberFlangeHalfZ"] = 2.50*mm;
    
    m_hGeometryParameters["CenterToPMTSlit"] = 40.50*mm;
    m_hGeometryParameters["CenterToPMTCathode"] = 69.25*mm;
    
    // closer to PMT, screw holes for PTFE sample holder
    m_hGeometryParameters["CenterFlangeToPTFESamples"] = 6.25*mm;
    
    m_hGeometryParameters["HolderFlangeRadius"] = 35.00*mm;
    m_hGeometryParameters["HolderFlangeHeight"] = 12.00*mm;
    m_hGeometryParameters["HolderFlangeQuartzTubeInnerRadius"] = 39.00*mm;
    m_hGeometryParameters["HolderFlangeQuartzTubeOuterRadius"] = 43.00*mm;
    
    m_hGeometryParameters["PTFEHolderHalfX"] = 0.5*50.00*mm;
    m_hGeometryParameters["PTFEHolderHalfY"] = 0.5*27.00*mm;
    m_hGeometryParameters["PTFEHolderHalfZ"] = 0.5*9.00*mm;
    
    m_hGeometryParameters["PTFESampleHalfZ"] = 0.5*7.00*mm;
    m_hGeometryParameters["PTFESampleHalfX"] = 0.5*10.00*mm;
    m_hGeometryParameters["PTFESampleCutHalfX"] = 0.5*5.00*mm;
    m_hGeometryParameters["PTFESampleHalfY"] = 0.5*25.00*mm;
    m_hGeometryParameters["PTFESampleCutHalfY"] = 0.5*10.00*mm;
    m_hGeometryParameters["PTFESampleHoleHalfX"] = 0.5*4.00*mm;
    
    m_hGeometryParameters["PMTScreenHeight"] = 100.00*mm;
}

void RefSetupDetectorConstruction::ConstructLaboratory() {
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

void RefSetupDetectorConstruction::ConstructDetector() {
  G4SDManager *pSDManager = G4SDManager::GetSDMpointer();
  
  G4Material *Vacuum = G4Material::GetMaterial("Vacuum");
  G4Material *SS316LSteel = G4Material::GetMaterial("SS316LSteel");
  G4Material *LXe = G4Material::GetMaterial("LXe");
  G4Material *GXe = G4Material::GetMaterial("GXe");
  G4Material *GXePTFE = G4Material::GetMaterial("GXePTFE");
  G4Material *PhotoDetAluminium = G4Material::GetMaterial("PhotoCathodeAluminium");
  
  // Main vacuum chamber
  G4Tubs *pVacuumTubs = new G4Tubs("VacuumTubs", 0., GetGeometryParameter("MainVacuumChamberOuterRadius")-0.01, 
                                              GetGeometryParameter("MainVacuumChamberHalfZ")+0.01, 0.*deg, 360.*deg);
  m_pVacuumLogicalVolume = new G4LogicalVolume(pVacuumTubs, Vacuum, "VacuumVolume", 0, 0, 0);
  m_pVacuumPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), m_pVacuumLogicalVolume,
                                              "Vacuum", m_pLabLogicalVolume, false, 0);
  
  G4Tubs *pMainVacuumChamberTubs = new G4Tubs("MainVacuumChamberTubs", GetGeometryParameter("MainVacuumChamberInnerRadius"), 
                                             GetGeometryParameter("MainVacuumChamberOuterRadius"), 
                                             GetGeometryParameter("MainVacuumChamberHalfZ"), 
                                             0.*deg, 360.*deg);
  m_pMainVacuumChamberLogicalVolume = new G4LogicalVolume(pMainVacuumChamberTubs, SS316LSteel, "MainVacuumChamberVolume", 0, 0, 0);
  m_pMainVacuumChamberPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), m_pMainVacuumChamberLogicalVolume, 
                                            "MainVacuumChamberCylinder", m_pVacuumLogicalVolume, false, 0);
  
  G4Tubs *pMainVacuumChamberFlangeTubs = new G4Tubs("MainVacuumChamberFlangeTubs", 0., GetGeometryParameter("MainVacuumChamberOuterRadius"), 
                                              GetGeometryParameter("MainVacuumChamberFlangeHalfZ"), 0.*deg, 360.*deg);
  
  m_pMainVacuumChamberFlangeLogicalVolume = new G4LogicalVolume(pMainVacuumChamberFlangeTubs, SS316LSteel, "MainVacuumChamberFlangeVolume", 0, 0, 0);
  m_pMainVacuumChamberFlangeTopPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., GetGeometryParameter("MainVacuumChamberHalfZ")+GetGeometryParameter("MainVacuumChamberFlangeHalfZ")), m_pMainVacuumChamberFlangeLogicalVolume,
                                              "MainVacuumChamberFlangeTop", m_pVacuumLogicalVolume, false, 0);
  m_pMainVacuumChamberFlangeBottomPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., -GetGeometryParameter("MainVacuumChamberHalfZ")-GetGeometryParameter("MainVacuumChamberFlangeHalfZ")), m_pMainVacuumChamberFlangeLogicalVolume,
                                              "MainVacuumChamberFlangeBottom", m_pVacuumLogicalVolume, false, 0);
  
  XeSimLXeSensitiveDetector *pLXeSD = new XeSimLXeSensitiveDetector("RefSetup/LXeSD");
  pSDManager->AddNewDetector(pLXeSD);
  m_pMainVacuumChamberFlangeLogicalVolume->SetSensitiveDetector(pLXeSD);
  
  // PMT screen
  G4Tubs *pPMTScreenTubs = new G4Tubs("PMTScreenTubs", GetGeometryParameter("CenterToPMTCathode"), 
                                             GetGeometryParameter("CenterToPMTCathode")+2.00*mm, 
                                             0.5*GetGeometryParameter("PMTScreenHeight"), 
                                             0.*deg, 320.*deg);
  G4RotationMatrix *rm = new G4RotationMatrix;
  rm->rotateZ(-20*deg);
  
  m_pPMTScreenLogicalVolume = new G4LogicalVolume(pPMTScreenTubs, PhotoDetAluminium, "PMTScreenVolume", 0, 0, 0);
  m_pPMTScreenPhysicalVolume = new G4PVPlacement(rm, G4ThreeVector(0., 0., 0.), m_pPMTScreenLogicalVolume, 
                                            "PMTScreen", m_pVacuumLogicalVolume, false, 0);
  
  XeSimPhotoDetSensitiveDetector *pPmtSD = new XeSimPhotoDetSensitiveDetector("RefSetup/PhotoDetSD");
  pSDManager->AddNewDetector(pPmtSD);
  m_pPMTScreenLogicalVolume->SetSensitiveDetector(pPmtSD);  
  
  // Flange
  G4Tubs *pSampleHolderFlangeTubs = new G4Tubs("SampleHolderFlangeFlangeTubs", 0., GetGeometryParameter("HolderFlangeRadius"), 
                                               0.5*GetGeometryParameter("HolderFlangeHeight"), 0.*deg, 360.*deg);
  
  m_pSampleHolderFlangeLogicalVolume = new G4LogicalVolume(pSampleHolderFlangeTubs, SS316LSteel, "SampleHolderFlangeVolume", 0, 0, 0);
  m_pSampleHolderFlangePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., -0.5*GetGeometryParameter("HolderFlangeHeight")), m_pSampleHolderFlangeLogicalVolume,
                                              "SampleHolderFlange", m_pVacuumLogicalVolume, false, 0);

 
  // Sample(Holder)
  G4Box *pPTFEHolderOuterBox = new G4Box("PTFEHolderOuterBox", GetGeometryParameter("PTFEHolderHalfX"), 
                                                     GetGeometryParameter("PTFEHolderHalfY"), 
                                                     GetGeometryParameter("PTFEHolderHalfZ"));
  G4Box *pPTFEHolderInnerBox = new G4Box("PTFEHolderInnerBox", GetGeometryParameter("PTFEHolderHalfX")+0.01, 
                                                               GetGeometryParameter("PTFEHolderHalfY")-1., 
                                                               GetGeometryParameter("PTFEHolderHalfZ")-1.);                                           
  G4SubtractionSolid* pPTFEHolderBox = new G4SubtractionSolid("PTFEHolderBox", pPTFEHolderOuterBox, pPTFEHolderInnerBox);                                                   
                                                     
  m_pPTFEHolderLogicalVolume = new G4LogicalVolume(pPTFEHolderBox, SS316LSteel, "PTFEHolderFlangeVolume", 0, 0, 0);
  m_pPTFEHolderPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(GetGeometryParameter("CenterFlangeToPTFESamples"), 0., GetGeometryParameter("PTFEHolderHalfZ")), m_pPTFEHolderLogicalVolume,
                                              "PTFEHolderBox", m_pVacuumLogicalVolume, false, 0);
 
  G4Box *pPTFEBox = new G4Box("PTFEBox", GetGeometryParameter("PTFESampleHalfX"), 
                                         GetGeometryParameter("PTFESampleHalfY"), 
                                         GetGeometryParameter("PTFESampleHalfZ"));
                                         
  G4Box *pPTFECutBox = new G4Box("PTFECutBox", GetGeometryParameter("PTFESampleCutHalfX")+0.01, 
                                               GetGeometryParameter("PTFESampleCutHalfY"), 
                                               GetGeometryParameter("PTFESampleHalfZ")+0.01);
  G4SubtractionSolid* pPTFESample = new G4SubtractionSolid("_PTFESample", pPTFEBox, pPTFECutBox,
                                                           0, G4ThreeVector(-GetGeometryParameter("PTFESampleCutHalfX"), 0., 0.));
                                                           
  G4Tubs *pHoleTubs = new G4Tubs("HoleTubs", 0., 3., 
                                 GetGeometryParameter("PTFESampleHoleHalfX"), 0.*deg, 360.*deg);                                    
  G4RotationMatrix *rmHole = new G4RotationMatrix;
  rmHole->rotateY(90*deg);
  G4SubtractionSolid* pPTFESampleHole = new G4SubtractionSolid("PTFESample", pPTFESample, pHoleTubs,
                                                           rmHole, G4ThreeVector(GetGeometryParameter("PTFESampleHalfX")+0.01, 0., 0.));                                                        
  
  G4Tubs *pPhotonTubs = new G4Tubs("PhotonTubs", 0., 1., 
                                   0.1, 0.*deg, 360.*deg);                                    
  G4SubtractionSolid* pPTFESampleHoles = new G4SubtractionSolid("PTFESample with source", pPTFESampleHole, pPhotonTubs,
                                                           rmHole, G4ThreeVector(0.1-GetGeometryParameter("PTFESampleHalfX")-0.00001+2.*GetGeometryParameter("PTFESampleCutHalfX"), 0., 0.));                                                        
  
  m_pPTFELogicalVolume = new G4LogicalVolume(pPTFESampleHoles, GXePTFE, "PTFEVolume", 0, 0, 0);
  m_pPTFEPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(GetGeometryParameter("CenterFlangeToPTFESamples"), 0., GetGeometryParameter("PTFEHolderHalfZ")), m_pPTFELogicalVolume,
                                              "PTFEHolderBox", m_pVacuumLogicalVolume, false, 0);
 
  m_pPhotonsLogicalVolume = new G4LogicalVolume(pPhotonTubs, Vacuum, "PhotonVolume", 0, 0, 0);
  m_pPhotonsPhysicalVolume = new G4PVPlacement(rmHole, G4ThreeVector(0.1+GetGeometryParameter("CenterFlangeToPTFESamples"), 0., GetGeometryParameter("PTFEHolderHalfZ")), m_pPhotonsLogicalVolume,
                                              "PhotonSource", m_pVacuumLogicalVolume, false, 0);
 
  // optical border surface
  G4double dSigmaAlpha = 0.1;
  
  G4OpticalSurface *pSS316LSteelOpticalSurface = new G4OpticalSurface("SS316LSteelOpticalSurface",
		unified, polished, dielectric_metal, 0.);
		
  G4OpticalSurface *pPTFEOpticalSurface = new G4OpticalSurface("PTFEOpticalSurface", 
		unified, groundbackpainted, dielectric_dielectric, dSigmaAlpha);
		
  pPTFEOpticalSurface->SetMaterialPropertiesTable(GXePTFE->GetMaterialPropertiesTable());
  pSS316LSteelOpticalSurface->SetMaterialPropertiesTable(SS316LSteel->GetMaterialPropertiesTable());
  
  new G4LogicalBorderSurface("VacuumPTFELogicalBorderSurface",
		m_pVacuumPhysicalVolume, m_pPTFEPhysicalVolume, pPTFEOpticalSurface);
        
  new G4LogicalBorderSurface("PhotonsPTFELogicalBorderSurface",
		m_pPhotonsPhysicalVolume, m_pPTFEPhysicalVolume, pPTFEOpticalSurface);

  new G4LogicalBorderSurface("VacuumPTFELogicalBorderSurface",
		m_pVacuumPhysicalVolume, m_pSampleHolderFlangePhysicalVolume, pSS316LSteelOpticalSurface);
  
  new G4LogicalBorderSurface("VacuumPTFELogicalBorderSurface",
		m_pVacuumPhysicalVolume, m_pPTFEHolderPhysicalVolume, pSS316LSteelOpticalSurface);
  
  // VI attributes
  G4Colour hPTFEColor(1., 0., 1., 1.);
  G4Colour hSSColor(1., 1., 0., 1.);
  G4Colour hSSFlangeColor(1., 0.75, 0., 1.);
  G4Colour hPMTColor(0., 1., 0., 1.);
  G4Colour hMCColor(0.0,0.0,1.0,0.25);
  G4Colour hPhotonsColor(0.0,1.0,1.0,0.25);

  G4VisAttributes *pPhotonsVisAtt = new G4VisAttributes(hPhotonsColor);
  pPhotonsVisAtt->SetVisibility(true);
  m_pPhotonsLogicalVolume->SetVisAttributes(pPhotonsVisAtt);
  
  G4VisAttributes *pPTFEVisAtt = new G4VisAttributes(hPTFEColor);
  pPTFEVisAtt->SetVisibility(true);
  m_pPTFELogicalVolume->SetVisAttributes(pPTFEVisAtt);
  
  G4VisAttributes *pMCVisAtt = new G4VisAttributes(hMCColor);
  pMCVisAtt->SetVisibility(true);
  m_pMainVacuumChamberLogicalVolume->SetVisAttributes(pMCVisAtt);
  m_pMainVacuumChamberFlangeLogicalVolume->SetVisAttributes(pMCVisAtt);
  
  G4VisAttributes *pSSVisAtt = new G4VisAttributes(hSSColor);
  pSSVisAtt->SetVisibility(true);
  m_pPTFEHolderLogicalVolume->SetVisAttributes(pSSVisAtt);
  
  G4VisAttributes *pSSFlangeVisAtt = new G4VisAttributes(hSSFlangeColor);
  pSSFlangeVisAtt->SetVisibility(true);
  m_pSampleHolderFlangeLogicalVolume->SetVisAttributes(pSSFlangeVisAtt);
  
  G4VisAttributes *pPMTVisAtt = new G4VisAttributes(hPMTColor);
  pPMTVisAtt->SetVisibility(true);
  m_pPMTScreenLogicalVolume->SetVisAttributes(pPMTVisAtt);
}