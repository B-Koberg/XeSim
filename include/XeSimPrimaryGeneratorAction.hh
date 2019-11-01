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
	G4double GetEnergyOfPrimary() { return m_dEnergyOfPrimary; }
	G4ThreeVector GetPositionOfPrimary() { return m_hPositionOfPrimary; }

	void GeneratePrimaries(G4Event *pEvent);
	void     SetWriteEmpty(G4bool doit){writeEmpty = doit;};
	G4bool   GetWriteEmpty(){return writeEmpty;};

  private:
	XeSimPrimaryGeneratorMessenger *m_pMessenger;
	long m_lSeeds[2];
	G4bool	writeEmpty;
	G4String m_hParticleTypeOfPrimary;
	G4double m_dEnergyOfPrimary;
	G4ThreeVector m_hPositionOfPrimary;

	XeSimParticleSource *m_pParticleSource;
};

#endif

