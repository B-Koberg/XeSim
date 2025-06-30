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
#include "EnergyDistributionRock.hh"
//#include "NeutronShieldingMessenger.hh"

map<G4String, G4double> EnergyDistributionRock::m_hGeometryParameters;

EnergyDistributionRock::EnergyDistributionRock() {
    // needs to be set for the AnalysisManager
    m_hGeometryParameters["NbPhotoDets"] = 1;
    //m_pDetectorMessenger = new NeutronShieldingMessenger(this);
}

EnergyDistributionRock::~EnergyDistributionRock() {
  //delete m_pDetectorMessenger;
}

G4VPhysicalVolume* EnergyDistributionRock::Construct() {
    DefineMaterials();
    
    DefineGeometryParameters();
    
    ConstructLaboratory();
    
    ConstructDetector();
    
    return m_pLabPhysicalVolume;
}

G4double EnergyDistributionRock::GetGeometryParameter(const char *szParameter) {
  return m_hGeometryParameters[szParameter];
}

void EnergyDistributionRock::DefineMaterials() {
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
  G4Element *Fe = new G4Element("Iron",      "Fe", 26., 55.845*g/mole);
  G4Element *Ni = new G4Element("Nickel",    "Ni", 28., 58.693*g/mole);
  G4Element *Cu = new G4Element("Copper",    "Cu", 29., 63.546*g/mole);
  G4Element *Pb = new G4Element("Lead",      "Pb", 82., 207.200*g/mole);
  G4Element *Mo = new G4Element("Molybdenum","Mo", 42., 95.950*g/mole);
  G4Element *Mg = new G4Element("Magnesium", "Mg", 12., 24.305*g/mole);
  G4Element *K = new G4Element("Potassium", "K", 19., 39.098*g/mole);
  G4Element *Ca = new G4Element("Calcium", "Ca", 20., 40.078*g/mole);
  G4Element *S = new G4Element("Sulfur", "S", 16., 32.065*g/mole); 
  G4Element *Na = new G4Element("Natrium", "Na", 11., 22.989*g/mole);
  G4Element* P = new G4Element("Phosphorus", "P", 15., 30.974 * g/mole);
  G4Element* Ti = new G4Element("Titanium", "Ti", 22., 47.867 * g/mole);
  G4Element* B = new G4Element("Boron", "B", 5., 10.811 * g/mole);
  G4Element* Li = new G4Element("Lithium", "Li", 3., 6.941 * g/mole);

  G4NistManager* pNistManager = G4NistManager::Instance();
  pNistManager->FindOrBuildMaterial("G4_AIR");
  pNistManager->FindOrBuildMaterial("G4_WATER");
  pNistManager->FindOrBuildMaterial("G4_Galactic");
  
//------------------------------- Liquid Xenon with natural abundance element -----------------------------
  G4Isotope* Xe124 = new G4Isotope("Xe124", 54, 124, 123.90589*g/mole);
  G4Isotope* Xe126 = new G4Isotope("Xe126", 54, 126, 125.90430*g/mole);
  G4Isotope* Xe128 = new G4Isotope("Xe128", 54, 128, 127.90353*g/mole);
  G4Isotope* Xe129 = new G4Isotope("Xe129", 54, 129, 128.90478*g/mole);
  G4Isotope* Xe130 = new G4Isotope("Xe130", 54, 130, 129.90351*g/mole);
  G4Isotope* Xe131 = new G4Isotope("Xe131", 54, 131, 130.90508*g/mole);
  G4Isotope* Xe132 = new G4Isotope("Xe132", 54, 132, 131.90415*g/mole);
  G4Isotope* Xe134 = new G4Isotope("Xe134", 54, 134, 133.90540*g/mole);
  G4Isotope* Xe136 = new G4Isotope("Xe136", 54, 136, 135.90722*g/mole);

  G4Element* Xe_nA = new G4Element("Liquid Xenon", "LXe_nA", 9);
  Xe_nA->AddIsotope(Xe124, 0.0009);  // 0.09%
  Xe_nA->AddIsotope(Xe126, 0.0009);  // 0.09%
  Xe_nA->AddIsotope(Xe128, 0.0192);  // 1.92%
  Xe_nA->AddIsotope(Xe129, 0.2644);  // 26.44%
  Xe_nA->AddIsotope(Xe130, 0.0408);  // 4.08%
  Xe_nA->AddIsotope(Xe131, 0.2118);  // 21.18%
  Xe_nA->AddIsotope(Xe132, 0.2689);  // 26.89%
  Xe_nA->AddIsotope(Xe134, 0.1044);  // 10.44%
  Xe_nA->AddIsotope(Xe136, 0.0887);  // 8.87%

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
	
  pLXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD", 0./(21.6*eV));
  pLXePropertiesTable->AddConstProperty("RESOLUTIONSCALE", 0);
  pLXePropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 3.*ns);
  pLXePropertiesTable->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 27.*ns);
  pLXePropertiesTable->AddConstProperty("SCINTILLATIONYIELD1", 1.0);//ratio btw fast time constant and slow time constant
	
  LXe->SetMaterialPropertiesTable(pLXePropertiesTable);

  //-------------------------------- LXe_nA ---------------------------------
  G4Material *LXe_nA = new G4Material("LXe_nA", 2.9172*g/cm3, 1, kStateLiquid, 168.15*kelvin, 1.5*atmosphere);
  LXe_nA->AddElement(Xe_nA, 1);

  //-------------------------------- Rock ---------------------------------
  G4Material *Rock = new G4Material("Rock", 2.6*g/cm3, 7);

  Rock->AddElement(C, 0.1200);  // 12.00 % Kohlenstoff
  Rock->AddElement(O, 0.4840);  // 48.40 % Sauerstoff
  Rock->AddElement(Mg, 0.0564); // 5.64 % Magnesium
  Rock->AddElement(Al, 0.0104); // 1.04 % Aluminium
  Rock->AddElement(Si, 0.0128); // 1.28 % Silizium
  Rock->AddElement(K, 0.0104);  // 1.04 % Kalium
  Rock->AddElement(Ca, 0.3060); // 30.60 % Calcium

  //-------------------------------- Concrete ---------------------------------
  G4Material *Concrete = new G4Material("Concrete", 2.5*g/cm3, 13); //8% Water 

  Concrete->AddElement(H, 0.0090027);   // 0.90027 % Wasserstoff
  Concrete->AddElement(C, 0.0808242);   // 8.08242 % Kohlenstoff
  Concrete->AddElement(O, 0.4896469);   // 48.96469 % Sauerstoff
  Concrete->AddElement(Na, 0.0061018);  // 0.61018 % Natrium
  Concrete->AddElement(Mg, 0.0086025);  // 0.86025 % Magnesium
  Concrete->AddElement(Al, 0.0091009);  // 0.91009 % Aluminium
  Concrete->AddElement(Si, 0.0390234);  // 3.90234 % Silizium
  Concrete->AddElement(P, 0.0004003);   // 0.04003 % Phosphor
  Concrete->AddElement(S, 0.0016013);   // 0.16013 % Schwefel
  Concrete->AddElement(K, 0.0055016);   // 0.55016 % Kalium
  Concrete->AddElement(Ca, 0.3445414);  // 34.45414 % Calcium
  Concrete->AddElement(Ti, 0.0004003);  // 0.04003 % Titan
  Concrete->AddElement(Fe, 0.0043034);  // 0.43034 % Eisen


  //------------------------------- stainless steel -------------------------------
  G4Material *SS304LSteel = new G4Material("SS304LSteel", 8.00*g/cm3, 5, kStateSolid);
  SS304LSteel->AddElement(Fe, 0.65);
  SS304LSteel->AddElement(Cr, 0.20);
  SS304LSteel->AddElement(Ni, 0.12);
  SS304LSteel->AddElement(Mn, 0.02);
  SS304LSteel->AddElement(Si, 0.01);

}

