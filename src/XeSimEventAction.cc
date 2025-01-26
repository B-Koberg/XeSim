#include <G4Event.hh>
#include <string>
#include <stdio.h>
#include <sstream>
#include <unistd.h>
#include <sys/time.h>
#include "XeSimEventAction.hh"

XeSimEventAction::XeSimEventAction(XeSimAnalysisManager *pAnalysisManager) {
	m_pAnalysisManager = pAnalysisManager;
	time_un = time(0);
	time_now = localtime(&time_un);
}

XeSimEventAction::~XeSimEventAction() { }

void XeSimEventAction::BeginOfEventAction(const G4Event *pEvent) {
	if(pEvent->GetEventID() == 0) {
		m_iNbEventsToSimulate = m_pAnalysisManager->GetNbEventsToSimulate();
		starttime.str(std::string());
		starttime << time_now->tm_year+1900 << "-" << time_now->tm_mon+1 
		     << "-" << time_now->tm_mday << " " << time_now->tm_hour
		     << "-" << time_now->tm_min << "-" << time_now->tm_sec; 
		starttimeunix = mktime ( time_now ); // for UTC time use "timegm"
	}
	
	if ( (pEvent->GetEventID() % 10000 == 0) || (pEvent->GetEventID() % (m_iNbEventsToSimulate/5) == 0) ) {
		time_un = time(0);
		time_now = localtime(&time_un);
		currtime.str(std::string());
		currtime << time_now->tm_year+1900 << "-" << time_now->tm_mon+1 
		     << "-" << time_now->tm_mday << " " << time_now->tm_hour
		     << "-" << time_now->tm_min << "-" << time_now->tm_sec;
		currtimeunix = time_un - starttimeunix;

		if ( (pEvent->GetEventID() > 0) && ((pEvent->GetEventID()/currtimeunix) > 0) ) {
			endtimeunix = starttimeunix + m_iNbEventsToSimulate / (pEvent->GetEventID()/currtimeunix);
			time_end = localtime(&endtimeunix);
			strftime(endtime, sizeof(endtime), "%Y-%m-%d %H-%M-%S", time_end);
			G4cout 	<< "\r" << currtime.str() << " || Event " << pEvent->GetEventID()
							<< " / " << m_iNbEventsToSimulate << " (" << (pEvent->GetEventID()*100)/m_iNbEventsToSimulate << "%)"
							<< " || E/s " << pEvent->GetEventID()/currtimeunix 
							<< " || ETA: " << endtime << " " << std::flush;
		}
		else if (pEvent->GetEventID() == 0) {
			G4cout << currtime.str() << " || Start of first event || " << currtimeunix << "s initialization" << G4endl;
		}
	}

	if(m_pAnalysisManager) m_pAnalysisManager->BeginOfEvent(pEvent);
}

void XeSimEventAction::EndOfEventAction(const G4Event *pEvent) {
	if(pEvent->GetEventID() == ( m_iNbEventsToSimulate - 1 ) )
	{
		time_un = time(0);
		time_now = localtime(&time_un);
		currtime.str(std::string());
		currtime << time_now->tm_year+1900 << "-" << time_now->tm_mon+1 
		     << "-" << time_now->tm_mday << " " << time_now->tm_hour
		     << "-" << time_now->tm_min << "-" << time_now->tm_sec;
		currtimeunix = time_un - starttimeunix;
		G4cout << G4endl << currtime.str() << " || End of last event || " << currtimeunix << "s running time" << G4endl;
	}	

	if(m_pAnalysisManager) m_pAnalysisManager->EndOfEvent(pEvent);
}


