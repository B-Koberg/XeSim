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

  // Store interactions resulting from neutron capture throughout the detector,
  // not only sensitive volumes.
  m_pActivationCmd = new G4UIcmdWithABool("/analysis/settings/setRecordNeutronCapture", this);
  m_pActivationCmd->SetGuidance("Choose whether or not to store neutron capture information");
  m_pActivationCmd->SetParameterName("RecordNeutronCapture", false);
  m_pActivationCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  // Do no record primary information or hit collections. Just eventid and special saves.
  m_pRecordOnlyEventIDCmd = new G4UIcmdWithABool("/analysis/settings/setRecordOnlyEventID", this);
  m_pRecordOnlyEventIDCmd->SetGuidance("Choose whether or not to record primary and sensitive volume information.");
  m_pRecordOnlyEventIDCmd->SetParameterName("RecordOnlyEventID", false);
  m_pRecordOnlyEventIDCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  // Simple cylindrical selection to store ONLY the interaction near to the sensitive volume.
  // This acts on the LXeHits collection and LXe sensitive volume.
  // You need to define the volume shape and dimensions in the detector construction.
  // Can be used to study only a specific region of the detector while still producing events everywhere.
  m_pRecordOnlyActiveVolumeCmd = new G4UIcmdWithABool("/analysis/settings/setRecordOnlyActiveVolume", this);
  m_pRecordOnlyActiveVolumeCmd->SetGuidance("Choose whether or not to record neutron capture information ONLY");
  m_pRecordOnlyActiveVolumeCmd->SetParameterName("RecordOnlyActiveVolume", false);
  m_pRecordOnlyActiveVolumeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

XeSimAnalysisMessenger::~XeSimAnalysisMessenger() {
  delete m_pDirectory;
  delete m_pPhotoDetHitsDetailsCmd;
  delete m_pActivationCmd;
  delete m_pRecordOnlyEventIDCmd;
  delete m_pRecordOnlyActiveVolumeCmd;
}

void XeSimAnalysisMessenger::SetNewValue(G4UIcommand *command, G4String newValues) {
  if (command == m_pPhotoDetHitsDetailsCmd) {
    m_pAnalysis->SetPhotoDetHitsDetails(
        m_pPhotoDetHitsDetailsCmd->GetNewBoolValue(newValues));
  }
  if (command == m_pActivationCmd) {
    m_pAnalysis->SetNeutronActivation(m_pActivationCmd->GetNewBoolValue(newValues));
  }
  if (command == m_pRecordOnlyEventIDCmd) {
    m_pAnalysis->SetRecordOnlyEventID(m_pRecordOnlyEventIDCmd->GetNewBoolValue(newValues));
  }
  if (command == m_pRecordOnlyActiveVolumeCmd) {
    m_pAnalysis->SetRecordOnlyActiveVolume(m_pRecordOnlyActiveVolumeCmd->GetNewBoolValue(newValues));
  }
}
