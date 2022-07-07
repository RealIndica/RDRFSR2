# FidelityFx Super Resolution 2.0 for Red Dead Redemption 2

 This mod is a library for RDR2 which replaces Nvidia DLSS with AMD FidelityFX Super Resolution 2.0.
 This will not work with cracked versions of RDR2.
 
 An extended guide to help with any issues can be found on the Nexus mod description [here](https://www.nexusmods.com/reddeadredemption2/mods/1550)
 
## Installation

* Download the latest release 
* Unzip the contents to your RDR2 executable directory
* Run RDR2

## Compilation

* Clone this repo including all submodules
* Download and install the Vulkan SDK [here](https://vulkan.lunarg.com/sdk/home) and ensure VULKAN_SDK environment variable is set
* Compile the FSR 2.0 submodule in external/FidelityFX-FSR2 like it is described in their Readme https://github.com/GPUOpen-Effects/FidelityFX-FSR2#quick-start-checklist. (be sure to build DX and VK!) Note: I used the GenerateSolutionsDLL.bat but I am sure static libraries will work fine too
* Open the CyberFSR.sln with Visual Studio 2022
* Compile the entire solution
* Copy the compiled DLLs (nvngx.dll & d3d11.dll), ffx_fsr2_api_dx12_x64.dll, ffx_fsr2_api_vk_x64.dll and ffx_fsr2_api_x64.dll from the FidelityFX Directory to your RDR2 executable directory
* Run the game and set the quality in the DLSS settings
* Play the game with FSR 2.0

## Advisory
* Like all mods for Rockstar games, please avoid using this online as I am unsure if you will get banned.
