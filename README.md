# XeSim
 Geant4 simulations of xenon-based detectors

  $G4WORKDIR/bin/Linux-g++/XeSim -d muensterTPC -p ./macros/muensterTPC/preinit.mac -c ./macros/muensterTPC/det_setup.mac -f ./macros/muensterTPC/src_optPhot_DP_S1.mac -o test_optphot.root -n 100000
  
  $G4WORKDIR/bin/Linux-g++/XeSim -d muensterTPC -p ./macros/muensterTPC/preinit.mac -c ./macros/muensterTPC/det_setup.mac -f ./macros/muensterTPC/src_optPhot_DP_S1.mac -n 1 -v
  
  $G4WORKDIR/bin/Linux-g++/XeSim -d template -p ./macros/template/preinit.mac -c ./macros/template/det_setup.mac -f ./macros/template/src_geantino.mac -n 1 -v
  
  $G4WORKDIR/bin/Linux-g++/XeSim -d template -p ./macros/template/preinit.mac -c ./macros/template/det_setup.mac -f ./macros/template/src_optPhot.mac -i
  
  