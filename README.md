# SteamVR-ForceGPUHorsepower
Resets the GpuSpeed settings in steamvr.vrsettings to force VR compositor work with custom resolution.

# Why
SteamVR's compositor always runs at the recommended render resolution, which is defined by integrated benchmark. And there is no standart way to fix this. 
It can create problems - compositor's resolution can bee too low. Or to high - for example, in [VR mode for GTFO](https://github.com/DSprtn/GTFO_VR_Plugin) it creates problem, because game menu is drawn in compositor's mode, and it usually has higher resolution. And it leads to game hanging when switching to menu.
Another issue is with Vive HMD's like Vive Pro 2, as they don't force base resolution, but calculate it basing on the same benchmark. As a result yor game resoultion may vary very much from one session to another.

# What it does
This tool overwrites GpuSpeed settings in steamvr.vrsettings with value, calculated basing on desired resolution. So Steam VR composer (or all vr apps in case of VP2) use this overwritten value to set resolution.
There are several options to set it, they are described below.

# Installation
1. Download archive and extract it somewhere on your PC, where it could stay
2. Start Steam VR
3. Run SteamVR-ForceGPUHorsePower.exe (from extracted folder). It will register itself as overlay app and will start every time you start Steam VR

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

This is default mode, which forces resolution, which you've set manually for VR apps in general, to be used as compositor's resolution as well. Tested this for Meta Quest with Virtual desktop. Other modes and options are described below

# Logging and checking
Tool log shoud be created beside the .exe file and updated every time tool is working. There you can find information about used modes and values.
To check compositor's resolution you can
- Use VD stat overlay - it shows current SS, so you can check, that before installing tool it is different in compositor (and don't match setted in Steam VR settings) and in app.
- use [fpsVR](https://store.steampowered.com/app/908520/fpsVR/) - there is a tool there to detect actual render resolution.

# Usage modes and options
Modes and settings are listed in order of priority, from higher to lower priority. All parameters should be added in steamvr.vrsettings in  "steamvr" section
1. Set exact gpu horsepowers value
```
"fgsh_horsepowerManualOverrideValue" : 1234
```
2. Set desired resolution
```
"fgsh_renderResolutionWidthOverrideValue" : 2000,
"fgsh_renderResolutionHeightOverrideValue" : 2000,
```
Important thing here! You are NOT actually setting resolution. This values will be used to calculate horsepowers, but actual resolution will be determined accordingly to you HMD proportions by Steam VR. So it's better to take some known value (from Steam VR settings) and change it proportionally.
Another important thing. I didn't tested it now, but i had VP2 in the past. And basing on experience this is recommended mode for VP2 (setting resolution here should force stable resolution for VP2 in general. I'd recommend values like 3500 to match its native resolution, taking into account distortion) 

Also here parameter 
```
"fgsh_horsepowerNormalizedCoeficientManualOverrideValue" : 1.784970906023052
```
can be used. What is this - this is empirically determined coefficient how to calculate GPU horsepowers basing on HMD render resolution and Hz. So when Steam VR will use this calculated value, it will set exactly that resolution value, which we need. But in future Steam VR benchmark's algorythm (proportion) can possibly change, so you can ovverride this coeficient, if needed. If not set - default value will be used.

3. If exact horsepowers were not set, and the same for resolution ovverride for width, or height, or both - tool then detects current HMD resolution.
   
   You can use 
```
"fgsh_resolutionScaleOverrideValue": 0.5
```
parameter to make compositor's resolution higher (or lower) then detected Steam VR resolution. If not set horsepowers will be calculated to match actual Steam VR resolution.
This is the most default scenario, which is used if only fgsh_horsepowerManualOverride is set to true and no other option set.


Here is also one more parameter, which is not used for setting GPU horsepowers, but can be usefull in case if you are not using manual ss, but prefer to let Steam decide.
Basically Steam VR may increase basic render resolution not more that for 150%. But if you want to override this behaviour, you may set 
```
"fgsh_renderTargetScaleOverrideValue": 2.5
```
to let resolution be increased to higher values, for example.

# !!! IMPORTANT !!!
After changing any option you need to start Steam VR. Stop it. And then start again - and only then setting will be applied and used.

# Building
Just compile main.cpp and link it with openvr_api.dll.

For example: 
```
g++ -o SteamVR-ForceGPUHorsePower.exe main.cpp -lopenvr_api -L.
```

# License
SteamVR-ForceCompositorScale is licensed under WTFPL 2.0.  
Do what you want with it.

For the OpenVR API header and library, see LICENSE-OpenVR.txt.

# Special thanks
To [elvissteinjr](https://github.com/elvissteinjr) and [akaydev-coder](https://github.com/akaydev-coder) - for source branches and ideas for this tool.
To [AnBlackful](https://github.com/AnBlackful) - for testing and feedback

# One more advice for GTFO VR mod users
It's additionally helps (with map/menu issue) to set low dashbord render mode in Steam VR menu (or directly in steamvr. vrsettings in steamvr section)
```
"overlayRenderQuality_2" : 1
```
