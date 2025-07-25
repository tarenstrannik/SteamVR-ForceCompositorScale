# SteamVR-ForceCompositorScale
Resets the GpuSpeed settings in steamvr.vrsettings to force VR compositor work with custom resolution.

# Why
SteamVR's compositor always runs at the recommended render resolution, which is defined by integrated benchmark. And there is no standart way to fix this. 
It can create problems - compositor's resolution can be too low. Or to high - for example, in [VR mode for GTFO](https://github.com/DSprtn/GTFO_VR_Plugin) it creates problem, because game menu is drawn in compositor's mode, and it usually has higher resolution. And it leads to game hanging when switching to menu.

# What it does
This tool overwrites GpuSpeed settings in steamvr.vrsettings with value, calculated basing on desired scale of base resolution. So Steam VR composer use this overwritten value to set resolution.

# Installation
1. Download archive and extract it somewhere on your PC, where it could stay
2. Start Steam VR
3. Run SteamVR-ForceCompositorScale.exe (from extracted folder). It will register itself as overlay app and will start every time you start Steam VR

# Update from 0.0.1
1. Uninstall previous version. 
2. Install new

# Uninstallation
1. Just remove folder. overlay app will be unregistered next time you run Steam VR

# Usage
1. Tool is activated on first start and adds its' parameter to
```
   C:\Program Files (x86)\Steam\config\steamvr.vrsettings
```
2. It is in section
```
   "steamvr" : {
      ...
      "compositorResolutionSSOverrideValue" : 0,
      ...
   }
```

value 0 effectively means that tool is disabled. To enable it set int value from 30 to 225 (these are Steam VR limits) (set 100 to force compositor work on 100% SS resolution)
Value represents at which SS in % relatively to base HMD resolution composer will work. 
In the same manner as you can set manual SS override for the Steam VR in general or per app. And value exactly matches values in Steam VR settings in meaning of acting resolution.

So, for example, if you've set general steam VR SS (or some app SS) to 150% and want composer run at exactly the same resolution - you need set compositorResolutionSSOverrideValue to 150.

3. After setting necessary value - 
- (If Steam VR was open - close it)
- (HMD should be active and connected to PC - wired or wirelessly - so Steam VR can see it)
- Start SteamVR.  
- Close it.
- Start again. Value is applied and Steam VR composer is working with SS you've set

# Logging and checking
Tool log shoud be created beside the .exe file and updated every time tool is working. There you can find information about used values.
To check compositor's resolution you can
- Use VD stat overlay - it shows current SS, so you can check, that before installing tool it is different in compositor (and don't match setted in Steam VR settings) and in app.
!!! IMPORTANT !!! VD uses other principle of showing vR resolution. So 150% in VD matches 225% in Steam VR (and in tool settings) and 55% in VD matches 30% resolution in Steam VR and tool.
- use [fpsVR](https://store.steampowered.com/app/908520/fpsVR/) - there is a tool in it to detect actual render resolution.

# !!! IMPORTANT !!!
After changing scale option (or switching profile in VD or anything like this) you need to start Steam VR. Stop it. And then start again - and only then setting will be applied and used.

# Building
Just compile main.cpp and link it with openvr_api.dll.

For example: 
```
g++ -o SteamVR-ForceCompositorScale.exe main.cpp -lopenvr_api -L.
```

# License
SteamVR-ForceCompositorScale is licensed under WTFPL 2.0.  
Do what you want with it.

For the OpenVR API header and library, see LICENSE-OpenVR.txt.

# Special thanks
To [elvissteinjr](https://github.com/elvissteinjr) and [akaydev-coder](https://github.com/akaydev-coder) - for source branches and ideas for this tool.
To [AnBlackful](https://github.com/AnBlackful) - for testing and feedback
To [fpsVR dev](https://store.steampowered.com/app/908520/fpsVR/) - for information how Steam VR is working and tool can be done better

# One more advice for GTFO VR mod users
It's additionally helps (with map/menu issue) to set low dashbord render mode in Steam VR menu (or directly in steamvr. vrsettings in steamvr section)
```
"overlayRenderQuality_2" : 1
```
