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
#include "RnColumnDetectorConstruction.hh"
#include "RnColumnDetectorMessenger.hh"

map<G4String, G4double> RnColumnDetectorConstruction::m_hGeometryParameters;

RnColumnDetectorConstruction::RnColumnDetectorConstruction() {
    // needs to be set for the AnalysisManager
    m_hGeometryParameters["NbPhotoDets"] = 1;
    m_pDetectorMessenger = new RnColumnDetectorMessenger(this);

    pLXeLevel = 100.0;
    m_iNbTopPmts = 0;
    m_iNbBottomPmts = 0;
}

RnColumnDetectorConstruction::~RnColumnDetectorConstruction() {
  //delete m_pDetectorMessenger;
}

G4VPhysicalVolume* RnColumnDetectorConstruction::Construct() {
    DefineMaterials();
    
    DefineGeometryParameters();
    
    ConstructLaboratory();
    
    ConstructReboiler();
    ConstructPMTs();
    
    return m_pLabPhysicalVolume;
}

G4double RnColumnDetectorConstruction::GetGeometryParameter(const char *szParameter) {
  return m_hGeometryParameters[szParameter];
}

void RnColumnDetectorConstruction::DefineMaterials() {
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
	
  pLXePropertiesTable->AddProperty("SCINTILLATIONCOMPONENT1", pdLXePhotonMomentum, pdLXeScintillation, iNbEntries);
  pLXePropertiesTable->AddProperty("SCINTILLATIONCOMPONENT2", pdLXePhotonMomentum, pdLXeScintillation, iNbEntries);
  pLXePropertiesTable->AddProperty("RINDEX", pdLXePhotonMomentum, pdLXeRefractiveIndex, iNbEntries);
  pLXePropertiesTable->AddProperty("ABSLENGTH", pdLXePhotonMomentum, pdLXeAbsorbtionLength, iNbEntries);
  pLXePropertiesTable->AddProperty("RAYLEIGH", pdLXePhotonMomentum, pdLXeScatteringLength, iNbEntries);
	
  pLXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD", 0./(21.6*eV));
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

  pGXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD", 0./(21.6*eV));
  pGXePropertiesTable->AddConstProperty("RESOLUTIONSCALE", 0);
  pGXePropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 3.*ns);
  pGXePropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 27.*ns);
  pGXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD1", 1.0);

  GXe->SetMaterialPropertiesTable(pGXePropertiesTable);
  
  //------------------------------------- cirlex ---------------------------------
  G4Material *Cirlex = new G4Material("Cirlex", 1.43*g/cm3, 4, kStateSolid); //PMT base material
  Cirlex->AddElement(C, 22);
  Cirlex->AddElement(H, 10);
  Cirlex->AddElement(N, 2);
  Cirlex->AddElement(O, 5);

  G4double pdCirlexPhotonMomentum[]  = {6.91*eV, 6.98*eV, 7.05*eV};
  G4double pdCirlexReflectivity[]   = {0.5,    0.5,     0.5};

  G4MaterialPropertiesTable *pCirlexPropertiesTable = new G4MaterialPropertiesTable();

  pCirlexPropertiesTable->AddProperty("REFLECTIVITY", pdCirlexPhotonMomentum, pdCirlexReflectivity, iNbEntries);
  Cirlex->SetMaterialPropertiesTable(pCirlexPropertiesTable);

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

  //==== Torlon ====
  G4Material *Torlon = new G4Material("Torlon", 1.41 * g / cm3, 4, kStateSolid);
  Torlon->AddElement(N, 0.07862);
  Torlon->AddElement(C, 0.70784);
  Torlon->AddElement(O, 0.17960);
  Torlon->AddElement(H, 0.03394);

  //------------------------------------ PTFE -----------------------------------
  G4Material* PTFE = new G4Material("LXePTFE", 2.2*g/cm3, 2, kStateSolid);
  PTFE->AddElement(C, 0.240183);
  PTFE->AddElement(F, 0.759817);

  G4double pdPTFEPhotonMomentum[iNbEntries]  = {6.91*eV, 6.98*eV, 7.05*eV};
  G4double pdPTFERefractiveIndex[iNbEntries] = {1.63,    1.61,    1.58};
  G4double pdPTFEReflectivity[iNbEntries]    = {0.95,    0.95,    0.95};
  G4double pdPTFESpecularLobe[iNbEntries]    = {0.01,    0.01,    0.01};
  G4double pdPTFESpecularSpike[iNbEntries]   = {0.01,    0.01,    0.01};
  G4double pdPTFEBackscatter[iNbEntries]     = {0.01,    0.01,    0.01};
  G4double pdPTFEEfficiency[iNbEntries]      = {1.0,     1.0,     1.0};
  G4double pdPTFEAbsorbtionLength[iNbEntries] = {0.1 * cm, 0.1 * cm, 0.1 * cm};
  G4double pdPTFESufraceTransmittance[iNbEntries] = {1e-12, 1e-12, 1e-12};

  G4MaterialPropertiesTable *pPTFEPropertiesTable = new G4MaterialPropertiesTable();
  pPTFEPropertiesTable->AddProperty("RINDEX", pdPTFEPhotonMomentum, pdPTFERefractiveIndex, iNbEntries);
  pPTFEPropertiesTable->AddProperty("REFLECTIVITY", pdPTFEPhotonMomentum, pdPTFEReflectivity, iNbEntries);
  pPTFEPropertiesTable->AddProperty("SPECULARLOBECONSTANT", pdPTFEPhotonMomentum, pdPTFESpecularLobe, iNbEntries);
  pPTFEPropertiesTable->AddProperty("SPECULARSPIKECONSTANT", pdPTFEPhotonMomentum, pdPTFESpecularSpike, iNbEntries);
  pPTFEPropertiesTable->AddProperty("BACKSCATTERCONSTANT", pdPTFEPhotonMomentum, pdPTFEBackscatter, iNbEntries);
  pPTFEPropertiesTable->AddProperty("EFFICIENCY", pdPTFEPhotonMomentum, pdPTFEEfficiency, iNbEntries);
  pPTFEPropertiesTable->AddProperty("ABSLENGTH", pdPTFEPhotonMomentum, pdPTFEAbsorbtionLength, iNbEntries);
  pPTFEPropertiesTable->AddProperty("TRANSMITTANCE", pdPTFEPhotonMomentum, pdPTFESufraceTransmittance, iNbEntries);
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
  G4double pdGXePTFEAbsorbtionLength[iNbEntries] = {0.1 * cm, 0.1 * cm, 0.1 * cm};
  G4double pdGXePTFESufraceTransmittance[iNbEntries] = {1e-12, 1e-12, 1e-12};
  
  G4MaterialPropertiesTable *pGXePTFEPropertiesTable = new G4MaterialPropertiesTable();
  pGXePTFEPropertiesTable->AddProperty("RINDEX", pdGXePTFEPhotonMomentum, pdGXePTFERefractiveIndex, iNbEntries);
  pGXePTFEPropertiesTable->AddProperty("REFLECTIVITY", pdGXePTFEPhotonMomentum, pdGXePTFEReflectivity, iNbEntries);
  pGXePTFEPropertiesTable->AddProperty("SPECULARLOBECONSTANT", pdGXePTFEPhotonMomentum, pdGXePTFESpecularLobe, iNbEntries);
  pGXePTFEPropertiesTable->AddProperty("SPECULARSPIKECONSTANT", pdGXePTFEPhotonMomentum, pdGXePTFESpecularSpike, iNbEntries);
  pGXePTFEPropertiesTable->AddProperty("BACKSCATTERCONSTANT", pdGXePTFEPhotonMomentum, pdGXePTFEBackscatter, iNbEntries);
  pGXePTFEPropertiesTable->AddProperty("EFFICIENCY", pdGXePTFEPhotonMomentum, pdGXePTFEEfficiency, iNbEntries);
  pGXePTFEPropertiesTable->AddProperty("ABSLENGTH", pdGXePTFEPhotonMomentum, pdGXePTFEAbsorbtionLength, iNbEntries);
  pGXePTFEPropertiesTable->AddProperty("TRANSMITTANCE", pdGXePTFEPhotonMomentum, pdGXePTFESufraceTransmittance, iNbEntries);
  
  GXePTFE->SetMaterialPropertiesTable(pGXePTFEPropertiesTable);
}

