# DPC-experiments
This repository hosts experiments with DPC++ (Data Parallel C++).

For more information on DPC++, see https://software.intel.com/content/www/us/en/develop/documentation/oneapi-programming-guide/top/oneapi-programming-model/data-parallel-c-dpc.html

# DPCDemo
## Purpose
DPCDemo is a particle simulator to test fan performance

https://devmesh.intel.com/projects/fan-efficiency-particle-simulation

DPCDemo is still a work in progress.
![Status Image](./status-v2.PNG "Status Image")

## Usage
1. Build (See "Building" below)
2. Run "DPCDemo.exe"
   - Specify "--device <device_type>" to use a different device. 
   - Specify "--input <input-file>" to specify a different fan to test the performance of.

### Running on DevCloud
1. Build using the "Building in DevCloud" section below.
2. `make run` to run locally.

## Building
Either download DPCDemo from (this link TODO) or build it using the steps below

### Build Locally (Windows, Visual Studio Community 2019)
1. Install the Intel oneAPI DPC++/C++ compiler
   - See https://software.intel.com/content/www/us/en/develop/documentation/get-started-with-dpcpp-compiler/top.html for more information.
2. Get 'vcpkg' to install dependent libraries
   - `git clone https://github.com/microsoft/vcpkg.git`
   - `cd vcpkg`
   - `bootstrap-vcpkg.bat`
3. Install dependent libraries using 'vcpkg' and integrate them into Visual Studio
   - `vcpkg install glew:x64-windows glfw3:x64-windows glm:x64-windows imgui:x64-windows libigl[imgui]:x64-windows magic-enum:x64-windows stb:x64-windows argparse:x64-windows`
   - `vcpkg integrate install`
4. Download this repository
   - `git clone https://github.com/GuMiner/DPC-experiments.git`
5. Open in Visual Studio, build, and run!
   - Edit `SimConstants.h` to change any settings as desired.

### Building in DevCloud 
** Setup **
1. `ssh devcloud`
2. `git clone https://github.com/GuMiner/DPC-experiments`
3. `cd DPC-experiments/DPCDemo`
4. `nano SimConstants.h` and set *ENABLE_GUI* to 0

** Queue build **
1. `qsub -l nodes=1:gpu:ppn=2 -d . devcloud-build.sh`
- This currently doesn't work in DevCloud (it performs `rm -f a.out` and exits).
- Run `chmod +x ./devcloud-build.sh` followed by `./devcloud-build.sh` locally instead.
2. `watch -n 1 qstat -n -1`

** Queue run (CPU) **
1. `qsub -d . devcloud-run-cpu.sh`
2. Watch for the STDOUT output file to be created from the run.
3. `nano devcloud-run.sh.oABCDEF` to inspect the output!

** Queue run (GPU) **
1. `qsub -l nodes=1:gpu:ppn=2 -d . devcloud-run-gpu.sh`
2. Watch for the STDOUT output file to be created from the run.
3. `nano devcloud-run.sh.oABCDEF` to inspect the output!

## Inspiration
This application was inspired by the [The Great Cross Architecture Challenge](https://www.codeproject.com/Competitions/1098/The-Great-Cross-Architecture-Challenge). 