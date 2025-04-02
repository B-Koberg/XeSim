#include <G4SDManager.hh>
#include <G4Run.hh>
#include <G4Event.hh>
#include <G4HCofThisEvent.hh>
#include <G4SystemOfUnits.hh>
#include <G4Version.hh>

#include <numeric>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TParameter.h>
#include <TDirectory.h>

#include "XeSimPrimaryGeneratorAction.hh"
#include "XeSimAnalysisManager.hh"
#include "XeSimAnalysisMessenger.hh"
#include "XeSimEventData.hh"
#include "XeSimLXeHit.hh"
#include "XeSimPhotoDetHit.hh"

XeSimAnalysisManager::XeSimAnalysisManager(XeSimPrimaryGeneratorAction *pPrimaryGeneratorAction) {
  runTime = new G4Timer();
  
  m_iLXeHitsCollectionID = -1;
  m_iPhotoDetHitsCollectionID = -1;

  m_hDataFilename = "events.root";
  m_hVersionTag = "0.0.0";
  m_hExperimentTag = "XeSim";
  m_hMacroFiles.clear();
  m_iNbPhotoDets = 0;
  
  m_pPrimaryGeneratorAction = pPrimaryGeneratorAction;
  m_pEventData = new XeSimEventData();
  writeEmptyEvents = kFALSE;
  
  m_pAnalysisMessenger = new XeSimAnalysisMessenger(this);
  
  m_PhotoDetHitsDetails = kFALSE;

  m_pNeutronActivation = kFALSE;
  m_pRecordOnlyEventID = kFALSE;
  m_pRecordOnlyActiveVolume = kFALSE;
}

XeSimAnalysisManager::~XeSimAnalysisManager(){
    delete m_pAnalysisMessenger;
}

