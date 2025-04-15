#ifndef __XeSimEventAction_H__
#define __XeSimEventAction_H__

#include <G4UserEventAction.hh>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/time.h>
#include "XeSimAnalysisManager.hh"

class G4Event;

class XeSimEventAction : public G4UserEventAction {
public:
	XeSimEventAction(XeSimAnalysisManager *pAnalysisManager = 0);
	~XeSimEventAction();

public:
	void BeginOfEventAction(const G4Event *pEvent);
	void EndOfEventAction(const G4Event *pEvent);

private:
	XeSimAnalysisManager *m_pAnalysisManager;
	time_t time_un;
	time_t starttimeunix, currtimeunix, endtimeunix;
	tm *time_now, *time_end;
	char endtime[80];
	std::stringstream starttime, currtime;
	G4int m_iNbEventsToSimulate;
};

#endif

