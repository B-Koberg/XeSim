#include "XeSimStackingActionMessenger.hh"
#include "XeSimStackingAction.hh"

#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcommand.hh"
#include "globals.hh"
#include <sstream>

XeSimStackingActionMessenger::XeSimStackingActionMessenger(
  XeSimStackingAction* msa)
  : XeSimAction(msa)
{
  PostponeCmd = new G4UIcmdWithABool("/xe/Postponedecay", this);
  PostponeCmd->SetGuidance("Postpone radioactive decays");
  PostponeCmd->SetGuidance("Default = false");
  PostponeCmd->SetParameterName("PostponeFlag", false);
  PostponeCmd->SetDefaultValue(false);

  MaxLifeTimeCmd = new G4UIcmdWithADoubleAndUnit("/xe/MaxLifeTime", this);
  MaxLifeTimeCmd->SetGuidance("Define max lifetime for daughter isotopes to be kept in one event");
  MaxLifeTimeCmd->SetGuidance("Default = 1 ns");
  MaxLifeTimeCmd->SetParameterName("MaxLifeTime", false);
  MaxLifeTimeCmd->SetDefaultValue(1.);
  MaxLifeTimeCmd->SetUnitCategory("Time");
  MaxLifeTimeCmd->SetDefaultUnit("ns");

  KillPostponedNucleusCmd = new G4UIcmdWithAString("/xe/KillPostponedNucleusName", this);
  KillPostponedNucleusCmd->SetGuidance("Kill the track with the specified nucleus name if it was postponed "
                                       "(to stop decay chains at a given step)");
  KillPostponedNucleusCmd->SetParameterName("KillPostNuc", false);
  KillPostponedNucleusCmd->SetDefaultValue("None");
  KillPostponedNucleusCmd->AvailableForStates(G4State_Idle);
}

XeSimStackingActionMessenger::~XeSimStackingActionMessenger()
{
  delete PostponeCmd;
  delete MaxLifeTimeCmd;
  delete KillPostponedNucleusCmd;
}

void XeSimStackingActionMessenger::SetNewValue(G4UIcommand* command,
    G4String newValue)
{
  if (command == PostponeCmd)
    XeSimAction->SetPostponeFlag(PostponeCmd->GetNewBoolValue(newValue));

  if (command == MaxLifeTimeCmd)
    XeSimAction->SetMaxLifeTime(MaxLifeTimeCmd->GetNewDoubleValue(newValue));

  if (command == KillPostponedNucleusCmd)
    XeSimAction->SetKillPostponedNucleus(newValue);
}