void RnColumnDetectorConstruction::DefineGeometryParameters() {
  // Shrinkage coefficients (i.e., multiply warm dimensions by these factors to get cold dimensions)
  // For pillars and walls:
  m_hGeometryParameters["PTFE_ShrinkageZ"] = 0.014; //1,4 %
  // For PMT holders and reflector plates:
  m_hGeometryParameters["PTFE_ShrinkageR"] = 0.011; //1,1 %
  // For electrode frame holders:
  m_hGeometryParameters["Torlon_ShrinkageZ"] = 0.005; //0,5 %

  m_hGeometryParameters["MainVacuumChamberOuterRadius"] = 330.00*mm;
  m_hGeometryParameters["MainVacuumChamberInnerRadius"] = 325.00*mm;
  m_hGeometryParameters["MainVacuumChamberHalfZ"] = 400.00*mm;
  m_hGeometryParameters["MainVacuumChamberFlangeHalfZ"] = 2.50*mm;
  
  // Reboiler: CF400 vessel with flanges
  m_hGeometryParameters["ReboilerTubeOuterRadius"] = 0.5 * 400.00*mm;
  m_hGeometryParameters["ReboilerTubeInnerRadius"] = 0.5 * 395.00*mm;
  m_hGeometryParameters["ReboilerTubeHalfZ"] = 150.00*mm;
  m_hGeometryParameters["ReboilerTopFlangeOuterRadius"] = 0.5 * 470.00*mm;
  m_hGeometryParameters["ReboilerTopFlangeHalfZ"] = 0.5 * 28.00*mm;
  m_hGeometryParameters["ReboilerBottomFlangeOuterRadius"] = 0.5 * 470.00*mm;
  m_hGeometryParameters["ReboilerBottomFlangeHalfZ"] = 0.5 * 28.00*mm;

  // Xenon volumes
  m_hGeometryParameters["LXeHalfZ"] = GetGeometryParameter("ReboilerTubeHalfZ");
  m_hGeometryParameters["LXeOuterRadius"] = GetGeometryParameter("ReboilerTubeInnerRadius");
  m_hGeometryParameters["GXeHalfZ"] = 0.5*(2*GetGeometryParameter("ReboilerTubeHalfZ")-pLXeLevel);
  m_hGeometryParameters["GXeOuterRadius"] = GetGeometryParameter("LXeOuterRadius");

  m_hGeometryParameters["PmtWidth"]                 = 25.4*mm;
  m_hGeometryParameters["PmtSpacing"]               = 2.0*mm;
  m_hGeometryParameters["PmtWindowWidth"]           = 25.00*mm;
  m_hGeometryParameters["PmtWindowThickness"]       = 1.50*mm;
  m_hGeometryParameters["PmtCasingWidth"]           = 25.40*mm;
  m_hGeometryParameters["PmtCasingHeight"]          = 27.00*mm;
  m_hGeometryParameters["PmtCasingThickness"]       = 0.50*mm;
  m_hGeometryParameters["PmtPhotoCathodeWidth"]     = 22.00*mm;
  m_hGeometryParameters["PmtPhotoCathodeThickness"] = 0.50*mm;
  m_hGeometryParameters["PmtBaseThickness"]         = 1.50*mm;
  m_hGeometryParameters["PmtToPmtBase"]             = 3.00*mm;

}

