#include <G4HCofThisEvent.hh>
#include <G4Step.hh>
#include <G4VProcess.hh>
#include <G4ThreeVector.hh>
#include <G4SDManager.hh>
#include <G4ios.hh>

#include <map>

using std::map;

#include "XeSimLXeSensitiveDetector.hh"

XeSimLXeSensitiveDetector::XeSimLXeSensitiveDetector(G4String hName): G4VSensitiveDetector(hName) {
	collectionName.insert("LXeHitsCollection");
}

XeSimLXeSensitiveDetector::~XeSimLXeSensitiveDetector()
{
}

void XeSimLXeSensitiveDetector::Initialize(G4HCofThisEvent* pHitsCollectionOfThisEvent)
{
	m_pLXeHitsCollection = new XeSimLXeHitsCollection(SensitiveDetectorName, collectionName[0]);

	static G4int iHitsCollectionID = -1;

	if(iHitsCollectionID < 0)
		iHitsCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	
	pHitsCollectionOfThisEvent->AddHitsCollection(iHitsCollectionID, m_pLXeHitsCollection);

	m_hParticleTypes.clear();
	m_hParticleTypeIDs.clear();
}

G4bool XeSimLXeSensitiveDetector::ProcessHits(G4Step* pStep, G4TouchableHistory *pHistory)
{
	G4double dEnergyDeposited = pStep->GetTotalEnergyDeposit();
	G4Track *pTrack = pStep->GetTrack();

	XeSimLXeHit* pHit = new XeSimLXeHit();

	pHit->SetTrackId(pTrack->GetTrackID());

	if(!m_hParticleTypes.count(pTrack->GetTrackID())) {
		m_hParticleTypes[pTrack->GetTrackID()] = pTrack->GetDefinition()->GetParticleName();
		m_hParticleTypeIDs[pTrack->GetTrackID()] = pTrack->GetDefinition()->GetPDGEncoding();
	}
	
	pHit->SetParentId(pTrack->GetParentID());
	pHit->SetParticleType(pTrack->GetDefinition()->GetParticleName());
	pHit->SetParticleTypeID(pTrack->GetDefinition()->GetPDGEncoding());

	if(pTrack->GetParentID()) {
		pHit->SetParentType(m_hParticleTypes[pTrack->GetParentID()]);
		pHit->SetParentTypeID(m_hParticleTypeIDs[pTrack->GetParentID()]);
	} else {
		pHit->SetParentType(G4String("none"));
		pHit->SetParentTypeID(0);
	}

	if(pTrack->GetCreatorProcess()) {
		pHit->SetCreatorProcess(pTrack->GetCreatorProcess()->GetProcessName());
		pHit->SetCreatorProcessType(pTrack->GetCreatorProcess()->GetProcessType());
		pHit->SetCreatorProcessSubType(pTrack->GetCreatorProcess()->GetProcessSubType());
	} else {
		pHit->SetCreatorProcess(G4String("Null"));
		pHit->SetCreatorProcessType(0);
		pHit->SetCreatorProcessSubType(0);
	}

	pHit->SetDepositingProcess(pStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName());
	pHit->SetDepositingProcessType(pStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessType());
	pHit->SetDepositingProcessSubType(pStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessSubType());
	pHit->SetPosition(pStep->GetPostStepPoint()->GetPosition());
	pHit->SetEnergyDeposited(dEnergyDeposited);
	pHit->SetKineticEnergy(pTrack->GetKineticEnergy());
    pHit->SetPreStepEnergy(pStep->GetPreStepPoint()->GetKineticEnergy());
    pHit->SetPostStepEnergy(pStep->GetPostStepPoint()->GetKineticEnergy());
	pHit->SetTime(pTrack->GetGlobalTime());

	m_pLXeHitsCollection->insert(pHit);
	
	return true;
}

void XeSimLXeSensitiveDetector::EndOfEvent(G4HCofThisEvent *pHitsCollectionOfThisEvent)
{
//  if (verboseLevel>0) { 
//     G4int NbHits = m_pLXeHitsCollection->entries();
//     G4cout << "\n-------->Hits Collection: in this event they are " << NbHits 
//            << " hits in the tracker chambers: " << G4endl;
//     for (G4int i=0;i<NbHits;i++) (*m_pLXeHitsCollection)[i]->Print();
//    } 
}

