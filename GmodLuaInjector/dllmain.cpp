#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
#include <atomic>
#include <mutex>

#include "Console.h"
#include "Color.h"
#include "ICVar.h"
#include "CLuaShared.h"
#include "Hooking.h"
#include "Utils.h"
#include "GUI.h"
#include "Executor.h"

#if _WIN64
#define ConColorMsg "?ConColorMsg@@YAXAEBVColor@@PEBDZZ"
#else
#define ConColorMsg "?ConColorMsg@@YAXABVColor@@PBDZZ"
#endif

typedef __int64(__fastcall* RunStringEx)(PVOID _this, const char* filename, const char* path, const char* stringToRun, bool run, bool printErrors, bool dontPushErrors, bool noReturns);

// Globals
std::atomic<bool> g_Running(true);
std::mutex g_unloadMutex;
bool g_unloaded = false;
HMODULE g_hModule = nullptr;

RunStringEx oRunStringEx;
PVOID cLuaInterface = nullptr;
CCvar* cvarInterface = nullptr;
std::string cvarName = "";
std::string toRun = "";
std::string lastFileName = "";

// Hook function
bool __fastcall hkRunStringEx(PVOID _this,
#ifndef _WIN64
    void*,
#endif
    const char* filename, const char* path, const char* stringToRun, bool run, bool printErrors, bool dontPushErrors, bool noReturns)
{
    if (!strcmp(filename, "LuaCmd") || !strcmp(filename, "RunString(Ex)") || !strlen(filename))
        return oRunStringEx(_this, filename, path, stringToRun, run, printErrors, dontPushErrors, noReturns);

    lastFileName = filename;

    oRunStringEx(_this, filename, path, toRun.c_str(), run, printErrors, dontPushErrors, noReturns);

    uintptr_t cvar = (uintptr_t)cvarInterface->FindVar(cvarName.c_str());
    if (!cvar)
        return oRunStringEx(_this, filename, path, stringToRun, run, printErrors, dontPushErrors, noReturns);

    std::string ip = CVarStr(cvar);
    Fix(ip);
    if (ip.find(" - loopback") != std::string::npos)
        return oRunStringEx(_this, filename, path, stringToRun, run, printErrors, dontPushErrors, noReturns);

    std::string SavePath = "C:/GaztoofScriptHook/" + ip + "/" + std::string(filename);
    Sanitize(SavePath);
    StrToAscii(SavePath);
    std::string extension = ToLower(GetExtension(SavePath));
    if (extension == ".lua")
    {
        CreateDirectoryRec(SavePath);
        std::ofstream outfile(SavePath);
        outfile << stringToRun << std::endl;
        PrintWithPrefix("Downloading file " + std::string(filename), Color(0, 145, 255));
    }

    return oRunStringEx(_this, filename, path, stringToRun, run, printErrors, dontPushErrors, noReturns);
}

void RestoreHooks()
{
    if (cLuaInterface && oRunStringEx)
    {
        PVOID* VMT = *(PVOID**)cLuaInterface;
        DWORD oldProt = 0;
        VirtualProtect(&VMT[111], sizeof(PVOID), PAGE_EXECUTE_READWRITE, &oldProt);
        VMT[111] = (PVOID)oRunStringEx;
        VirtualProtect(&VMT[111], sizeof(PVOID), oldProt, &oldProt);
    }
}

void ShutdownConsole()
{
#ifdef _DEBUG
    fflush(stdout); fflush(stdin);
    fclose(stdout); fclose(stdin);
    FreeConsole();
#endif
}

