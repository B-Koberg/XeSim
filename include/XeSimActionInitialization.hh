#ifndef XeSimActionInitialization_h
#define XeSimActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "XeSimAnalysisManager.hh"
#include "XeSimPrimaryGeneratorAction.hh"

#include <string.h>    
#include <iostream>
#include <sstream>

class XeSimActionInitialization : public G4VUserActionInitialization
{
  public:
  	XeSimActionInitialization(XeSimAnalysisManager*, int, int, std::string, std::string, XeSimPrimaryGeneratorAction*);
    virtual ~XeSimActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;

  private:
  	XeSimAnalysisManager *pAnalysisManager;
  	XeSimPrimaryGeneratorAction *pPrimaryGeneratorAction;

};

#endif
