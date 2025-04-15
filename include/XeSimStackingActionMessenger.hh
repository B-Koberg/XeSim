#ifndef __XeSimStackingActionMessenger_H__
#define __XeSimStackingActionMessenger_H__

#include <globals.hh>
#include "G4UImessenger.hh"
#include "XeSimStackingAction.hh"

// class XeSimStackingAction* XeSimAction;
class XeSimStackingAction;
class G4UIcmdWithABool;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithAString;

class XeSimStackingActionMessenger : public G4UImessenger
{
public:
  XeSimStackingActionMessenger(XeSimStackingAction* pStackAction);
  ~XeSimStackingActionMessenger();

public:
  void SetNewValue(G4UIcommand*, G4String);

private:
  XeSimStackingAction* XeSimAction;

private:
  G4UIcmdWithABool* PostponeCmd;
  G4UIcmdWithADoubleAndUnit* MaxLifeTimeCmd;
  G4UIcmdWithAString* KillPostponedNucleusCmd;
  G4UIcmdWithAString* KillGeneratedParticleCmd;
};

#endif  // __XeSimStackingActionMessenger_H__