void ShutdownGUIAndResources()
{
    if (oEndScene) {
        kiero::unbind(42);
        oEndScene = nullptr;
    }

    // Restore original WndProc
    if (window && oWndProc) {
        SetWindowLongPtrA(window, GWL_WNDPROC, (LONG_PTR)oWndProc);
        oWndProc = nullptr;
    }

    // Shutdown ImGui
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void RequestUnload()
{
    std::thread([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        RestoreHooks();
        ShutdownGUIAndResources();
        ShutdownConsole();
        FreeLibraryAndExitThread(g_hModule, 0);
        }).detach();
}

void SaveSettings()
{
    std::filesystem::create_directories("C:\\GaztoofScriptHook");
    
    std::ofstream file("C:\\GaztoofScriptHook\\Settings.ini", std::ios::out | std::ios::trunc);
    if (!file.is_open()) return;
    
    file << std::boolalpha;
    file << "keepCmdPrompt=" << keepCmdPrompt << "\n";
    
    auto writeColor = [&](const char* name, const ImVec4& c)
        {
            file << std::fixed << std::setprecision(3);
            file << name << "=" << c.x << "," << c.y << "," << c.z << "," << c.w << "\n";
        };

    writeColor("textColor", textColor);
    writeColor("textDisabledColor", textDisabledColor); 
    writeColor("windowBgColor", windowBgColor); 
    writeColor("popupBgColor", popupBgColor); 
    writeColor("borderColor", borderColor); 
    writeColor("borderShadowColor", borderShadowColor); 
    writeColor("frameBgColor", frameBgColor); 
    writeColor("frameBgHoveredColor", frameBgHoveredColor);
    writeColor("frameBgActiveColor", frameBgActiveColor); 
    writeColor("headerBgColor", headerBgColor); 
    writeColor("titleBgCollapsedColor", titleBgCollapsedColor); 
    writeColor("titleBgActiveColor", titleBgActiveColor); 
    writeColor("menuBarBgColor", menuBarBgColor);
    writeColor("scrollbarBgColor", scrollbarBgColor);
    writeColor("scrollbarGrabColor", scrollbarGrabColor);
    writeColor("scrollbarGrabHoveredColor", scrollbarGrabHoveredColor);
    writeColor("scrollbarGrabActiveColor", scrollbarGrabActiveColor);
    writeColor("checkMarkColor", checkMarkColor);
    writeColor("sliderGrabColor", sliderGrabColor);
    writeColor("sliderGrabActiveColor", sliderGrabActiveColor);
    writeColor("buttonColor", buttonColor);
    writeColor("buttonHoveredColor", buttonHoveredColor);
    writeColor("buttonActiveColor", buttonActiveColor);
    writeColor("headerColor", headerColor);
    writeColor("headerHoveredColor", headerHoveredColor);
    writeColor("headerActiveColor", headerActiveColor);
    writeColor("resizeGripColor", resizeGripColor);
    writeColor("resizeGripHoveredColor", resizeGripHoveredColor);
    writeColor("resizeGripActiveColor", resizeGripActiveColor);
    writeColor("plotLinesColor", plotLinesColor);
    writeColor("plotLinesHoveredColor", plotLinesHoveredColor);
    writeColor("plotHistogramColor", plotHistogramColor);
    writeColor("plotHistogramHoveredColor", plotHistogramHoveredColor);
    writeColor("textSelectedBgColor", textSelectedBgColor);
    writeColor("modalWindowDarkeningColor", modalWindowDarkeningColor);
    
    file.close();
} 

void LoadSettings() 
{
    std::ifstream file("C:\\GaztoofScriptHook\\Settings.ini"); if (!file.is_open()) 
    {
        SaveSettings();
        return;
    } 
    
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty()) continue;
        auto pos = line.find('=');
        if (pos == std::string::npos)
            continue; std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        auto readColor = [&](std::string val, ImVec4& c)
            {
                sscanf_s(val.c_str(), "%f,%f,%f,%f", &c.x, &c.y, &c.z, &c.w);
            };
        if (key == "keepCmdPrompt")
        {
            std::string lower = value;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            keepCmdPrompt = (lower == "true" || lower == "1");
        }
        else if (key == "textColor") readColor(value, textColor);
        else if (key == "textDisabledColor") readColor(value, textDisabledColor);
        else if (key == "windowBgColor") readColor(value, windowBgColor);
        else if (key == "popupBgColor") readColor(value, popupBgColor);
        else if (key == "borderColor") readColor(value, borderColor);
        else if (key == "borderShadowColor") readColor(value, borderShadowColor);
        else if (key == "frameBgColor") readColor(value, frameBgColor);
        else if (key == "frameBgHoveredColor") readColor(value, frameBgHoveredColor);
        else if (key == "frameBgActiveColor") readColor(value, frameBgActiveColor);
        else if (key == "headerBgColor") readColor(value, headerBgColor);
        else if (key == "titleBgCollapsedColor") readColor(value, titleBgCollapsedColor);
        else if (key == "titleBgActiveColor") readColor(value, titleBgActiveColor);
        else if (key == "menuBarBgColor") readColor(value, menuBarBgColor);
        else if (key == "scrollbarBgColor") readColor(value, scrollbarBgColor);
        else if (key == "scrollbarGrabColor") readColor(value, scrollbarGrabColor);
        else if (key == "scrollbarGrabHoveredColor") readColor(value, scrollbarGrabHoveredColor);
        else if (key == "scrollbarGrabActiveColor") readColor(value, scrollbarGrabActiveColor);
        else if (key == "checkMarkColor") readColor(value, checkMarkColor);
        else if (key == "sliderGrabColor") readColor(value, sliderGrabColor);
        else if (key == "sliderGrabActiveColor") readColor(value, sliderGrabActiveColor);
        else if (key == "buttonColor") readColor(value, buttonColor);
        else if (key == "buttonHoveredColor") readColor(value, buttonHoveredColor);
        else if (key == "buttonActiveColor") readColor(value, buttonActiveColor);
        else if (key == "headerColor") readColor(value, headerColor);
        else if (key == "headerHoveredColor") readColor(value, headerHoveredColor); 
        else if (key == "headerActiveColor") readColor(value, headerActiveColor); 
        else if (key == "resizeGripColor") readColor(value, resizeGripColor); 
        else if (key == "resizeGripHoveredColor") readColor(value, resizeGripHoveredColor); 
        else if (key == "resizeGripActiveColor") readColor(value, resizeGripActiveColor); 
        else if (key == "plotLinesColor") readColor(value, plotLinesColor); 
        else if (key == "plotLinesHoveredColor") readColor(value, plotLinesHoveredColor); 
        else if (key == "plotHistogramColor") readColor(value, plotHistogramColor); 
        else if (key == "plotHistogramHoveredColor") readColor(value, plotHistogramHoveredColor); 
        else if (key == "textSelectedBgColor") readColor(value, textSelectedBgColor); 
        else if (key == "modalWindowDarkeningColor") readColor(value, modalWindowDarkeningColor); 
    } 
    
    file.close(); 
    
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = textColor; 
    style.Colors[ImGuiCol_TextDisabled] = textDisabledColor;
    style.Colors[ImGuiCol_WindowBg] = windowBgColor; 
    style.Colors[ImGuiCol_PopupBg] = popupBgColor; 
    style.Colors[ImGuiCol_Border] = borderColor; 
    style.Colors[ImGuiCol_BorderShadow] = borderShadowColor; 
    style.Colors[ImGuiCol_FrameBg] = frameBgColor; 
    style.Colors[ImGuiCol_FrameBgHovered] = frameBgHoveredColor; 
    style.Colors[ImGuiCol_FrameBgActive] = frameBgActiveColor; 
    style.Colors[ImGuiCol_TitleBg] = headerBgColor; 
    style.Colors[ImGuiCol_TitleBgCollapsed] = titleBgCollapsedColor; 
    style.Colors[ImGuiCol_TitleBgActive] = titleBgActiveColor;
    style.Colors[ImGuiCol_MenuBarBg] = menuBarBgColor;
    style.Colors[ImGuiCol_ScrollbarBg] = scrollbarBgColor;
    style.Colors[ImGuiCol_ScrollbarGrab] = scrollbarGrabColor;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = scrollbarGrabHoveredColor;
    style.Colors[ImGuiCol_ScrollbarGrabActive] = scrollbarGrabActiveColor;
    style.Colors[ImGuiCol_CheckMark] = checkMarkColor;
    style.Colors[ImGuiCol_SliderGrab] = sliderGrabColor;
    style.Colors[ImGuiCol_SliderGrabActive] = sliderGrabActiveColor;
    style.Colors[ImGuiCol_Button] = buttonColor;
    style.Colors[ImGuiCol_ButtonHovered] = buttonHoveredColor;
    style.Colors[ImGuiCol_ButtonActive] = buttonActiveColor;
    style.Colors[ImGuiCol_Header] = headerColor;
    style.Colors[ImGuiCol_HeaderHovered] = headerHoveredColor;
    style.Colors[ImGuiCol_HeaderActive] = headerActiveColor;
    style.Colors[ImGuiCol_ResizeGrip] = resizeGripColor;
    style.Colors[ImGuiCol_ResizeGripHovered] = resizeGripHoveredColor;
    style.Colors[ImGuiCol_ResizeGripActive] = resizeGripActiveColor;
    style.Colors[ImGuiCol_PlotLines] = plotLinesColor;
    style.Colors[ImGuiCol_PlotLinesHovered] = plotLinesHoveredColor;
    style.Colors[ImGuiCol_PlotHistogram] = plotHistogramColor;
    style.Colors[ImGuiCol_PlotHistogramHovered] = plotHistogramHoveredColor;
    style.Colors[ImGuiCol_TextSelectedBg] = textSelectedBgColor;
    style.Colors[ImGuiCol_ModalWindowDarkening] = modalWindowDarkeningColor;

    if (!keepCmdPrompt) {
        ShutdownConsole();
    }
}

