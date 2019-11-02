# XeSim
 Geant4 simulations of xenon-based detectors

## Installation instructions

You can find this package at https://github.com/l-althueser/XeSim and check out the most recent version with:

    git clone https://<username>:<password>@github.com/l-althueser/XeSim.git

Ensure ROOT (version 6) and GEANT4 (tested with G4.10.3.3) are properly setup.

Then to compile, run:

    make clean
    make -j # "-j" for parallel compilation using all available CPU

## Running instructions

You can run the code with the following command

    $G4WORKDIR/bin/Linux-g++/XeSim -d <detector> -p <preinit file>
    -c <detector setup file> -f <run macro file> -o <output .root file>
    -n <Nb of generated events>

where `-d`, `-p`, `-f` and `-n` arguments are required for the simulation to run without errors. The optional parameters `-v` and `-i` trigger the visualization and interactive modes.

### template detector
**interactive**
```
$G4WORKDIR/bin/Linux-g++/XeSim -d template -p ./macros/template/preinit.mac -c ./macros/template/det_setup.mac -f ./macros/template/src_optPhot.mac -i
```

**VRML generation**
```
$G4WORKDIR/bin/Linux-g++/XeSim -d template -p ./macros/template/preinit.mac -c ./macros/template/det_setup.mac -f ./macros/template/src_geantino.mac -n 1 -v
```

### muensterTPC
**optical photons**
```
$G4WORKDIR/bin/Linux-g++/XeSim -d muensterTPC -p ./macros/muensterTPC/preinit.mac -c ./macros/muensterTPC/det_setup.mac -f ./macros/muensterTPC/src_optPhot_DP_S1.mac -o test_optphot.root -n 100000
```

**interactive**
```
$G4WORKDIR/bin/Linux-g++/XeSim -d muensterTPC -p ./macros/muensterTPC/preinit.mac -c ./macros/muensterTPC/det_setup.mac -i
```

**VRML generation**
```
$G4WORKDIR/bin/Linux-g++/XeSim -d muensterTPC -p ./macros/muensterTPC/preinit.mac -c ./macros/muensterTPC/det_setup.mac -f ./macros/muensterTPC/src_geantino.mac -n 1 -v
```

## The output file/file format
You can simply view the generated simulation data with any version of [ROOT](https://root.cern.ch/). Just type ..
```
root
new TBrowser
```
.. and navigate to the generated `events.root` file. Now you can do the normal click and drag routine of ROOT.  

The output file has an specific file format which is described in the following.

### Top directory
| Name | type | description |  
| --- | --- | --- |
| G4VERSION | TName<string> | version of geant4 |  
| MC_TAG | TName<string> | name of the simulation toolkit (e.g. "template") |  

### TDirectory::events
| Name | type | description |  
| --- | --- | --- |
| nbevents | TParameter<int> | number of simulated events |  

### TDirectory::events/events
| Name | type | description |  
| --- | --- | --- |
| eventid | int | event number |
| nphotodethits | int | total number of hits for all photo detectors |
| photodethits | int | number of hits for each photo detector as list |
| etot | float | total G4 energy deposit in this event |
| nsteps | int | number of G4 steps |
| trackid  | int | track ID |
| type  | string | particle type |
| parentid  | int | track ID of parent |
| parenttype  | string | particle type of parent |
| creaproc  | string | process that created this particle |
| edproc  | string | process for this particular energy deposit |
| xp  | vector<float> | x coordinate of energy deposit (mm) |
| yp  | vector<float> | y coordinate of energy deposit (mm) |
| zp  | vector<float> | z coordinate of energy deposit (mm) |
| ed  | vector<float> | energy deposit (keV) |
| time  | vector<float> | timestamp of the current particle/trackid |
| type_pri  | string | particle type of primary  |
| e_pri  | vector<float> | energy of primary (keV) |
| xp_pri  | vector<float> | x coordinate of primary particle (mm) |
| yp_pri  | vector<float> | y coordinate of primary particle (mm) |
| zp_pri  | vector<float> | z coordinate of primary particle (mm) |
