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

const char* kAppKey                = "tarenstrannik.ForceGPUSpeedHorsepower";
const char* kManifestFileName     = "\\manifest.vrmanifest";
const char* kSectionSteamVR       = "steamvr";

const char* kKeyManualOverrideHorsepower = "fgsh_horsepowerManualOverride";
const char* kKeyOverrideHorsepowerValue = "fgsh_horsepowerManualOverrideValue";

const char* kKeyHorsepowerNormalizedCoeficientOverrideValue = "fgsh_horsepowerNormalizedCoeficientManualOverrideValue";

const char* kKeyWidthOverrideValue = "fgsh_renderResolutionWidthOverrideValue";
const char* kKeyHeightOverrideValue = "fgsh_renderResolutionHeightOverrideValue";

const char* kKeyScaleOverrideValue = "fgsh_resolutionScaleOverrideValue";
const char* kKeyRenderTargetScaleOverrideValue = "fgsh_renderTargetScaleOverrideValue";

const char* kSectionGpuSpeed      = "GpuSpeed";
const char* kGpuSpeedKeyFormat = "gpuSpeed%d";
const char* kKeyHorsepower        = "gpuSpeedHorsepower";
const char* kKeyRenderTargetScale = "gpuSpeedRenderTargetScale";

constexpr float kHorsepowerNormalizedCoeficient = 1.784970906023052f;
constexpr int kHorsepowerNormalizationCoeficient = 1000000;

std::ofstream g_logFile;

void InitializeLogging();
void LogMessage(const std::string& message);
void LogReadValue(const std::string& section, const std::string& key, const std::string& value, const std::string& type);
void LogWriteValue(const std::string& section, const std::string& key, const std::string& value, const std::string& type);

int GetCalculatedGPUHorsepower();

int main()
{
    InitializeLogging();
    LogMessage("=== SteamVR Force GPU Horsepower Started ===");
    
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

    bool isActive = vr::VRSettings()->GetBool(kSectionSteamVR, kKeyManualOverrideHorsepower);
    LogReadValue(kSectionSteamVR, kKeyManualOverrideHorsepower, isActive ? "true" : "false", "bool");
    
    if(!isActive)
    {
        LogMessage("Manual override horsepower is disabled, exiting");
        return 0;
    }

    int calculatedHorsepower = GetCalculatedGPUHorsepower();
    if (calculatedHorsepower == 0)
    {
        LogMessage("Calculated horsepower is 0, exiting");
        return 0;
    }
    
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

    float renderTargetScaleOverride = vr::VRSettings()->GetFloat(kSectionSteamVR, kKeyRenderTargetScaleOverrideValue);
    LogReadValue(kSectionSteamVR, kKeyRenderTargetScaleOverrideValue, std::to_string(renderTargetScaleOverride), "float");
    
    if(renderTargetScaleOverride != 0.0f)
    {
        vr::VRSettings()->SetFloat(kSectionGpuSpeed, kKeyRenderTargetScale, renderTargetScaleOverride);
        LogWriteValue(kSectionGpuSpeed, kKeyRenderTargetScale, std::to_string(renderTargetScaleOverride), "float");
    }
    else
    {
        LogMessage("Render target scale override not set, skipping");
    }
    
    vr::VRSettings()->Sync(true);
    LogMessage("Settings synchronized to SteamVR");

    vr::VR_Shutdown();
    LogMessage("=== SteamVR Force GPU Horsepower Finished ===");

    return 0;
}