void RnColumnDetectorConstruction::ConstructPMTs() {
  const G4double dPmtWindowWidth = GetGeometryParameter("PmtWindowWidth");
  const G4double dPmtWindowThickness = GetGeometryParameter("PmtWindowThickness");

  const G4double dPmtCasingWidth = GetGeometryParameter("PmtCasingWidth");
  const G4double dPmtCasingHeight = GetGeometryParameter("PmtCasingHeight");
  const G4double dPmtCasingThickness = GetGeometryParameter("PmtCasingThickness");

  const G4double dPmtPhotoCathodeWidth = GetGeometryParameter("PmtPhotoCathodeWidth");
  const G4double dPmtPhotoCathodeThickness = GetGeometryParameter("PmtPhotoCathodeThickness");

  const G4double dPmtToPmtBase= GetGeometryParameter("PmtToPmtBase");
  const G4double dPmtBaseThickness = GetGeometryParameter("PmtBaseThickness");

  const G4double dSpaceBelowTopPMTHeight= GetGeometryParameter("SpaceBelowTopPMTHeight");

  G4Material *Quartz = G4Material::GetMaterial("Quartz"); //for the window
  G4Material *SS304LSteel = G4Material::GetMaterial("SS304LSteel"); //for the frames
  G4Material *Vacuum = G4Material::GetMaterial("Vacuum");
  G4Material *Aluminium = G4Material::GetMaterial("PhotoCathodeAluminium"); //for the photocathode
  G4Material *Cirlex = G4Material::GetMaterial("Cirlex"); //for the base
  G4Material *GXe = G4Material::GetMaterial("GXe");
  G4Material *PTFE = G4Material::GetMaterial("LXePTFE");

  // PMT window made of quartz
  // Sits on front face of PMT casing (not inside)
  const G4double dPmtWindowHalfX = 0.5*dPmtWindowWidth;
  const G4double dPmtWindowHalfZ = 0.5*dPmtWindowThickness;

  G4Box *pPmtWindowBox = new G4Box("PmtWindowBox", dPmtWindowHalfX, dPmtWindowHalfX, dPmtWindowHalfZ);
  m_pPmtWindowLogicalVolume = new G4LogicalVolume(pPmtWindowBox, Quartz, "PmtWindowLogicalVolume", 0, 0, 0);

  // Main PMT body, called casing
  // Filled with vacuum and the photocathode is on the front inside this casing
  const G4double dPmtCasingHalfX = 0.5*dPmtCasingWidth;
  const G4double dPmtCasingHalfZ = 0.5*dPmtCasingHeight;

  G4Box *pPmtCasingBox = new G4Box("PmtCasingBox", dPmtCasingHalfZ, dPmtCasingHalfZ, dPmtCasingHalfX);
  m_pPmtCasingLogicalVolume = new G4LogicalVolume(pPmtCasingBox, SS304LSteel, "PmtCasingLogicalVolume", 0, 0, 0);

  // PMT interior -- vacuum filled space inside the casing
  const G4double dPmtInteriorHalfX = dPmtCasingHalfX-dPmtCasingThickness;
  const G4double dPmtInteriorHalfZ = dPmtCasingHalfZ-dPmtCasingThickness;

  G4Box *pPmtInteriorBox = new G4Box("PmtInteriorBox", dPmtInteriorHalfZ, dPmtInteriorHalfZ, dPmtInteriorHalfX);
  m_pPmtInteriorLogicalVolume = new G4LogicalVolume(pPmtInteriorBox, Vacuum, "PmtInteriorLogicalVolume", 0, 0, 0);

  // PMT photocathode sits on the front face of the interior/casing
  const G4double dPmtPhotoCathodeHalfX = 0.5*dPmtPhotoCathodeWidth;
  const G4double dPmtPhotoCathodeHalfZ = 0.5*dPmtPhotoCathodeThickness;
  const G4double dPmtPhotoCathodeOffsetZ = dPmtCasingHalfZ-dPmtPhotoCathodeHalfZ;

  G4Box *pPmtPhotoCathodeBox = new G4Box("PmtPhotoCathodeBox", dPmtPhotoCathodeHalfX, dPmtPhotoCathodeHalfX, dPmtPhotoCathodeHalfZ);
  m_pPmtPhotoCathodeLogicalVolume = new G4LogicalVolume(pPmtPhotoCathodeBox, Aluminium, "PmtPhotoCathodeLogicalVolume", 0, 0, 0);

  // PMT base with a small offset to the bottom of the casing
  // Electronics are not simulated which make up the offset
  const G4double dPmtBaseHalfX = dPmtCasingHalfX;
  const G4double dPmtBaseHalfZ = 0.5*dPmtBaseThickness;

  G4Box *pPmtBaseBox = new G4Box("PmtBaseBox", dPmtBaseHalfX, dPmtBaseHalfX, dPmtBaseHalfZ);
  m_pPmtBaseLogicalVolume = new G4LogicalVolume(pPmtBaseBox, Cirlex, "PmtBaseLogicalVolume", 0, 0, 0);

  // Rotate along Z to along X
  G4RotationMatrix *pPmtZYRotation = new G4RotationMatrix();
  pPmtZYRotation->rotateY(90.*deg);

  // Place the vacuum volume inside the casing
  // Placement relative to the casing
  m_pPmtInteriorPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.),
						   m_pPmtInteriorLogicalVolume, "PmtInterior", m_pPmtCasingLogicalVolume, false, 0);

  // Place the photocathode inside the casing
  // Placement relative to the casing
  m_pPmtPhotoCathodePhysicalVolume = new G4PVPlacement(pPmtZYRotation, G4ThreeVector(-dPmtPhotoCathodeOffsetZ, 0., 0),
						       m_pPmtPhotoCathodeLogicalVolume, "PmtPhotoCathode", m_pPmtCasingLogicalVolume, false, 0);

  // Make the absolute placements of the PMT parts in the detector volume
  // Place PMTs in a circular pattern facing the center of the LXe volume
  G4int iNbTopPmts = m_iNbTopPmts;
  G4int iNbBottomPmts = m_iNbBottomPmts;
  
  // Radius of the central reboiler to the PMT window face
  G4double dPmtRadius = 100.00*mm;

  const G4double dPmtBaseOffsetZ =(dPmtCasingHeight-dPmtWindowThickness-dPmtToPmtBase-dPmtBaseHalfZ);
  const G4double dPmtCasingOffsetZ = (dPmtBaseOffsetZ+dPmtToPmtBase+dPmtBaseHalfZ+dPmtCasingHalfZ);
  const G4double dPmtWindowOffsetZ = dPmtCasingOffsetZ+dPmtCasingHalfZ+dPmtWindowHalfZ;

  G4int PmtNb = 0;
  G4double dPmtAngle = 0;
  G4double dPmtOffsetZ = 0;
  G4double dDistanceTopBottom = 40.0*mm;

  G4double PTFESeparatorToBottomFlange = 20.00*mm+2*dPmtCasingHalfX-GetGeometryParameter("ReboilerTubeHalfZ");

  for (G4int i = 0; i < iNbTopPmts+iNbBottomPmts; i++) {
    if (i < iNbTopPmts) {
      PmtNb = i;
      dPmtOffsetZ = PTFESeparatorToBottomFlange+0.5*dDistanceTopBottom;
      dPmtAngle = 2.0*CLHEP::pi/iNbTopPmts;
    } else {
      PmtNb = i - iNbTopPmts;
      dPmtOffsetZ = PTFESeparatorToBottomFlange-0.5*dDistanceTopBottom;
      dPmtAngle = 2.0*CLHEP::pi/iNbBottomPmts;
    }

    G4double dPmtX = dPmtRadius*cos(PmtNb*dPmtAngle);
    G4double dPmtY = dPmtRadius*sin(PmtNb*dPmtAngle);
    G4ThreeVector hPmtPosition = G4ThreeVector(dPmtX, dPmtY, dPmtOffsetZ);

    // First rotate the PMTs to face the center of the LXe volume
    G4RotationMatrix *pPmtCasingRotation = new G4RotationMatrix();
    pPmtCasingRotation->rotateZ(-PmtNb*dPmtAngle);
    G4RotationMatrix *pPmtBaseRotation = new G4RotationMatrix();
    pPmtBaseRotation->rotateZ(-PmtNb*dPmtAngle);
    pPmtBaseRotation->rotateY(90.*deg);

    G4double dPmtBaseX = (dPmtRadius+dPmtBaseOffsetZ)*cos(PmtNb*dPmtAngle);
    G4double dPmtBaseY = (dPmtRadius+dPmtBaseOffsetZ)*sin(PmtNb*dPmtAngle);
    G4ThreeVector hPmtBasePosition = G4ThreeVector(dPmtBaseX, dPmtBaseY, dPmtOffsetZ);

    G4double dPmtWindowX = (dPmtRadius-dPmtCasingHalfX-dPmtWindowThickness)*cos(PmtNb*dPmtAngle);
    G4double dPmtWindowY = (dPmtRadius-dPmtCasingHalfX-dPmtWindowThickness)*sin(PmtNb*dPmtAngle);
    G4ThreeVector hPmtWindowPosition = G4ThreeVector(dPmtWindowX, dPmtWindowY, dPmtOffsetZ);

    m_hPmtWindowPhysicalVolumes.push_back(new G4PVPlacement(pPmtBaseRotation, hPmtWindowPosition, m_pPmtWindowLogicalVolume, "PmtWindow", m_pLXeLogicalVolume, false, i));
    m_hPmtCasingPhysicalVolumes.push_back(new G4PVPlacement(pPmtCasingRotation, hPmtPosition, m_pPmtCasingLogicalVolume, "PmtCasing", m_pLXeLogicalVolume, false, i));
    m_hPmtBasePhysicalVolumes.push_back(new G4PVPlacement(pPmtBaseRotation, hPmtBasePosition, m_pPmtBaseLogicalVolume, "PmtBase", m_pLXeLogicalVolume, false, i));
  }

  // Add a PTFE plate between the top and bottom PMTs
  const G4double dPTFEPMTSeparationTubsRadius = dPmtRadius+dPmtBaseOffsetZ+15*mm;
  const G4double dPTFETopPMTHolderHalfZ = 0.5*3*mm;
  G4Tubs *pPTFEPMTSeparationTubs = new G4Tubs("PTFEPmtSeparationTubs", 0, dPTFEPMTSeparationTubsRadius, dPTFETopPMTHolderHalfZ, 0.*deg, 360.*deg);
  m_pPTFEPMTSeparationLogicalVolume = new G4LogicalVolume(pPTFEPMTSeparationTubs, PTFE, "PTFEPmtSeparationLogicalVolume", 0, 0, 0);
  m_pPTFEPMTSeparationPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0, 0, PTFESeparatorToBottomFlange), m_pPTFEPMTSeparationLogicalVolume, "PTFEPmtSeparation", m_pLXeLogicalVolume, false, 0);

  //------------------------------- pmt sensitivity -------------------------------
  G4SDManager *pSDManager = G4SDManager::GetSDMpointer();

  XeSimPhotoDetSensitiveDetector *pPmtSD = new XeSimPhotoDetSensitiveDetector("RnColumn/PmtSD");
  pSDManager->AddNewDetector(pPmtSD);
  m_pPmtPhotoCathodeLogicalVolume->SetSensitiveDetector(pPmtSD);

  //================================== optical surface =================================	
	G4OpticalSurface *pSS304LSteelOpticalSurface = new G4OpticalSurface("SS304LSteelOpticalSurface",
		unified, polished, dielectric_metal);
        pSS304LSteelOpticalSurface->SetMaterialPropertiesTable(SS304LSteel->GetMaterialPropertiesTable());

	stringstream hStream;
	for(G4int iPmtNb=0; iPmtNb<iNbTopPmts+iNbBottomPmts; iPmtNb++)
	{
		hStream.str("PmtCasingLogicalBorderSurfacePmt");
		hStream << iPmtNb;
		new G4LogicalBorderSurface(hStream.str(),
 		   m_pLXePhysicalVolume, m_hPmtCasingPhysicalVolumes[iPmtNb], pSS304LSteelOpticalSurface);
		   //m_pGXePhysicalVolume, m_hPmtCasingPhysicalVolumes[iPmtNb], pSS304LSteelOpticalSurface);
	}

  //---------------------------------- attributes ---------------------------------
  G4Colour hPmtWindowColor(0.4,0.804, 0.666,0.75);  //102 205 170 aquamarine
  G4VisAttributes *pPmtWindowVisAtt = new G4VisAttributes(hPmtWindowColor);
  pPmtWindowVisAtt->SetVisibility(true);
  m_pPmtWindowLogicalVolume->SetVisAttributes(pPmtWindowVisAtt);

  G4Colour hPmtCasingColor(0.623, 0.713, 0.804,0.75); //159 182 205 slate gray
  G4VisAttributes *pPmtCasingVisAtt = new G4VisAttributes(hPmtCasingColor);
  pPmtCasingVisAtt->SetVisibility(true);
  m_pPmtCasingLogicalVolume->SetVisAttributes(pPmtCasingVisAtt);

  G4VisAttributes *pPmtInteriorVisAtt = new G4VisAttributes();
  pPmtInteriorVisAtt->SetVisibility(true);
  m_pPmtInteriorLogicalVolume->SetVisAttributes(pPmtInteriorVisAtt);	

  G4Colour hPmtPhotoCathodeColor(1., 0.725, 0.059,1.0); //255 185 15 gold
  G4VisAttributes *pPmtPhotoCathodeVisAtt = new G4VisAttributes(hPmtPhotoCathodeColor);
  pPmtPhotoCathodeVisAtt->SetVisibility(true);
  m_pPmtPhotoCathodeLogicalVolume->SetVisAttributes(pPmtPhotoCathodeVisAtt);

  G4Colour hPmtBaseColor(1., 0.204, 0.702,0.75); //255;52;179 pink
  G4VisAttributes *pPmtBaseVisAtt = new G4VisAttributes(hPmtBaseColor);
  pPmtBaseVisAtt->SetVisibility(true);
  m_pPmtBaseLogicalVolume->SetVisAttributes(pPmtBaseVisAtt);

}

