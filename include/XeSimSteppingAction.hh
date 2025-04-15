#ifndef __XeSimSteppingAction_H__
#define __XeSimSteppingAction_H__

#include "globals.hh"
#include "G4UserSteppingAction.hh"

class XeSimSteppingAction: public G4UserSteppingAction {
public:
  XeSimSteppingAction(XeSimAnalysisManager *pAnalysisManager=0);
  ~XeSimSteppingAction();

  void UserSteppingAction(const G4Step *);

private:
  G4String particle;
  XeSimAnalysisManager *m_pAnalysisManager;
};

#endif
