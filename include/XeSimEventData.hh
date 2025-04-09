#ifndef __XeSimEventData_H__
#define __XeSimEventData_H__

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
	vector<string> *m_pParentType;
	vector<string> *m_pCreatorProcess;
	vector<string> *m_pDepositingProcess;
	vector<float> *m_pX;
	vector<float> *m_pY;
	vector<float> *m_pZ;
	vector<float> *m_pEnergyDeposited;
	vector<float> *m_pKineticEnergy;
	vector<double> *m_pTime;
	vector<string> *m_pPrimaryParticleType;
	float m_fPrimaryEnergy;
	float m_fPrimaryX;
	float m_fPrimaryY;
	float m_fPrimaryZ;	
	float m_fPrimaryCx;
	float m_fPrimaryCy;
	float m_fPrimaryCz;
    string m_fPrimaryVolume;

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
	vector<string> *m_pNAct_name;
	vector<string> *m_pNAct_process;
	vector<string> *m_pNAct_volume;
	vector<int> *m_pNAct_number;
	vector<int> *m_pNAct_mass;
	vector<float> *m_pNAct_x;
	vector<float> *m_pNAct_y;
	vector<float> *m_pNAct_z;
	vector<float> *m_pNAct_t;
	vector<int> *m_pNAct_event;
};  

#endif

