NOT FINISHED

# SteamVR-ForceGPUHorsepower
Resets the GpuSpeed settings in steamvr.vrsettings to force VR compositor work with custom resolution.

# Why
SteamVR's compositor always runs at the recommended render resolution, which is defined by integrated benchmark. And there is no standart way to fix this. 
It can create problems - compositor's resolution can bee too low. Or to high - for example, in vr mode for GTFO it creates problem, because game menu is drawn in compositor's mode, and it usually has higher resolution. And it leads to game hanging when switching to menu,
Another issue is with Vive HMD's like Vive Pro 2, as they don't force base resolution, but calculate it basing on the same benchmark. As a result yor game resoultion may vary very much from one session to another.

# What it does
This tool overwrites GpuSpeed settings in steamvr.vrsettings with value, calculated basing on desired resolution. So Steam VR composer (or all vr apps in case of VP2) use this overwritten value to set resolution.
There are several options to set it, they are described belowe

# Installation
1. Download archive and place it somewhere on your PC, where it could stay
2. Start steam vr
3. Run SteamVR-ForceGPUHorsePower.exe. It will register itself as overlay app and will start every time you start steam vr

# Uninstallation
1. Just remove folder. overlay app will be unregistered next time you run Steam VR

# Usage
1. First you need to activate tool. To do this edit file
```
   C:\Program Files (x86)\Steam\config\steamvr.vrsettings
```
2. Find in it section
```
   "steamvr" : {
...
```
and add there parameter 
```
"fgsh_horsepowerManualOverride" : true
```
3. 
- (If Steam VR was open - close it)
- (HMD should be active and connected to PC - wired or wirelessly - so Steam VR can see it)
- Start SteamVR.  
- Close it.
- Start again.
- ...
- PROFIT

All should work now :)

This is default mode, which forces resolution, which you've set manually for VR apps in general, to be used as compositor's resolution as well. It works ok for Meta Quest with Virtual desktop, but could be not ok for VP2 for example. Other modes and options are described below

# Logging and checking
Tool log shoud be created beside the .exe file and updated every time tool is working. There you can find information about used modes and values.
To check compositor's resolution you can
- Use VD stat overlay - it shows current SS, so you can check, that before installing tool it is different in compositor (and don't match setted in Steam VR settings) and in app.
- use [fpsVR](https://store.steampowered.com/app/908520/fpsVR/) - there is a tool there to detect actual render resolution.





Make sure SteamVR is running, then execute SteamVR-ForceCompositorScale.exe. As the changes only apply on SteamVR restart, you'll have to restart SteamVR to see the effect.

SteamVR-ForceCompositorScale will register itself as an overlay application to SteamVR and run automatically on following SteamVR launches. If you move the files of this application you'll have to repeat the first-run steps.

The compositor resolution appears to cap out at 1.5x super sample scale, so higher values do not increase the resolution any further.

## Using a custom compositor scale
You can use a custom scale value for the compositor separate from the application one. For that, add a *"supersampleScaleCompositor"* value in the *"steamvr"* section of steamvr.vrsettings.

For example:
    
    "steamvr" : {
      "supersampleManualOverride" : true,
	  "supersampleScaleCompositor" : 1.5
    }
	
Make sure to not break the JSON format or SteamVR will silently reset the entire file to default values. Backup recommended.

# Building
Just compile main.cpp and link it with openvr_api.dll.

For example: g++ -o SteamVR-ForceCompositorScale.exe main.cpp openvr_api.dll

# License
SteamVR-ForceCompositorScale is licensed under WTFPL 2.0.  
Do what you want with it.

For the OpenVR API header and library, see LICENSE-OpenVR.txt.