void RnColumnDetectorConstruction::ConstructLaboratory() {
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

void RnColumnDetectorConstruction::ConstructReboiler() {
  G4SDManager *pSDManager = G4SDManager::GetSDMpointer();
  
  G4Material *Vacuum = G4Material::GetMaterial("Vacuum");
  G4Material *SS316LSteel = G4Material::GetMaterial("SS316LSteel");
  G4Material *Quartz = G4Material::GetMaterial("Quartz");
  G4Material *LXe = G4Material::GetMaterial("LXe");
  G4Material *GXe = G4Material::GetMaterial("GXe");
  G4Material *GXePTFE = G4Material::GetMaterial("GXePTFE");
  G4Material *PhotoDetAluminium = G4Material::GetMaterial("PhotoCathodeAluminium");
  
  // At this point, the lab volume is the mother volume for the detector components
  // Since the lab volume is filled with air, the first thing is to build a vacuum chamber

  // Main vacuum chamber
  // We first fill the air volume with a vacuum volume and then place the vacuum chamber inside the vacuum volume
  // The vacuum volume is 0.01mm larger than the vacuum chamber to avoid overlaps
  G4Tubs *pVacuumTubs = new G4Tubs("VacuumTubs", 0., GetGeometryParameter("MainVacuumChamberOuterRadius")-0.01, 
                                                     GetGeometryParameter("MainVacuumChamberHalfZ")+0.01, 0.*deg, 360.*deg);
  m_pVacuumLogicalVolume = new G4LogicalVolume(pVacuumTubs, Vacuum, "VacuumVolume", 0, 0, 0);
  m_pVacuumPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), m_pVacuumLogicalVolume,
                                              "Vacuum", m_pLabLogicalVolume, false, 0);
  
  // Make the steel tube for the vacuum chamber
  G4Tubs *pMainVacuumChamberTubs = new G4Tubs("MainVacuumChamberTubs", GetGeometryParameter("MainVacuumChamberInnerRadius"), 
                                             GetGeometryParameter("MainVacuumChamberOuterRadius"), 
                                             GetGeometryParameter("MainVacuumChamberHalfZ"), 
                                             0.*deg, 360.*deg);
  m_pMainVacuumChamberLogicalVolume = new G4LogicalVolume(pMainVacuumChamberTubs, SS316LSteel, "MainVacuumChamberVolume", 0, 0, 0);
  m_pMainVacuumChamberPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., 0.), m_pMainVacuumChamberLogicalVolume, 
                                            "MainVacuumChamberCylinder", m_pVacuumLogicalVolume, false, 0);
  
  // Define the flanges
  G4Tubs *pMainVacuumChamberFlangeTubs = new G4Tubs("MainVacuumChamberFlangeTubs", 0., GetGeometryParameter("MainVacuumChamberOuterRadius"), 
                                              GetGeometryParameter("MainVacuumChamberFlangeHalfZ"), 0.*deg, 360.*deg);
  
  m_pMainVacuumChamberFlangeLogicalVolume = new G4LogicalVolume(pMainVacuumChamberFlangeTubs, SS316LSteel, "MainVacuumChamberFlangeVolume", 0, 0, 0);
  
  // Use two physicak volumes for the top and bottom flanges
  m_pMainVacuumChamberFlangeTopPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., GetGeometryParameter("MainVacuumChamberHalfZ")+GetGeometryParameter("MainVacuumChamberFlangeHalfZ")), m_pMainVacuumChamberFlangeLogicalVolume,
                                              "MainVacuumChamberFlangeTop", m_pVacuumLogicalVolume, false, 0);
  m_pMainVacuumChamberFlangeBottomPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., -GetGeometryParameter("MainVacuumChamberHalfZ")-GetGeometryParameter("MainVacuumChamberFlangeHalfZ")), m_pMainVacuumChamberFlangeLogicalVolume,
                                              "MainVacuumChamberFlangeBottom", m_pVacuumLogicalVolume, false, 0);

  // Make the reboiler vessel
  G4Tubs *pReboilerTubeTubs = new G4Tubs("ReboilerTubeTubs", GetGeometryParameter("ReboilerTubeInnerRadius"), 
                                             GetGeometryParameter("ReboilerTubeOuterRadius"), 
                                             GetGeometryParameter("ReboilerTubeHalfZ"), 0.*deg, 360.*deg);

  m_pReboilerTubeLogicalVolume = new G4LogicalVolume(pReboilerTubeTubs, SS316LSteel, "ReboilerTubeVolume", 0, 0, 0);
  m_pReboilerTubePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., -GetGeometryParameter("ReboilerTubeHalfZ")), m_pReboilerTubeLogicalVolume, 
                                            "ReboilerTube", m_pVacuumLogicalVolume, false, 0);

  // Top and bottom flanges of the reboiler
  G4Tubs *pReboilerTopFlangeTubs = new G4Tubs("ReboilerTopFlangeTubs", 0., GetGeometryParameter("ReboilerTopFlangeOuterRadius"), 
                                               GetGeometryParameter("ReboilerTopFlangeHalfZ"), 0.*deg, 360.*deg);
  
  m_pReboilerTopFlangeLogicalVolume = new G4LogicalVolume(pReboilerTopFlangeTubs, SS316LSteel, "ReboilerTopFlangeVolume", 0, 0, 0);
  m_pReboilerTopFlangePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., GetGeometryParameter("ReboilerTopFlangeHalfZ")), m_pReboilerTopFlangeLogicalVolume,
                                              "ReboilerTopFlange", m_pVacuumLogicalVolume, false, 0);

  m_pReboilerBottomFlangeLogicalVolume = new G4LogicalVolume(pReboilerTopFlangeTubs, SS316LSteel, "ReboilerBottomFlangeVolume", 0, 0, 0);
  m_pReboilerBottomFlangePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., -2*GetGeometryParameter("ReboilerTubeHalfZ")-GetGeometryParameter("ReboilerTopFlangeHalfZ")), m_pReboilerBottomFlangeLogicalVolume,
                                              "ReboilerBottomFlange", m_pVacuumLogicalVolume, false, 0);

  // Generate a liquid xenon volume for the LXe vessel
  // Make the volume a bit smaller than the Reboiler vessel to avoid overlaps
  G4Tubs *pLXeTubs = new G4Tubs("LXeTubs", 0., GetGeometryParameter("LXeOuterRadius")+0.01, 
                                               GetGeometryParameter("LXeHalfZ")-0.01, 0.*deg, 360.*deg);

  m_pLXeLogicalVolume = new G4LogicalVolume(pLXeTubs, LXe, "LXeVolume", 0, 0, 0);
  m_pLXePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., -GetGeometryParameter("ReboilerTubeHalfZ")), m_pLXeLogicalVolume, 
                                           "LXe", m_pVacuumLogicalVolume, false, 0);

  XeSimLXeSensitiveDetector *pLXeSD = new XeSimLXeSensitiveDetector("RefSetup/LXeSD");
  pSDManager->AddNewDetector(pLXeSD);
  m_pLXeLogicalVolume->SetSensitiveDetector(pLXeSD);
  
  // Generate a gaseous xenon volume in the top of the liquid xenon
  G4Tubs *pGXeTubs = new G4Tubs("GXeTubs", 0., GetGeometryParameter("GXeOuterRadius")+0.01, 
                                               GetGeometryParameter("GXeHalfZ")-0.01, 0.*deg, 360.*deg);

  m_pGXeLogicalVolume = new G4LogicalVolume(pGXeTubs, GXe, "GXeVolume", 0, 0, 0);
  m_pGXePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., GetGeometryParameter("ReboilerTubeHalfZ")-GetGeometryParameter("GXeHalfZ")), m_pGXeLogicalVolume, 
                                           "GXe", m_pLXeLogicalVolume, false, 0);

  //XeSimLXeSensitiveDetector *pGXeSD = new XeSimLXeSensitiveDetector("RefSetup/GXeSD");
  //pSDManager->AddNewDetector(pGXeSD);
  //m_pGXeLogicalVolume->SetSensitiveDetector(pGXeSD);

  // optical border surface
  G4double dSigmaAlpha = 0.1;
  
  G4OpticalSurface *pSS316LSteelOpticalSurface = new G4OpticalSurface("SS316LSteelOpticalSurface",
		unified, polished, dielectric_metal, 0.);
		
  G4OpticalSurface *pPTFEOpticalSurface = new G4OpticalSurface("PTFEOpticalSurface", 
		unified, ground, dielectric_dielectric, dSigmaAlpha);
		
  pPTFEOpticalSurface->SetMaterialPropertiesTable(GXePTFE->GetMaterialPropertiesTable());
  pSS316LSteelOpticalSurface->SetMaterialPropertiesTable(SS316LSteel->GetMaterialPropertiesTable());
  
  new G4LogicalBorderSurface("LXeReboilerTubeSurface",
    m_pLXePhysicalVolume, m_pReboilerTubePhysicalVolume, pSS316LSteelOpticalSurface);
  
  new G4LogicalBorderSurface("LXeReboilerTopFlangeSurface",
    m_pLXePhysicalVolume, m_pReboilerTopFlangePhysicalVolume, pSS316LSteelOpticalSurface);

  new G4LogicalBorderSurface("LXeReboilerBottomFlangeSurface",
    m_pLXePhysicalVolume, m_pReboilerBottomFlangePhysicalVolume, pSS316LSteelOpticalSurface);

    new G4LogicalBorderSurface("LXeReboilerTubeSurface",
    m_pGXePhysicalVolume, m_pReboilerTubePhysicalVolume, pSS316LSteelOpticalSurface);
  
  new G4LogicalBorderSurface("LXeReboilerTopFlangeSurface",
    m_pGXePhysicalVolume, m_pReboilerTopFlangePhysicalVolume, pSS316LSteelOpticalSurface);

  new G4LogicalBorderSurface("LXeReboilerBottomFlangeSurface",
    m_pGXePhysicalVolume, m_pReboilerBottomFlangePhysicalVolume, pSS316LSteelOpticalSurface);
  
  // VI attributes
  G4Colour hPTFEColor(1., 0., 1., 1.);
  G4Colour hSSColor(1., 1., 0., 1.);
  G4Colour hQuartzColor(1., .5, 0., 1.);
  G4Colour hSSFlangeColor(1., 0.75, 0., 1.);
  G4Colour hPMTColor(0., 1., 0., 1.);
  G4Colour hMCColor(0.0,0.0,1.0,0.25);
  G4Colour hPhotonsColor(0.0,1.0,1.0,0.25);

  G4Colour hLXeColor(0.0,0.0,1.0,0.25); //blue
  G4Colour hGXeColor(0.0,1.0,1.0,0.25); //cyan

  G4VisAttributes *pLXeVisAtt = new G4VisAttributes(hLXeColor);
  pLXeVisAtt->SetVisibility(true);
  m_pLXeLogicalVolume->SetVisAttributes(pLXeVisAtt);
	
  G4VisAttributes *pGXeVisAtt = new G4VisAttributes(hGXeColor);
  pGXeVisAtt->SetVisibility(true);
  m_pGXeLogicalVolume->SetVisAttributes(pGXeVisAtt);
  
  G4VisAttributes *pVacuumVisAtt = new G4VisAttributes(hMCColor);
  pVacuumVisAtt->SetVisibility(false);
  m_pVacuumLogicalVolume->SetVisAttributes(pVacuumVisAtt);

  G4VisAttributes *pMCVisAtt = new G4VisAttributes(hMCColor);
  pMCVisAtt->SetVisibility(true);
  m_pMainVacuumChamberLogicalVolume->SetVisAttributes(pMCVisAtt);
  m_pMainVacuumChamberFlangeLogicalVolume->SetVisAttributes(pMCVisAtt);
  
  G4VisAttributes *pSSFlangeVisAtt = new G4VisAttributes(hSSFlangeColor);
  pSSFlangeVisAtt->SetVisibility(true);
  m_pReboilerTubeLogicalVolume->SetVisAttributes(pSSFlangeVisAtt);
  m_pReboilerTopFlangeLogicalVolume->SetVisAttributes(pSSFlangeVisAtt);
  m_pReboilerBottomFlangeLogicalVolume->SetVisAttributes(pSSFlangeVisAtt);
  
  //G4VisAttributes *pPMTVisAtt = new G4VisAttributes(hPMTColor);
  //pPMTVisAtt->SetVisibility(true);
  //m_pPMTScreenLogicalVolume->SetVisAttributes(pPMTVisAtt);
}

