#include "XeSimEventData.hh"

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
	m_pParentType = new vector<string>;
	m_pCreatorProcess = new vector<string>;
	m_pDepositingProcess = new vector<string>;
	m_pX = new vector<float>;
	m_pY = new vector<float>;
	m_pZ = new vector<float>;
	m_pEnergyDeposited = new vector<float>;
	m_pKineticEnergy = new vector<float>;
    
	m_pTime = new vector<double>;

	m_pPrimaryParticleType = new vector<string>;
	m_fPrimaryEnergy = 0.;
	m_fPrimaryX = 0.;
	m_fPrimaryY = 0.;
	m_fPrimaryZ = 0.;
	m_fPrimaryCx = 0.;
	m_fPrimaryCy = 0.;
	m_fPrimaryCz = 0.;
    m_fPrimaryVolume = "";

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

	// Neutron capture
	m_iNAct = 0;
	m_pNAct_name = new vector<string>;
	m_pNAct_process = new vector<string>;
	m_pNAct_volume = new vector<string>;
	m_pNAct_mass = new vector<int>;
	m_pNAct_number = new vector<int>;
	m_pNAct_x = new vector<float>;
	m_pNAct_y = new vector<float>;
	m_pNAct_z = new vector<float>;
	m_pNAct_event = new vector<int>;
	m_pNAct_t = new vector<float>;
	m_pNAct_lifetime = new vector<float>;
	m_pNAct_excitation_energy = new vector<float>;
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
	delete m_pParentType;
	delete m_pCreatorProcess;
	delete m_pDepositingProcess;
	delete m_pX;
	delete m_pY;
	delete m_pZ;
	delete m_pEnergyDeposited;
	delete m_pKineticEnergy;
	delete m_pTime;

	delete m_pPrimaryParticleType;

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

	delete m_pNAct_name;
	delete m_pNAct_volume;
	delete m_pNAct_process;
	delete m_pNAct_event;
	delete m_pNAct_mass;
	delete m_pNAct_number;
	delete m_pNAct_x;
	delete m_pNAct_y;
	delete m_pNAct_z;
	delete m_pNAct_t;
	delete m_pNAct_lifetime;
	delete m_pNAct_excitation_energy;
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
	m_pParentType->clear();
	m_pCreatorProcess->clear();
	m_pDepositingProcess->clear();
	m_pX->clear();
	m_pY->clear();
	m_pZ->clear();
	m_pEnergyDeposited->clear();
	m_pKineticEnergy->clear();
	m_pTime->clear();

	m_pPrimaryParticleType->clear();
	m_fPrimaryEnergy = 0.;
	m_fPrimaryX = 0.;
	m_fPrimaryY = 0.;
	m_fPrimaryZ = 0.;
	m_fPrimaryCx = 0.;
	m_fPrimaryCy = 0.;
	m_fPrimaryCz = 0.;
    m_fPrimaryVolume = "";

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

	m_iNAct = 0;
	m_pNAct_name->clear();
	m_pNAct_volume->clear();
	m_pNAct_process->clear();
	m_pNAct_event->clear();
	m_pNAct_mass->clear();
	m_pNAct_number->clear();
	m_pNAct_x->clear();
	m_pNAct_y->clear();
	m_pNAct_z->clear();
	m_pNAct_t->clear();
	m_pNAct_lifetime->clear();
	m_pNAct_excitation_energy->clear();
}

