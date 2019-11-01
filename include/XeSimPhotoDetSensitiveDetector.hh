#ifndef __XeSimPhotoDetSensitiveDetector_H__
#define __XeSimPhotoDetSensitiveDetector_H__

#include <G4VSensitiveDetector.hh>

#include "XeSimPhotoDetHit.hh"

class G4Step;
class G4HCofThisEvent;

class XeSimPhotoDetSensitiveDetector: public G4VSensitiveDetector {
public:
	XeSimPhotoDetSensitiveDetector(G4String hName);
	~XeSimPhotoDetSensitiveDetector();

	void Initialize(G4HCofThisEvent *pHitsCollectionOfThisEvent);
	G4bool ProcessHits(G4Step *pStep, G4TouchableHistory *pHistory);
	void EndOfEvent(G4HCofThisEvent *pHitsCollectionOfThisEvent);

private:
	XeSimPhotoDetHitsCollection* m_pPhotoDetHitsCollection;
};

#endif