void RnColumnDetectorConstruction::SetNbTopPmts(G4int iNbTopPmts) {
  m_iNbTopPmts = iNbTopPmts;
}

void RnColumnDetectorConstruction::SetNbBottomPmts(G4int iNbBottomPmts) {
  m_iNbBottomPmts = iNbBottomPmts;
}

void RnColumnDetectorConstruction::SetSeparationPlateMaterial(const G4String& name) {
  	// search the material by its name
  G4Material* mat = G4Material::GetMaterial(name, false);
  G4Material *pLXeMaterial = G4Material::GetMaterial(G4String("LXe"));

  // create the material by its name
  if(!mat) { mat = G4NistManager::Instance()->FindOrBuildMaterial(name); }

  if(mat && mat != pLXeMaterial) {
		m_pPTFEPMTSeparationLogicalVolume->SetMaterial(mat);
    UpdateGeometry();
		G4cout << "----> New SeparationPlate material " << mat->GetName() << G4endl;
  }

  if(!mat) {
    G4cout << "----> WARNING from DetectorConstruction::SetSeparationPlateMaterial : "
           << name << " not found" << G4endl;  
  } 
}

//******************************************************************/
// SetLXeLevel
//******************************************************************/
void RnColumnDetectorConstruction::SetLXeLevel(G4double dlevel) {
	G4cout << "----> Setting LXe level to " << dlevel/cm << " cm" << G4endl;
	G4cout << "----> default: 0 mm | no LXe: X mm | no GXe: -Y mm" << G4endl;

  pLXeLevel = dlevel;
}

