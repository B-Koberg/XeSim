#include <globals.hh>
#include <G4RunManagerKernel.hh>
#include <G4Event.hh>
#include <Randomize.hh>

#include "XeSimParticleSource.hh"
#include "XeSimPrimaryGeneratorAction.hh"
#include "XeSimPrimaryGeneratorMessenger.hh"

XeSimPrimaryGeneratorAction::XeSimPrimaryGeneratorAction()
{
	m_pMessenger = new XeSimPrimaryGeneratorMessenger(this);
	m_pParticleSource = new XeSimParticleSource();

	m_hParticleTypeOfPrimary = "";
	m_hParticleTypeVectorOfPrimary = new std::vector<G4String>();
	m_dEnergyOfPrimary = 0.;
	m_hPositionOfPrimary = G4ThreeVector(0., 0., 0.);

	m_lSeeds[0] = -1;
	m_lSeeds[1] = -1;
}

XeSimPrimaryGeneratorAction::~XeSimPrimaryGeneratorAction()
{
	delete m_pMessenger;
	delete m_pParticleSource;
	delete m_hParticleTypeVectorOfPrimary;
}

void XeSimPrimaryGeneratorAction::FillPrimaryType(G4Event *pEvent)
{
  G4PrimaryVertex * primVertex = pEvent->GetPrimaryVertex();
  m_hParticleTypeVectorOfPrimary->push_back(primVertex->GetPrimary()->GetParticleDefinition()->GetParticleName());

  while(primVertex->GetNext() != 0)
  {
    primVertex = primVertex->GetNext();
    m_hParticleTypeVectorOfPrimary->push_back(primVertex->GetPrimary()->GetParticleDefinition()->GetParticleName());
  }
}

void XeSimPrimaryGeneratorAction::GeneratePrimaries(G4Event *pEvent)
{
	m_hParticleTypeVectorOfPrimary->clear();
	m_lSeeds[0] = *(CLHEP::HepRandom::getTheSeeds());
	m_lSeeds[1] = *(CLHEP::HepRandom::getTheSeeds()+1);

	G4StackManager *pStackManager = (G4RunManagerKernel::GetRunManagerKernel())->GetStackManager();

	if(!pStackManager->GetNPostponedTrack())
	{
		m_pParticleSource->GeneratePrimaryVertex(pEvent);
		FillPrimaryType(pEvent);
	}
	else {
		pStackManager->TransferStackedTracks(fPostpone, fUrgent);
		G4VTrajectory* pTrajectory;
		G4Track *pTrack = pStackManager->PopNextTrack(&pTrajectory);

		m_pParticleSource->GeneratePrimaryVertexFromTrack(pTrack, pEvent);
		FillPrimaryType(pEvent);

		delete pTrack;
	}
	G4PrimaryVertex *pVertex = pEvent->GetPrimaryVertex();
	G4PrimaryParticle *pPrimaryParticle = pVertex->GetPrimary();

	m_hParticleTypeOfPrimary = pPrimaryParticle->GetG4code()->GetParticleName();

	G4double dP = pPrimaryParticle->GetMomentum().mag();
	G4double dMass = pPrimaryParticle->GetMass();

	m_dEnergyOfPrimary = std::sqrt(dP*dP + dMass*dMass) - dMass;
	m_hPositionOfPrimary = pVertex->GetPosition();
}

