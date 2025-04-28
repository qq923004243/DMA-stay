#pragma once
#include <DMALibrary/Memory/Memory.h>
#include <Common/Data.h>
#include <Common/Entitys.h>
#include <Utils/Utils.h>
#include <Utils/Throttler.h>
#include <imgui/imgui.h>
#include <Overlay/Texture.h>
#include <windows.h>
#include <shellapi.h>
#pragma comment(lib,"shell32.lib")



class MenuWebRadar
{
public:
	static void Render()
	{
		ImVec2 Pos = ImGui::GetWindowPos();
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(Pos.x + 165, Pos.y + 85), ImVec2(Pos.x + 580 + 165, Pos.y + 350 + 130), ImGui::GetColorU32(Style::Layout::Background), Style::Window::Rounding);

		ImGui::SetCursorPos(ImVec2(165, 85));
		ImGuiStyle* ImStyle = &ImGui::GetStyle();
		ImGui::BeginChild("O", U8("网页雷达"), ImVec2(276, 350));
		{
			ImGui::NewInputText(U8("服务器"), GameData.Config.WebRadar.IP, IM_ARRAYSIZE(GameData.Config.WebRadar.IP));
			//ImGui::NewInputText(U8("端口"), GameData.Config.WebRadar.Port, IM_ARRAYSIZE(GameData.Config.WebRadar.Port));


			if (ImGui::Button(GameData.Config.WebRadar.isWebRadarConnect ? U8("断开连接") : U8("连接"), ImVec2(ImGui::GetContentRegionMax().x - ImStyle->WindowPadding.x, 25)))
			{
				GameData.Config.WebRadar.isWebRadarEnable = !GameData.Config.WebRadar.isWebRadarEnable;
			}


			if (GameData.Config.WebRadar.isWebRadarConnect) {
				if (ImGui::Button(U8("打开网页地址"), ImVec2(ImGui::GetContentRegionMax().x - ImStyle->WindowPadding.x, 25))) {
					Clipboard(GameData.Config.WebRadar.RadarUrl.c_str());
				}
			}

		}
		ImGui::EndChild();

	}


	static void Clipboard(const char* data) {
		const TCHAR szOperation[] = _T("open");
		//const TCHAR szAddress[] = data;
		HINSTANCE hRslt = ShellExecute(NULL, szOperation,
			data, NULL, NULL, SW_SHOWNORMAL);
		assert(hRslt > (HINSTANCE)HINSTANCE_ERROR);
	}
};