void XeSimAnalysisManager::BeginOfRun(const G4Run *pRun) {
    runTime->Start();
    writeEmptyEvents = m_pPrimaryGeneratorAction->GetWriteEmpty();

    m_pTreeFile = new TFile(m_hDataFilename.c_str(), "RECREATE", "File containing event data for XeSim");
    
    TNamed *G4version = new TNamed("G4VERSION_TAG", G4VERSION_TAG);
    G4version->Write();
    TNamed *G4MCname = new TNamed("MC_TAG", m_hExperimentTag);
    G4MCname->Write();
    TNamed *G4MCVersion = new TNamed("MCVERSION_TAG", m_hVersionTag);
    G4MCname->Write();
    
    _macros = m_pTreeFile->mkdir("macros");
    _macros->cd();

    for (std::vector<G4String>::iterator it = m_hMacroFiles.begin();
         it != m_hMacroFiles.end(); ++it) {
      G4String macrofile = *it;
      G4String datafilename = macrofile.substr(macrofile.find_last_of("/\\") + 1);
      std::ifstream fs(macrofile);
      std::string datafilecontent((std::istreambuf_iterator<char>(fs)),
                                   std::istreambuf_iterator<char>());
      TNamed *MacroFiles = new TNamed(datafilename.c_str(),
                                      "# " + macrofile + "\n\n" + datafilecontent);
      MacroFiles->Write();
    }
    
    _events = m_pTreeFile->mkdir("events");
    _events->cd();
    
    // create ROOT Tree for the simulation data
    m_pTree = new TTree("events", "Tree containing event data for XeSim");

    // include missing ROOT classes
    gROOT->ProcessLine("#include <vector>");

    // eventid:	The eventid contains the individual ID of each event. Since 'event' is used in 
    //          different situations, it means, every particle created by 'beamOn' starts a 
    //          detector event.
    m_pTree->Branch("eventid", &m_pEventData->m_iEventId, "eventid/I");

    if (!m_pRecordOnlyEventID) {
      // photodethits:	total amount of PMT hits for a specific eventid and for each PMT
      m_pTree->Branch("photodethits", "vector<int>", &m_pEventData->m_pPhotoDetHits);
      m_pTree->Branch("nphotodethits", &m_pEventData->m_iNbPhotoDetHits, "nphotodethits/I");
      // etot: Amount of energy, which is deopsited during this eventid/particle run.
      m_pTree->Branch("etot", &m_pEventData->m_fTotalEnergyDeposited, "etot/F");
      m_pTree->Branch("nsteps", &m_pEventData->m_iNbSteps, "nsteps/I");

      // trackid:	ID of the current event in the event track. All listed events are
      //			generated within the main eventid. (e.g. emitted gammas)
      m_pTree->Branch("trackid", "vector<int>", &m_pEventData->m_pTrackId);
      // type:	type of the particles in the event track
      m_pTree->Branch("type", "vector<string>", &m_pEventData->m_pParticleType);
      // parentid:	trackid of the parent track event
      m_pTree->Branch("parentid", "vector<int>", &m_pEventData->m_pParentId);
      // parenttype:	parenttype of the parent track event
      m_pTree->Branch("parenttype", "vector<string>", &m_pEventData->m_pParentType);
      // creaproc:	name of the creation process of the track particle/trackid
      m_pTree->Branch("creaproc", "vector<string>", &m_pEventData->m_pCreatorProcess);
      // edproc:	name of the energy deposition process of the track particle/trackid
      m_pTree->Branch("edproc", "vector<string>", &m_pEventData->m_pDepositingProcess);
      // Positions of the current particle/trackid
      m_pTree->Branch("xp", "vector<float>", &m_pEventData->m_pX);
      m_pTree->Branch("yp", "vector<float>", &m_pEventData->m_pY);
      m_pTree->Branch("zp", "vector<float>", &m_pEventData->m_pZ);
      // ed:	energy deposition of the current particle/trackid
      m_pTree->Branch("ed", "vector<float>", &m_pEventData->m_pEnergyDeposited);
      // time:	timestamp of the current particle/trackid
      m_pTree->Branch("time", "vector<double>", &m_pEventData->m_pTime);

      // type_pri:	type of the primary event/main event
      m_pTree->Branch("type_pri", "vector<string>", &m_pEventData->m_pPrimaryParticleType);
      // Energy and positions of the current particle/trackid
      m_pTree->Branch("e_pri", &m_pEventData->m_fPrimaryEnergy, "e_pri/F");
      m_pTree->Branch("xp_pri", &m_pEventData->m_fPrimaryX, "xp_pri/F");
      m_pTree->Branch("yp_pri", &m_pEventData->m_fPrimaryY, "yp_pri/F");
      m_pTree->Branch("zp_pri", &m_pEventData->m_fPrimaryZ, "zp_pri/F");
      m_pTree->Branch("vol_pri", &m_pEventData->m_fPrimaryVolume);
      
      // Array of PmtHits, indexed by PMT ID
      if (m_PhotoDetHitsDetails == kTRUE) {
        m_pTree->Branch("photodethitID", "vector<int>", &m_pEventData->m_pPhotoDetHitID);
        m_pTree->Branch("photodethitTime", "vector<double>", &m_pEventData->m_pPhotoDetHitTime);
        m_pTree->Branch("photodethitEnergy", "vector<float>", &m_pEventData->m_pPhotoDetHitEnergy);
        m_pTree->Branch("photodethitTheta", "vector<float>",&m_pEventData->m_pPhotoDetHitTheta);
        m_pTree->Branch("photodethitPhi", "vector<float>", &m_pEventData->m_pPhotoDetHitPhi);
        m_pTree->Branch("photodethitXp", "vector<float>", &m_pEventData->m_pPhotoDetHitX);
        m_pTree->Branch("photodethitYp", "vector<float>", &m_pEventData->m_pPhotoDetHitY);
        m_pTree->Branch("photodethitZp", "vector<float>", &m_pEventData->m_pPhotoDetHitZ);
      }
    }

    // Branches for configurable saves
    m_pTree->Branch("NSave", &m_pEventData->m_iNSave, "NSave/I");
    m_pTree->Branch("Save_flag", "vector<int>", &m_pEventData->m_pSave_flag);
    m_pTree->Branch("Save_type", "vector<int>", &m_pEventData->m_pSave_type);
    m_pTree->Branch("Save_x", "vector<float>", &m_pEventData->m_pSave_x);
    m_pTree->Branch("Save_y", "vector<float>", &m_pEventData->m_pSave_y);
    m_pTree->Branch("Save_z", "vector<float>", &m_pEventData->m_pSave_z);
    m_pTree->Branch("Save_cx", "vector<float>", &m_pEventData->m_pSave_cx);
    m_pTree->Branch("Save_cy", "vector<float>", &m_pEventData->m_pSave_cy);
    m_pTree->Branch("Save_cz", "vector<float>", &m_pEventData->m_pSave_cz);
    m_pTree->Branch("Save_e", "vector<float>", &m_pEventData->m_pSave_e);
    m_pTree->Branch("Save_t", "vector<float>", &m_pEventData->m_pSave_t);
    m_pTree->Branch("Save_trkid", "vector<int>", &m_pEventData->m_pSave_trkid);

    // Branches for storing Neutron Activation information
    if (m_pNeutronActivation == kTRUE) {
      m_pTree->Branch("NAct_volume", "vector<string>", &m_pEventData->m_pNAct_volume);
      m_pTree->Branch("NAct_name", "vector<string>", &m_pEventData->m_pNAct_name);
      m_pTree->Branch("NAct_x", "vector<float>", &m_pEventData->m_pNAct_x);
      m_pTree->Branch("NAct_y", "vector<float>", &m_pEventData->m_pNAct_y);
      m_pTree->Branch("NAct_z", "vector<float>", &m_pEventData->m_pNAct_z);
      m_pTree->Branch("NAct_process", "vector<string>", &m_pEventData->m_pNAct_process);
      m_pTree->Branch("NAct_event", "vector<int>", &m_pEventData->m_pNAct_event);
      m_pTree->Branch("NAct_mass", "vector<int>", &m_pEventData->m_pNAct_mass);
      m_pTree->Branch("NAct_number", "vector<int>", &m_pEventData->m_pNAct_number);
      m_pTree->Branch("NAct_t", "vector<float>", &m_pEventData->m_pNAct_t);
      m_pTree->Branch("NAct", &m_pEventData->m_iNAct, "nAct/I");
    }

    //m_pTree->SetMaxTreeSize(1000*Long64_t(2000000000)); //2TB
    //m_pTree->AutoSave();

    // Write the number of events in the output file
    m_iNbEventsToSimulate = pRun->GetNumberOfEventToBeProcessed();
    m_pNbEventsToSimulateParameter = new TParameter<int>("nbevents", m_iNbEventsToSimulate);
    m_pNbEventsToSimulateParameter->Write();

    m_pTreeFile->cd();
}

