#include <Windows.h>
#include "auth.hpp"
#include <string>
#include <d3d9.h>
#include "skCrpyt.h"
#pragma comment(lib,"d3d9.lib")
//#pragma comment(lib, "D3dx9")

IDirect3DTexture9* masterlogo;

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
#include "anti_debug.h"

std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);

using namespace KeyAuth;
std::string name = ("");
std::string ownerid = ("");
std::string secret = ("");
std::string version = ("1.0");
std::string url = "https://keyauth.win/api/1.1/"; // change if you're self-hosting
std::string sslPin = "ssl pin key (optional)"; // don't change unless you intend to pin public certificate key. you can get here in the "Pin SHA256" field https://www.ssllabs.com/ssltest/analyze.html?d=keyauth.win&latest. If you do this you need to be aware of when SSL key expires so you can update it

bool loader_active = true;
bool ranOnce = true;

static int width = 350;
static int height = 200;

char PassWord[20] = "";
char Licence[50] = "";
char UserName[20] = "";
char RgPassWord[20] = "";
char RgUserName[20] = "";

api KeyAuthApp(name, ownerid, secret, version, url, sslPin);

HWND main_hwnd = nullptr;

LPDIRECT3DDEVICE9        g_pd3dDevice;
D3DPRESENT_PARAMETERS    g_d3dpp;
LPDIRECT3D9              g_pD3D;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	// Create the D3DDevice
	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;
	return true;
}

void CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

bool LoginCheck = false;