void EnergyDistributionRock::DefineGeometryParameters() {
  m_hGeometryParameters["dLabHalfX"] = 5.*m;
  m_hGeometryParameters["dLabHalfY"] = 5.*m;
  m_hGeometryParameters["dLabHalfZ"] = 5.*m;

  m_hGeometryParameters["dHalfZ"] = 3.5*m;

  m_hGeometryParameters["dCountHalfX"] = 5*cm;

  m_hGeometryParameters["dRockSpawnHalfX"] = 50*cm;
  m_hGeometryParameters["dRockSpawnHalfY"] = 50*cm;
  m_hGeometryParameters["dRockSpawnHalfZ"] = 50*cm;

  m_hGeometryParameters["dRockShellThick"] = 3*m;

  m_hGeometryParameters["dConcreteHalfX"] = 25*cm;

  m_hGeometryParameters["dControlHalfThick"] = 5*cm;
}

void EnergyDistributionRock::ConstructLaboratory() {
  const G4double dLabHalfX = m_hGeometryParameters["dLabHalfX"];
  const G4double dLabHalfY = m_hGeometryParameters["dLabHalfY"];
  const G4double dLabHalfZ = m_hGeometryParameters["dLabHalfZ"];

  G4Box *pLabBox = new G4Box("LabBox", dLabHalfX, dLabHalfY, dLabHalfZ);
	
  G4Material *Air = G4Material::GetMaterial("G4_AIR");
  G4Material *Vacuum = G4Material::GetMaterial("G4_Galactic");

  m_pLabLogicalVolume = new G4LogicalVolume(pLabBox, Vacuum, "LabLogicalVolume", 0, 0, 0);

  G4Colour LabColor(1.,1.,1.,0.2);
  G4VisAttributes *pLabVisAtt = new G4VisAttributes(LabColor);
  pLabVisAtt->SetVisibility(false);
  m_pLabLogicalVolume->SetVisAttributes(pLabVisAtt);

  m_pLabPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(), m_pLabLogicalVolume, "Lab", 0, false, 0);
}

