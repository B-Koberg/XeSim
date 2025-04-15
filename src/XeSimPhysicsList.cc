#include "XeSimPhysicsList.hh"
#include "XeSimPhysicsMessenger.hh"

#include <globals.hh>
#include <iomanip>
#include <vector>

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include <G4DecayPhysics.hh>
#include <G4NuclideTable.hh>
#include <G4EmCalculator.hh>
#include <G4EmExtraPhysics.hh>
#include <G4EmPenelopePhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4HadronicProcessStore.hh>
#include <G4NistManager.hh>
#include <G4OpticalPhysics.hh>
#include <G4StoppingPhysics.hh>
#include <G4ios.hh>

#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronInelasticQBBC.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronPhysicsINCLXX.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4IonElasticPhysics.hh"
#include "G4IonINCLXXPhysics.hh"
#include "G4IonPhysics.hh"

#include "G4EmLivermorePhysics.hh"
#include "G4PhysListFactory.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4VModularPhysicsList.hh"

XeSimPhysicsList::XeSimPhysicsList(): G4VModularPhysicsList() {
  VerboseLevel = 0;
  SetVerboseLevel(VerboseLevel);

  defaultCutValue = 1.0 * mm;
  cutForGamma = defaultCutValue;
  cutForElectron = defaultCutValue;
  cutForPositron = defaultCutValue;
  // cutForPotron   = defaultCutValue;

  m_pMessenger = new XeSimPhysicsMessenger(this);

  RegisterPhysics(OpticalPhysicsModel = new G4OpticalPhysics(VerboseLevel));
  //((G4OpticalPhysics *)OpticalPhysicsModel)->SetMaxNumPhotonsPerStep(1000);
  //((G4OpticalPhysics *)OpticalPhysicsModel)->SetMaxBetaChangePerStep(10);
  //((G4OpticalPhysics *)OpticalPhysicsModel)->SetTrackSecondariesFirst(kCerenkov, true);
  //((G4OpticalPhysics *)OpticalPhysicsModel)->Configure(kCerenkov, false);

  // Hadron Elastic scattering
  RegisterPhysics(new G4HadronElasticPhysicsHP(VerboseLevel));

  // Hadron Inelastic Physics
  RegisterPhysics(new G4HadronPhysicsQGSP_BERT(VerboseLevel));
  // RegisterPhysics( new G4HadronPhysicsShielding(VerboseLevel) );
  // RegisterPhysics( new G4HadronStoppingProcess(VerboseLevel) );

  // Ion Elastic scattering
  RegisterPhysics(new G4IonElasticPhysics(VerboseLevel));

  // Ion Inelastic Physics
  RegisterPhysics(new G4IonPhysics(VerboseLevel));
  ////RegisterPhysics( new G4IonINCLXXPhysics(VerboseLevel));

  // Gamma-Nuclear Physics - see Hadr03
  // RegisterPhysics( new GammaPhysics("gamma") );

  // EM physics
  RegisterPhysics(new G4EmStandardPhysics(VerboseLevel, ""));

  /// G4EmExtraPhysics
  RegisterPhysics(new G4EmExtraPhysics(VerboseLevel));

  // Decay
  RegisterPhysics(new G4DecayPhysics(VerboseLevel));

  // Radioactive decay
  RegisterPhysics(new G4RadioactiveDecayPhysics(VerboseLevel));
  G4NuclideTable::GetInstance()->SetThresholdOfHalfLife(1*nanosecond);
  G4NuclideTable::GetInstance()->SetLevelTolerance(1.0*eV);
  
  /// G4StoppingPhysics
  RegisterPhysics(new G4StoppingPhysics(VerboseLevel));
}

void XeSimPhysicsList::SetCerenkov(G4bool useCerenkov) {
  //((G4OpticalPhysics *)OpticalPhysicsModel)->Configure(kCerenkov, useCerenkov);
  //  G4cout << "XeSimPhysicsList::SetCerenkov(): " << useCerenkov << G4endl;  
  G4cout << "XeSimPhysicsList::SetCerenkov(): Not configured!" << G4endl;
}

void XeSimPhysicsList::SetEMlowEnergyModel(G4String name) {
    if (name == "emstandard") {
    ReplacePhysics(new G4EmStandardPhysics(VerboseLevel, ""));
  } else if (name == "emlivermore") {
    ReplacePhysics(new G4EmLivermorePhysics(VerboseLevel, ""));
  } else if (name == "empenelope") {
    ReplacePhysics(new G4EmPenelopePhysics(VerboseLevel, ""));
  } else {
    G4cout << "XeSimPhysicsList::XeSimPhysicsList() FATAL: Bad EM physics "
              "list chosen: " << name << G4endl;
    G4String msg = " Available choices are: <emstandard> <emlivermore "
                   "(default)> <empenelope>";
    G4Exception("XeSimPhysicsList::XeSimPhysicsList()", "PhysicsList",
                FatalException, msg);
  }
  G4cout << "XeSimPhysicsList::SetEMlowEnergyModel(): " << name << G4endl;  
}

void XeSimPhysicsList::SetHadronicModel(G4String name) {
  // Hadron Inelastic Physics
  if (name == "QGSP_BIC_HP") {
    ReplacePhysics(new G4HadronPhysicsQGSP_BIC_HP(VerboseLevel));
  } else if (name == "FTFP_BERT_HP") {
    ReplacePhysics(new G4HadronPhysicsFTFP_BERT_HP(VerboseLevel));
  } else if (name == "QBBC") {
    ReplacePhysics(new G4HadronInelasticQBBC(VerboseLevel));
  } else if (name == "INCLXX") {
    ReplacePhysics(new G4HadronPhysicsINCLXX(VerboseLevel));
  } else if (name == "QGSP_BERT_HP") {
    ReplacePhysics(new G4HadronPhysicsQGSP_BERT_HP(VerboseLevel));
  } else if (name == "QGSP_BERT") {
    ReplacePhysics(new G4HadronPhysicsQGSP_BERT(VerboseLevel));
  } else {
    G4String msg =
        "XeSimPhysicsList::XeSimPhysicsList() Available choices "
        "for Hadronic Physics are: <QGSP_BIC_HP> <QGSP_BERT_HP>";
    G4Exception("XeSimPhysicsList::XeSimPhysicsList()", "PhysicsList",
                FatalException, msg);
  }
  // ReplacePhysics( new G4HadronPhysicsShielding(VerboseLevel) );
  // ReplacePhysics( new G4HadronStoppingProcess(VerboseLevel) );
  G4cout << "XeSimPhysicsList::SetHadronicModel(): " << name << G4endl;  
} 
 
XeSimPhysicsList::~XeSimPhysicsList() {
  delete m_pMessenger;
}

void XeSimPhysicsList::SetCuts() {
  if (VerboseLevel > 1) G4cout << "XeSimPhysicsList::SetCuts:";

  if (VerboseLevel > 0) {
    G4cout << "XeSimPhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue, "Length") << G4endl;
  }

  // special for low energy physics
  G4double lowlimit = 250 * eV;

  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowlimit,
                                                                  100. * GeV);

  SetCutValue(1 * um, "proton");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");
  SetCutValue(cutForGamma, "gamma");
}