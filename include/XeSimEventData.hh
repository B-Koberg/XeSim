#ifndef __XeSimEventData_H__
#define __XeSimEventData_H__

#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TParameter.h>
#include <TDirectory.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

class XeSimEventData {
public:
	 XeSimEventData();
	~XeSimEventData();

public:
	void Clear();

public:
	int m_iEventId;
	int m_iNParticle;
    int m_iNbPhotoDetHits;
    vector<int> *m_pPhotoDetHits;
    vector<int> *m_pPhotoDetHitID;
    vector<double> *m_pPhotoDetHitTime;
    vector<float> *m_pPhotoDetHitEnergy;
    vector<float> *m_pPhotoDetHitTheta;
    vector<float> *m_pPhotoDetHitPhi;
    vector<float> *m_pPhotoDetHitX;
    vector<float> *m_pPhotoDetHitY;
    vector<float> *m_pPhotoDetHitZ;
	float m_fTotalEnergyDeposited;
	int m_iNbSteps;
	vector<int> *m_pTrackId;
	vector<int> *m_pParentId;
	vector<string> *m_pParticleType;
	vector<int> *m_pParticleTypeID;
	vector<string> *m_pParentType;
	vector<int> *m_pParentTypeID;
	vector<string> *m_pCreatorProcess;
	vector<int> *m_pCreatorProcessType;
	vector<int> *m_pCreatorProcessSubType;
	vector<string> *m_pDepositingProcess;
	vector<int> *m_pDepositingProcessType;
	vector<int> *m_pDepositingProcessSubType;
	vector<float> *m_pX;
	vector<float> *m_pY;
	vector<float> *m_pZ;
	vector<float> *m_pEnergyDeposited;
	vector<float> *m_pKineticEnergy;
	vector<double> *m_pTime;
	vector<string> *m_pPrimaryParticleType;
	vector<int> *m_pPrimaryParticleTypeID;
	float m_fPrimaryEnergy;
	float m_fPrimaryX;
	float m_fPrimaryY;
	float m_fPrimaryZ;	
	float m_fPrimaryCx;
	float m_fPrimaryCy;
	float m_fPrimaryCz;
    string m_fPrimaryVolume;
	ULong64_t m_fPrimaryVolumeHash;

	int m_iNSave;
	vector<int> *m_pSave_flag;
	vector<string> *m_pSave_type;
	vector<string> *m_pSave_desc;
	vector<float> *m_pSave_x;
	vector<float> *m_pSave_y;
	vector<float> *m_pSave_z;
	vector<float> *m_pSave_cx;
	vector<float> *m_pSave_cy;
	vector<float> *m_pSave_cz;
	vector<float> *m_pSave_e;
	vector<float> *m_pSave_t;
	vector<int> *m_pSave_number;
	vector<int> *m_pSave_trkid;

	// For Storing Information about Activation Through Neutron Capture
	int m_iNAct;
	vector<string> *m_pNAct_process_name;
	vector<int> *m_pNAct_process_category;
	vector<int> *m_pNAct_process_ID;
	vector<string> *m_pNAct_volume;
	vector<ULong64_t> *m_pNAct_volume_hash;
	vector<int> *m_pNAct_particle_atomic_number;
	vector<int> *m_pNAct_particle_mass;
	vector<string> *m_pNAct_particle_name;
	vector<int> *m_pNAct_particle_id;
	vector<double> *m_pNAct_particle_excitationEnergy;
	vector<double> *m_pNAct_particle_lifetime;
	vector<float> *m_pNAct_x;
	vector<float> *m_pNAct_y;
	vector<float> *m_pNAct_z;
	vector<float> *m_pNAct_t;
	vector<int> *m_pNAct_eventID;
	vector<int> *m_pNAct_trackID;
	vector<int> *m_pNAct_parentID;
};  

#endif

