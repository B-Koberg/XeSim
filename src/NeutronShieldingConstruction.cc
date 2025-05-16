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
#include "NeutronShieldingConstruction.hh"
//#include "NeutronShieldingMessenger.hh"

map<G4String, G4double> NeutronShieldingConstruction::m_hGeometryParameters;

NeutronShieldingConstruction::NeutronShieldingConstruction() {
    // needs to be set for the AnalysisManager
    m_hGeometryParameters["NbPhotoDets"] = 1;
    //m_pDetectorMessenger = new NeutronShieldingMessenger(this);
}

NeutronShieldingConstruction::~NeutronShieldingConstruction() {
  //delete m_pDetectorMessenger;
}

G4VPhysicalVolume* NeutronShieldingConstruction::Construct() {
    DefineMaterials();
    
    DefineGeometryParameters();
    
    ConstructLaboratory();
    
    ConstructDetector();
    
    return m_pLabPhysicalVolume;
}

G4double NeutronShieldingConstruction::GetGeometryParameter(const char *szParameter) {
  return m_hGeometryParameters[szParameter];
}

void NeutronShieldingConstruction::DefineMaterials() {
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
  
  //------------------------------- Enriched Boron -----------------------------
  G4Isotope* isoB10 = new G4Isotope("B10", 5, 10, 10.0129*g/mole);
  G4Isotope* isoB11 = new G4Isotope("B11", 5, 11, 11.0093*g/mole);

  G4Element* eB = new G4Element("EnrichedBoron", "EB", 2);
  eB->AddIsotope(isoB10, 19.6*perCent);
  eB->AddIsotope(isoB11, 80.4*perCent);

  //------------------------------- Enriched Lithium -----------------------------
  G4Isotope* isoLi6 = new G4Isotope("Li6", 3, 6, 6.0151*g/mole);
  G4Isotope* isoLi7 = new G4Isotope("Li7", 3, 7, 7.0160*g/mole);

  G4Element* eLi = new G4Element("EnrichedLithium", "ELi", 2);
  eLi->AddIsotope(isoLi7, 92.6*perCent);
  eLi->AddIsotope(isoLi6, 7.4*perCent);

  //------------------------------- Enriched Lithium -----------------------------
  G4Element* eLi6 = new G4Element("EnrichedLithium6", "ELi6", 2);
  eLi6->AddIsotope(isoLi6, 79*perCent);
  eLi6->AddIsotope(isoLi7, 21*perCent);

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

  //------------------------------- Paraffin -------------------------------
  G4Material* paraffin = new G4Material("Paraffin", 0.9*g/cm3, 2, kStateSolid);
  paraffin->AddElement(C, (100-14.96)*perCent);
  paraffin->AddElement(H, 14.96*perCent);

  //------------------------------- Paraffin 25% Boron-------------------------------
  G4Material* paraffin25 = new G4Material("Paraffin25", 1*g/cm3, 3, kStateSolid);
  paraffin25->AddElement(C, (100-25-9.72)*perCent);
  paraffin25->AddElement(H, 9.72*perCent);
  paraffin25->AddElement(eB, 25*perCent);

  //------------------------------- Polyethylen -------------------------------
  G4Material* polyethylen = new G4Material("Polyethylen", 0.93*g/cm3, 2, kStateSolid);
  polyethylen->AddElement(C, (100-13.38)*perCent);
  polyethylen->AddElement(H, 13.38*perCent);

  //------------------------------- Polyethylen 5% Boron-------------------------------

  G4Material* polyethylen5 = new G4Material("Polyethylen5", 1.07*g/cm3, 3, kStateSolid);
  polyethylen5->AddElement(C, (100-5-11.7)*perCent);
  polyethylen5->AddElement(H, 11.7*perCent);
  polyethylen5->AddElement(eB, 5*perCent);

  //------------------------------- Polyethylen 25% Boron-------------------------------
  G4Material* polyethylen25 = new G4Material("Polyethylen25", 1.17*g/cm3, 3, kStateSolid);
  polyethylen25->AddElement(C, (100-25-9.3)*perCent);
  polyethylen25->AddElement(H, 9.3*perCent);
  polyethylen25->AddElement(eB, 25*perCent);

  //------------------------------- Polyethylen 7.5% lithium -------------------------------
  G4Material* polyethylen7Lit = new G4Material("Polyethylen7Lit", 1.06*g/cm3, 3, kStateSolid);
  polyethylen7Lit->AddElement(C, (100-7.5-8.59)*perCent);
  polyethylen7Lit->AddElement(H, 8.59*perCent);
  polyethylen7Lit->AddElement(eLi, 7.5*perCent);

  //------------------------------- Silicone 5% Boron-------------------------------
  G4Material* silicone5 = new G4Material("Silicone5", 1.34*g/cm3, 4, kStateSolid);
  silicone5->AddElement(eB, 5*perCent);      // B
  silicone5->AddElement(Si, 87.25/3*perCent);     // Si
  silicone5->AddElement(C, 87.25*2/3*perCent + 0.25 * perCent);      // C
  silicone5->AddElement(H, 7.5*perCent);     // H

  //------------------------------- Silicone 25% Lithium 6-------------------------------
  G4Material* silicone25Lit6 = new G4Material("Silicone25Lit6", 1.34*g/cm3, 4, kStateSolid);
  silicone25Lit6->AddElement(eLi6, 25*perCent);      // Li
  silicone25Lit6->AddElement(Si, (100-25-1.3)/3*perCent);     // Si
  silicone25Lit6->AddElement(C, (100-25-1.3)*2/3*perCent);      // C
  silicone25Lit6->AddElement(H, 1.3*perCent);     // H

}

