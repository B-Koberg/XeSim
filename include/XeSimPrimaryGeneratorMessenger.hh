#ifndef __XeSimPrimaryGeneratorMessenger_H__
#define __XeSimPrimaryGeneratorMessenger_H__

#include <G4UImessenger.hh>
#include <globals.hh>

class XeSimPrimaryGeneratorAction;

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

class XeSimPrimaryGeneratorMessenger: public G4UImessenger
{
public:
  XeSimPrimaryGeneratorMessenger(XeSimPrimaryGeneratorAction *pPrimaryGeneratorAction);
  ~XeSimPrimaryGeneratorMessenger();
  
  void SetNewValue(G4UIcommand *pCommand, G4String hNewValues);

private:
  XeSimPrimaryGeneratorAction   *m_pPrimaryGeneratorAction;
  G4UIdirectory                 *m_pDirectory;
  G4UIcmdWithABool              *m_pWriteEmptyCmd;

};

#endif 