DWORD WINAPI FinalUnloadThread(LPVOID lpParam)
{
    HMODULE hMod = (HMODULE)lpParam;

    g_Running = false;
    RestoreHooks();
    ShutdownGUIAndResources();
    ShutdownConsole();

    Sleep(100);
    FreeLibraryAndExitThread(hMod, 0);
    return 0;
}

// Main injection logic
void Main()
{
    srand(time(nullptr));
    cvarName = RandomString(10);
    toRun = "CreateClientConVar(\"" + cvarName + "\", \"\", true, false) :SetString(GetHostName() .. \" - \" ..game.GetIPAddress())";

#ifdef _DEBUG
    if (keepCmdPrompt) {
        AllocConsole();
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        freopen_s(&f, "CONIN$", "r", stdin);
        SetConsoleTitle(L"Epic Gmod Scripthook - Coded by t.me/Gaztoof | Revamp by github.com/solal0");
    }
#endif

    fn = (MsgFn)GetProcAddress(GetModuleHandleW(L"tier0.dll"), ConColorMsg);
    PrintWithPrefix("Successfully Injected!", Color(255, 255, 0));

    InitializeGUI();

    cvarInterface = (CCvar*)GetInterface("vstdlib.dll", "VEngineCvar007");
    if (cvarInterface)
    {
        std::cout << "Found CCVar: " << cvarInterface << std::endl;
        PrintWithPrefix("Successfully found CCVar!", Color(255, 255, 0));
    }

    CLuaShared* LuaShared = (CLuaShared*)GetInterface("lua_shared.dll", "LUASHARED003");
    do {
        cLuaInterface = LuaShared->GetLuaInterface(0);
        Sleep(5);
    } while (!cLuaInterface);
    std::cout << "Found CLuaInterface: " << std::hex << cLuaInterface << std::endl;

    oRunStringEx = (RunStringEx)VMTHook((PVOID**)cLuaInterface, hkRunStringEx, 111);
    PrintWithPrefix("Successfully Hooked!", Color(255, 255, 0));

    // Re-hook loop
    while (g_Running)
    {
        cLuaInterface = LuaShared->GetLuaInterface(0);
        while (!cLuaInterface && g_Running)
        {
            cLuaInterface = LuaShared->GetLuaInterface(0);
            if (cLuaInterface)
            {
                oRunStringEx = (RunStringEx)VMTHook((PVOID**)cLuaInterface, hkRunStringEx, 111);
                PrintWithPrefix("Successfully ReHooked!", Color(255, 255, 0));
                break;
            }
            Sleep(10);
        }
        Sleep(10);
    }
}

void DrawKillDLLButton()
{
    if (ImGui::Button("Kill DLL"))
        RequestUnload();
}

// DLL entry
BOOL APIENTRY DllMain(HMODULE hModule, uintptr_t ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        g_hModule = hModule;
        std::thread(Main).detach();
    }
    return TRUE;
}
