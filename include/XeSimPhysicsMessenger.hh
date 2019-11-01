#ifndef __XeSimPhysicsMessenger_H__
#define __XeSimPhysicsMessenger_H__

#include <G4UImessenger.hh>
#include <globals.hh>

class XeSimPhysicsList;

class G4UIcommand;
class G4UIdirectory;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithABool;
class G4UIcmdWithoutParameter;

class XeSimPhysicsMessenger: public G4UImessenger
{
public:
  XeSimPhysicsMessenger(XeSimPhysicsList *pPhysicsList);
  ~XeSimPhysicsMessenger();
  
  void SetNewValue(G4UIcommand *pCommand, G4String hNewValues);
private:
  XeSimPhysicsList     *m_pPhysicsList;
  G4UIdirectory              *m_pDirectory;
  G4UIcmdWithAString         *m_pEMlowEnergyModelCmd;
  G4UIcmdWithAString         *m_pHadronicModelCmd;
  G4UIcmdWithABool           *m_pCerenkovCmd;
};

#endif

