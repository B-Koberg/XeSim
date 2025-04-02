#ifndef __XeSimAanalysisManager_H__
#define __XeSimAanalysisManager_H__

#include <globals.hh>
#include <TParameter.h>

#include <G4ThreeVector.hh>
#include <G4Timer.hh>

class G4Run;
class G4Event;
class G4Step;

class TFile;
class TTree;

class XeSimEventData;
class XeSimPrimaryGeneratorAction;
class XeSimAnalysisMessenger;

class XeSimAnalysisManager {
public:
  XeSimAnalysisManager(XeSimPrimaryGeneratorAction *pPrimaryGeneratorAction);
  virtual ~XeSimAnalysisManager();

public:
  virtual void BeginOfRun(const G4Run *pRun); 
  virtual void EndOfRun(const G4Run *pRun); 
  virtual void BeginOfEvent(const G4Event *pEvent); 
  virtual void EndOfEvent(const G4Event *pEvent); 
  virtual void Step(const G4Step *pStep);	

  void SetDataFilename(const G4String &hFilename) { m_hDataFilename = hFilename; }
  void SetNbEventsToSimulate(G4int iNbEventsToSimulate) { m_iNbEventsToSimulate = iNbEventsToSimulate; }
  void SetNbPhotoDets(G4int iNbPhotoDets) { m_iNbPhotoDets = iNbPhotoDets; }
  G4int GetNbEventsToSimulate() { return m_iNbEventsToSimulate; }
  void SetVersionTag(const G4String &hVersionTag) { m_hVersionTag = hVersionTag; }
  void SetExperimentTag(const G4String &hExperimentTag) { m_hExperimentTag = hExperimentTag; }

  void SetPhotoDetHitsDetails(G4bool details) { m_PhotoDetHitsDetails = details; }

  void AddMacroFile(const G4String &hMacroFile) { m_hMacroFiles.push_back(hMacroFile); }

  void SetRecordOnlyEventID(G4bool mRecordOnlyEventID) { m_pRecordOnlyEventID = mRecordOnlyEventID; }
  void SetRecordOnlyActiveVolume(G4bool mRecordOnlyActiveVolume) { m_pRecordOnlyActiveVolume = mRecordOnlyActiveVolume; }
  void SetNeutronActivation(G4bool mStoreNeutronActivation) { m_pNeutronActivation = mStoreNeutronActivation; }

  void FillParticleInSave(G4int flag, G4int partPDGcode, G4ThreeVector pos,
                          G4ThreeVector dir, G4float nrg, G4float time,
                          G4int trackID);
  void FillNeutronCaptureInSave(G4String name, G4String process,
                                G4int atomic_mass, G4int atomic_number,
                                G4ThreeVector pos, G4String volume,
                                G4int event_number, G4float time);

private:
    G4Timer *runTime;

    XeSimAnalysisMessenger *m_pAnalysisMessenger;

	G4int m_iLXeHitsCollectionID;
	G4int m_iPhotoDetHitsCollectionID;

	G4String m_hDataFilename;
    G4String m_hVersionTag;
    G4String m_hExperimentTag;
	G4int m_iNbEventsToSimulate;
    G4int m_iNbPhotoDets;

    std::vector<G4String> m_hMacroFiles;

	TFile *m_pTreeFile;
	TTree *m_pTree;
    TDirectory *_events;
    TDirectory *_macros;
	
	TParameter<int> *m_pNbEventsToSimulateParameter;

	XeSimPrimaryGeneratorAction *m_pPrimaryGeneratorAction;

	XeSimEventData *m_pEventData;
	
    G4bool writeEmptyEvents;
    G4bool m_PhotoDetHitsDetails;

    G4bool m_pNeutronActivation;
    G4bool m_pRecordOnlyEventID;
    G4bool m_pRecordOnlyActiveVolume;
};

#endif