//******************************************************************/
// SetLXeAbsorbtionLength
//******************************************************************/
void RnColumnDetectorConstruction::SetLXeAbsorbtionLength(G4double dAbsorbtionLength) {
  G4Material *pLXeMaterial = G4Material::GetMaterial(G4String("LXe"));

  if(pLXeMaterial)
    {
      G4cout << "----> Setting LXe absorbtion length to " << dAbsorbtionLength/cm << " cm" << G4endl;

      G4MaterialPropertiesTable *pLXePropertiesTable = pLXeMaterial->GetMaterialPropertiesTable();
			
      G4double LXe_PP[] = {6.91*eV, 6.98*eV, 7.05*eV};
      G4double LXe_ABSL[] = {dAbsorbtionLength, dAbsorbtionLength, dAbsorbtionLength};
      pLXePropertiesTable->RemoveProperty("ABSLENGTH");
      pLXePropertiesTable->AddProperty("ABSLENGTH", LXe_PP, LXe_ABSL, 3);
    }
  else
    {
      G4cout << "ls!> LXe materials not found!" << G4endl;
      exit(-1);
    }
}

void RnColumnDetectorConstruction::SetGXeAbsorbtionLength(G4double dAbsorbtionLength)
{
  G4Material *pGXeMaterial = G4Material::GetMaterial(G4String("GXe"));
  
  if(pGXeMaterial)
    {
      G4cout << "----> Setting GXe absorbtion length to " << dAbsorbtionLength/m << " m" << G4endl;
    
      G4MaterialPropertiesTable *pGXePropertiesTable = pGXeMaterial->GetMaterialPropertiesTable();
    
      const G4int iNbEntries = 3;
    
      G4double GXe_PP[iNbEntries] = {6.91*eV, 6.98*eV, 7.05*eV};
      G4double GXe_ABSL[iNbEntries] = {dAbsorbtionLength, dAbsorbtionLength, dAbsorbtionLength};
      pGXePropertiesTable->RemoveProperty("ABSLENGTH");
      pGXePropertiesTable->AddProperty("ABSLENGTH", GXe_PP, GXe_ABSL, iNbEntries);
    }
  else
    {
      G4cout << "ls!> GXe materials not found!" << G4endl;
      exit(-1);
    }
}

