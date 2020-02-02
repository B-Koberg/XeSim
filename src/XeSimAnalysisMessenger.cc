#include <G4Geantino.hh>
#include <G4ParticleTable.hh>
#include <G4ThreeVector.hh>
#include <G4Tokenizer.hh>
#include <G4UIcmdWith3Vector.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIdirectory.hh>
#include <G4ios.hh>
#include <fstream>
#include <iomanip>

#include "XeSimAnalysisManager.hh"
#include "XeSimAnalysisMessenger.hh"

XeSimAnalysisMessenger::XeSimAnalysisMessenger(XeSimAnalysisManager *pAnalysis): m_pAnalysis(pAnalysis) {
  // create directory
  m_pDirectory = new G4UIdirectory("/analysis/settings/");
  m_pDirectory->SetGuidance("Analysis control commands.");

  // save detailed infos for PMT hits
  m_pPhotoDetHitsDetailsCmd =
      new G4UIcmdWithABool("/analysis/settings/setPhotoDetdetails", this);
  m_pPhotoDetHitsDetailsCmd->SetGuidance(
      "Choose to register or not PhotoDetID of PhotoDetHits for optical photons");
  m_pPhotoDetHitsDetailsCmd->SetParameterName("PhotoDetHitsDetails", false);
  m_pPhotoDetHitsDetailsCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

XeSimAnalysisMessenger::~XeSimAnalysisMessenger() {
  delete m_pDirectory;
  delete m_pPhotoDetHitsDetailsCmd;
}

void XeSimAnalysisMessenger::SetNewValue(G4UIcommand *command, G4String newValues) {
  if (command == m_pPhotoDetHitsDetailsCmd) {
    m_pAnalysis->SetPhotoDetHitsDetails(
        m_pPhotoDetHitsDetailsCmd->GetNewBoolValue(newValues));
  }
}
