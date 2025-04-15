#include <G4UnitsTable.hh>
#include <G4VVisManager.hh>
#include <G4Circle.hh>
#include <G4Colour.hh>
#include <G4VisAttributes.hh>

#include "G4SystemOfUnits.hh"
#include "XeSimPhotoDetHit.hh"

G4Allocator<XeSimPhotoDetHit> XeSimPhotoDetHitAllocator;

G4int XeSimPhotoDetHit::operator==(const XeSimPhotoDetHit &hXeSimPhotoDetHit) const {
	return ((this == &hXeSimPhotoDetHit) ? (1) : (0));
}

void XeSimPhotoDetHit::Draw()
{
    //G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    
    //if(pVVisManager)
    //{
    //    G4Circle hCircle(m_hPosition);
    //    G4Colour hColour(1.000, 0.973, 0.184);
    //    G4VisAttributes hVisAttributes(hColour);
    //    
    //    hCircle.SetScreenSize(0.1);
    //    hCircle.SetFillStyle(G4Circle::filled);
    //    hCircle.SetVisAttributes(hVisAttributes);
    //    pVVisManager->Draw(hCircle);
    //}
}

void XeSimPhotoDetHit::Print()
{
	G4cout << "PhotoDet hit ---> " 
		<< "PhotoDet#" << m_iPhotoDetNb
		<< " Position: " << m_hPosition.x()/mm
		<< " " << m_hPosition.y()/mm
		<< " " << m_hPosition.z()/mm
		<< " mm"
		<< " Time: " << m_dTime/s << " s" << G4endl;
}

