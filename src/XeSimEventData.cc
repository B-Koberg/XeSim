#include "XeSimEventData.hh"

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TParameter.h>
#include <TDirectory.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

XeSimEventData::XeSimEventData() {
	m_iEventId = 0;
	m_iNParticle = 0;
	m_iNbPhotoDetHits = 0;
    m_pPhotoDetHits = new vector<int>;
    m_pPhotoDetHitID = new vector<int>;
	m_pPhotoDetHitTime = new vector<double>;
	m_pPhotoDetHitEnergy = new vector<float>;
	m_pPhotoDetHitTheta = new vector<float>;
	m_pPhotoDetHitPhi = new vector<float>;
	//m_pPhotoDetHitVolumeName = new vector<string>;
	m_pPhotoDetHitX = new vector<float>;
	m_pPhotoDetHitY = new vector<float>;
	m_pPhotoDetHitZ = new vector<float>;
    
	m_fTotalEnergyDeposited = 0.;
	m_iNbSteps = 0;

	m_pTrackId = new vector<int>;
	m_pParentId = new vector<int>;
	m_pParticleType = new vector<string>;
	m_pParticleTypeID = new vector<int>;
	m_pParentType = new vector<string>;
	m_pParentTypeID = new vector<int>;
	m_pCreatorProcess = new vector<string>;
	m_pCreatorProcessType = new vector<int>;
	m_pCreatorProcessSubType = new vector<int>;
	m_pDepositingProcess = new vector<string>;
	m_pDepositingProcessType = new vector<int>;
	m_pDepositingProcessSubType = new vector<int>;
	m_pX = new vector<float>;
	m_pY = new vector<float>;
	m_pZ = new vector<float>;
	m_pEnergyDeposited = new vector<float>;
	m_pKineticEnergy = new vector<float>;
    
	m_pTime = new vector<double>;

	m_pPrimaryParticleType = new vector<string>;
	m_pPrimaryParticleTypeID = new vector<int>;
	m_fPrimaryEnergy = 0.;
	m_fPrimaryX = 0.;
	m_fPrimaryY = 0.;
	m_fPrimaryZ = 0.;
	m_fPrimaryCx = 0.;
	m_fPrimaryCy = 0.;
	m_fPrimaryCz = 0.;
    m_fPrimaryVolume = "";
	m_fPrimaryVolumeHash = 0;

	m_iNSave = 0;
	m_pSave_flag = new vector<int>;
	m_pSave_type = new vector<string>;
	m_pSave_desc = new vector<string>;
	m_pSave_x = new vector<float>;
	m_pSave_y = new vector<float>;
	m_pSave_z = new vector<float>;
	m_pSave_cx = new vector<float>;
	m_pSave_cy = new vector<float>;
	m_pSave_cz = new vector<float>;
	m_pSave_e = new vector<float>;
	m_pSave_t = new vector<float>;
	m_pSave_number = new vector<int>;
	m_pSave_trkid = new vector<int>;
	m_pSave_volume = new vector<string>;

	// Neutron capture
	m_iNAct = 0;
	m_pNAct_process_name = new vector<string>;
	m_pNAct_process_category = new vector<int>;
	m_pNAct_process_ID = new vector<int>;
	m_pNAct_volume = new vector<string>;
	m_pNAct_volume_hash = new vector<std::uint64_t>;
	m_pNAct_particle_name = new vector<string>;
	m_pNAct_particle_id = new vector<int>;
	m_pNAct_particle_mass = new vector<int>;
	m_pNAct_particle_atomic_number = new vector<int>;
	m_pNAct_particle_excitationEnergy = new vector<double>;
	m_pNAct_particle_lifetime = new vector<double>;
	m_pNAct_x = new vector<float>;
	m_pNAct_y = new vector<float>;
	m_pNAct_z = new vector<float>;
	m_pNAct_eventID = new vector<int>;
	m_pNAct_trackID = new vector<int>;
	m_pNAct_parentID = new vector<int>;
	m_pNAct_t = new vector<float>;
}

