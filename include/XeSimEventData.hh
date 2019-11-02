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
	vector<int> *m_pPhotoDetHits;
    int m_iNbPhotoDetHits;
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
	vector<float> *m_pTime;
	vector<string> *m_pPrimaryParticleType;
	float m_fPrimaryEnergy;
	float m_fPrimaryX;
	float m_fPrimaryY;
	float m_fPrimaryZ;	
    string m_fPrimaryVolume;
};  

#endif