//******************************************************************/
// SetLXeRayScatterLength
//******************************************************************/
void RnColumnDetectorConstruction::SetLXeRayScatterLength(G4double dRayScatterLength) {
  G4Material *pLXeMaterial = G4Material::GetMaterial(G4String("LXe"));
  
  if(pLXeMaterial)
    {
      
      G4cout << "----> Setting LXe scattering length to " << dRayScatterLength/cm << " cm" << G4endl;
      
      G4MaterialPropertiesTable *pLXePropertiesTable = pLXeMaterial->GetMaterialPropertiesTable();
              
      G4double LXe_PP[] = {6.91*eV, 6.98*eV, 7.05*eV};
      G4double LXe_SCAT[] = {dRayScatterLength, dRayScatterLength, dRayScatterLength};
      pLXePropertiesTable->RemoveProperty("RAYLEIGH");
      pLXePropertiesTable->AddProperty("RAYLEIGH", LXe_PP, LXe_SCAT, 3);
 
    }
  else
    {
      G4cout << "ls!> LXe materials not found!" << G4endl;
      exit(-1);
    }

}

void RnColumnDetectorConstruction::SetLXeRefractionIndex(G4double dRefractionIndex)
{
  G4Material *pLXeMaterial = G4Material::GetMaterial(G4String("LXe"));
  
  if(pLXeMaterial)
    {
      G4cout << "----> Setting LXe refraction index to " << dRefractionIndex << G4endl;
    
      G4MaterialPropertiesTable *pLXePropertiesTable = pLXeMaterial->GetMaterialPropertiesTable();
	
      const G4int iNbEntries = 3;
    
      G4double LXe_PP[iNbEntries] = {6.91*eV, 6.98*eV, 7.05*eV};
      G4double LXe_RI[iNbEntries] = {dRefractionIndex, dRefractionIndex, dRefractionIndex};
      pLXePropertiesTable->RemoveProperty("RINDEX");
	  pLXePropertiesTable->AddProperty("RINDEX", LXe_PP, LXe_RI, iNbEntries);
    }
  else
    {
      G4cout << "ls!> LXe materials not found!" << G4endl;
      exit(-1);
    }
}