XeSimEventData::~XeSimEventData() {
	delete m_pPhotoDetHits;
    delete m_pPhotoDetHitID;
    delete m_pPhotoDetHitTime;
    delete m_pPhotoDetHitEnergy;
    delete m_pPhotoDetHitTheta;
    delete m_pPhotoDetHitPhi;
    delete m_pPhotoDetHitX;
    delete m_pPhotoDetHitY;
    delete m_pPhotoDetHitZ;
    
	delete m_pTrackId;
	delete m_pParentId;
	delete m_pParticleType;
	delete m_pParticleTypeID;
	delete m_pParentType;
	delete m_pParentTypeID;
	delete m_pCreatorProcess;
	delete m_pCreatorProcessType;
	delete m_pCreatorProcessSubType;
	delete m_pDepositingProcess;
	delete m_pDepositingProcessType;
	delete m_pDepositingProcessSubType;
	delete m_pX;
	delete m_pY;
	delete m_pZ;
	delete m_pEnergyDeposited;
	delete m_pKineticEnergy;
	delete m_pTime;

	delete m_pPrimaryParticleType;
	delete m_pPrimaryParticleTypeID;

	delete m_pSave_flag;
	delete m_pSave_type;
	delete m_pSave_desc;
	delete m_pSave_x;
	delete m_pSave_y;
	delete m_pSave_z;
	delete m_pSave_cx;
	delete m_pSave_cy;
	delete m_pSave_cz;
	delete m_pSave_e;
	delete m_pSave_t;
	delete m_pSave_number;
	delete m_pSave_trkid;
	delete m_pSave_volume;

	
	delete m_pNAct_volume;
	delete m_pNAct_volume_hash;
	delete m_pNAct_process_name;
	delete m_pNAct_process_category;
	delete m_pNAct_process_ID;
	delete m_pNAct_eventID;
	delete m_pNAct_trackID;
	delete m_pNAct_parentID;
	delete m_pNAct_particle_name;
	delete m_pNAct_particle_id;
	delete m_pNAct_particle_mass;
	delete m_pNAct_particle_atomic_number;
	delete m_pNAct_particle_excitationEnergy;
	delete m_pNAct_particle_lifetime;
	delete m_pNAct_x;
	delete m_pNAct_y;
	delete m_pNAct_z;
	delete m_pNAct_t;
	delete m_pNAct_particle_excitationEnergy;
}

void XeSimEventData::Clear() {
	m_iEventId = 0;
	m_iNParticle = 0;
    m_iNbPhotoDetHits = 0;
    m_pPhotoDetHits->clear();
    m_pPhotoDetHitID->clear();
    m_pPhotoDetHitTime->clear();
    m_pPhotoDetHitEnergy->clear();
    m_pPhotoDetHitTheta->clear();
    m_pPhotoDetHitPhi->clear();
    m_pPhotoDetHitX->clear();
    m_pPhotoDetHitY->clear();
    m_pPhotoDetHitZ->clear();
    
	m_fTotalEnergyDeposited = 0.0;
	m_iNbSteps = 0;

	m_pTrackId->clear();
	m_pParentId->clear();
	m_pParticleType->clear();
	m_pParticleTypeID->clear();
	m_pParentType->clear();
	m_pParentTypeID->clear();
	m_pCreatorProcess->clear();
	m_pCreatorProcessType->clear();
	m_pCreatorProcessSubType->clear();
	m_pDepositingProcess->clear();
	m_pDepositingProcessType->clear();
	m_pDepositingProcessSubType->clear();
	m_pX->clear();
	m_pY->clear();
	m_pZ->clear();
	m_pEnergyDeposited->clear();
	m_pKineticEnergy->clear();
	m_pTime->clear();

	m_pPrimaryParticleType->clear();
	m_pPrimaryParticleTypeID->clear();
	m_fPrimaryEnergy = 0.;
	m_fPrimaryX = 0.;
	m_fPrimaryY = 0.;
	m_fPrimaryZ = 0.;
	m_fPrimaryCx = 0.;
	m_fPrimaryCy = 0.;
	m_fPrimaryCz = 0.;
    m_fPrimaryVolume = "";
	m_fPrimaryVolumeHash = 0;

	m_iNSave = 0;
	m_pSave_flag->clear();
	m_pSave_type->clear();
	m_pSave_desc->clear();
	m_pSave_x->clear();
	m_pSave_y->clear();
	m_pSave_z->clear();
	m_pSave_cx->clear();
	m_pSave_cy->clear();
	m_pSave_cz->clear();
	m_pSave_e->clear();
	m_pSave_t->clear();
	m_pSave_number->clear();
	m_pSave_trkid->clear();
	m_pSave_volume->clear();

	m_iNAct = 0;
	
	m_pNAct_volume->clear();
	m_pNAct_volume_hash->clear();
	m_pNAct_process_name->clear();
	m_pNAct_process_category->clear();
	m_pNAct_process_ID->clear();
	m_pNAct_eventID->clear();
	m_pNAct_trackID->clear();
	m_pNAct_parentID->clear();
	m_pNAct_particle_name->clear();
	m_pNAct_particle_id->clear();
	m_pNAct_particle_mass->clear();
	m_pNAct_particle_atomic_number->clear();
	m_pNAct_particle_excitationEnergy->clear();
	m_pNAct_particle_lifetime->clear();
	m_pNAct_x->clear();
	m_pNAct_y->clear();
	m_pNAct_z->clear();
	m_pNAct_t->clear();
	m_pNAct_particle_excitationEnergy->clear();
}

