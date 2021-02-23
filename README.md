# Models - HPCS

This repository maintains the simulator models from the paper "A comparative study between Cadmium and LibCppSim". Inside each folder there is a dataset with the traces used in the paper experiments.

## To compile the versions, each folder must be located in the directory tree as appropriate for each simulator.
For example:
- Cadmium models should be into:
   - CADMIUM_ROOT / Cadmium-Simulation-Environment / DEVS-Models /
- LibCppSim models should be into:
   - LibCppSim_ROOT / sources / 
   
## Requirements:
- To compile the Cooperative Caching example in LibCppSim, it is necessary to properly install the OpenSSL library, which is used to generate the node IDs for the routing tables through a hash function. 
- For the simulator execution scripts it is necessary to have “psrecord” installed. 

### Note: 
The "pseudocode" file contains pseudo-codes of the main modules of the Cooperative Caching case study for Cadmium. 