//******************************************************************/
// SetTeflonReflectivity
//******************************************************************/
void RnColumnDetectorConstruction::SetTeflonReflectivity(G4double dReflectivity) {
  G4Material *pTeflonMaterial = G4Material::GetMaterial(G4String("LXePTFE"));

  if(pTeflonMaterial)
    {
      G4cout << "----> Setting PTFE reflectivity to " << dReflectivity << G4endl;

      G4MaterialPropertiesTable *pTeflonPropertiesTable = pTeflonMaterial->GetMaterialPropertiesTable();
		
      G4double teflon_PP[] = { 6.91 * eV, 6.98 * eV, 7.05 * eV };
      G4double teflon_REFL[] = {dReflectivity, dReflectivity, dReflectivity};
      pTeflonPropertiesTable->RemoveProperty("REFLECTIVITY");
      pTeflonPropertiesTable->AddProperty("REFLECTIVITY", teflon_PP, teflon_REFL, 3);
    }
  else
    {
      G4cout << "!!!!> PTFE material not found!" << G4endl;
      exit(-1);
    }
}

void RnColumnDetectorConstruction::SetGXeTeflonReflectivity(G4double dGXeReflectivity)
{
  G4Material *pGXeTeflonMaterial = G4Material::GetMaterial(G4String("GXePTFE"));
  
  if(pGXeTeflonMaterial)
    {
      G4cout << "----> Setting GXe PTFE reflectivity to " << dGXeReflectivity << G4endl;
    
      G4MaterialPropertiesTable *pGXeTeflonPropertiesTable = pGXeTeflonMaterial->GetMaterialPropertiesTable();
    
      const G4int iNbEntries = 3;
    
      G4double teflon_PP[iNbEntries] = { 6.91 * eV, 6.98 * eV, 7.05 * eV };
      G4double teflon_REFL[iNbEntries] = {dGXeReflectivity, dGXeReflectivity, dGXeReflectivity};
      pGXeTeflonPropertiesTable->RemoveProperty("REFLECTIVITY");
      pGXeTeflonPropertiesTable->AddProperty("REFLECTIVITY", teflon_PP, teflon_REFL, iNbEntries);
      //pTeflonPropertiesTable->DumpTable();
   }
  else
    {
      G4cout << "ls!> PTFE material not found!" << G4endl;
      exit(-1);
    }
}

//******************************************************************/
// SetSS304LSteelReflectivity
//******************************************************************/
void RnColumnDetectorConstruction::SetSSteelReflectivity(G4double dSteelReflectivity) {
  G4Material *pSS304Material = G4Material::GetMaterial(G4String("SS304LSteel"));
  G4Material *pSS316Material = G4Material::GetMaterial(G4String("SS316LSteel"));

  if(pSS304Material)
    {
      G4cout << "----> Setting SS304LSteel reflectivity to " << dSteelReflectivity << G4endl;

      G4MaterialPropertiesTable *pSteelPropertiesTable = pSS304Material->GetMaterialPropertiesTable();
		
      G4double Steel_PP[] = { 6.91 * eV, 6.98 * eV, 7.05 * eV };
      G4double Steel_REFL[] = {dSteelReflectivity, dSteelReflectivity, dSteelReflectivity};
      pSteelPropertiesTable->RemoveProperty("REFLECTIVITY");
      pSteelPropertiesTable->AddProperty("REFLECTIVITY", Steel_PP, Steel_REFL, 3);
    }
  else
    {
      G4cout << "!!!!> SS304LSteel material not found!" << G4endl;
      exit(-1);
    }

  if (pSS316Material)
    {
      G4cout << "----> Setting SS316LSteel reflectivity to " << dSteelReflectivity << G4endl;

      G4MaterialPropertiesTable *pSteelPropertiesTable = pSS316Material->GetMaterialPropertiesTable();
    
      G4double Steel_PP[] = { 6.91 * eV, 6.98 * eV, 7.05 * eV };
      G4double Steel_REFL[] = {dSteelReflectivity, dSteelReflectivity, dSteelReflectivity};
      pSteelPropertiesTable->RemoveProperty("REFLECTIVITY");
      pSteelPropertiesTable->AddProperty("REFLECTIVITY", Steel_PP, Steel_REFL, 3);
    }
  else
    {
      G4cout << "!!!!> SS316LSteel material not found!" << G4endl;
      exit(-1);
    }
}


//******************************************************************/
// UpdateGeometry
//******************************************************************/
void RnColumnDetectorConstruction::UpdateGeometry() {
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}