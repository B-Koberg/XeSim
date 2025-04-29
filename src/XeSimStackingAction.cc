#include <G4ios.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTypes.hh>
#include <G4Track.hh>
#include <G4Event.hh>
#include <G4VProcess.hh>
#include <G4StackManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4TransportationManager.hh>
#include <G4EventManager.hh>

#include "XeSimAnalysisManager.hh"
#include "XeSimStackingAction.hh"

XeSimStackingAction::XeSimStackingAction(XeSimAnalysisManager *pAnalysisManager)
{
	m_pAnalysisManager = pAnalysisManager;
	theMessenger = new XeSimStackingActionMessenger(this);
	PostponeFlag = false;
	MaxLifeTime = 1 * ns;
	KillGeneratedParticleNames.clear();
}

XeSimStackingAction::~XeSimStackingAction() {
	delete theMessenger;
	KillGeneratedParticleNames.clear();
 }

G4ClassificationOfNewTrack XeSimStackingAction::ClassifyNewTrack(const G4Track *pTrack)
{
	G4ClassificationOfNewTrack hTrackClassification = fUrgent;

	// For each particle in KillGeneratedParticleNames check if secondary and kill it
	if (KillGeneratedParticleNames.size() > 0 && pTrack->GetParentID() > 0) {
		for (size_t i = 0; i < KillGeneratedParticleNames.size(); i++) {
			if (pTrack->GetDefinition()->GetParticleName() == KillGeneratedParticleNames[i]) {
				return fKill;
			}
		}
	}

	// For storing Neutron Activation information
	G4float timeP = pTrack->GetGlobalTime();
	G4int trackID = pTrack->GetTrackID();
	G4int parentID = pTrack->GetParentID();
	G4String particleType = pTrack->GetDefinition()->GetParticleType();
	G4String volumeName = " ";

	if (particleType == "nucleus" && pTrack->GetParentID() > 0) {
		G4int processCategory = pTrack->GetCreatorProcess()->GetProcessType();
		G4int processID = pTrack->GetCreatorProcess()->GetProcessSubType();
		G4String processName = pTrack->GetCreatorProcess()->GetProcessName();
		
		G4ThreeVector position = pTrack->GetPosition();

		if (G4TransportationManager::GetTransportationManager()
			->GetNavigatorForTracking()->LocateGlobalPointAndSetup(pTrack->GetPosition())) {
			volumeName = G4TransportationManager::GetTransportationManager()
							->GetNavigatorForTracking()
							->LocateGlobalPointAndSetup(pTrack->GetPosition())
							->GetName();
		} else {
			volumeName = "unknown";
		}
		
		
		G4Ions *ion = (G4Ions *)pTrack->GetDefinition();
		G4String particle_name = ion->GetParticleName();
		G4int particle_ID = ion->GetPDGEncoding();
		G4int particle_A = ion->GetAtomicMass();
		G4int particle_Z = ion->GetAtomicNumber();
		G4double particle_excitationEnergy = ion->GetExcitationEnergy()*1e3; // in eV
		G4double particle_lifetime = ion->GetPDGLifeTime();
		
		G4int eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();

		// Remove long-lived isotopes
		if (particle_lifetime > 1e18 * second) particle_lifetime = -1;  

		// (Z > 30 && Z < 40) to add stable isotopes
		//if (particle_lifetime > 0 || particle_excitationEnergy > 0) {
			m_pAnalysisManager->FillNeutronCaptureInSave(particle_name, particle_A, particle_Z, particle_excitationEnergy, particle_ID, particle_lifetime,
														 processName, processCategory, processID,
														 position, volumeName, eventID, timeP,
														 trackID, parentID);
		//}
	}

	//if(pTrack->GetDefinition()->GetParticleType() == "nucleus" && !pTrack->GetDefinition()->GetPDGStable())
	//{
	//	if(pTrack->GetParentID() > 0 && pTrack->GetCreatorProcess()->GetProcessName() == "RadioactiveDecay")
	//		hTrackClassification = fPostpone;
	//}

	// Radioactive decays
	if (PostponeFlag) {
		// Postpone radioactive decays if you want, for backward compatibility
		if (pTrack->GetDefinition()->GetParticleType() == "nucleus" &&
			!pTrack->GetDefinition()->GetPDGStable()) {

			if (pTrack->GetParentID() > 0 &&
				pTrack->GetCreatorProcess()->GetProcessName().find("RadioactiveDecay") != std::string::npos &&
				pTrack->GetDefinition()->GetPDGLifeTime() > MaxLifeTime)
				{
					hTrackClassification = fPostpone;
				}

			if (pTrack->GetDefinition()->GetParticleName() == KillPostponedNucleusName)
				{
					hTrackClassification = fKill;
				}

		}
	}

	return hTrackClassification;
}

void XeSimStackingAction::NewStage() { }

void XeSimStackingAction::PrepareNewEvent() { }

