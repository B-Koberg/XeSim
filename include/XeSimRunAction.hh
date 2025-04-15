#ifndef __XeSimRunAction_H__
#define __XeSimRunAction_H__

#include <G4UserRunAction.hh>

class G4Run;

class XeSimAnalysisManager;

class XeSimRunAction: public G4UserRunAction {
public:
	XeSimRunAction(XeSimAnalysisManager *pAnalysisManager=0);
	~XeSimRunAction();

public:
	void BeginOfRunAction(const G4Run *pRun);
	void EndOfRunAction(const G4Run *pRun);

private:
	XeSimAnalysisManager *m_pAnalysisManager;
};

#endif