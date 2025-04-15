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
	G4String particleType = pTrack->GetDefinition()->GetParticleType();
	G4String volumeName = " ";

	if (particleType == "nucleus" && pTrack->GetParentID() > 0) {
		G4String processName = pTrack->GetCreatorProcess()->GetProcessName();
		G4String particleName = pTrack->GetDefinition()->GetParticleName();
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
		G4int A = ion->GetAtomicMass();
		G4int Z = ion->GetAtomicNumber();
		G4double excitationEnergy = ion->GetExcitationEnergy();
		G4double lifetime = ion->GetPDGLifeTime();
		
		G4int eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();

		// Remove long-lived isotopes
		//if (lifetime > 1e18 * second) lifetime = -1;  

		// (Z > 30 && Z < 40) to add stable isotopes
		//if (lifetime > 0 || excitationEnergy > 0) {
		if (true) {
			m_pAnalysisManager->FillNeutronCaptureInSave(particleName, processName, A,
														 Z, position, volumeName,
														 eventID, timeP, lifetime,
														 excitationEnergy);
		}
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

