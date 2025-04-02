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
}

XeSimStackingAction::~XeSimStackingAction() { }

G4ClassificationOfNewTrack XeSimStackingAction::ClassifyNewTrack(const G4Track *pTrack)
{
	G4ClassificationOfNewTrack hTrackClassification = fUrgent;

	// For storing Neutron Activation information
	G4float timeP = pTrack->GetGlobalTime();
	G4String particleType = pTrack->GetDefinition()->GetParticleType();
	G4String volumeName = " ";
	if (particleType == "nucleus" && pTrack->GetParentID() > 0) {
		G4String processName = pTrack->GetCreatorProcess()->GetProcessName();
		G4Ions *ion = (G4Ions *)pTrack->GetDefinition();
		G4double lifetime = ion->GetPDGLifeTime();
		G4String particleName = pTrack->GetDefinition()->GetParticleName();
		volumeName = G4TransportationManager::GetTransportationManager()
							->GetNavigatorForTracking()
							->LocateGlobalPointAndSetup(pTrack->GetPosition())
							->GetName();
		G4int Z = ion->GetAtomicNumber();
		G4double excitationEnergy = ion->GetExcitationEnergy();
		G4int eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();

		if (lifetime > 1e18 * second) lifetime = -1;  // too long-lived
		if (lifetime > 0 || excitationEnergy > 0 || (Z > 30 && Z < 40)) {
			G4int A = ion->GetAtomicMass();
			G4ThreeVector position = pTrack->GetPosition();
			m_pAnalysisManager->FillNeutronCaptureInSave(particleName, processName, A,
														 Z, position, volumeName,
														 eventID, timeP);
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

