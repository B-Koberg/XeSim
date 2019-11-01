#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/time.h>

#include "XeSimActionInitialization.hh"
#include "XeSimPrimaryGeneratorAction.hh"
#include "XeSimAnalysisManager.hh"
#include "XeSimStackingAction.hh"
#include "XeSimRunAction.hh"
#include "XeSimEventAction.hh"

XeSimActionInitialization::XeSimActionInitialization (int NbEventsToSimulate, int NbPhotoDets,
std::string NewDatafileName, std::string ExperimentTag, XeSimPrimaryGeneratorAction *pNewPrimaryGeneratorAction) {	
	pPrimaryGeneratorAction = pNewPrimaryGeneratorAction;
  
	pAnalysisManager = new XeSimAnalysisManager(pPrimaryGeneratorAction);
    pAnalysisManager->SetNbEventsToSimulate(NbEventsToSimulate);
	pAnalysisManager->SetDataFilename(NewDatafileName);
    pAnalysisManager->SetExperimentTag(ExperimentTag);
    pAnalysisManager->SetNbPhotoDets(NbPhotoDets);
}

XeSimActionInitialization::~XeSimActionInitialization() { }

void XeSimActionInitialization::BuildForMaster() const {
  SetUserAction(new XeSimRunAction(pAnalysisManager));
}

void XeSimActionInitialization::Build() const {
	SetUserAction(pPrimaryGeneratorAction);
	SetUserAction(new XeSimStackingAction(pAnalysisManager));
	SetUserAction(new XeSimRunAction(pAnalysisManager));
	SetUserAction(new XeSimEventAction(pAnalysisManager));
}