void NeutronShieldingConstruction::DefineGeometryParameters() {
    m_hGeometryParameters["dLabHalfX"] = 60.*m;
    m_hGeometryParameters["dLabHalfY"] = 60.*m;
    m_hGeometryParameters["dLabHalfZ"] = 60.*m;

    m_hGeometryParameters["dRockHalfThick"] = 2500*mm;
    m_hGeometryParameters["dConcreteHalfThick"] = 25*cm;
    m_hGeometryParameters["dSpawnHalfThick"] = 0.5*cm;

    m_hGeometryParameters["dHallHalfX"] = 9.1*m;
    m_hGeometryParameters["dHallHalfY"] = 30.*m;
    m_hGeometryParameters["dHallHalfZ"] = (20*m - 9.1*m)/2;
    m_hGeometryParameters["dHallRadius"] = 9.1*m;

    m_hGeometryParameters["dBuildHalfX"] = 3.*m;
    m_hGeometryParameters["dBuildHalfY"] = 3.*m;
    m_hGeometryParameters["dBuildHalfZ"] = 3.*m;
    m_hGeometryParameters["dBuildHalfThick"] = 0.5*cm;

    m_hGeometryParameters["dAbsorberHalfThick"]= 2.5*cm;

    m_hGeometryParameters["dLXeHalfX"] = 0.23*m;
    m_hGeometryParameters["dLXeHalfY"] = 0.23*m;
    m_hGeometryParameters["dLXeHalfZ"] = 0.23*m;
    m_hGeometryParameters["dLXeContainerHalfThick"] = 0.05*cm;

    m_hGeometryParameters["dLXeHalfY_single"] = 0.71*m;

    m_hGeometryParameters["dLXeContainerDist"] = 0.5*m;

  
    m_hGeometryParameters["dWaterContainerHalfZ"] = 5*m;
    m_hGeometryParameters["dWaterContainerRadius"] = 5*m;
    m_hGeometryParameters["dWaterConatinerThick"] = 0.25*cm;
    m_hGeometryParameters["dWaterContainerDistance"] = 20*cm;
}