int APIENTRY WindownsMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL,skCrypt("Loaders"), NULL };
	RegisterClassEx(&wc);
	main_hwnd = CreateWindow(wc.lpszClassName, skCrypt("Loaders"), WS_POPUP, 0, 0, 5, 5, NULL, NULL, wc.hInstance, NULL);

	if (!CreateDeviceD3D(main_hwnd)) {
		CleanupDeviceD3D();
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}
	ShowWindow(main_hwnd, SW_HIDE);
	UpdateWindow(main_hwnd);

        HWND Stealth;
        AllocConsole();
        Stealth = FindWindowA(skCrypt("ConsoleWindowClass"), NULL);
        ShowWindow(Stealth, 0);
	
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	constexpr auto ColorFromBytes = [](uint8_t r, uint8_t g, uint8_t b)
	{
		return ImVec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 1.0f);
	};

	auto& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	const ImVec4 bgColor = ColorFromBytes(37, 37, 38);
	const ImVec4 lightBgColor = ColorFromBytes(82, 82, 85);
	const ImVec4 veryLightBgColor = ColorFromBytes(90, 90, 95);

	const ImVec4 panelColor = ColorFromBytes(51, 51, 55);
	const ImVec4 panelHoverColor = ColorFromBytes(29, 151, 236);
	const ImVec4 panelActiveColor = ColorFromBytes(0, 224, 255);

	const ImVec4 textColor = ColorFromBytes(255, 255, 255);
	const ImVec4 textDisabledColor = ColorFromBytes(151, 151, 151);
	const ImVec4 borderColor = ColorFromBytes(78, 78, 78);

	colors[ImGuiCol_Text] = textColor;
	colors[ImGuiCol_TextDisabled] = textDisabledColor;
	colors[ImGuiCol_TextSelectedBg] = panelActiveColor;
	colors[ImGuiCol_WindowBg] = bgColor;
	colors[ImGuiCol_ChildBg] = ColorFromBytes(121, 121, 121);;
	colors[ImGuiCol_PopupBg] = bgColor;
	colors[ImGuiCol_Border] = borderColor;
	colors[ImGuiCol_BorderShadow] = borderColor;
	colors[ImGuiCol_FrameBg] = panelColor;
	colors[ImGuiCol_FrameBgHovered] = panelHoverColor;
	colors[ImGuiCol_FrameBgActive] = panelActiveColor;
	colors[ImGuiCol_TitleBg] = bgColor;
	colors[ImGuiCol_TitleBgActive] = bgColor;
	colors[ImGuiCol_TitleBgCollapsed] = bgColor;
	colors[ImGuiCol_MenuBarBg] = panelColor;
	colors[ImGuiCol_ScrollbarBg] = panelColor;
	colors[ImGuiCol_ScrollbarGrab] = lightBgColor;
	colors[ImGuiCol_ScrollbarGrabHovered] = veryLightBgColor;
	colors[ImGuiCol_ScrollbarGrabActive] = veryLightBgColor;
	colors[ImGuiCol_CheckMark] = panelActiveColor;
	colors[ImGuiCol_SliderGrab] = panelHoverColor;
	colors[ImGuiCol_SliderGrabActive] = panelActiveColor;
	colors[ImGuiCol_Button] = panelColor;
	colors[ImGuiCol_ButtonHovered] = panelHoverColor;
	colors[ImGuiCol_ButtonActive] = panelHoverColor;
	colors[ImGuiCol_Header] = panelColor;
	colors[ImGuiCol_HeaderHovered] = panelHoverColor;
	colors[ImGuiCol_HeaderActive] = panelActiveColor;
	colors[ImGuiCol_Separator] = borderColor;
	colors[ImGuiCol_SeparatorHovered] = borderColor;
	colors[ImGuiCol_SeparatorActive] = borderColor;
	colors[ImGuiCol_ResizeGrip] = bgColor;
	colors[ImGuiCol_ResizeGripHovered] = panelColor;
	colors[ImGuiCol_ResizeGripActive] = lightBgColor;
	colors[ImGuiCol_PlotLines] = panelActiveColor;
	colors[ImGuiCol_PlotLinesHovered] = panelHoverColor;
	colors[ImGuiCol_PlotHistogram] = panelActiveColor;
	colors[ImGuiCol_PlotHistogramHovered] = panelHoverColor;
	colors[ImGuiCol_DragDropTarget] = bgColor;
	colors[ImGuiCol_NavHighlight] = bgColor;
	colors[ImGuiCol_DockingPreview] = panelActiveColor;
	colors[ImGuiCol_Tab] = bgColor;
	colors[ImGuiCol_TabActive] = panelActiveColor;
	colors[ImGuiCol_TabUnfocused] = bgColor;
	colors[ImGuiCol_TabUnfocusedActive] = panelActiveColor;
	colors[ImGuiCol_TabHovered] = panelHoverColor;

	style.WindowRounding = 0.0f;
	style.ChildRounding = 0.0f;
	style.FrameRounding = 0.0f;
	style.GrabRounding = 0.0f;
	style.PopupRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.TabRounding = 0.0f;

	ImGui_ImplWin32_Init(main_hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	DWORD window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar;
	RECT screen_rect;
	GetWindowRect(GetDesktopWindow(), &screen_rect);
	auto x = float(screen_rect.right - width) / 2.f;
	auto y = float(screen_rect.bottom - height) / 2.f;

	static int Tabs = 2;
	int currItem = 0;
	const char* AllCheats[] = {"None", "file name"};

	bool InfWindow = true;

	KeyAuthApp.init();
	/*vector<string> targetLocation = grabPath();*/
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT && !LoginCheck)
	{
		//if (security::check_security() != security::internal::debug_results::none) {
		//	//for (int i = 0; i < targetLocation.size(); i++) {
		//	//	if (pathExist(targetLocation[i])) {
		//	//		sendMsgIfCracking(targetLocation[i], std::to_string(security::check_security()));
		//	//	}
		//	//}
		//	KeyAuthApp.ban();
		//	exit(0);
		//}
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(width, height));
			ImGui::SetNextWindowBgAlpha(1.0f);

			ImGui::Begin(skCrypt("DeletedTOS#1027"), &loader_active, window_flags);
			{
				if (Tabs == 1 || Tabs == 2)
				{

					if (ImGui::Button(skCrypt("Regster"), ImVec2(100, 20)))
					{
						Tabs = 1;
					}

					ImGui::SameLine();

					if (ImGui::Button(skCrypt("Login"), ImVec2(100, 20)))
					{
						Tabs = 2;
					}
				}
				if (Tabs == 1)
				{

					ImGui::Spacing();

					ImGui::Separator();

					ImGui::InputText(skCrypt("Username##rg"), RgUserName, IM_ARRAYSIZE(RgUserName));

					ImGui::InputText(skCrypt("Password##rg"), RgPassWord, IM_ARRAYSIZE(RgPassWord));

					ImGui::InputText(skCrypt("Licence##rg"), Licence, IM_ARRAYSIZE(Licence));

					if (ImGui::Button(skCrypt("Regster "), ImVec2(320, 25)))
					{
						KeyAuthApp.regstr(RgUserName, RgPassWord, Licence);

						if (!KeyAuthApp.data.success)
						{
							MessageBox(NULL, skCrypt("Regster unsuccessful!"), skCrypt("Regster"), MB_OK);
						}
						else
							MessageBox(NULL, skCrypt("Regster successful!"), skCrypt("Regster"), MB_OK);
					}
				}

				if (Tabs == 2)
				{

					ImGui::Spacing();

					ImGui::Separator();

					ImGui::InputText(skCrypt("Username##rg"), UserName, IM_ARRAYSIZE(UserName));

					ImGui::InputText(skCrypt("Password##rg"), PassWord, IM_ARRAYSIZE(PassWord), ImGuiInputTextFlags_Password);

					if (ImGui::Button(skCrypt("Login##Log"), ImVec2(320, 25)))
					{
						KeyAuthApp.login(UserName, PassWord);
						
						if (!KeyAuthApp.data.success)
						{
							MessageBox(NULL, skCrypt("Login Error!"), skCrypt("Login"), MB_OK);
						}
						else
						{
							MessageBox(NULL, skCrypt("Login successful!"), skCrypt("Login"), MB_OK);
							InfWindow = true;
							Tabs = 3;
						}
					}
				}

				if (Tabs == 3)
				{
					ImGui::Combo(skCrypt("Cheats"), &currItem, AllCheats, IM_ARRAYSIZE(AllCheats));
					if (ImGui::Button(skCrypt("Launch")))
					{
						std::string downloadCode;
						switch (currItem)
						{
						case 0:
							downloadCode = skCrypt("None");
							break;
						case 1:
							downloadCode = skCrypt("158725"); //file script
							break;
						default:
							downloadCode = skCrypt("None");
							break;
						}

						if (downloadCode != "None")
						{
							std::vector<std::uint8_t> bytes = KeyAuthApp.download(downloadCode);
							void* pe = bytes.data();

							IMAGE_DOS_HEADER* DOSHeader;
							IMAGE_NT_HEADERS64* NtHeader;
							IMAGE_SECTION_HEADER* SectionHeader;

							PROCESS_INFORMATION PI;
							STARTUPINFOA SI;
							ZeroMemory(&PI, sizeof(PI));
							ZeroMemory(&SI, sizeof(SI));

							void* pImageBase;
							char currentFilePath[1024];

							DOSHeader = PIMAGE_DOS_HEADER(pe);
							NtHeader = PIMAGE_NT_HEADERS64(DWORD64(pe) + DOSHeader->e_lfanew);

							if (NtHeader->Signature == IMAGE_NT_SIGNATURE) {
								GetModuleFileNameA(NULL, currentFilePath, MAX_PATH);

								if (CreateProcessA(currentFilePath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &SI, &PI)) {
									CONTEXT* CTX;
									CTX = LPCONTEXT(VirtualAlloc(NULL, sizeof(CTX), MEM_COMMIT, PAGE_READWRITE));
									CTX->ContextFlags = CONTEXT_FULL;
									UINT64 imageBase = 0;

									if (GetThreadContext(PI.hThread, LPCONTEXT(CTX))) {
										pImageBase = VirtualAllocEx(
											PI.hProcess,
											LPVOID(NtHeader->OptionalHeader.ImageBase),
											NtHeader->OptionalHeader.SizeOfImage,
											MEM_COMMIT | MEM_RESERVE,
											PAGE_EXECUTE_READWRITE
										);

										WriteProcessMemory(PI.hProcess, pImageBase, pe, NtHeader->OptionalHeader.SizeOfHeaders, NULL);

										for (size_t i = 0; i < NtHeader->FileHeader.NumberOfSections; i++)
										{
											SectionHeader = PIMAGE_SECTION_HEADER(DWORD64(pe) + DOSHeader->e_lfanew + 264 + (i * 40));

											WriteProcessMemory(
												PI.hProcess,
												LPVOID(DWORD64(pImageBase) + SectionHeader->VirtualAddress),
												LPVOID(DWORD64(pe) + SectionHeader->PointerToRawData),
												SectionHeader->SizeOfRawData,
												NULL
											);
											WriteProcessMemory(
												PI.hProcess,
												LPVOID(CTX->Rdx + 0x10),
												LPVOID(&NtHeader->OptionalHeader.ImageBase),
												8,
												NULL
											);
										}

										CTX->Rcx = DWORD64(pImageBase) + NtHeader->OptionalHeader.AddressOfEntryPoint;
										SetThreadContext(PI.hThread, LPCONTEXT(CTX));
										ResumeThread(PI.hThread);
										WaitForSingleObject(PI.hProcess, NULL);
									}
								}
							}
							exit(0);
						}
						else
						{
							MessageBox(0, skCrypt("Select A Cheat"), skCrypt("Error"), 0);
						}

						
					}
				}
			}
			ImGui::End();
		}
		ImGui::EndFrame();

		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
			ResetDevice();
		}
		if (!loader_active) {
			msg.message = WM_QUIT;
		}
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CleanupDeviceD3D();
	//	DestroyWindow(main_hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);
	//	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;
	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			g_d3dpp.BackBufferWidth = LOWORD(lParam);
			g_d3dpp.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	WindownsMain(0, 0, 0, 0);
}

std::string tm_to_readable_time(tm ctx) {
	char buffer[80];

	strftime(buffer, sizeof(buffer), skCrypt("%a %m/%d/%y %H:%M:%S %Z"), &ctx);

	return std::string(buffer);
}

static std::time_t string_to_timet(std::string timestamp) {
	auto cv = strtol(timestamp.c_str(), NULL, 10); // long

	return (time_t)cv;
}

static std::tm timet_to_tm(time_t timestamp) {
	std::tm context;

	localtime_s(&context, &timestamp);

	return context;
}
