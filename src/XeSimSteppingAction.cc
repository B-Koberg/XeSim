#include <string.h>
#include <cmath>
#include <iostream>

#include "G4SteppingManager.hh"
#include <G4SystemOfUnits.hh>
#include <G4EventManager.hh>
#include <G4Event.hh>

#include "XeSimAnalysisManager.hh"
#include "XeSimSteppingAction.hh"

XeSimSteppingAction::XeSimSteppingAction(XeSimAnalysisManager *pAnalysisManager) {
	m_pAnalysisManager = pAnalysisManager;
}

XeSimSteppingAction::~XeSimSteppingAction() { }

void XeSimSteppingAction::UserSteppingAction(const G4Step *aStep) {
	m_pAnalysisManager->Step(aStep);

	G4int eventID = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
	G4int trackID = aStep->GetTrack()->GetTrackID();
	particle = aStep->GetTrack()->GetDefinition()->GetParticleName();
	G4int particlePDGcode = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
	G4float xP = aStep->GetPostStepPoint()->GetPosition().x();
	G4float yP = aStep->GetPostStepPoint()->GetPosition().y();
	G4float zP = aStep->GetPostStepPoint()->GetPosition().z();
	//G4float rP = sqrt(xP*xP+yP*yP+zP*zP);
	G4float eP = aStep->GetPostStepPoint()->GetKineticEnergy();
	G4float timeP = aStep->GetPostStepPoint()->GetGlobalTime();
	G4float eDep = aStep->GetTotalEnergyDeposit();

	// Direction of the particle Pre
	G4float preMomModulo = sqrt(pow(aStep->GetPreStepPoint()->GetMomentum().x(), 2) +
								pow(aStep->GetPreStepPoint()->GetMomentum().y(), 2) +
								pow(aStep->GetPreStepPoint()->GetMomentum().z(), 2));
	G4ThreeVector preDirection(
					aStep->GetPreStepPoint()->GetMomentum().x() / preMomModulo,
					aStep->GetPreStepPoint()->GetMomentum().y() / preMomModulo,
					aStep->GetPreStepPoint()->GetMomentum().z() / preMomModulo);

	// Direction of the particle Post
	G4float MomModulo = sqrt(pow(aStep->GetPostStepPoint()->GetMomentum().x(), 2) +
							pow(aStep->GetPostStepPoint()->GetMomentum().y(), 2) +
							pow(aStep->GetPostStepPoint()->GetMomentum().z(), 2));
	G4ThreeVector direction(aStep->GetPostStepPoint()->GetMomentum().x() / MomModulo,
							aStep->GetPostStepPoint()->GetMomentum().y() / MomModulo,
							aStep->GetPostStepPoint()->GetMomentum().z() / MomModulo);

	// variables to check secondaries
	G4int fNSecAtRestDoIt;
	G4int fNSecAlongStepDoIt;
	G4int fNSecPostStepDoIt;
	G4int totalSecThisStep;
	G4TrackVector *vectorPartSec;
	G4String nameSec;
	G4int totalSec;
	G4String finProc;

	//if (particle =="opticalphoton") {
	//	aStep->GetTrack()->SetTrackStatus(fStopAndKill);
	//}
	//if (particle =="e-") {
	//	aStep->GetTrack()->SetTrackStatus(fStopAndKill);
	//}

	if (aStep->GetTrack()->GetNextVolume()) {
		// Example to track particles entering the template detector
		// exclude optical photons and take all other particles
		if (particle != "opticalphoton" &&
			(aStep->GetTrack()->GetVolume()->GetName() == "Lab" &&
			aStep->GetTrack()->GetNextVolume()->GetName() == "GXePTFETop")) {
			// G4cout << particle << " inside the LNGS Hall at " << xP/cm << " " <<
			// yP/cm << " " << zP/cm << " cm, with energy " << eP/MeV << " MeV" << " and
			// Pz = " <<  aStep->GetPreStepPoint()->GetMomentum().z()/MeV << " MeV/c "
			// << G4endl;

			m_pAnalysisManager->FillParticleInSave(
							1, // 1==Particle entering the template detector
							"entering GXePTFETop from Lab",
							particle, aStep->GetPostStepPoint()->GetPosition(),
							direction, eP, timeP, trackID, eventID);
		}

		// Example for neutrons entering the template detector
		if (particle == "neutron" && aStep->GetTrack()->GetNextVolume()->GetName() == "Water") {
			
			// Get process at the end of the step
			if (aStep->GetPostStepPoint()->GetProcessDefinedStep()) {
				finProc = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
			} else {
				finProc = "nothing";
			}

			// determine if it is a neutron capture on Boron or Oxygen
			// (G4 sees it as an inelastic interaction)

			fNSecAtRestDoIt = 0;
			fNSecAlongStepDoIt = 0;
			fNSecPostStepDoIt = 0;
			totalSecThisStep = 0;

			fNSecAtRestDoIt = fpSteppingManager->GetfN2ndariesAtRestDoIt();
			fNSecAlongStepDoIt = fpSteppingManager->GetfN2ndariesAlongStepDoIt();
			fNSecPostStepDoIt = fpSteppingManager->GetfN2ndariesPostStepDoIt();
			totalSecThisStep = fNSecAtRestDoIt + fNSecAlongStepDoIt + fNSecPostStepDoIt;
			vectorPartSec = fpSteppingManager->GetfSecondary();
			totalSec = vectorPartSec->size();

			G4int flagCaptureInBoron = 0;
			G4int flagCaptureInOxygen = 0;

			if (finProc == "neutronInelastic") {
				G4int flagAlpha = 0;
				G4int flagLitium = 0;
				G4int flagCarbon = 0;

				// loop over secondary particles to look for alpha && Li7
				for (int i = totalSec - totalSecThisStep; i < totalSec; i++) {
					nameSec = (*vectorPartSec)[i]->GetDefinition()->GetParticleName();
					if (nameSec == "alpha") flagAlpha = 1;
					if (nameSec.find("Li7") == 0) flagLitium = 1;
					if (nameSec.find("C13") == 0) flagCarbon = 1;
				}
				if (flagAlpha && flagLitium) flagCaptureInBoron = 1;
				if (flagAlpha && flagCarbon) flagCaptureInOxygen = 1;
			}

			if(finProc == "nCapture" || flagCaptureInBoron || flagCaptureInOxygen ) {
				m_pAnalysisManager->FillParticleInSave(
									100, "neutron capture in Water",
									particle, aStep->GetPostStepPoint()->GetPosition(),
									direction, eP, timeP, trackID, eventID);

				for(int i=totalSec-totalSecThisStep; i<totalSec; i++)   {
					nameSec = (*vectorPartSec)[i]->GetDefinition()->GetParticleName();
					if(nameSec == "gamma"){
						m_pAnalysisManager->FillParticleInSave(101, "gamma after n-capture",
											particle, aStep->GetPostStepPoint()->GetPosition(),
											direction, (*vectorPartSec)[i]->GetKineticEnergy(),
											timeP, trackID, eventID);
					}
					if (nameSec == "deuteron") {
						m_pAnalysisManager->FillParticleInSave(
											102, "n-capture on H",
											particle, aStep->GetPostStepPoint()->GetPosition(),
											direction, eP, timeP, trackID, eventID);
					}
					if (nameSec.find("Gd") == 0){
						m_pAnalysisManager->FillParticleInSave(
											103, "n-capture on Gd",
											particle, aStep->GetPostStepPoint()->GetPosition(),
											direction, eP, timeP, trackID, eventID);
					}
					if (flagCaptureInOxygen) {
						m_pAnalysisManager->FillParticleInSave(
											104, "n-capture on O",
											particle, aStep->GetPostStepPoint()->GetPosition(),
											direction, eP, timeP, trackID, eventID);
					}
				}
			}
		}
	}
}