void NeutronShieldingConstruction::ConstructLaboratory() {
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

void NeutronShieldingConstruction::ConstructDetector() {
  G4SDManager *pSDManager = G4SDManager::GetSDMpointer();

  G4Material *Air = G4Material::GetMaterial("G4_AIR");
  G4Material *Water = G4Material::GetMaterial("G4_WATER");
  G4Material *Vacuum = G4Material::GetMaterial("G4_Galactic");

  G4Material *LXe = G4Material::GetMaterial("LXe");
  G4Material *LXe_nA = G4Material::GetMaterial("LXe_nA");

  G4Material *StainlessSteel = G4Material::GetMaterial("SS304LSteel");

  G4Material *Rock = G4Material::GetMaterial("Rock");
  G4Material *Concrete = G4Material::GetMaterial("Concrete");

  G4Material *AbsorberMat = G4Material::GetMaterial("Polyethylen5");

  G4Material *Paraffin = G4Material::GetMaterial("Paraffin");
  G4Material *Paraffin25 = G4Material::GetMaterial("Paraffin25");
  G4Material *Polyethylen = G4Material::GetMaterial("Polyethylen");
  G4Material *Polyethylen5 = G4Material::GetMaterial("Polyethylen5");
  G4Material *Polyethylen25 = G4Material::GetMaterial("Polyethylen25");
  G4Material *Polyethylen7Lit = G4Material::GetMaterial("Polyethylen7Lit");
  G4Material *Silicone5 = G4Material::GetMaterial("Silicone5");
  G4Material *Silicone25Lit6 = G4Material::GetMaterial("Silicone25Lit6");


  const G4double dLabHalfX = m_hGeometryParameters["dLabHalfX"];
  const G4double dLabHalfY = m_hGeometryParameters["dLabHalfY"];
  const G4double dLabHalfZ = m_hGeometryParameters["dLabHalfZ"];

  const G4double dHallHalfX = m_hGeometryParameters["dHallHalfX"];
  const G4double dHallHalfY = m_hGeometryParameters["dHallHalfY"];
  const G4double dHallHalfZ = m_hGeometryParameters["dHallHalfZ"];
  const G4double dHallRadius = m_hGeometryParameters["dHallRadius"];

	const G4double dConcreteHalfThick = m_hGeometryParameters["dConcreteHalfThick"];
  const G4double dRockHalfThick = m_hGeometryParameters["dRockHalfThick"];
  const G4double dSpawnHalfThick = m_hGeometryParameters["dSpawnHalfThick"];

  const G4double dBuildHalfX = m_hGeometryParameters["dBuildHalfX"];
  const G4double dBuildHalfY = m_hGeometryParameters["dBuildHalfY"];
  const G4double dBuildHalfZ = m_hGeometryParameters["dBuildHalfZ"];
  const G4double dBuildHalfThick = m_hGeometryParameters["dBuildHalfThick"];
  const G4double dLXeHalfX = m_hGeometryParameters["dLXeHalfX"];
  const G4double dLXeHalfY = m_hGeometryParameters["dLXeHalfY"];
  const G4double dLXeHalfZ = m_hGeometryParameters["dLXeHalfZ"];
  const G4double dLXeContainerHalfThick = m_hGeometryParameters["dLXeContainerHalfThick"];
  const G4double dAbsorberHalfThick = m_hGeometryParameters["dAbsorberHalfThick"];
  const G4double dLXeContainerDist = m_hGeometryParameters["dLXeContainerDist"];
  const G4double dLXeHalfY_single = m_hGeometryParameters["dLXeHalfY_single"];

  const G4double dWaterContainerHalfZ = m_hGeometryParameters["dWaterContainerHalfZ"];
  const G4double dWaterContainerRadius = m_hGeometryParameters["dWaterContainerRadius"];
  const G4double dWaterContainerThick = m_hGeometryParameters["dWaterConatinerThick"];
  const G4double dWaterContainerDistance = m_hGeometryParameters["dWaterContainerDistance"];
  

  //-------------------------------- Rock Volume ---------------------------------
  const G4double dRockBoxHalfX = dHallHalfX + 2 * dConcreteHalfThick + 2 * dRockHalfThick;
  const G4double dRockBoxHalfY = dHallHalfY + 2 * dConcreteHalfThick + 2 * dRockHalfThick;
  const G4double dRockBoxHalfZ = dHallHalfZ + dConcreteHalfThick + dRockHalfThick;

  const G4double dOffSetZ = dRockBoxHalfZ - 2*dConcreteHalfThick - 2*dRockHalfThick;

  G4Box *pRockBox = new G4Box("RockBox", dRockBoxHalfX, dRockBoxHalfY, dRockBoxHalfZ);


  G4double dRockVaultInnerRadius = 0;  
  G4double dRockVaultOuterRadius = dHallRadius + 2*dRockHalfThick + 2*dConcreteHalfThick;   
  G4double dRockVaultHalfLength = dHallHalfY + 2*dRockHalfThick + 2*dConcreteHalfThick;    
  G4double dRockVaultStartAngle = 0.0 * deg; 
  G4double dRockVaultSweepAngle = 180.0 * deg; 
  
  G4Tubs* pRockVault = new G4Tubs("RockVault",
                                  dRockVaultInnerRadius,
                                  dRockVaultOuterRadius,
                                  dRockVaultHalfLength,
                                  dRockVaultStartAngle,
                                  dRockVaultSweepAngle);
  
  G4RotationMatrix* rotation = new G4RotationMatrix();
  rotation->rotateX(-90.0 * deg); // Drehung um 90° um die x-Achse
  G4UnionSolid *pRockBoxWithVault = new G4UnionSolid("RockBoxWithVault", pRockBox, pRockVault, rotation, G4ThreeVector(0., 0., dRockBoxHalfZ));

  m_pRockLogicalVolume = new G4LogicalVolume(pRockBoxWithVault, Rock, "RockLogicalVolume");

  G4ThreeVector rockPosition(0., 0., dOffSetZ);
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

  //-------------------------------- Concrete Volume ---------------------------------
  const G4double dConcreteBoxHalfX = dHallHalfX + 2 * dConcreteHalfThick ;
  const G4double dConcreteBoxHalfY = dHallHalfY + 2 * dConcreteHalfThick ; 
  const G4double dConcreteBoxHalfZ = dHallHalfZ + dConcreteHalfThick;
  G4Box *pConcreteBox = new G4Box("ConcreteBox", dConcreteBoxHalfX, dConcreteBoxHalfY, dConcreteBoxHalfZ);

  G4double dConcreteVaultInnerRadius = 0;  
  G4double dConcreteVaultOuterRadius = dHallRadius + 2*dConcreteHalfThick;   
  G4double dConcreteVaultHalfLength = dHallHalfY + 2*dConcreteHalfThick;    
  G4double dConcreteVaultStartAngle = 0.0 * deg; 
  G4double dConcreteVaultSweepAngle = 180.0 * deg; 
  
  G4Tubs* pConcreteVault = new G4Tubs("ConcreteVault",
                                  dConcreteVaultInnerRadius,
                                  dConcreteVaultOuterRadius,
                                  dConcreteVaultHalfLength,
                                  dConcreteVaultStartAngle,
                                  dConcreteVaultSweepAngle);

  G4UnionSolid *pConcreteBoxWithVault = new G4UnionSolid("ConcreteBoxWithVault", pConcreteBox, pConcreteVault, rotation, G4ThreeVector(0., 0., dConcreteBoxHalfZ));

  m_pConcreteLogicalVolume = new G4LogicalVolume(pConcreteBoxWithVault, Concrete, "ConcreteLogicalVolume");

  G4ThreeVector concretePosition(0., 0., dRockHalfThick);
  m_pConcretePhysicalVolume = new G4PVPlacement(
      0,                        // Keine Rotation
      concretePosition,         // Verschiebung
      m_pConcreteLogicalVolume, // Logisches Volumen
      "ConcretePhysicalVolume", // Name des Volumens
      m_pRockLogicalVolume,      // Muttervolumen 
      false,                    // Keine Mehrfachplatzierung
      0                         // Kopiennummer
  );

  G4Colour concreteColor(0.5, 0.5, 0.5, 0.5); 
  G4VisAttributes *pConcreteVisAtt = new G4VisAttributes(concreteColor);
  pConcreteVisAtt->SetVisibility(true);
  m_pConcreteLogicalVolume->SetVisAttributes(pConcreteVisAtt);

  //-------------------------------- Spawn Volume ---------------------------------
  const G4double dSpawnBoxHalfX = dHallHalfX ;
  const G4double dSpawnBoxHalfY = dHallHalfY ; 
  const G4double dSpawnBoxHalfZ = dHallHalfZ ;
  G4Box *pSpawnBox = new G4Box("SpawnBox", dSpawnBoxHalfX, dSpawnBoxHalfY, dSpawnBoxHalfZ);

  G4double dSpawnVaultInnerRadius = 0;  
  G4double dSpawnVaultOuterRadius = dHallRadius ;   
  G4double dSpawnVaultHalfLength = dHallHalfY ;    
  G4double dSpawnVaultStartAngle = 0.0 * deg; 
  G4double dSpawnVaultSweepAngle = 180.0 * deg; 
  
  G4Tubs* pSpawnVault = new G4Tubs("SpawnVault",
                                  dSpawnVaultInnerRadius,
                                  dSpawnVaultOuterRadius,
                                  dSpawnVaultHalfLength,
                                  dSpawnVaultStartAngle,
                                  dSpawnVaultSweepAngle);

  G4UnionSolid *pSpawnBoxWithVault = new G4UnionSolid("SpawnBoxWithVault", pSpawnBox, pSpawnVault, rotation, G4ThreeVector(0., 0., dSpawnBoxHalfZ));

  m_pSpawnLogicalVolume = new G4LogicalVolume(pSpawnBoxWithVault, Air, "SpawnLogicalVolume");

  G4ThreeVector SpawnPosition(0., 0., dConcreteHalfThick);
  m_pSpawnPhysicalVolume = new G4PVPlacement(
      0,                        // Keine Rotation
      SpawnPosition,         // Verschiebung
      m_pSpawnLogicalVolume, // Logisches Volumen
      "SpawnPhysicalVolume", // Name des Volumens
      m_pConcreteLogicalVolume,      // Muttervolumen 
      false,                    // Keine Mehrfachplatzierung
      0                         // Kopiennummer
  );

  G4Colour SpawnColor(0.5, 0., 0.5, 0.3); 
  G4VisAttributes *pSpawnVisAtt = new G4VisAttributes(SpawnColor);
  pSpawnVisAtt->SetVisibility(true);
  m_pSpawnLogicalVolume->SetVisAttributes(pSpawnVisAtt);

  //-------------------------------- Air Volume ---------------------------------
  const G4double dAirBoxHalfX = dHallHalfX - 2*dSpawnHalfThick ;
  const G4double dAirBoxHalfY = dHallHalfY - 2*dSpawnHalfThick; 
  const G4double dAirBoxHalfZ = dHallHalfZ - dSpawnHalfThick;
  G4Box *pAirBox = new G4Box("AirBox", dAirBoxHalfX, dAirBoxHalfY, dAirBoxHalfZ);

  G4double dAirVaultInnerRadius = 0;  
  G4double dAirVaultOuterRadius = dHallRadius - 2*dSpawnHalfThick;   
  G4double dAirVaultHalfLength = dHallHalfY - 2*dSpawnHalfThick; ;    
  G4double dAirVaultStartAngle = 0.0 * deg; 
  G4double dAirVaultSweepAngle = 180.0 * deg; 
  
  G4Tubs* pAirVault = new G4Tubs("AirVault",
                                  dAirVaultInnerRadius,
                                  dAirVaultOuterRadius,
                                  dAirVaultHalfLength,
                                  dAirVaultStartAngle,
                                  dAirVaultSweepAngle);

  G4UnionSolid *pAirBoxWithVault = new G4UnionSolid("AirBoxWithVault", pAirBox, pAirVault, rotation, G4ThreeVector(0., 0., dAirBoxHalfZ));

  m_pAirLogicalVolume = new G4LogicalVolume(pAirBoxWithVault, Air, "AirLogicalVolume");

  G4ThreeVector airPosition(0., 0., dSpawnHalfThick);
  m_pAirPhysicalVolume = new G4PVPlacement(
      0,                        // Keine Rotation
      airPosition,         // Verschiebung
      m_pAirLogicalVolume, // Logisches Volumen
      "AirPhysicalVolume", // Name des Volumens
      m_pSpawnLogicalVolume,      // Muttervolumen 
      false,                    // Keine Mehrfachplatzierung
      0                         // Kopiennummer
  );

  G4Colour airColor(0.5, 0., 0.5, 0.3); 
  G4VisAttributes *pAirVisAtt = new G4VisAttributes(airColor);
  pAirVisAtt->SetVisibility(true);
  m_pAirLogicalVolume->SetVisAttributes(pAirVisAtt);

  

  //-------------------------------- Buidling Volume ---------------------------------
  
  // Building
  G4Box *pBuildBox = new G4Box("BuildBox", dBuildHalfX, dBuildHalfY, dBuildHalfZ);
  m_pBuildLogicalVolume = new G4LogicalVolume(pBuildBox, StainlessSteel, "BuildVolume", 0, 0, 0);
  m_pBuildPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., -dHallHalfZ + dBuildHalfZ + dSpawnHalfThick ), m_pBuildLogicalVolume,
                                              "Build", m_pAirLogicalVolume, false, 0);
  
  G4Colour hBuildColor(0.0,1.,0.,0.5);

  G4VisAttributes *pBuildVisAtt = new G4VisAttributes(hBuildColor);
  pBuildVisAtt->SetVisibility(true);
  m_pBuildLogicalVolume->SetVisAttributes(pBuildVisAtt);

  // Air in the building
  G4Box *pBuildAirBox = new G4Box("BuildAirBox", dBuildHalfX-2*dBuildHalfThick, dBuildHalfY-2*dBuildHalfThick, dBuildHalfZ-2*dBuildHalfThick);
  m_pBuildAirLogicalVolume = new G4LogicalVolume(pBuildAirBox, Air, "BuildAirVolume", 0, 0, 0);
  m_pBuildAirPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., 0), m_pBuildAirLogicalVolume,
                                              "BuildAir", m_pBuildLogicalVolume, false, 0);

  m_pBuildAirLogicalVolume->SetVisAttributes(pAirVisAtt);

  // Absorber
  const G4double dAbsorberHalfX = dLXeHalfX + 2*dLXeContainerHalfThick + 2*dAbsorberHalfThick; ;
  const G4double dAbsorberHalfY = dLXeHalfY + 2*dLXeContainerHalfThick + dLXeContainerDist + 2*dLXeContainerHalfThick + 2*dLXeHalfY + 2*dLXeContainerHalfThick + 2*dAbsorberHalfThick;
  const G4double dAbsorberHalfZ = dAbsorberHalfX ;

  G4Box *pAbsorber_out = new G4Box("Absorber_outer",dAbsorberHalfX, dAbsorberHalfY, dAbsorberHalfZ);
  /* G4Box *pAbsorber_in = new G4Box("Absorber_inner",dAbsorberHalfX-2*dAbsorberHalfThick, dAbsorberHalfY-2*dAbsorberHalfThick, dAbsorberHalfZ-2*dAbsorberHalfThick);
  
  G4SubtractionSolid* pAbsorber = new G4SubtractionSolid("Absorber",pAbsorber_out , pAbsorber_in, 0, G4ThreeVector(0., 0., 0.));
 */
  
  m_pAbsorberLogicalVolume = new G4LogicalVolume(pAbsorber_out, AbsorberMat, "AbsorberVolume", 0, 0, 0);
  m_pAbsorberPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., 0), m_pAbsorberLogicalVolume,
                                              "Absorber", m_pBuildAirLogicalVolume, false, 0);
  
  G4Colour hAbsorberColor(0.5, 0.5, 0.5, 0.3);
  G4VisAttributes *pAbsorberVisAtt = new G4VisAttributes(hAbsorberColor);
  pAbsorberVisAtt->SetVisibility(true);
  m_pAbsorberLogicalVolume->SetVisAttributes(pAbsorberVisAtt);

  for (size_t j = 0; j < AbsorberMat->GetNumberOfElements(); ++j) {
                  G4cout << AbsorberMat->GetFractionVector()[j] << AbsorberMat->GetElement(j)->GetName() << AbsorberMat->GetDensity()*cm3/g << G4endl;
              } 
  
  // Air in the absorber
  G4Box *pAbsorberAir = new G4Box("AbsorberAir",dAbsorberHalfX-2*dAbsorberHalfThick, dAbsorberHalfY-2*dAbsorberHalfThick, dAbsorberHalfZ-2*dAbsorberHalfThick);
  m_pAbsorberAirLogicalVolume = new G4LogicalVolume(pAbsorberAir, Air, "AbsorberAirVolume", 0, 0, 0);
  m_pAbsorberAirPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., 0), m_pAbsorberAirLogicalVolume,
                                              "AbsorberAir", m_pAbsorberLogicalVolume, false, 0);

  m_pAbsorberAirLogicalVolume->SetVisAttributes(pAirVisAtt);

  // Container
  G4Box *pLXeContainerBox1 = new G4Box("LXeContainerBox1", dLXeHalfX+2*dLXeContainerHalfThick, dLXeHalfY+2*dLXeContainerHalfThick, dLXeHalfZ+2*dLXeContainerHalfThick);
  
  G4ThreeVector LXeContainerPosition(0., dLXeContainerDist + 2* dLXeHalfY + 4*dLXeContainerHalfThick, 0.);

  G4UnionSolid *pLXeContainerBox2 = new G4UnionSolid("LXeContainer2", pLXeContainerBox1, pLXeContainerBox1, 0, LXeContainerPosition);

  G4UnionSolid *pLXeContainerBox = new G4UnionSolid("LXeContainer", pLXeContainerBox2, pLXeContainerBox1, 0, - LXeContainerPosition);

  m_pLXeContainerLogicalVolume = new G4LogicalVolume(pLXeContainerBox, StainlessSteel, "LXeContainerVolume", 0, 0, 0);
  m_pLXeContainerPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., 0), m_pLXeContainerLogicalVolume,
                                              "LXeContainer", m_pAbsorberAirLogicalVolume, false, 0);

  m_pLXeContainerLogicalVolume->SetVisAttributes(pBuildVisAtt);
  
  // LXe
  G4Box *pLXeBox1 = new G4Box("LXeBox1", dLXeHalfX, dLXeHalfY, dLXeHalfZ);

  G4UnionSolid *pLXeBox2 = new G4UnionSolid("LXeBox2", pLXeBox1, pLXeBox1, 0, LXeContainerPosition);
  G4UnionSolid *pLXeBox = new G4UnionSolid("LXeBox", pLXeBox2, pLXeBox1, 0, - LXeContainerPosition);

  m_pLXeLogicalVolume = new G4LogicalVolume(pLXeBox, LXe_nA, "LXeVolume", 0, 0, 0);
  m_pLXePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., 0), m_pLXeLogicalVolume,
                                              "LXe", m_pLXeContainerLogicalVolume, false, 0);

  G4Colour hLXeColor(0.0,0.0,1.0,0.5);
  G4VisAttributes *pLXeVisAtt = new G4VisAttributes(hLXeColor);
  pLXeVisAtt->SetVisibility(true);
  m_pLXeLogicalVolume->SetVisAttributes(pLXeVisAtt);

  XeSimLXeSensitiveDetector *pLXeSD = new XeSimLXeSensitiveDetector("template/LXeSD");
  pSDManager->AddNewDetector(pLXeSD);
  m_pLXeLogicalVolume->SetSensitiveDetector(pLXeSD); 

  /* //single LXe
  // Container
  G4Box *pLXeContainerBox = new G4Box("LXeContainerBox", dLXeHalfX+2*dLXeContainerHalfThick, dLXeHalfY_single+2*dLXeContainerHalfThick, dLXeHalfZ+2*dLXeContainerHalfThick);

  m_pLXeContainerLogicalVolume = new G4LogicalVolume(pLXeContainerBox, StainlessSteel, "LXeContainerVolume", 0, 0, 0);
  m_pLXeContainerPhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., 0), m_pLXeContainerLogicalVolume,
                                              "LXeContainer", m_pBuildAirLogicalVolume, false, 0);
  
  m_pLXeContainerLogicalVolume->SetVisAttributes(pBuildVisAtt);
  
  // LXe
  G4Box *pLXeBox = new G4Box("LXeBox", dLXeHalfX, dLXeHalfY_single, dLXeHalfZ);

  m_pLXeLogicalVolume = new G4LogicalVolume(pLXeBox, LXe, "LXeVolume", 0, 0, 0);
  m_pLXePhysicalVolume = new G4PVPlacement(0, G4ThreeVector(0., 0., 0), m_pLXeLogicalVolume,
                                              "LXe", m_pLXeContainerLogicalVolume, false, 0);

  G4Colour hLXeColor(0.0,0.0,1.0,0.5);
  G4VisAttributes *pLXeVisAtt = new G4VisAttributes(hLXeColor);
  pLXeVisAtt->SetVisibility(true);
  m_pLXeLogicalVolume->SetVisAttributes(pLXeVisAtt);

  XeSimLXeSensitiveDetector *pLXeSD = new XeSimLXeSensitiveDetector("template/LXeSD");
  pSDManager->AddNewDetector(pLXeSD);
  m_pLXeLogicalVolume->SetSensitiveDetector(pLXeSD); */
  


  //-------------------------------- Water Container Volume ---------------------------------
  G4Tubs* pWaterContainer = new G4Tubs("WaterContainer",
                                  0,
                                  dWaterContainerRadius,
                                  dWaterContainerHalfZ,
                                  0,
                                  360*deg);
  m_pWaterContainerLogicalVolume = new G4LogicalVolume(pWaterContainer,
                                                      StainlessSteel,
                                                      "WaterContainerLogicalVolume");
  G4ThreeVector WaterContainerPosition(0., dWaterContainerRadius + dBuildHalfY + dWaterContainerDistance, -dHallHalfZ+dWaterContainerHalfZ + dSpawnHalfThick );
  m_pWaterContainerPhysicalVolume = new G4PVPlacement(0, WaterContainerPosition, m_pWaterContainerLogicalVolume,
                                              "WaterContainer", m_pAirLogicalVolume, false, 0);

  m_pWaterContainerLogicalVolume->SetVisAttributes(pBuildVisAtt);

  // Water
  G4Tubs* pWater = new G4Tubs("Water",
                                  0,
                                  dWaterContainerRadius - 2*dWaterContainerThick,
                                  dWaterContainerHalfZ - 2*dWaterContainerThick,
                                  0,
                                  360*deg);
  m_pWaterLogicalVolume = new G4LogicalVolume(pWater,
                                              Water,
                                              "WaterContainerLogicalVolume");

  G4ThreeVector WaterPosition(0., 0., 0);
  m_pWaterPhysicalVolume = new G4PVPlacement(0, WaterPosition, m_pWaterLogicalVolume,
                                              "Water", m_pWaterContainerLogicalVolume, false, 0);


  G4Colour hWaterColor(0.0,1.,1.0,0.5);
  G4VisAttributes *pWaterVisAtt = new G4VisAttributes(hWaterColor);
  pWaterVisAtt->SetVisibility(true);
  m_pWaterLogicalVolume->SetVisAttributes(pWaterVisAtt);
                                                         

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
}