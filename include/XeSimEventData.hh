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
    string m_fPrimaryVolume;
};  

#endif

