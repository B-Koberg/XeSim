#ifndef __XeSimStackingAction_H__
#define __XeSimStackingAction_H__

#include <globals.hh>
#include <G4UserStackingAction.hh>

class XeSimAnalysisManager;

class XeSimStackingAction: public G4UserStackingAction {
public:
	XeSimStackingAction(XeSimAnalysisManager *pAnalysisManager=0);
	~XeSimStackingAction();
  
	virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
	virtual void NewStage();
	virtual void PrepareNewEvent();

private:
	XeSimAnalysisManager *m_pAnalysisManager;
};

#endif