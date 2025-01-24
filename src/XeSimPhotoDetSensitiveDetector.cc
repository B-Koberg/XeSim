#include <G4HCofThisEvent.hh>
#include <G4Step.hh>
#include <G4VProcess.hh>
#include <G4ThreeVector.hh>
#include <G4SDManager.hh>
#include <G4ios.hh>

#include <map>

using namespace std;

#include "XeSimPhotoDetSensitiveDetector.hh"

XeSimPhotoDetSensitiveDetector::XeSimPhotoDetSensitiveDetector(G4String hName): G4VSensitiveDetector(hName)
{
	collectionName.insert("PhotoDetHitsCollection");
}

XeSimPhotoDetSensitiveDetector::~XeSimPhotoDetSensitiveDetector()
{
}

void XeSimPhotoDetSensitiveDetector::Initialize(G4HCofThisEvent* pHitsCollectionOfThisEvent)
{
	m_pPhotoDetHitsCollection = new XeSimPhotoDetHitsCollection(SensitiveDetectorName, collectionName[0]);

	static G4int iHitsCollectionID = -1;

	if(iHitsCollectionID < 0)
		iHitsCollectionID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	
	pHitsCollectionOfThisEvent->AddHitsCollection(iHitsCollectionID, m_pPhotoDetHitsCollection); 
}

G4bool XeSimPhotoDetSensitiveDetector::ProcessHits(G4Step* pStep, G4TouchableHistory *pHistory)
{
	G4Track *pTrack = pStep->GetTrack();

	if(pTrack->GetDefinition()->GetParticleName() == "opticalphoton")
	{
		XeSimPhotoDetHit* pHit = new XeSimPhotoDetHit();

		pHit->SetPosition(pStep->GetPreStepPoint()->GetPosition());
		pHit->SetTime(pTrack->GetGlobalTime());
        pHit->SetDirection(pStep->GetPreStepPoint()->GetMomentumDirection());
        pHit->SetVolumeName(pTrack->GetTouchable()->GetVolume(0)->GetName());
		pHit->SetEnergy(pStep->GetPreStepPoint()->GetTotalEnergy());
        pHit->SetPhotoDetNb(pTrack->GetTouchable()->GetVolume(1)->GetCopyNo());
        pHit->SetTrackId(pTrack->GetTrackID());
        
		m_pPhotoDetHitsCollection->insert(pHit);

		//G4cout << "PhotoDetHit: " << pHit->GetPhotoDetNb() << " " << pHit->GetTime() << " " << pHit->GetEnergy() << " " << pHit->GetPosition() << G4endl;
        
		return true;
	}
	else
	{
		return false;
	}
}

void XeSimPhotoDetSensitiveDetector::EndOfEvent(G4HCofThisEvent *pHitsCollectionOfThisEvent) {
}

