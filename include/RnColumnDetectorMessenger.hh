#ifndef RnColumnDetectorMessenger_h
#define RnColumnDetectorMessenger_h 1

#include <G4UImessenger.hh>
#include <globals.hh>

class RnColumnDetectorConstruction;

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

class RnColumnDetectorMessenger: public G4UImessenger {
public:
	RnColumnDetectorMessenger(RnColumnDetectorConstruction *pXeDetector);
	~RnColumnDetectorMessenger();

	void SetNewValue(G4UIcommand *pUIcommand, G4String hString);

private:
	RnColumnDetectorConstruction* m_pXeDetector;

	G4UIdirectory *m_pDetectorDir;

	G4UIcmdWithAnInteger *m_pNbTopPmtsCmd;
	G4UIcmdWithAnInteger *m_pNbBottomPmtsCmd;
	G4UIcmdWithAString *m_pSeparationPlateMaterialCmd;
	G4UIcmdWithADoubleAndUnit *m_pLXeLevelCmd;
	G4UIcmdWithADouble *m_pSS304LSteelReflectivityCmd;
	G4UIcmdWithADouble *m_pTeflonReflectivityCmd;
	G4UIcmdWithADouble *m_pGXeTeflonReflectivityCmd;
	G4UIcmdWithADoubleAndUnit *m_pLXeAbsorbtionLengthCmd;
	G4UIcmdWithADoubleAndUnit *m_pGXeAbsorbtionLengthCmd;
	G4UIcmdWithADoubleAndUnit *m_pLXeRayScatterLengthCmd;
	G4UIcmdWithADouble *m_pLXeRefractionIndexCmd;

};
#endif

