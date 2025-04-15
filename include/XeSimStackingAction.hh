#ifndef __XeSimStackingAction_H__
#define __XeSimStackingAction_H__

#include <globals.hh>
#include <G4UserStackingAction.hh>
#include <XeSimStackingActionMessenger.hh>

class XeSimAnalysisManager;
class XeSimStackingActionMessenger;

class XeSimStackingAction: public G4UserStackingAction {
public:
	XeSimStackingAction(XeSimAnalysisManager *pAnalysisManager=0);
	~XeSimStackingAction();
  
	virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
	virtual void NewStage();
	virtual void PrepareNewEvent();
	XeSimStackingActionMessenger *theMessenger;

	inline void SetPostponeFlag(G4bool val) { PostponeFlag = val; };
	inline void SetMaxLifeTime(G4double val) { MaxLifeTime = val; };
	inline void SetKillPostponedNucleus(G4String val) { KillPostponedNucleusName = val; };
	inline void SetKillGeneratedParticles(std::vector<G4String> val) { KillGeneratedParticleNames = val; };

private:
	XeSimAnalysisManager *m_pAnalysisManager;
	G4bool PostponeFlag;
	G4double MaxLifeTime;
	G4String KillPostponedNucleusName = "None";
	std::vector<G4String> KillGeneratedParticleNames;

};

#endif