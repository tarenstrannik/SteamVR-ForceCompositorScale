/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details. */

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <ctime>
#include "openvr.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

const char* kAppKey                = "tarenstrannik.ForceCompositorScale";
const char* kManifestFileName     = "\\manifest.vrmanifest";
const char* kSectionSteamVR       = "steamvr";

const char* kKeyCompositorScaleOverrideValue  = "compositorResolutionSSOverrideValue";

const char* kSectionGpuSpeed      = "GpuSpeed";
const char* kGpuSpeedKeyFormat = "gpuSpeed%d";
const char* kKeyHorsepower        = "gpuSpeedHorsepower";

std::ofstream g_logFile;

void InitializeLogging();
void LogMessage(const std::string& message);
void LogReadValue(const std::string& section, const std::string& key, const std::string& value, const std::string& type);
void LogWriteValue(const std::string& section, const std::string& key, const std::string& value, const std::string& type);

int main()
{
    InitializeLogging();
    LogMessage("=== SteamVR Force Compositor Scale Started ===");
    
    vr::EVRInitError vr_error;
    vr::IVRSystem* vr_system = vr::VR_Init(&vr_error, vr::VRApplication_Background);

    if (!vr_system)
    {
        std::string errorMsg = "Failed to load OpenVR: " + std::string(vr::VR_GetVRInitErrorAsEnglishDescription(vr_error));
        LogMessage(errorMsg);
        printf("Failed to load OpenVR: %s", vr::VR_GetVRInitErrorAsEnglishDescription(vr_error));
        return -1;
    }
    
    LogMessage("OpenVR initialized successfully");

    if (!vr::VRApplications()->IsApplicationInstalled(kAppKey))
    {
        LogMessage("Application not installed, registering...");
        DWORD length = GetCurrentDirectory(0, NULL);
        char* path   = new char[length];
        length       = GetCurrentDirectory(length, path);

        if (length != 0)
        {
            std::string path_str(path);
            path_str.append(kManifestFileName);
            LogMessage("Manifest path: " + path_str);

            vr::EVRApplicationError app_error;
            app_error = vr::VRApplications()->AddApplicationManifest(path_str.c_str());

            if (app_error == vr::VRApplicationError_None)
            {
                vr::VRApplications()->SetApplicationAutoLaunch(kAppKey, true);
                LogMessage("Application registered and auto-launch enabled");
            }
            else
            {
                LogMessage("Failed to register application, error: " + std::to_string(app_error));
            }
        }

        delete[] path;
    }
    else
    {
        LogMessage("Application already installed");
    }

    int compositorScale = vr::VRSettings()->GetInt32(kSectionSteamVR, kKeyCompositorScaleOverrideValue);
    LogReadValue(kSectionSteamVR, kKeyCompositorScaleOverrideValue, std::to_string(compositorScale), "int32");

    if(compositorScale == 0)
    {
        vr::VRSettings()->SetInt32(kSectionSteamVR, kKeyCompositorScaleOverrideValue, compositorScale);
        LogMessage("Manual compositor scale override is disabled, exiting");
        return 0;
    }

   if(compositorScale<30)
    {
        compositorScale = 30;
        LogMessage("Compositor scale can not be less then 30%! Setting it to minimum possible value");
        vr::VRSettings()->SetInt32(kSectionSteamVR, kKeyCompositorScaleOverrideValue, compositorScale);
        LogWriteValue(kSectionSteamVR, kKeyCompositorScaleOverrideValue, std::to_string(compositorScale), "int32");
    }

    if(compositorScale>225)
    {
        compositorScale = 225;
        LogMessage("Compositor scale can not be more then 225%! Setting it to maximal possible value");
        vr::VRSettings()->SetInt32(kSectionSteamVR, kKeyCompositorScaleOverrideValue, compositorScale);
        LogWriteValue(kSectionSteamVR, kKeyCompositorScaleOverrideValue, std::to_string(compositorScale), "int32");
    }

    vr::Compositor_BenchmarkResults benchmarkResults;
    vr::VRCompositor()->GetCompositorBenchmarkResults(&benchmarkResults, sizeof(vr::Compositor_BenchmarkResults));

    float hmdRecommendedMegaPixelsPerSecond = benchmarkResults.m_flHmdRecommendedMegaPixelsPerSecond;
    LogMessage("HMD Recommended MPPS: " + std::to_string(hmdRecommendedMegaPixelsPerSecond));

    int calculatedHorsepower = (int)(hmdRecommendedMegaPixelsPerSecond*compositorScale/100.0f);    
    LogMessage("Calculated GPU Horsepower: " + std::to_string(calculatedHorsepower));

    char buffer[16];
    for (int i = 0; i < 10; ++i)
    {
        sprintf_s(buffer, 16, kGpuSpeedKeyFormat, i);
        vr::VRSettings()->SetInt32(kSectionGpuSpeed, buffer, calculatedHorsepower);
        LogWriteValue(kSectionGpuSpeed, buffer, std::to_string(calculatedHorsepower), "int32");
    }
    vr::VRSettings()->SetInt32(kSectionGpuSpeed, kKeyHorsepower, calculatedHorsepower);
    
    LogWriteValue(kSectionGpuSpeed, kKeyHorsepower, std::to_string(calculatedHorsepower), "int32");

    LogMessage("Settings written to SteamVR");
    vr::VR_Shutdown();
    LogMessage("=== SteamVR Force Compositor Scale Finished ===");

    return 0;
}

void InitializeLogging()
{
    char exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    
    std::string logPath(exePath);
    size_t lastDot = logPath.find_last_of('.');
    if (lastDot != std::string::npos) {
        logPath = logPath.substr(0, lastDot) + ".log";
    } else {
        logPath += ".log";
    }
    
    const long maxLogSize = 1024 * 1024; // 1 MB
    std::ifstream checkFile(logPath, std::ios::ate | std::ios::binary);
    if (checkFile.is_open()) {
        long fileSize = checkFile.tellg();
        checkFile.close();
        
        if (fileSize > maxLogSize) {
            remove(logPath.c_str());
        }
    }

    g_logFile.open(logPath, std::ios::app);
    if (g_logFile.is_open()) {
        time_t now = time(0);
        char* timeStr = ctime(&now);
 
        if (timeStr && strlen(timeStr) > 0) {
            timeStr[strlen(timeStr) - 1] = '\0';
        }
        g_logFile << "\n\n[" << timeStr << "] Log session started" << std::endl;
    }
}

void LogMessage(const std::string& message)
{
    if (g_logFile.is_open()) {
        time_t now = time(0);
        char timeBuffer[20];
        strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", localtime(&now));
        g_logFile << "[" << timeBuffer << "] " << message << std::endl;
        g_logFile.flush();
    }
}

void LogReadValue(const std::string& section, const std::string& key, const std::string& value, const std::string& type)
{
    LogMessage("READ  [" + section + "] " + key + " = " + value + " (" + type + ")");
}

void LogWriteValue(const std::string& section, const std::string& key, const std::string& value, const std::string& type)
{
    LogMessage("WRITE [" + section + "] " + key + " = " + value + " (" + type + ")");
}