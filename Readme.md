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

**Queue run on a CPU**
1. `qsub -d . devcloud-run-cpu.sh`
2. Watch for the STDOUT output file to be created from the run.
3. `nano devcloud-run-cpu.sh.oABCDEF` to inspect the output!

**Queue run on a GPU**
1. `qsub -l nodes=1:gpu:ppn=2 -d . devcloud-run-gpu.sh`
2. Watch for the STDOUT output file to be created from the run.
3. `nano devcloud-run-gpu.sh.oABCDEF` to inspect the output!

**Queue run on an FPGA**

TODO -- running on an FPGA seems to hang. This needs some local diagnosis (with the FPGA emulator) to see what's going wrong here.

Running on an FPGA likely requires building specifically *for* the FPGA, unlike CPU/GPU building.

1. `qsub -l nodes=1:fpga_runtime:ppn=2 -d . devcloud-run-fpga.sh`
2. Watch for the STDOUT output file to be created from the run.
3. `nano devcloud-run-fpga.sh.oABCDEF` to inspect the output!

**Sample Output (CPU)**
```
########################################################################
#      Date:           Tue Feb  2 00:50:33 PST 2021
#    Job ID:           787613.v-qsvr-1.aidevcloud
#      User:           u59417
# Resources:           neednodes=1:batch:ppn=2,nodes=1:batch:ppn=2,walltime=06:00:00
########################################################################

Loading "test-fans/1-many-blade-fan.stl"...
  Read in 3648 vertices, 1216 normals, and 1216 faces.
  Mesh extents: (-5.24972 - 5.24972, -5.24972 - 5.24972, -0.282 - 0.282)
  Rescaled mesh extents: (2.5 - 7.5, 2.5 - 7.5, 4.86571 - 5.34192)
Loaded!
Selecting the DEFAULT device...
Name: Intel(R) OpenCL (Intel(R) Xeon(R) Gold 6128 CPU @ 3.40GHz)
  Device Type: cpu
  Vendor: Intel(R) Corporation (Intel(R) Corporation)
  Version: OpenCL 2.1 LINUX (OpenCL 2.1 (Build 0))
  Profile: FULL_PROFILE (FULL_PROFILE)
  0: 0 sec (in 1.32081 sec).
  50: 0.00025 sec (in 0.00417088 sec).
  100: 0.0005 sec (in 0.00464104 sec).
  150: 0.00075 sec (in 0.00452471 sec).
  200: 0.001 sec (in 0.00389265 sec).
  250: 0.00125 sec (in 0.0057376 sec).
  300: 0.0015 sec (in 0.00375028 sec).
  350: 0.00175 sec (in 0.00376327 sec).
  ...
  3000: 0.015 sec (in 0.0047184 sec).
Average exit position: (4.90548, 5.03886, 5.02328), with 2588 particles in 3000 steps.
Directional Energy Flow: (186780, 200636, 81797.1).

Mesh Mass: 1.67721
Energy Transfer Performance Score: 362590
Directionality Performance Score: 286064
Done.

########################################################################
# End of output for job 787613.v-qsvr-1.aidevcloud
# Date: Tue Feb  2 00:50:54 PST 2021
########################################################################
```

**Sample Output (GPU)**

```
########################################################################
#      Date:           Tue 02 Feb 2021 12:54:23 AM PST
#    Job ID:           787614.v-qsvr-1.aidevcloud
#      User:           u59417
# Resources:           neednodes=1:gpu:ppn=2,nodes=1:gpu:ppn=2,walltime=06:00:00
########################################################################

./a.out --device gpu
Loading "test-fans/1-many-blade-fan.stl"...
  Read in 3648 vertices, 1216 normals, and 1216 faces.
  Mesh extents: (-5.24972 - 5.24972, -5.24972 - 5.24972, -0.282 - 0.282)
  Rescaled mesh extents: (2.5 - 7.5, 2.5 - 7.5, 4.86571 - 5.34192)
Loaded!
Selecting a GPU device...
Name: Intel(R) Level-Zero (Intel(R) Graphics [0x4905])
  Device Type: gpu
  Vendor: Intel(R) Corporation (Intel(R) Corporation)
  Version: 1.0 (1.0)
  Profile: FULL_PROFILE (FULL_PROFILE)
  0: 0 sec (in 0.280207 sec).
  50: 0.00025 sec (in 0.00494955 sec).
  100: 0.0005 sec (in 0.00500337 sec).
  150: 0.00075 sec (in 0.00501407 sec).
  200: 0.001 sec (in 0.0049906 sec).
  250: 0.00125 sec (in 0.00451487 sec).
  300: 0.0015 sec (in 0.00457958 sec).
  350: 0.00175 sec (in 0.00506785 sec).
  ...
  3000: 0.015 sec (in 0.00510159 sec).
Average exit position: (4.90548, 5.03886, 5.02328), with 2588 particles in 3000 steps.
Directional Energy Flow: (186780, 200636, 81796.8).

Mesh Mass: 1.67721
Energy Transfer Performance Score: 362590
Directionality Performance Score: 286063
Done.

########################################################################
# End of output for job 787614.v-qsvr-1.aidevcloud
# Date: Tue 02 Feb 2021 12:54:43 AM PST
########################################################################
```

**Sample Output (FPGA)**

In process.

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
**Setup**
1. `ssh devcloud`
2. `git clone https://github.com/GuMiner/DPC-experiments`
3. `cd DPC-experiments/DPCDemo`
4. `nano SimConstants.h` and set *ENABLE_GUI* to 0

**Queue build**
1. `qsub -d . devcloud-build.sh`
- If this is running into problems, locally run `devcloud-build.sh` or `make all`
2. `watch -n 1 qstat -n -1`

## Inspiration
This application was inspired by the [The Great Cross Architecture Challenge](https://www.codeproject.com/Competitions/1098/The-Great-Cross-Architecture-Challenge). 