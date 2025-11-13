#pragma once
#include <Windows.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <filesystem>
#include <functional>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
#include "Kiero/kiero.h"
#include "Executor.h"
#include "Utils.h"
#pragma comment(lib,"d3d9.lib")

#if _WIN64
#define GWL_WNDPROC GWLP_WNDPROC
#pragma comment(lib,"libMinHook-x64.lib")
#else
#pragma comment(lib,"libMinHook-x86.lib")
#endif

typedef long(__stdcall* EndScene)(LPDIRECT3DDEVICE9);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

EndScene oEndScene = NULL;
WNDPROC oWndProc;
static HWND window = NULL;
char buf[65535];
namespace fs = std::filesystem;
static bool openMenu = false;
static bool openFileExistsPopup = false;
void RequestUnload();
void ShutdownConsole();
void LoadSettings();
void SaveSettings();

// I'm aware this hurts eyes, credits to https://discourse.dearimgui.org/t/how-to-mix-imgui-treenode-and-filesystem-to-print-the-current-directory-recursively/37
// I was just real lazy to make my own...
std::string doFileStuff()
{
	static int selection_mask = (1 << 2);
	int node_clicked = -1;
	static std::string output = "";
	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize());
	std::function<void(const fs::path&, unsigned int, unsigned int&)> functor = [&](const fs::path& path, unsigned int depth, unsigned int& idx) {
		for (auto&& p : fs::directory_iterator(path)) {
			if (fs::is_directory(p.path())) {
				using namespace std::string_literals;
				std::string s = p.path().filename().string().c_str();
				if (ImGui::TreeNodeEx((void*)(intptr_t)idx, ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick, "%s", s.c_str())) {
					functor(p, depth + 1, ++idx);
					ImGui::TreePop();
				}
			}
			else {
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ((selection_mask & (1 << idx)) ? ImGuiTreeNodeFlags_Selected : 0);
				if (depth > 0) {
					ImGui::Indent();
				}
				ImGui::TreeNodeEx((void*)(intptr_t)idx, node_flags, "%s", p.path().filename().string().c_str());
				if (ImGui::IsItemClicked() || ImGui::IsItemFocused())
				{
					node_clicked = idx;
					output = p.path().string();
				}
				if (depth > 0) {
					ImGui::Unindent();
				}
			}
			++idx;
		}
		depth -= 1;
	};
	unsigned int idx = 0u;
	functor(fs::path("C:\\GaztoofScriptHook"), 0u, idx);
	if (node_clicked != -1) {
		selection_mask = (1 << node_clicked);
	}
	ImGui::PopStyleVar();
	return output;
}
extern std::string lastFileName;
extern std::string selectedFilePath;
static bool executeAfterScript = false;
static bool keepCmdPrompt = true;

// Settings colors statics
static ImVec4 textColor = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
static ImVec4 windowBgColor = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
static ImVec4 headerBgColor = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
static ImVec4 elementBgColor = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
static ImVec4 textDisabledColor = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
static ImVec4 popupBgColor = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
static ImVec4 borderColor = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
static ImVec4 borderShadowColor = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
static ImVec4 frameBgColor = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
static ImVec4 frameBgHoveredColor = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
static ImVec4 frameBgActiveColor = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
static ImVec4 titleBgCollapsedColor = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
static ImVec4 titleBgActiveColor = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
static ImVec4 menuBarBgColor = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
static ImVec4 scrollbarBgColor = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
static ImVec4 scrollbarGrabColor = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
static ImVec4 scrollbarGrabHoveredColor = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
static ImVec4 scrollbarGrabActiveColor = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
static ImVec4 checkMarkColor = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
static ImVec4 sliderGrabColor = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
static ImVec4 sliderGrabActiveColor = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
static ImVec4 buttonColor = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
static ImVec4 buttonHoveredColor = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
static ImVec4 buttonActiveColor = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
static ImVec4 headerColor = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
static ImVec4 headerHoveredColor = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
static ImVec4 headerActiveColor = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
static ImVec4 resizeGripColor = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
static ImVec4 resizeGripHoveredColor = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
static ImVec4 resizeGripActiveColor = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
static ImVec4 plotLinesColor = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
static ImVec4 plotLinesHoveredColor = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
static ImVec4 plotHistogramColor = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
static ImVec4 plotHistogramHoveredColor = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
static ImVec4 textSelectedBgColor = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
static ImVec4 modalWindowDarkeningColor = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

