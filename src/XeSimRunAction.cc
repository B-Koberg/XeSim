#include <G4SDManager.hh>

#include <Randomize.hh>
#include <sys/time.h>

#include "XeSimAnalysisManager.hh"
#include "XeSimRunAction.hh"

XeSimRunAction::XeSimRunAction(XeSimAnalysisManager *pAnalysisManager) {
	m_pAnalysisManager = pAnalysisManager;
}

XeSimRunAction::~XeSimRunAction() {
}

void XeSimRunAction::BeginOfRunAction(const G4Run *pRun) {
	if (( ! G4Threading::IsMultithreadedApplication() ) ||
			( G4Threading::IsMultithreadedApplication() && ! G4Threading::IsWorkerThread() )) {
		if(m_pAnalysisManager)
			m_pAnalysisManager->BeginOfRun(pRun);

		struct timeval hTimeValue;
		gettimeofday(&hTimeValue, NULL);
		
		CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
		CLHEP::HepRandom::setTheSeed(hTimeValue.tv_usec);
	}
}

void XeSimRunAction::EndOfRunAction(const G4Run *pRun) {
	if (( ! G4Threading::IsMultithreadedApplication() ) ||
				( G4Threading::IsMultithreadedApplication() && ! G4Threading::IsWorkerThread() )) {
		if(m_pAnalysisManager)
			m_pAnalysisManager->EndOfRun(pRun);
	}
}