#ifndef __XeSimPhysicsList_H__
#define __XeSimPhysicsList_H__ 1

#include "XeSimPhysicsMessenger.hh"

#include <globals.hh>
#include <vector>
using namespace std;

#include "G4VModularPhysicsList.hh"

class XeSimPhysicsList : public G4VModularPhysicsList {
 public:
  XeSimPhysicsList();
  virtual ~XeSimPhysicsList();

 public:
  void SetCuts();

  // functions for the messenger interface
  void SetCerenkov(G4bool useCerenkov);
  void SetEMlowEnergyModel(G4String theModel);
  void SetHadronicModel(G4String theModel);

 private:
  G4VPhysicsConstructor *OpticalPhysicsModel;

  G4double defaultCutValue;
  G4double cutForGamma;
  G4double cutForElectron;
  G4double cutForPositron;
  G4double cutForProton;
  G4double cutForAlpha;
  G4double cutForGenericIon;

  G4int VerboseLevel;

  XeSimPhysicsMessenger *m_pMessenger;
  G4String m_hEMlowEnergyModel;
  G4String m_hHadronicModel;
  G4bool m_bCerenkov;
};
#endif
