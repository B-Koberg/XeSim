#ifndef __XeSimANALYSISMESSENGER_H__
#define __XeSimANALYSISMESSENGER_H__

#include <G4UImessenger.hh>
#include <globals.hh>

class XeSimAnalysisManager;

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

class XeSimAnalysisMessenger : public G4UImessenger {
 public:
  XeSimAnalysisMessenger(XeSimAnalysisManager *pAnalysis);
  ~XeSimAnalysisMessenger();

  void SetNewValue(G4UIcommand *pCommand, G4String hNewValues);

 private:
  XeSimAnalysisManager *m_pAnalysis;
  G4UIdirectory *m_pDirectory;
  G4UIcmdWithABool *m_pPhotoDetHitsDetailsCmd;

  G4UIcmdWithABool *m_pActivationCmd;
  G4UIcmdWithABool *m_pRecordOnlyEventIDCmd;
  G4UIcmdWithABool *m_pRecordOnlyActiveVolumeCmd;
};

#endif
