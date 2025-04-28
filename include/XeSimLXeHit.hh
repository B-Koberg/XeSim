#ifndef __XeSimPLXEHIT_H__
#define __XeSimPLXEHIT_H__

#include <G4VHit.hh>
#include <G4THitsCollection.hh>
#include <G4Allocator.hh>
#include <G4ThreeVector.hh>

class XeSimLXeHit: public G4VHit {
public:
	G4int operator==(const XeSimLXeHit &) const;

	inline void* operator new(size_t);
	inline void  operator delete(void*);

	void Draw();
	void Print();

public:
	void SetTrackId(const G4int iTrackId) { m_iTrackId = iTrackId; };
	void SetParentId(const G4int iParentId) { m_iParentId = iParentId; };
	void SetParticleType(const G4String &hParticleType) { m_pParticleType = hParticleType; };
	void SetParticleTypeID(const G4int iParticleTypeID) { m_iParticleTypeID = iParticleTypeID; };
	void SetParentType(const G4String &hParentType) { m_pParentType = hParentType; };
	void SetParentTypeID(const G4int iParentTypeID) { m_iParentTypeID = iParentTypeID; };
	void SetCreatorProcess(const G4String &hProcessName) { m_pCreatorProcess = hProcessName; };
	void SetCreatorProcessType(const G4int iProcessType) { m_iCreatorProcessType = iProcessType; };
	void SetCreatorProcessSubType(const G4int iProcessSubType) { m_iCreatorProcessSubType = iProcessSubType; };
	void SetDepositingProcess(const G4String &hProcessName) { m_pDepositingProcess = hProcessName; };
	void SetDepositingProcessType(const G4int iProcessType) { m_iDepositingProcessType = iProcessType; };
	void SetDepositingProcessSubType(const G4int iProcessSubType) { m_iDepositingProcessSubType = iProcessSubType; };
	void SetPosition(G4ThreeVector hPosition) { m_hPosition = hPosition; };
	void SetEnergyDeposited(G4double dEnergyDeposited) { m_dEnergyDeposited = dEnergyDeposited; };
	void SetKineticEnergy(G4double dKineticEnergy) { m_dKineticEnergy = dKineticEnergy; };
	void SetPreStepEnergy(G4double dPreStepEnergy) { m_dPreStepEnergy = dPreStepEnergy; };
    void SetPostStepEnergy(G4double dPostStepEnergy) { m_dPostStepEnergy = dPostStepEnergy; };
    void SetTime(G4double dTime) { m_dTime = dTime; };

	G4int GetTrackId() const { return m_iTrackId; };
	G4int GetParentId() const { return m_iParentId; };
	const G4String &GetParticleType() const { return m_pParticleType; };
	const G4int GetParticleTypeID() const { return m_iParticleTypeID; };
	const G4String &GetParentType() const { return m_pParentType; };
	const G4int GetParentTypeID() const { return m_iParentTypeID; };
	const G4String &GetCreatorProcess() const { return m_pCreatorProcess; };
	const G4int GetCreatorProcessType() const { return m_iCreatorProcessType; };
	const G4int GetCreatorProcessSubType() const { return m_iCreatorProcessSubType; };
	const G4String &GetDepositingProcess() const { return m_pDepositingProcess; };
	const G4int GetDepositingProcessType() const { return m_iDepositingProcessType; };
	const G4int GetDepositingProcessSubType() const { return m_iDepositingProcessSubType; };
	G4ThreeVector GetPosition() const { return m_hPosition; };
	G4double GetEnergyDeposited() const { return m_dEnergyDeposited; };
	G4double GetKineticEnergy() const { return m_dKineticEnergy; };
    G4double GetPreStepEnergy() const { return m_dPreStepEnergy; };
    G4double GetPostStepEnergy() const { return m_dPostStepEnergy; };
	G4double GetTime() const { return m_dTime; };

private:
	G4int m_iTrackId;
	G4int m_iParentId;
	G4String m_pParticleType;
	G4int m_iParticleTypeID;
	G4String m_pParentType;
	G4int m_iParentTypeID;
	G4String m_pCreatorProcess;
	G4int m_iCreatorProcessType;
	G4int m_iCreatorProcessSubType;
	G4String m_pDepositingProcess;
	G4int m_iDepositingProcessType;
	G4int m_iDepositingProcessSubType;
	G4ThreeVector m_hPosition;
	G4double m_dEnergyDeposited;
	G4double m_dKineticEnergy;
    G4double m_dPreStepEnergy;
    G4double m_dPostStepEnergy;
	G4double m_dTime;
};

typedef G4THitsCollection<XeSimLXeHit> XeSimLXeHitsCollection;

extern G4Allocator<XeSimLXeHit> XeSimLXeHitAllocator;

inline void* XeSimLXeHit::operator new(size_t) {
	return((void *) XeSimLXeHitAllocator.MallocSingle());
}

inline void XeSimLXeHit::operator delete(void *pXeSimLXeHit) {
	XeSimLXeHitAllocator.FreeSingle((XeSimLXeHit*) pXeSimLXeHit);
}

#endif