void XeSimAnalysisManager::EndOfRun(const G4Run *pRun) {
    runTime->Stop();
    G4double dt = runTime->GetRealElapsed();
  
    // Make tree structure
    TParameter<G4double> *dtPar = new TParameter<G4double>("G4RUNTIME", dt);
    dtPar->Write();
  
    m_pTreeFile->cd();
  
  // write and remove old revisions
  m_pTreeFile->Write(0, TObject::kOverwrite);
  //m_pTreeFile->Write();
  m_pTreeFile->Close();
}

void XeSimAnalysisManager::BeginOfEvent(const G4Event *pEvent) {
  // initialize the HitCollections if needed (-1 = not initialized)
  if(m_iLXeHitsCollectionID == -1)
  {
    G4SDManager *pSDManager = G4SDManager::GetSDMpointer();
    m_iLXeHitsCollectionID = pSDManager->GetCollectionID("LXeHitsCollection");
  } 

  if(m_iPhotoDetHitsCollectionID == -1)
  {
    G4SDManager *pSDManager = G4SDManager::GetSDMpointer();
    m_iPhotoDetHitsCollectionID = pSDManager->GetCollectionID("PhotoDetHitsCollection");
  }
}

void XeSimAnalysisManager::EndOfEvent(const G4Event *pEvent) {
  G4HCofThisEvent* pHCofThisEvent = pEvent->GetHCofThisEvent();
  XeSimLXeHitsCollection* pLXeHitsCollection = 0;
  XeSimPhotoDetHitsCollection* pPhotoDetHitsCollection = 0;

  G4int iNbLXeHits = 0, iNbPhotoDetHits = 0;
  
  if(pHCofThisEvent)
  {
    if(m_iLXeHitsCollectionID != -1)
    {
      pLXeHitsCollection = (XeSimLXeHitsCollection *)(pHCofThisEvent->GetHC(m_iLXeHitsCollectionID));
      iNbLXeHits = (pLXeHitsCollection)?(pLXeHitsCollection->entries()):(0);
    }

    if(m_iPhotoDetHitsCollectionID != -1)
    {
      pPhotoDetHitsCollection = (XeSimPhotoDetHitsCollection *)(pHCofThisEvent->GetHC(m_iPhotoDetHitsCollectionID));
      iNbPhotoDetHits = (pPhotoDetHitsCollection)?(pPhotoDetHitsCollection->entries()):(0);
    }
  }

  m_pEventData->m_iEventId = pEvent->GetEventID();

  m_pEventData->m_pPrimaryParticleType->push_back(m_pPrimaryGeneratorAction->GetParticleTypeOfPrimary());

  m_pEventData->m_fPrimaryEnergy = m_pPrimaryGeneratorAction->GetEnergyOfPrimary()/keV;
  m_pEventData->m_fPrimaryX = m_pPrimaryGeneratorAction->GetPositionOfPrimary().x()/mm;
  m_pEventData->m_fPrimaryY = m_pPrimaryGeneratorAction->GetPositionOfPrimary().y()/mm;
  m_pEventData->m_fPrimaryZ = m_pPrimaryGeneratorAction->GetPositionOfPrimary().z()/mm;

  G4int iNbSteps = 0;
  G4float fTotalEnergyDeposited = 0.;
  
  if(iNbLXeHits || iNbPhotoDetHits)
  {
    // LXe hits
    for(G4int i=0; i<iNbLXeHits; i++)
    {
      XeSimLXeHit *pHit = (*pLXeHitsCollection)[i];

      if(pHit->GetParticleType() != "opticalphoton")
      {
        if (m_pRecordOnlyActiveVolume){
          G4double ix = pHit->GetPosition().x() / mm;
          G4double iy = pHit->GetPosition().y() / mm;
          G4double iz = pHit->GetPosition().z() / mm;
          if ((ix*ix+iy*iy > 665*665) || (iz < -1548) || (iz > 10) ) continue;
	        // Be very careful if using this option and document it in your simulations log!
        }

        m_pEventData->m_pTrackId->push_back(pHit->GetTrackId());
        m_pEventData->m_pParentId->push_back(pHit->GetParentId());

        m_pEventData->m_pParticleType->push_back(pHit->GetParticleType());
        m_pEventData->m_pParentType->push_back(pHit->GetParentType());
        m_pEventData->m_pCreatorProcess->push_back(pHit->GetCreatorProcess());
        m_pEventData->m_pDepositingProcess->push_back(pHit->GetDepositingProcess());

        m_pEventData->m_pX->push_back(pHit->GetPosition().x()/mm);
        m_pEventData->m_pY->push_back(pHit->GetPosition().y()/mm);
        m_pEventData->m_pZ->push_back(pHit->GetPosition().z()/mm);

        fTotalEnergyDeposited += pHit->GetEnergyDeposited()/keV;
        m_pEventData->m_pEnergyDeposited->push_back(pHit->GetEnergyDeposited()/keV);

        m_pEventData->m_pKineticEnergy->push_back(pHit->GetKineticEnergy()/keV);
        m_pEventData->m_pTime->push_back(pHit->GetTime()/second);

        iNbSteps++;
      }
    }

    m_pEventData->m_iNbSteps = iNbSteps;
    m_pEventData->m_fTotalEnergyDeposited = fTotalEnergyDeposited;
    m_pEventData->m_pPhotoDetHits->resize(m_iNbPhotoDets, 0);

    // PhotoDet hits
    for(G4int i=0; i<iNbPhotoDetHits; i++)
        {
      (*(m_pEventData->m_pPhotoDetHits))[(*pPhotoDetHitsCollection)[i]->GetPhotoDetNb()]++;
            
            if (m_PhotoDetHitsDetails) {
                m_pEventData->m_pPhotoDetHitID->push_back((*pPhotoDetHitsCollection)[i]->GetPhotoDetNb());
                m_pEventData->m_pPhotoDetHitTime->push_back((*pPhotoDetHitsCollection)[i]->GetTime() / second);

                m_pEventData->m_pPhotoDetHitEnergy->push_back((*pPhotoDetHitsCollection)[i]->GetEnergy()/eV);
                // G4cout <<  pEvent->GetEventID() << " - TIME " <<
                // (*pPhotoDetHitsCollection)[i]->GetTime()/second << " - Energy " << (*pPhotoDetHitsCollection)[i]->GetEnergy()/eV << G4endl;

                // m_pEventData->m_pTrackId->push_back((*pPhotoDetHitsCollection)[i]->GetTrackId());
                m_pEventData->m_pPhotoDetHitX->push_back((*pPhotoDetHitsCollection)[i]->GetPosition().x() / mm);
                m_pEventData->m_pPhotoDetHitY->push_back((*pPhotoDetHitsCollection)[i]->GetPosition().y() / mm);
                m_pEventData->m_pPhotoDetHitZ->push_back((*pPhotoDetHitsCollection)[i]->GetPosition().z() / mm);

                G4ThreeVector direction =(*pPhotoDetHitsCollection)[i]->GetDirection();  // Normalized vector
                m_pEventData->m_pPhotoDetHitTheta->push_back(std::acos(direction.z()));  // Direction.theta()
                m_pEventData->m_pPhotoDetHitPhi->push_back(std::atan2(direction.y(), direction.x()));  // Direction.phi()
                // m_pEventData->m_pPmtHitVolumeName->push_back((*pPhotoDetHitsCollection)[i]->GetVolumeName());

                m_pEventData->m_pParticleType->push_back("opticalphoton");
              }
        }
        
        m_pEventData->m_iNbPhotoDetHits = iNbPhotoDetHits;
        //accumulate(m_pEventData->m_pPhotoDetHits->begin(),
        //           m_pEventData->m_pPhotoDetHits->begin() + iNbPhotoDetHits, 0);

      // save only energy depositing events
      if(writeEmptyEvents || m_pRecordOnlyEventID) {
        m_pTree->Fill(); // write all events to the tree
      } else {
        // only events with some activity are written to the tree
        if(fTotalEnergyDeposited > 0. || iNbPhotoDetHits > 0) m_pTree->Fill(); 
      }

    // auto save functionality to avoid data loss/ROOT can recover aborted simulations
    if ( pEvent->GetEventID() % 10000 == 0) {
      m_pTree->AutoSave();
    }

    m_pEventData->Clear();
    m_pTreeFile->cd();
  }
}

