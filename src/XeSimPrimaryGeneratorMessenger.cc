#include <G4Geantino.hh>
#include <G4ThreeVector.hh>
#include <G4ParticleTable.hh>
#include <G4UIdirectory.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWith3Vector.hh>
#include <G4UIcmdWith3VectorAndUnit.hh>
#include <G4UIcmdWithAnInteger.hh>
#include <G4UIcmdWithADouble.hh>
#include <G4UIcmdWithABool.hh>
#include <G4Tokenizer.hh>
#include <G4ios.hh>
#include <fstream>
#include <iomanip>

#include "XeSimPrimaryGeneratorMessenger.hh"
#include "XeSimPrimaryGeneratorAction.hh"

XeSimPrimaryGeneratorMessenger::XeSimPrimaryGeneratorMessenger(XeSimPrimaryGeneratorAction *pPrimaryGeneratorAction):
  m_pPrimaryGeneratorAction(pPrimaryGeneratorAction) {
  // write empty events to file or not  
  m_pWriteEmptyCmd = new G4UIcmdWithABool("/run/writeEmpty", this);
  m_pWriteEmptyCmd->SetGuidance("Write empty events to the root tree true/false");
  m_pWriteEmptyCmd->SetDefaultValue(false);
  //m_pWriteEmptyCmd->AvailableForStates(G4State_PreInit);
}

XeSimPrimaryGeneratorMessenger::~XeSimPrimaryGeneratorMessenger() {
  delete m_pDirectory;
}

void
XeSimPrimaryGeneratorMessenger::SetNewValue(G4UIcommand * command, G4String newValues) {
  if(command == m_pWriteEmptyCmd) 
    m_pPrimaryGeneratorAction->SetWriteEmpty(m_pWriteEmptyCmd->GetNewBoolValue(newValues));
}