// Theme stuff
static bool showImportPopup = false;
static bool showExportPopup = false;
static char exportFileName[64] = "";
static std::string selectedImportFile = "";
static std::vector<std::string> themeFiles;

void RefreshThemeFiles() {
	themeFiles.clear();
	std::string path = "C:\\GaztoofScriptHook\\Themes\\";
	for (auto& p : fs::directory_iterator(path)) {
		if (p.path().extension() == ".ini")
			themeFiles.push_back(p.path().filename().string());
	}
}

void LoadTheme(const std::string& fileName)
{
	std::ifstream file("C:\\GaztoofScriptHook\\Themes\\" + fileName);
	if (!file.is_open()) return;

	std::string line;
	while (std::getline(file, line))
	{
		if (line.find("keepCmdPrompt=") == 0)
			keepCmdPrompt = line.substr(14) == "1" || line.substr(14) == "true";

		auto readColor = [&](const char* key, ImVec4& c)
			{
				if (line.find(key) == 0)
					sscanf_s(line.c_str(), (std::string(key) + "=%f,%f,%f,%f").c_str(), &c.x, &c.y, &c.z, &c.w);
			};

		readColor("textColor", textColor);
		readColor("textDisabledColor", textDisabledColor);
		readColor("windowBgColor", windowBgColor);
		readColor("popupBgColor", popupBgColor);
		readColor("borderColor", borderColor);
		readColor("borderShadowColor", borderShadowColor);
		readColor("frameBgColor", frameBgColor);
		readColor("frameBgHoveredColor", frameBgHoveredColor);
		readColor("frameBgActiveColor", frameBgActiveColor);
		readColor("titleBgCollapsedColor", titleBgCollapsedColor);
		readColor("titleBgActiveColor", titleBgActiveColor);
		readColor("menuBarBgColor", menuBarBgColor);
		readColor("scrollbarBgColor", scrollbarBgColor);
		readColor("scrollbarGrabColor", scrollbarGrabColor);
		readColor("scrollbarGrabHoveredColor", scrollbarGrabHoveredColor);
		readColor("scrollbarGrabActiveColor", scrollbarGrabActiveColor);
		readColor("checkMarkColor", checkMarkColor);
		readColor("sliderGrabColor", sliderGrabColor);
		readColor("sliderGrabActiveColor", sliderGrabActiveColor);
		readColor("buttonColor", buttonColor);
		readColor("buttonHoveredColor", buttonHoveredColor);
		readColor("buttonActiveColor", buttonActiveColor);
		readColor("headerColor", headerColor);
		readColor("headerHoveredColor", headerHoveredColor);
		readColor("headerActiveColor", headerActiveColor);
		readColor("resizeGripColor", resizeGripColor);
		readColor("resizeGripHoveredColor", resizeGripHoveredColor);
		readColor("resizeGripActiveColor", resizeGripActiveColor);
		readColor("plotLinesColor", plotLinesColor);
		readColor("plotLinesHoveredColor", plotLinesHoveredColor);
		readColor("plotHistogramColor", plotHistogramColor);
		readColor("plotHistogramHoveredColor", plotHistogramHoveredColor);
		readColor("textSelectedBgColor", textSelectedBgColor);
		readColor("modalWindowDarkeningColor", modalWindowDarkeningColor);
		readColor("headerBgColor", headerBgColor);
		readColor("elementBgColor", elementBgColor);
	}

	file.close();
	SaveSettings(); // sync to Settings.ini
}