// Can be called in the SteppingAction to store the particle information
void XeSimAnalysisManager::FillParticleInSave(G4int flag, G4int partPDGcode,
                                                G4ThreeVector pos,
                                                G4ThreeVector dir, G4float nrg,
                                                G4float time, G4int trackID) {
  m_pEventData->m_pSave_flag->push_back(flag);
  m_pEventData->m_pSave_type->push_back(partPDGcode);
  m_pEventData->m_pSave_x->push_back(pos.x() / mm);
  m_pEventData->m_pSave_y->push_back(pos.y() / mm);
  m_pEventData->m_pSave_z->push_back(pos.z() / mm);
  m_pEventData->m_pSave_cx->push_back(dir.x());
  m_pEventData->m_pSave_cy->push_back(dir.y());
  m_pEventData->m_pSave_cz->push_back(dir.z());
  m_pEventData->m_pSave_e->push_back(nrg / keV);
  m_pEventData->m_pSave_t->push_back(time / ns);
  m_pEventData->m_pSave_trkid->push_back(trackID);
  m_pEventData->m_iNSave++;
}

void XeSimAnalysisManager::FillNeutronCaptureInSave(
    G4String name, G4String process, G4int atomic_mass, G4int atomic_number,
    G4ThreeVector pos, G4String volume, G4int event_number, G4float time) {
  m_pEventData->m_pNAct_volume->push_back(volume);
  m_pEventData->m_pNAct_name->push_back(name);
  m_pEventData->m_pNAct_x->push_back(pos.x() / mm);
  m_pEventData->m_pNAct_y->push_back(pos.y() / mm);
  m_pEventData->m_pNAct_z->push_back(pos.z() / mm);
  m_pEventData->m_pNAct_process->push_back(process);
  m_pEventData->m_pNAct_event->push_back(event_number);
  m_pEventData->m_pNAct_mass->push_back(atomic_mass);
  m_pEventData->m_pNAct_number->push_back(atomic_number);
  m_pEventData->m_pNAct_t->push_back(time);
  m_pEventData->m_iNAct++;
}

void XeSimAnalysisManager::Step(const G4Step *pStep) {
  if (pStep->GetTrack()->GetTrackID() == 1 &&
      pStep->GetTrack()->GetCurrentStepNumber() == 1) {
    m_pEventData->m_fPrimaryVolume = pStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
  }
}
