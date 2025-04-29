#ifndef __XeSimPrimaryGeneratorAction_H__
#define __XeSimPrimaryGeneratorAction_H__

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4ThreeVector.hh>
#include <globals.hh>

#include <TRandom3.h>

#include "XeSimPrimaryGeneratorMessenger.hh"

class XeSimParticleSource;

class G4Event;

class XeSimPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction {
public:
	XeSimPrimaryGeneratorAction();
	~XeSimPrimaryGeneratorAction();

public:
	const long *GetEventSeeds() { return m_lSeeds; }
	const G4String &GetParticleTypeOfPrimary() { return m_hParticleTypeOfPrimary; }
	std::vector<G4String> * GetParticleTypeVectorOfPrimary() { return m_hParticleTypeVectorOfPrimary; }
	const std::vector<G4int> * GetParticleTypeIDVectorOfPrimary() { return m_hParticleTypeIDVectorOfPrimary; }
	G4double GetEnergyOfPrimary() { return m_dEnergyOfPrimary; }
	G4ThreeVector GetPositionOfPrimary() { return m_hPositionOfPrimary; }
	G4ThreeVector GetMomentumDirectionOfPrimary() { return m_hMomentumDirectionOfPrimary; }

	void GeneratePrimaries(G4Event *pEvent);
	void     SetWriteEmpty(G4bool doit){writeEmpty = doit;};
	G4bool   GetWriteEmpty(){return writeEmpty;};

  private:
	void FillPrimaryType(G4Event *pEvent);

	XeSimPrimaryGeneratorMessenger *m_pMessenger;
	long m_lSeeds[2];
	G4bool	writeEmpty;
	G4String m_hParticleTypeOfPrimary;
	std::vector<G4String> *m_hParticleTypeVectorOfPrimary;
	std::vector<G4int> *m_hParticleTypeIDVectorOfPrimary;
	G4double m_dEnergyOfPrimary;
	G4ThreeVector m_hPositionOfPrimary;
	G4ThreeVector m_hMomentumDirectionOfPrimary;

	XeSimParticleSource *m_pParticleSource;
};

#endif

