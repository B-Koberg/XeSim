#include <G4SDManager.hh>
#include <G4Run.hh>
#include <G4Event.hh>
#include <G4HCofThisEvent.hh>
#include <G4SystemOfUnits.hh>
#include <G4Version.hh>
#include <G4VProcess.hh>
#include <G4ProcessTable.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4LogicalVolume.hh>
#include <G4NeutronCaptureXS.hh>
#include <G4Neutron.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4VPhysicalVolume.hh>

#include <numeric>

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TParameter.h>
#include <TH1D.h>
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

  particle_name.clear();
  particle_id.clear();
  processNames.clear();
  processTypes.clear();
  processSubTypes.clear();
  
  m_pPrimaryGeneratorAction = pPrimaryGeneratorAction;
  m_pEventData = new XeSimEventData();
  writeEmptyEvents = kFALSE;
  
  m_pAnalysisMessenger = new XeSimAnalysisMessenger(this);
  
  m_PhotoDetHitsDetails = kFALSE;

  m_pRecordOnlyEventID = kFALSE;
  m_pNeutronActivation = kFALSE;
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
    G4MCVersion->Write();
    
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
    
    _tables = m_pTreeFile->mkdir("tables");
    _tables->cd();

    // Get the material table
    const G4MaterialTable* materialTable = G4Material::GetMaterialTable();

    int nBins = 1000;
    double min = 1 * eV;
    double max = 20.0 * MeV;
    std::vector<double> bins;
    std::vector<double> bin_centers;
    double step = (std::log10(max) - std::log10(min)) / nBins;

    for (int i = 0; i <= nBins; ++i) {
      bins.push_back(std::pow(10, std::log10(min) + i * step));
    }
    for (int i = 0; i < nBins; ++i) {
      bin_centers.push_back(std::pow(10, std::log10(min) + (i + 0.5) * step));
    }

    // Loop over all materials
    for (size_t i = 0; i < materialTable->size(); ++i) {
      G4Material* material = (*materialTable)[i];
      if (material) {
          // Create a TH1 histogram for this material
          std::string histName = "neutronXS_" + material->GetName();
          std::string histTitle = "Neutron Capture Cross-Section for " + material->GetName();
          TH1D* hist = new TH1D(histName.c_str(), histTitle.c_str(), nBins, bins.data());

          for (int k = 0; k < nBins; ++k) {
              // Get the neutron capture cross-section
              G4NeutronCaptureXS neutronXS;
              G4DynamicParticle neutron(G4Neutron::Definition(), G4ThreeVector(0, 0, 1), bin_centers[k]);
              double crossSection = 0.0;
              const G4Material* mat = material;
              
              for (size_t j = 0; j < mat->GetNumberOfElements(); ++j) {
                  const G4Element* element = mat->GetElement(j);
                  double fraction = mat->GetFractionVector()[j];
                  crossSection += fraction * neutronXS.GetCrossSection(&neutron, mat->GetElement(j), mat) * 10000;
                  //crossSection += fraction * neutronXS.GetElementCrossSection(&neutron, element->GetZ(), mat);
              } 
              hist->Fill(bin_centers[k] / MeV, crossSection / barn);
          }
          // Set errors to 0 for all bins
          for (int bin = 1; bin <= hist->GetNbinsX(); ++bin) {
            hist->SetBinError(bin, 0.0);
          }

          // Store the histogram in the ROOT file
          hist->Write();
      }
    }

    // Create list of process subtypes as vectors
    /*G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleTable::G4PTblDicIterator* particleIterator = particleTable->GetIterator();
    particleIterator->reset(); 

    // Iteration over all particles in the table
     while ((*particleIterator)()) {
      G4ParticleDefinition* particle = particleIterator->value();
      G4ProcessManager* processManager = particle->GetProcessManager();

      if (processManager) {
        G4ProcessVector* processVector = processManager->GetProcessList();

        for (size_t i = 0; i < processVector->size(); ++i) {
          G4VProcess* process = (*processVector)[i];
          if (process) {
            particle_name.push_back(particle->GetParticleName());
            particle_id.push_back(particle->GetPDGEncoding());
            processNames.push_back(process->GetProcessName());
            processTypes.push_back(process->GetProcessType());
            processSubTypes.push_back(process->GetProcessSubType());
          }
        }
      }
    } */

    //// Print all subtypes for debugging
    //G4cout << "List of Process Subtypes:" << G4endl;
    //// interate over the vectors to print the process names, types, and subtypes
    //for (size_t i = 0; i < particle_name.size(); ++i) {
    //    G4cout << "Particle: " << particle_name[i] << ", ID: " << particle_id[i]
    //           << ", Process Name: " << processNames[i]
    //           << ", Process Type: " << processTypes[i]
    //           << ", Process Subtype: " << processSubTypes[i] << G4endl;
    //}

    // Create a TTree to store the process subtypes
    TTree* processTree = new TTree("process_subtypes", "Process Subtypes Tree");

    processTree->Branch("particle_name", &particle_name);
    processTree->Branch("particle_id", &particle_id);
    processTree->Branch("process_name", &processNames);
    processTree->Branch("process_type", &processTypes);
    processTree->Branch("process_subtype", &processSubTypes);

    if (!particle_name.empty()) {
      processTree->Fill();
    }

    processTree->Write();

    const G4LogicalVolumeStore* logicalVolumeStore = G4LogicalVolumeStore::GetInstance();
    const G4PhysicalVolumeStore* physicalVolumeStore = G4PhysicalVolumeStore::GetInstance();
    vector<string> logicalvolumeNames;
    vector<ULong64_t> logicalvolumeNamesHash;
    vector<string> physicalvolumeNames;
    vector<ULong64_t> physicalvolumeNamesHash;

    for (const auto* logicalVolume : *logicalVolumeStore) {
        if (logicalVolume->GetName() != "") {
            logicalvolumeNames.push_back(logicalVolume->GetName());
            logicalvolumeNamesHash.push_back(std::hash<std::string>()(logicalVolume->GetName()));
        }
    }

    for (const auto* physicalVolume : *physicalVolumeStore) {
        if (physicalVolume->GetName() != "") {
            physicalvolumeNames.push_back(physicalVolume->GetName());
            physicalvolumeNamesHash.push_back(std::hash<std::string>()(physicalVolume->GetName()));
        }
    }

    TTree* logicalVolumeTree = new TTree("logical_volumes", "Logical Volumes Tree");
    logicalVolumeTree->Branch("logical_volume_name", &logicalvolumeNames);
    logicalVolumeTree->Branch("logical_volume_hash", &logicalvolumeNamesHash);

    TTree* physicalVolumeTree = new TTree("physical_volumes", "Physical Volumes Tree");
    physicalVolumeTree->Branch("physical_volume_name", &physicalvolumeNames);
    physicalVolumeTree->Branch("physical_volume_hash", &physicalvolumeNamesHash);

    if (!logicalvolumeNames.empty()) {
      logicalVolumeTree->Fill();
    }

    if (!physicalvolumeNames.empty()) {
      physicalVolumeTree->Fill();
    }

    logicalVolumeTree->Write();
    physicalVolumeTree->Write();

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

    // type_pri:	type of the primary event/main event
    m_pTree->Branch("type_pri", "vector<string>", &m_pEventData->m_pPrimaryParticleType);
    m_pTree->Branch("type_pri_id", "vector<int>", &m_pEventData->m_pPrimaryParticleTypeID);

    // Energy and positions of the current particle/trackid
    m_pTree->Branch("e_pri", &m_pEventData->m_fPrimaryEnergy, "e_pri/F");
    m_pTree->Branch("xp_pri", &m_pEventData->m_fPrimaryX, "xp_pri/F");
    m_pTree->Branch("yp_pri", &m_pEventData->m_fPrimaryY, "yp_pri/F");
    m_pTree->Branch("zp_pri", &m_pEventData->m_fPrimaryZ, "zp_pri/F");
    m_pTree->Branch("cx_pri", &m_pEventData->m_fPrimaryCx, "cx_pri/F");
    m_pTree->Branch("cy_pri", &m_pEventData->m_fPrimaryCy, "cy_pri/F");
    m_pTree->Branch("cz_pri", &m_pEventData->m_fPrimaryCz, "cz_pri/F");
    m_pTree->Branch("vol_pri", &m_pEventData->m_fPrimaryVolume);
    m_pTree->Branch("vol_pri_hash", &m_pEventData->m_fPrimaryVolumeHash);

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
      // parentid:	trackid of the parent track event
      m_pTree->Branch("parentid", "vector<int>", &m_pEventData->m_pParentId);
      // type:	type of the particles in the event track
      m_pTree->Branch("type", "vector<string>", &m_pEventData->m_pParticleType);
      m_pTree->Branch("typeid", "vector<int>", &m_pEventData->m_pParticleTypeID);
      // parenttype:	parenttype of the parent track event
      m_pTree->Branch("parenttype", "vector<string>", &m_pEventData->m_pParentType);
      m_pTree->Branch("parenttypeid", "vector<int>", &m_pEventData->m_pParentTypeID);
      // creaproc:	name of the creation process of the track particle/trackid
      m_pTree->Branch("creaproc", "vector<string>", &m_pEventData->m_pCreatorProcess);
      m_pTree->Branch("creaproctype", "vector<int>", &m_pEventData->m_pCreatorProcessType);
      m_pTree->Branch("creaprocsubtype", "vector<int>", &m_pEventData->m_pCreatorProcessSubType);
      // edproc:	name of the energy deposition process of the track particle/trackid
      m_pTree->Branch("edproc", "vector<string>", &m_pEventData->m_pDepositingProcess);
      m_pTree->Branch("edproctype", "vector<int>", &m_pEventData->m_pDepositingProcessType);
      m_pTree->Branch("edprocsubtype", "vector<int>", &m_pEventData->m_pDepositingProcessSubType);
      // Positions of the current particle/trackid
      m_pTree->Branch("xp", "vector<float>", &m_pEventData->m_pX);
      m_pTree->Branch("yp", "vector<float>", &m_pEventData->m_pY);
      m_pTree->Branch("zp", "vector<float>", &m_pEventData->m_pZ);
      // ed:	energy deposition of the current particle/trackid
      m_pTree->Branch("ed", "vector<float>", &m_pEventData->m_pEnergyDeposited);
      // time:	timestamp of the current particle/trackid
      m_pTree->Branch("time", "vector<double>", &m_pEventData->m_pTime);
      
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
    m_pTree->Branch("Save_desc", "vector<string>", &m_pEventData->m_pSave_desc);
    m_pTree->Branch("Save_type", "vector<string>", &m_pEventData->m_pSave_type);
    m_pTree->Branch("Save_x", "vector<float>", &m_pEventData->m_pSave_x);
    m_pTree->Branch("Save_y", "vector<float>", &m_pEventData->m_pSave_y);
    m_pTree->Branch("Save_z", "vector<float>", &m_pEventData->m_pSave_z);
    m_pTree->Branch("Save_cx", "vector<float>", &m_pEventData->m_pSave_cx);
    m_pTree->Branch("Save_cy", "vector<float>", &m_pEventData->m_pSave_cy);
    m_pTree->Branch("Save_cz", "vector<float>", &m_pEventData->m_pSave_cz);
    m_pTree->Branch("Save_e", "vector<float>", &m_pEventData->m_pSave_e);
    m_pTree->Branch("Save_t", "vector<float>", &m_pEventData->m_pSave_t);
    m_pTree->Branch("Save_eventid", "vector<int>", &m_pEventData->m_pSave_number);
    m_pTree->Branch("Save_trackid", "vector<int>", &m_pEventData->m_pSave_trkid);

    // Branches for storing Neutron Activation information
    if (m_pNeutronActivation == kTRUE) {
      m_pTree->Branch("NNAct", &m_pEventData->m_iNAct, "NNAct/I");
      m_pTree->Branch("NAct_eventid", "vector<int>", &m_pEventData->m_pNAct_eventID);
      m_pTree->Branch("NAct_trackid", "vector<int>", &m_pEventData->m_pNAct_trackID);
      m_pTree->Branch("NAct_parentid", "vector<int>", &m_pEventData->m_pNAct_parentID);
      m_pTree->Branch("NAct_t", "vector<float>", &m_pEventData->m_pNAct_t);
      m_pTree->Branch("NAct_volume", "vector<string>", &m_pEventData->m_pNAct_volume);
      m_pTree->Branch("NAct_volume_hash", "vector<uint64_t>", &m_pEventData->m_pNAct_volume_hash);
      m_pTree->Branch("NAct_x", "vector<float>", &m_pEventData->m_pNAct_x);
      m_pTree->Branch("NAct_y", "vector<float>", &m_pEventData->m_pNAct_y);
      m_pTree->Branch("NAct_z", "vector<float>", &m_pEventData->m_pNAct_z);
      m_pTree->Branch("NAct_process_name", "vector<string>", &m_pEventData->m_pNAct_process_name);
      m_pTree->Branch("NAct_process_category", "vector<int>", &m_pEventData->m_pNAct_process_category);
      m_pTree->Branch("NAct_process_ID", "vector<int>", &m_pEventData->m_pNAct_process_ID);
      m_pTree->Branch("NAct_particle_name", "vector<string>", &m_pEventData->m_pNAct_particle_name);
      m_pTree->Branch("NAct_particle_lifetime", "vector<double>", &m_pEventData->m_pNAct_particle_lifetime);
      m_pTree->Branch("NAct_particle_id", "vector<int>", &m_pEventData->m_pNAct_particle_id);
      m_pTree->Branch("NAct_particle_atomicnumber", "vector<int>", &m_pEventData->m_pNAct_particle_atomic_number);
      m_pTree->Branch("NAct_particle_mass", "vector<int>", &m_pEventData->m_pNAct_particle_mass);
      m_pTree->Branch("NAct_particle_excitationEnergy", "vector<double>", &m_pEventData->m_pNAct_particle_excitationEnergy);
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
  m_pEventData->m_iNParticle = pEvent->GetPrimaryVertex()->GetNumberOfParticle();

  //m_pEventData->m_pPrimaryParticleType->push_back(m_pPrimaryGeneratorAction->GetParticleTypeOfPrimary());
  for(int j = 0; j < G4int(m_pPrimaryGeneratorAction->GetParticleTypeVectorOfPrimary()->size()); j++) {
    m_pEventData->m_pPrimaryParticleType->push_back(m_pPrimaryGeneratorAction->GetParticleTypeVectorOfPrimary()->at(j));
    m_pEventData->m_pPrimaryParticleTypeID->push_back(m_pPrimaryGeneratorAction->GetParticleTypeIDVectorOfPrimary()->at(j));
  }
  m_pEventData->m_fPrimaryEnergy = m_pPrimaryGeneratorAction->GetEnergyOfPrimary()/keV;
  m_pEventData->m_fPrimaryX = m_pPrimaryGeneratorAction->GetPositionOfPrimary().x()/mm;
  m_pEventData->m_fPrimaryY = m_pPrimaryGeneratorAction->GetPositionOfPrimary().y()/mm;
  m_pEventData->m_fPrimaryZ = m_pPrimaryGeneratorAction->GetPositionOfPrimary().z()/mm;
  m_pEventData->m_fPrimaryCx = m_pPrimaryGeneratorAction->GetMomentumDirectionOfPrimary().x();
  m_pEventData->m_fPrimaryCy = m_pPrimaryGeneratorAction->GetMomentumDirectionOfPrimary().y();
  m_pEventData->m_fPrimaryCz = m_pPrimaryGeneratorAction->GetMomentumDirectionOfPrimary().z();

  G4int iNbSteps = 0;
  G4float fTotalEnergyDeposited = 0.;
  
  if(iNbLXeHits > 0) {
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
        m_pEventData->m_pParticleTypeID->push_back(pHit->GetParticleTypeID());
        m_pEventData->m_pParentType->push_back(pHit->GetParentType());
        m_pEventData->m_pParentTypeID->push_back(pHit->GetParentTypeID());
        m_pEventData->m_pCreatorProcess->push_back(pHit->GetCreatorProcess());
        m_pEventData->m_pCreatorProcessType->push_back(pHit->GetCreatorProcessType());
        m_pEventData->m_pCreatorProcessSubType->push_back(pHit->GetCreatorProcessSubType());
        m_pEventData->m_pDepositingProcess->push_back(pHit->GetDepositingProcess());
        m_pEventData->m_pDepositingProcessType->push_back(pHit->GetDepositingProcessType());
        m_pEventData->m_pDepositingProcessSubType->push_back(pHit->GetDepositingProcessSubType());
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
  }

  if (iNbPhotoDetHits > 0) {
    // PhotoDet hits
    for(G4int i=0; i<iNbPhotoDetHits; i++) {
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
  }

  // save only energy depositing events
  if (writeEmptyEvents || m_pRecordOnlyEventID) {
    m_pTree->Fill(); // write all events to the tree
  } else if (fTotalEnergyDeposited > 0. || iNbPhotoDetHits > 0) {
    // only events with some activity are written to the tree
    m_pTree->Fill(); 
  } else if (m_pEventData->m_iNSave > 0 || m_pEventData->m_iNAct > 0) {
    // if length of NSave or NAct is > 0, then save the event
    m_pTree->Fill();
  }

  // auto save functionality to avoid data loss/ROOT can recover aborted simulations
  if ( pEvent->GetEventID() % 10000 == 0) {
    m_pTree->AutoSave();
  }

  m_pEventData->Clear();
  m_pTreeFile->cd();
}

// Can be called in the SteppingAction to store the particle information
void XeSimAnalysisManager::FillParticleInSave(G4int flag, G4String description,
                                              G4String particle, G4ThreeVector pos, 
                                              G4ThreeVector dir, G4float nrg,
                                              G4float time, G4int trackID, G4int eventID) {
  m_pEventData->m_pSave_flag->push_back(flag);
  m_pEventData->m_pSave_type->push_back(particle);
  m_pEventData->m_pSave_desc->push_back(description);
  m_pEventData->m_pSave_x->push_back(pos.x() / mm);
  m_pEventData->m_pSave_y->push_back(pos.y() / mm);
  m_pEventData->m_pSave_z->push_back(pos.z() / mm);
  m_pEventData->m_pSave_cx->push_back(dir.x());
  m_pEventData->m_pSave_cy->push_back(dir.y());
  m_pEventData->m_pSave_cz->push_back(dir.z());
  m_pEventData->m_pSave_e->push_back(nrg / keV);
  m_pEventData->m_pSave_t->push_back(time / ns);
  m_pEventData->m_pSave_number->push_back(eventID);
  m_pEventData->m_pSave_trkid->push_back(trackID);
  m_pEventData->m_iNSave++;
}

void XeSimAnalysisManager::FillNeutronCaptureInSave(
  G4String particle_name, G4int particle_atomic_mass, G4int particle_atomic_number, G4double particle_excitationEnergy, G4int particle_ID, G4double particle_lifetime,
  G4String creationprocess_name, G4int creationprocess_category, G4int creationprocess_ID,
  G4ThreeVector pos, G4String pos_volume, G4int event_number, G4float time,
  G4int trackID, G4int parentID) {
  m_pEventData->m_iNAct++;
  m_pEventData->m_pNAct_t->push_back(time);
  m_pEventData->m_pNAct_eventID->push_back(event_number);
  m_pEventData->m_pNAct_trackID->push_back(trackID);
  m_pEventData->m_pNAct_parentID->push_back(parentID);
  m_pEventData->m_pNAct_volume->push_back(pos_volume);
  m_pEventData->m_pNAct_volume_hash->push_back(std::hash<std::string>{}(pos_volume));
  m_pEventData->m_pNAct_x->push_back(pos.x() / mm);
  m_pEventData->m_pNAct_y->push_back(pos.y() / mm);
  m_pEventData->m_pNAct_z->push_back(pos.z() / mm);
  m_pEventData->m_pNAct_process_name->push_back(creationprocess_name);
  m_pEventData->m_pNAct_process_category->push_back(creationprocess_category);
  m_pEventData->m_pNAct_process_ID->push_back(creationprocess_ID);
  m_pEventData->m_pNAct_particle_name->push_back(particle_name);
  m_pEventData->m_pNAct_particle_id->push_back(particle_ID);
  m_pEventData->m_pNAct_particle_mass->push_back(particle_atomic_mass);
  m_pEventData->m_pNAct_particle_atomic_number->push_back(particle_atomic_number);
  m_pEventData->m_pNAct_particle_excitationEnergy->push_back(particle_excitationEnergy);
  m_pEventData->m_pNAct_particle_lifetime->push_back(particle_lifetime);
}

void XeSimAnalysisManager::Step(const G4Step *pStep) {
  if (pStep->GetTrack()->GetTrackID() == 1 &&
      pStep->GetTrack()->GetCurrentStepNumber() == 1) {
    m_pEventData->m_fPrimaryVolume = pStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
    m_pEventData->m_fPrimaryVolumeHash = std::hash<std::string>{}(m_pEventData->m_fPrimaryVolume);
  }
}
