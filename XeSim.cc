#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>

#include "G4RunManager.hh"
#include <G4UImanager.hh>
#include <G4UIterminal.hh>
#include <G4UItcsh.hh>
#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>

#include "muensterTPCDetectorConstruction.hh"

#include "XeSimPhysicsList.hh"
#include "XeSimPrimaryGeneratorAction.hh"
#include "XeSimAnalysisManager.hh"
#include "XeSimStackingAction.hh"
#include "XeSimRunAction.hh"
#include "XeSimEventAction.hh"
#include "XeSimActionInitialization.hh"

void usage();
inline bool fileexists (const std::string& name);
inline bool fileexists (const char* name);

int main(int argc, char **argv) {
    G4cout << G4endl << "Welcome to XeSim MC"<< G4endl;
         
	// switches
	int c = 0;
	
	bool bInteractive = false;
    bool bVisualize = false;
	bool bPreInitMacroFile = false;
    bool bDetectorMacroFile = false;
	bool bRunMacroFile = false;
	bool bDataFilename = false;
	int iNbEventsToSimulate = 0;
    std::string hCommand;
	std::string hPreInitMacroFilename, hDetectorMacroFilename, hRunMacroFilename;
    std::string hDataFilename, hVRMLFilename, hExperiment;
	std::stringstream hStream;
    
	if ( argc == 1 ) { bInteractive = true; }
	while((c = getopt(argc,argv,"p:f:o:n:v:c:d:i")) != -1) {
		switch(c)	{
			case 'p':
				bPreInitMacroFile = true;
				if (fileexists(optarg)) {
					hPreInitMacroFilename = optarg;
				} else {
					G4cout << "File " << optarg << " not found!" << G4endl; usage();
                }
				break;

			case 'f':
				bRunMacroFile = true;
				if (fileexists(optarg)) {
					hRunMacroFilename = optarg;
				} else {
					G4cout << "File " << optarg << " not found!" << G4endl; usage();
                }
				break;
                
            case 'c':
				bDetectorMacroFile = true;
				if (fileexists(optarg)) {
					hDetectorMacroFilename = optarg;
				} else {
					G4cout << "File " << optarg << " not found!" << G4endl; usage();
                }
				break;

			case 'o':
				bDataFilename = true;
				hDataFilename = optarg;
				break;

			case 'n':
				hStream.str(optarg);
				hStream.clear();
				hStream >> iNbEventsToSimulate;
				break;
				
			case 'v':
				bVisualize = true;
                hVRMLFilename = optarg;
				break;
				
			case 'i':
				bInteractive = true;
				break;

            case 'd':
                hExperiment = optarg;
                break;

			default:
				usage();
		}
	}
	
    if (hExperiment.empty()) {
        G4cout << "Experiment parameter not given!" << G4endl; usage();
    }
    
    if (hDataFilename.empty()) hDataFilename = "events.root";
    if (hVRMLFilename.empty()) hVRMLFilename = "XeSim.vrml";
    
    // create the run manager
    G4RunManager *pRunManager = new G4RunManager;    

    // trigger detector construction
    int iNbPhotoDets = 0;
    if (hExperiment == "muensterTPC") {
        muensterTPCDetectorConstruction *pmuensterTPCDetectorConstruction = new muensterTPCDetectorConstruction();
        iNbPhotoDets = pmuensterTPCDetectorConstruction->GetGeometryParameter("NbPhotoDets");
        pRunManager->SetUserInitialization(pmuensterTPCDetectorConstruction);
    }
	pRunManager->SetUserInitialization(new XeSimPhysicsList);

    if (iNbPhotoDets == 0) { 
        G4cout << "NbPhotoDets not defined in the DetectorConstruction!" << G4endl; usage();
    }
    
    // create the primary generator action
	XeSimPrimaryGeneratorAction *pPrimaryGeneratorAction = new XeSimPrimaryGeneratorAction();
	pRunManager->SetUserInitialization(new XeSimActionInitialization(iNbEventsToSimulate, iNbPhotoDets,
        hDataFilename, hExperiment, pPrimaryGeneratorAction));

    // Visualization Manager
    G4VisManager *pVisManager = new G4VisExecutive;
    pVisManager->SetVerboseLevel(0);
    pVisManager->Initialize();

    // geometry IO
    G4UImanager *pUImanager = G4UImanager::GetUIpointer();

    if(bPreInitMacroFile) {
		hCommand = "/control/execute " + hPreInitMacroFilename;
		pUImanager->ApplyCommand(hCommand);
	}
	
	pRunManager->Initialize();
    
	G4UIExecutive* ui = 0;
	if (bInteractive) {	
        ui = new G4UIExecutive(argc, argv, "Qt"); 
        //pUImanager->ApplyCommand("/control/execute macros/vis.mac");
    } else if (!bRunMacroFile) { usage(); }
	
    // detector ruintime setup (e.g. detector messenger functions)
	if(bDetectorMacroFile) {
		hCommand = "/control/execute " + hDetectorMacroFilename;
		pUImanager->ApplyCommand(hCommand);
	}

	// run event source setup
	if(bRunMacroFile) {
		hCommand = "/control/execute " + hRunMacroFilename;
		pUImanager->ApplyCommand(hCommand);
	}
		
    G4cout << "Loaded preinit macro: " << hPreInitMacroFilename << G4endl;
    G4cout << "Loaded detector setup: " << hDetectorMacroFilename << G4endl;
    G4cout << "Loaded run macro: " << hRunMacroFilename << G4endl;
        
	if(iNbEventsToSimulate)	{
		hStream.str("");
		hStream.clear();
		hStream << "/run/beamOn " << iNbEventsToSimulate;
		pUImanager->ApplyCommand(hStream.str());
        
        G4cout << "Total number of events requested: " << iNbEventsToSimulate << G4endl;
	}
	
	if ( bInteractive ) { 
		// start ui and delete it when the user closes the ui
		ui->SessionStart();  	
		delete ui;
	}
  
	// delete all created objects
	if(bInteractive) delete pVisManager;
	delete pRunManager;
	
	return 0;	
}

void usage() {
  exit(0);
}

inline bool fileexists (const std::string& name) {
	return fileexists(name.c_str());
}

inline bool fileexists (const char* name) {
    std::ifstream f(name);
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }   
}