void EnergyDistributionRock::ConstructDetector() {
  G4SDManager *pSDManager = G4SDManager::GetSDMpointer();

  G4Material *Air = G4Material::GetMaterial("G4_AIR");
  G4Material *Water = G4Material::GetMaterial("G4_WATER");
  G4Material *Vacuum = G4Material::GetMaterial("G4_Galactic");

  G4Material *Rock = G4Material::GetMaterial("Rock");
  G4Material *Concrete = G4Material::GetMaterial("Concrete");


  const G4double dLabHalfX = m_hGeometryParameters["dLabHalfX"];
  const G4double dLabHalfY = m_hGeometryParameters["dLabHalfY"];
  const G4double dLabHalfZ = m_hGeometryParameters["dLabHalfZ"];

  const G4double dHalfZ = m_hGeometryParameters["dHalfZ"];

  const G4double dCountHalfX = m_hGeometryParameters["dCountHalfX"];

  const G4double dRockSpawnHalfX = m_hGeometryParameters["dRockSpawnHalfX"];
  const G4double dRockSpawnHalfY = m_hGeometryParameters["dRockSpawnHalfY"];
  const G4double dRockSpawnHalfZ = m_hGeometryParameters["dRockSpawnHalfZ"];

  const G4double dRockShellThick = m_hGeometryParameters["dRockShellThick"];

  const G4double dConcreteHalfX = m_hGeometryParameters["dConcreteHalfX"];

  const G4double dControlHalfThick = m_hGeometryParameters["dControlHalfThick"];

  //-------------------------------- Rock Volume ---------------------------------
  const G4double dRockBoxHalfX = dRockSpawnHalfX;
  const G4double dRockBoxHalfY = dRockSpawnHalfY + dRockShellThick;
  const G4double dRockBoxHalfZ = dHalfZ;

  G4Box *pRockBox = new G4Box("RockBox", dRockBoxHalfX, dRockBoxHalfY, dRockBoxHalfZ);

  m_pRockLogicalVolume = new G4LogicalVolume(pRockBox, Rock, "RockLogicalVolume");

  G4ThreeVector rockPosition(0., 0., 0.);
  m_pRockPhysicalVolume = new G4PVPlacement(
      0,                        // Keine Rotation
      rockPosition,         // Verschiebung
      m_pRockLogicalVolume, // Logisches Volumen
      "RockPhysicalVolume", // Name des Volumens
      m_pLabLogicalVolume,      // Muttervolumen (Labor)
      false,                    // Keine Mehrfachplatzierung
      0                         // Kopiennummer
  );

  G4Colour rockColor(0.2, 0.2, 0.2, 0.5); 
  G4VisAttributes *pRockVisAtt = new G4VisAttributes(rockColor);
  pRockVisAtt->SetVisibility(true);
  m_pRockLogicalVolume->SetVisAttributes(pRockVisAtt);

  //-------------------------------- Rock Spawn Volume ---------------------------------
  const G4double dRockSpawnBoxHalfX = dRockSpawnHalfX;
  const G4double dRockSpawnBoxHalfY = dRockSpawnHalfY;
  const G4double dRockSpawnBoxHalfZ = dRockSpawnHalfZ;


  G4Box *pRockSpawnBox = new G4Box("RockSpawnBox", dRockSpawnBoxHalfX, dRockSpawnBoxHalfY, dRockSpawnBoxHalfZ);


  m_pRockSpawnLogicalVolume = new G4LogicalVolume(pRockSpawnBox, Rock, "RockSpawnLogicalVolume");

  G4ThreeVector RockSpawnPosition(0., 0., 0.);
  m_pRockSpawnPhysicalVolume = new G4PVPlacement(
      0,                        // Keine Rotation
      RockSpawnPosition,         // Verschiebung
      m_pRockSpawnLogicalVolume, // Logisches Volumen
      "RockSpawnPhysicalVolume", // Name des Volumens
      m_pRockLogicalVolume,      // Muttervolumen (Labor)
      false,                    // Keine Mehrfachplatzierung
      0                         // Kopiennummer
  );

  G4Colour RockSpawnColor(0.,1.,0., 0.5); 
  G4VisAttributes *pRockSpawnVisAtt = new G4VisAttributes(RockSpawnColor);
  pRockSpawnVisAtt->SetVisibility(true);
  m_pRockSpawnLogicalVolume->SetVisAttributes(pRockSpawnVisAtt);

  //-------------------------------- Concrete Volume ---------------------------------
  const G4double dConcreteBoxHalfX = dConcreteHalfX;
  const G4double dConcreteBoxHalfY = dRockBoxHalfY; 
  const G4double dConcreteBoxHalfZ = dHalfZ;
  
  G4Box *pConcreteBox = new G4Box("ConcreteBox", dConcreteBoxHalfX, dConcreteBoxHalfY, dConcreteBoxHalfZ);

  m_pConcreteLogicalVolume = new G4LogicalVolume(pConcreteBox, Concrete, "ConcreteLogicalVolume");

  G4ThreeVector concretePosition(dRockBoxHalfX + dConcreteBoxHalfX, 0., 0.);
  m_pConcretePhysicalVolume = new G4PVPlacement(
      0,                        // Keine Rotation
      concretePosition,         // Verschiebung
      m_pConcreteLogicalVolume, // Logisches Volumen
      "ConcretePhysicalVolume", // Name des Volumens
      m_pLabLogicalVolume,      // Muttervolumen 
      false,                    // Keine Mehrfachplatzierung
      0                         // Kopiennummer
  );

  G4Colour concreteColor(0.5, 0.5, 0.5, 0.5); 
  G4VisAttributes *pConcreteVisAtt = new G4VisAttributes(concreteColor);
  pConcreteVisAtt->SetVisibility(true);
  m_pConcreteLogicalVolume->SetVisAttributes(pConcreteVisAtt);

  //-------------------------------- Concrete Spawn Volume ---------------------------------
  const G4double dConcreteSpawnBoxHalfX = dConcreteBoxHalfX;
  const G4double dConcreteSpawnBoxHalfY = dRockSpawnBoxHalfY;
  const G4double dConcreteSpawnBoxHalfZ = dRockSpawnBoxHalfZ;


  G4Box *pConcreteSpawnBox = new G4Box("ConcreteSpawnBox", dConcreteSpawnBoxHalfX, dConcreteSpawnBoxHalfY, dConcreteSpawnBoxHalfZ);


  m_pConcreteSpawnLogicalVolume = new G4LogicalVolume(pConcreteSpawnBox, Concrete, "ConcreteSpawnLogicalVolume");

  G4ThreeVector ConcreteSpawnPosition(0., 0., 0.);
  m_pConcreteSpawnPhysicalVolume = new G4PVPlacement(
      0,                        // Keine Rotation
      ConcreteSpawnPosition,         // Verschiebung
      m_pConcreteSpawnLogicalVolume, // Logisches Volumen
      "ConcreteSpawnPhysicalVolume", // Name des Volumens
      m_pConcreteLogicalVolume,      // Muttervolumen (Labor)
      false,                    // Keine Mehrfachplatzierung
      0                         // Kopiennummer
  );

  G4Colour ConcreteSpawnColor(0.,1.,0., 0.5); 
  G4VisAttributes *pConcreteSpawnVisAtt = new G4VisAttributes(ConcreteSpawnColor);
  pConcreteSpawnVisAtt->SetVisibility(true);
  m_pConcreteSpawnLogicalVolume->SetVisAttributes(pConcreteSpawnVisAtt);


  //-------------------------------- Count Volume ---------------------------------
  const G4double dCountBoxHalfX = dCountHalfX ;
  const G4double dCountBoxHalfY = dRockBoxHalfY + 2 * dControlHalfThick; 
  const G4double dCountBoxHalfZ = dHalfZ + 2 * dControlHalfThick;

  G4Box *pCountBox = new G4Box("CountBox", dCountBoxHalfX, dCountBoxHalfY, dCountBoxHalfZ);

  m_pCountLogicalVolume = new G4LogicalVolume(pCountBox, Air, "CountLogicalVolume");

  G4ThreeVector CountPosition(dRockBoxHalfX + 2*dConcreteBoxHalfX + dCountHalfX, 0., 0.);
  m_pCountPhysicalVolume = new G4PVPlacement(
      0,                        // Keine Rotation
      CountPosition,         // Verschiebung
      m_pCountLogicalVolume, // Logisches Volumen
      "CountPhysicalVolume", // Name des Volumens
      m_pLabLogicalVolume,      // Muttervolumen 
      false,                    // Keine Mehrfachplatzierung
      0                         // Kopiennummer
  );

  G4Colour CountColor(0.5, 0., 0.5, 0.3); 
  G4VisAttributes *pCountVisAtt = new G4VisAttributes(CountColor);
  pCountVisAtt->SetVisibility(true);
  m_pCountLogicalVolume->SetVisAttributes(pCountVisAtt);

  //-------------------------------- Control Volume (4 Boxen als Umrandung) ---------------------------------
  const G4double dControlBoxHalfX = dRockBoxHalfX + dConcreteBoxHalfX ; 
  const G4double dControlBoxHalfY = dControlHalfThick; 
  const G4double dControlBoxHalfZ = dHalfZ;

  
  const G4double dPosY = dRockBoxHalfY + dControlHalfThick;
  const G4ThreeVector ControlCenterPosition(dConcreteBoxHalfX, -dPosY, 0.);

  // Erstelle 4 Boxen für die Umrandung
  G4Box *pControlBox1 = new G4Box("ControlBox1", dControlBoxHalfX, dControlBoxHalfY, dControlBoxHalfZ);
  G4Box *pControlBox2 = new G4Box("ControlBox2", dControlBoxHalfX, dControlBoxHalfY, dControlBoxHalfZ);
  G4Box *pControlBox3 = new G4Box("ControlBox3", dControlBoxHalfY, dControlBoxHalfX, dControlBoxHalfZ);
  G4Box *pControlBox4 = new G4Box("ControlBox4", dControlBoxHalfY, dControlBoxHalfX, dControlBoxHalfZ);

  // Rotationsmatrizen für das dritte und vierte Volumen
  G4RotationMatrix* rot = new G4RotationMatrix();
  rot->rotateY(90*deg);    
  rot->rotateX(90*deg); 

  // Positionen für die 4 Boxen um das Zentrum
  G4ThreeVector pos1(0., -dPosY, 0.);       
  G4ThreeVector pos2(0., dPosY, 0.);                        
  G4ThreeVector pos3(0., 0., dPosY);                                     
  G4ThreeVector pos4(0., 0., -dPosY);                                    

  // Union Solid erstellen - kombiniere alle 4 Boxen
  G4UnionSolid *pControlUnion1 = new G4UnionSolid("ControlUnion1", pControlBox1, pControlBox2, 0, pos2 - pos1);
  G4UnionSolid *pControlUnion2 = new G4UnionSolid("ControlUnion2", pControlUnion1, pControlBox3, rot, pos3 - pos1);
  G4UnionSolid *pControlUnionFinal = new G4UnionSolid("ControlUnionFinal", pControlUnion2, pControlBox4, rot, pos4 - pos1);

  m_pControlLogicalVolume = new G4LogicalVolume(pControlUnionFinal, Air, "ControlLogicalVolume");

  m_pControlPhysicalVolume = new G4PVPlacement(
      0,                        // Keine Rotation
      ControlCenterPosition,    // Position der ersten Box als Referenz
      m_pControlLogicalVolume, // Logisches Volumen
      "ControlPhysicalVolume", // Name des Volumens
      m_pLabLogicalVolume,      // Muttervolumen 
      false,                    // Keine Mehrfachplatzierung
      0                         // Kopiennummer
  );

  G4Colour ControlColor(1, 0., 0., 0.3); 
  G4VisAttributes *pControlVisAtt = new G4VisAttributes(ControlColor);
  pControlVisAtt->SetVisibility(true);
  m_pControlLogicalVolume->SetVisAttributes(pControlVisAtt);


  //-------------------------------- PhotoDet ---------------------------------
  const G4double dPhotoDetHeight = 1.*mm;
  const G4double dPhotoDetOffsetZ = dLabHalfZ - 2*dPhotoDetHeight;
  G4Box *pPhotoDetBox = new G4Box("PhotoDetBox", 1*mm, 1*mm, dPhotoDetHeight);
  m_pPhotoDetLogicalVolume = new G4LogicalVolume(pPhotoDetBox, Air, "PmtPhotoCathodeLogicalVolume", 0, 0, 0);
  m_pPhotoDetPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., dPhotoDetOffsetZ),
						       m_pPhotoDetLogicalVolume, "PmtPhotoCathode", m_pLabLogicalVolume, false, 0);

  XeSimPhotoDetSensitiveDetector *pPmtSD = new XeSimPhotoDetSensitiveDetector("template/PhotoDetSD");
  pSDManager->AddNewDetector(pPmtSD);
  m_pPhotoDetLogicalVolume->SetSensitiveDetector(pPmtSD); 

  //-------------------------------- LXe Volume ---------------------------------
  G4Box *pLXeBox1 = new G4Box("LXeBox1", 1*mm, 1*mm, 1*mm);

  m_pLXeLogicalVolume = new G4LogicalVolume(pLXeBox1, Air, "LXeVolume", 0, 0, 0);
  m_pLXePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., dLabHalfZ - 2*dPhotoDetHeight - 2 *mm), m_pLXeLogicalVolume,
                                               "LXe", m_pLabLogicalVolume, false, 0);
                                               

  XeSimLXeSensitiveDetector *pLXeSD = new XeSimLXeSensitiveDetector("template/LXeSD");
  pSDManager->AddNewDetector(pLXeSD);
  m_pLXeLogicalVolume->SetSensitiveDetector(pLXeSD); 
}