int GetCalculatedGPUHorsepower()
{
    LogMessage("--- Calculating GPU Horsepower ---");
    
    int gpuHorsepowerFromConfig = vr::VRSettings()->GetInt32(kSectionSteamVR, kKeyOverrideHorsepowerValue);
    LogReadValue(kSectionSteamVR, kKeyOverrideHorsepowerValue, std::to_string(gpuHorsepowerFromConfig), "int32");
    
    if(gpuHorsepowerFromConfig != 0)
	{
        LogMessage("Using manual horsepower override: " + std::to_string(gpuHorsepowerFromConfig));
        return gpuHorsepowerFromConfig;
    }

    float gpuHorsepowerCoeficient = kHorsepowerNormalizedCoeficient;
    float coefFromConfig = vr::VRSettings()->GetFloat(kSectionSteamVR, kKeyHorsepowerNormalizedCoeficientOverrideValue);
    LogReadValue(kSectionSteamVR, kKeyHorsepowerNormalizedCoeficientOverrideValue, std::to_string(coefFromConfig), "float");
    
    if(coefFromConfig != 0.0f)
    {
        gpuHorsepowerCoeficient = coefFromConfig;
        LogMessage("Using custom horsepower coefficient: " + std::to_string(gpuHorsepowerCoeficient));
    }
    else
    {
        LogMessage("Using default horsepower coefficient: " + std::to_string(gpuHorsepowerCoeficient));
    }
    
    float hmdHz = vr::VRSystem()->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_DisplayFrequency_Float);
    LogMessage("HMD Display Frequency: " + std::to_string(hmdHz) + " Hz");

    int widthFromConfig = vr::VRSettings()->GetInt32(kSectionSteamVR, kKeyWidthOverrideValue);
    int heightFromConfig = vr::VRSettings()->GetInt32(kSectionSteamVR, kKeyHeightOverrideValue);
    LogReadValue(kSectionSteamVR, kKeyWidthOverrideValue, std::to_string(widthFromConfig), "int32");
    LogReadValue(kSectionSteamVR, kKeyHeightOverrideValue, std::to_string(heightFromConfig), "int32");

    if(widthFromConfig != 0 && heightFromConfig != 0)
    {
        LogMessage("Using manual resolution override: " + std::to_string(widthFromConfig) + "x" + std::to_string(heightFromConfig));
        
        double result = static_cast<double>(widthFromConfig) *
                        static_cast<double>(heightFromConfig) *
                        static_cast<double>(hmdHz) *
                        static_cast<double>(gpuHorsepowerCoeficient);

        result /= static_cast<double>(kHorsepowerNormalizationCoeficient);
        
        LogMessage("Calculation: " + std::to_string(widthFromConfig) + " * " + std::to_string(heightFromConfig) + 
                  " * " + std::to_string(hmdHz) + " * " + std::to_string(gpuHorsepowerCoeficient) + 
                  " / " + std::to_string(kHorsepowerNormalizationCoeficient) + " = " + std::to_string(result));

        return static_cast<int>(result);
    }

    uint32_t w = 0, h = 0;
    vr::VRSystem()->GetRecommendedRenderTargetSize(&w, &h);

    int renderWidth = static_cast<int>(w);
    int renderHeight = static_cast<int>(h);
    LogMessage("Recommended render target size: " + std::to_string(renderWidth) + "x" + std::to_string(renderHeight));
 
    float customSSCoeficient = vr::VRSettings()->GetFloat(kSectionSteamVR, kKeyScaleOverrideValue);
    LogReadValue(kSectionSteamVR, kKeyScaleOverrideValue, std::to_string(customSSCoeficient), "float");
    
    if(customSSCoeficient == 0.0f)
    {
        customSSCoeficient= 1.0f;
        LogMessage("Custom resolution scale not set. Using general steam VR resolution");
    }
    else
    {
        LogMessage("Using scale override coefficient: " + std::to_string(customSSCoeficient));
    }

    double result = static_cast<double>(renderWidth) *
                    static_cast<double>(renderHeight) *
                    static_cast<double>(hmdHz) *
                    static_cast<double>(gpuHorsepowerCoeficient) *
                    static_cast<double>(customSSCoeficient);

    result /= static_cast<double>(kHorsepowerNormalizationCoeficient);
    
    LogMessage("Final calculation: " + std::to_string(renderWidth) + " * " + std::to_string(renderHeight) + 
               " * " + std::to_string(hmdHz) + " * " + std::to_string(gpuHorsepowerCoeficient) + 
               " * " + std::to_string(customSSCoeficient) + " / " + std::to_string(kHorsepowerNormalizationCoeficient) + 
               " = " + std::to_string(result));

    return static_cast<int>(result);
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