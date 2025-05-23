#ifndef __XeSimLXeSensitiveDetector_H__
#define __XeSimLXeSensitiveDetector_H__

#include <G4VSensitiveDetector.hh>

#include "XeSimLXeHit.hh"

class G4Step;
class G4HCofThisEvent;

class XeSimLXeSensitiveDetector: public G4VSensitiveDetector {
public:
	XeSimLXeSensitiveDetector(G4String hName);
	~XeSimLXeSensitiveDetector();

	void Initialize(G4HCofThisEvent *pHitsCollectionOfThisEvent);
	G4bool ProcessHits(G4Step *pStep, G4TouchableHistory *pHistory);
	void EndOfEvent(G4HCofThisEvent *pHitsCollectionOfThisEvent);

private:
	XeSimLXeHitsCollection* m_pLXeHitsCollection;

	std::map<int,G4String> m_hParticleTypes;
	std::map<int,G4int> m_hParticleTypeIDs;
};

#endif