void ExportTheme(const std::string& fileName)
{
	std::ofstream file("C:\\GaztoofScriptHook\\Themes\\" + fileName, std::ios::out | std::ios::trunc);
	if (!file.is_open()) return;

	file << std::boolalpha;
	file << "keepCmdPrompt=" << keepCmdPrompt << "\n";

	auto writeColor = [&](const char* name, const ImVec4& c)
		{
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
	writeColor("headerBgColor", headerBgColor);
	writeColor("elementBgColor", elementBgColor);

	file.close();
}

long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	static std::string selectedFilePathTemp = "";
	static bool initialized = false;
	if(!initialized)
	{
		ImGui::CreateContext();
		LoadSettings();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = "C:\\GaztoofScriptHook\\ImGui.ini";
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

		// Credits to some random guy who sent me this cool style.
		ImGui::StyleColorsClassic();
		auto vStyle = &ImGui::GetStyle();
		vStyle->WindowPadding = ImVec2(15, 15);
		vStyle->WindowRounding = 5.0f;
		vStyle->FramePadding = ImVec2(5, 5);
		vStyle->FrameRounding = 4.0f;
		vStyle->ItemSpacing = ImVec2(12, 8);
		vStyle->ItemInnerSpacing = ImVec2(8, 6);
		vStyle->IndentSpacing = 25.0f;
		vStyle->ScrollbarSize = 15.0f;
		vStyle->ScrollbarRounding = 9.0f;
		vStyle->GrabMinSize = 5.0f;
		vStyle->GrabRounding = 3.0f;
		vStyle->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		vStyle->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		vStyle->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		vStyle->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		vStyle->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		vStyle->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		vStyle->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		vStyle->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		vStyle->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		vStyle->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		vStyle->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		vStyle->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		vStyle->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		vStyle->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		vStyle->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		vStyle->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		vStyle->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		vStyle->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		vStyle->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		vStyle->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		vStyle->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		vStyle->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		vStyle->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		vStyle->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		vStyle->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		vStyle->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		vStyle->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		vStyle->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		vStyle->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		vStyle->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		vStyle->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		vStyle->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		vStyle->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		vStyle->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		vStyle->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
		vStyle->WindowTitleAlign.x = 0.50f;
		vStyle->FrameRounding = 2.0f;
		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX9_Init(pDevice);
		initialized = true;
	};

	bool tempState = GetAsyncKeyState(VK_INSERT);
	static bool lastState = false;
	if (tempState != lastState && tempState)
		openMenu = !openMenu;
	lastState = tempState;
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (openMenu)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		// Apply colors
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

		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(buttonColor.x + 0.1f, buttonColor.y + 0.1f, buttonColor.z + 0.1f, buttonColor.w);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(buttonColor.x - 0.1f, buttonColor.y - 0.1f, buttonColor.z - 0.1f, buttonColor.w);

		ImGui::Begin("Garry's Mod Lua Executor - Coded by t.me/Gaztoof | Revamp by github.com/solal0");
		static int i = 0;
		if (ImGui::Button("Executor", ImVec2(100, 20)))i = 0;
		ImGui::SameLine();
		if (ImGui::Button("Scripts", ImVec2(100, 20)))i = 1;
		ImGui::SameLine();
		if (ImGui::Button("Settings", ImVec2(100, 20)))i = 2;
		switch (i)
		{
		case 0:
		{
			ImVec2 avail = ImGui::GetContentRegionAvail();

			float reservedHeight = 40.0f;
			float height = avail.y - reservedHeight;
			if (height < 150.0f) height = 150.0f;

			ImGui::InputTextMultiline("Yourscript", buf, IM_ARRAYSIZE(buf), ImVec2(avail.x, height), ImGuiInputTextFlags_AllowTabInput);
			if (selectedFilePath != selectedFilePathTemp)
			{
				if (selectedFilePath.length())
				{
					std::ifstream stream(selectedFilePath);
					std::string fileContent = std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
					strcpy(buf, fileContent.c_str());
				}
			}
			selectedFilePathTemp = selectedFilePath;
			if (ImGui::Button(("Load Script")))
				std::thread(openFileDialog).detach();

			ImGui::SameLine();
			if (ImGui::Button(("Execute")))
			{
				Execute("", std::string(buf));
				if (!executeAfterScript)
					buf[0] = '\0';
			}
			break;
		}
		case 1:
		{
			static std::string filePath;
			std::string temp = doFileStuff();
			static std::string fileContent = "";
			if (filePath != temp)
			{
				std::ifstream stream(temp);
				fileContent = std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
			}
			filePath = temp;
			ImGui::InputTextMultiline("ScriptRead", (char*)fileContent.c_str(), fileContent.length(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_ReadOnly);
			break;
		}
		case 2:
		{
			ImGui::Text("Theme");
			if (ImGui::ColorEdit3("Header Background", (float*)&headerBgColor))
				SaveSettings();
			if (ImGui::ColorEdit3("Window Background", (float*)&windowBgColor))
				SaveSettings();
			if (ImGui::ColorEdit3("Text Color", (float*)&textColor))
				SaveSettings();
			if (ImGui::ColorEdit3("Button Color", (float*)&buttonColor))
				SaveSettings();
			if (ImGui::ColorEdit3("Element Background", (float*)&frameBgColor))
				SaveSettings();
			if (ImGui::Button("Import Theme"))
				showImportPopup = true;
			ImGui::SameLine();
			if (ImGui::Button("Export Theme"))
				showExportPopup = true;
			ImGui::SameLine();
			if (ImGui::Button("Deeper Theme Settings"))
				ImGui::OpenPopup("Deeper Theme Settings");

			if (ImGui::BeginPopupModal("Deeper Theme Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::ColorEdit4("Text", (float*)&textColor);
				ImGui::ColorEdit4("Text Disabled", (float*)&textDisabledColor);
				ImGui::ColorEdit4("Window Background", (float*)&windowBgColor);
				ImGui::ColorEdit4("Popup Background", (float*)&popupBgColor);
				ImGui::ColorEdit4("Border", (float*)&borderColor);
				ImGui::ColorEdit4("Border Shadow", (float*)&borderShadowColor);
				ImGui::ColorEdit4("Frame Background", (float*)&frameBgColor);
				ImGui::ColorEdit4("Frame Bg Hovered", (float*)&frameBgHoveredColor);
				ImGui::ColorEdit4("Frame Bg Active", (float*)&frameBgActiveColor);
				ImGui::ColorEdit4("Title Bg", (float*)&headerBgColor);
				ImGui::ColorEdit4("Title Bg Collapsed", (float*)&titleBgCollapsedColor);
				ImGui::ColorEdit4("Title Bg Active", (float*)&titleBgActiveColor);
				ImGui::ColorEdit4("Menu Bar Bg", (float*)&menuBarBgColor);
				ImGui::ColorEdit4("Scrollbar Bg", (float*)&scrollbarBgColor);
				ImGui::ColorEdit4("Scrollbar Grab", (float*)&scrollbarGrabColor);
				ImGui::ColorEdit4("Scrollbar Grab Hovered", (float*)&scrollbarGrabHoveredColor);
				ImGui::ColorEdit4("Scrollbar Grab Active", (float*)&scrollbarGrabActiveColor);
				ImGui::ColorEdit4("Check Mark", (float*)&checkMarkColor);
				ImGui::ColorEdit4("Slider Grab", (float*)&sliderGrabColor);
				ImGui::ColorEdit4("Slider Grab Active", (float*)&sliderGrabActiveColor);
				ImGui::ColorEdit4("Button", (float*)&buttonColor);
				ImGui::ColorEdit4("Button Hovered", (float*)&buttonHoveredColor);
				ImGui::ColorEdit4("Button Active", (float*)&buttonActiveColor);
				ImGui::ColorEdit4("Header Color", (float*)&headerColor);
				ImGui::ColorEdit4("Header Hovered", (float*)&headerHoveredColor);
				ImGui::ColorEdit4("Header Active", (float*)&headerActiveColor);
				ImGui::ColorEdit4("Resize Grip", (float*)&resizeGripColor);
				ImGui::ColorEdit4("Resize Grip Hovered", (float*)&resizeGripHoveredColor);
				ImGui::ColorEdit4("Resize Grip Active", (float*)&resizeGripActiveColor);
				ImGui::ColorEdit4("Plot Lines", (float*)&plotLinesColor);
				ImGui::ColorEdit4("Plot Lines Hovered", (float*)&plotLinesHoveredColor);
				ImGui::ColorEdit4("Plot Histogram", (float*)&plotHistogramColor);
				ImGui::ColorEdit4("Plot Histogram Hovered", (float*)&plotHistogramHoveredColor);
				ImGui::ColorEdit4("Text Selected Bg", (float*)&textSelectedBgColor);
				ImGui::ColorEdit4("Modal Window Darkening", (float*)&modalWindowDarkeningColor);

				if (ImGui::Button("Close"))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			if (showImportPopup)
			{
				ImGui::OpenPopup("Select Theme to Import");
				showImportPopup = false;
			}

			if (ImGui::BeginPopupModal("Select Theme to Import", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				RefreshThemeFiles();

				ImGui::Text("Select a theme to import:");
				ImGui::Separator();

				if (themeFiles.empty())
					ImGui::TextDisabled("No theme files found in Themes folder.");
				else
				{
					for (auto& f : themeFiles)
					{
						bool selected = (f == selectedImportFile);
						if (ImGui::Selectable(f.c_str(), selected, ImGuiSelectableFlags_DontClosePopups))
							selectedImportFile = f;
					}
				}

				ImGui::Separator();

				if (ImGui::Button("Import", ImVec2(100, 0)))
				{
					if (!selectedImportFile.empty())
					{
						LoadTheme(selectedImportFile);
						selectedImportFile.clear();
						ImGui::CloseCurrentPopup();
					}
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel", ImVec2(100, 0)))
				{
					selectedImportFile.clear();
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}

			if (showExportPopup)
			{
				ImGui::OpenPopup("Export Theme As...");
				showExportPopup = false;
			}

			if (ImGui::BeginPopupModal("Export Theme As...", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Enter a name for your theme:");
				ImGui::InputText("##exportFileName", exportFileName, IM_ARRAYSIZE(exportFileName));

				if (ImGui::Button("Save", ImVec2(100, 0)))
				{
					std::string fileNameStr = std::string(exportFileName);
					if (fileNameStr.empty())
						fileNameStr = "UnnamedTheme";
					if (fileNameStr.find(".ini") == std::string::npos)
						fileNameStr += ".ini";

					std::string fullPath = "C:\\GaztoofScriptHook\\Themes\\" + fileNameStr;
					std::ifstream check(fullPath);
					if (check.is_open())
					{
						check.close();
						openFileExistsPopup = true;
						ImGui::CloseCurrentPopup();
					}
					else
					{
						ExportTheme(fileNameStr);
						ImGui::CloseCurrentPopup();
					}
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel", ImVec2(100, 0)))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			if (openFileExistsPopup)
			{
				ImGui::OpenPopup("File Exists");
				openFileExistsPopup = false;
			}

			if (ImGui::BeginPopupModal("File Exists", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("A file with that name already exists.\nDo you want to overwrite it?");
				ImGui::Separator();

				if (ImGui::Button("Yes", ImVec2(100, 0)))
				{
					std::string fileNameStr = std::string(exportFileName);
					if (fileNameStr.find(".ini") == std::string::npos)
						fileNameStr += ".ini";
					ExportTheme(fileNameStr);
					ImGui::CloseCurrentPopup();
				}

				ImGui::SameLine();

				if (ImGui::Button("No", ImVec2(100, 0)))
					ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			ImGui::Text("Miscellaneous");
			if (ImGui::Checkbox("show Console on launch", &keepCmdPrompt))
				SaveSettings();

			if (ImGui::Button("terminate Console"))
			{
				ShutdownConsole();
			}

			if (ImGui::Button("terminate Lua Executor"))
				RequestUnload();
			break;
		}
		default:
			break;
		}
		ImGui::End();
	}


	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return oEndScene(pDevice);
}
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (openMenu && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}
void InitializeGUI() {
	ZeroMemory(buf, 65535);
	while (true)
	{
		if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success)
		{
			kiero::bind(42, (void**)&oEndScene, hkEndScene);

#ifndef _WIN64
#error Please, update this.
#endif
			window = FindWindow(NULL, L"Garry's Mod (x64)"); // Change that for x86!!!!
			oWndProc = (WNDPROC)SetWindowLongPtrA(window, GWL_WNDPROC, (LONG_PTR)WndProc);
			break;
		}
	}
}