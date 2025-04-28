#pragma once
#include <DMALibrary/Memory/Memory.h>
#include <Common/Data.h>
#include <Common/Entitys.h>
#include <Common/Config.h>
#include <Utils/Utils.h>
#include <Utils/Throttler.h>
#include <imgui/imgui.h>
#include <imgui/imgui_settings.h>
#include <Utils/KmBox.h>
#include <Utils/KmBoxNet.h>
#include <Utils/Lurker.h>
#include <Utils/MoBox.h>
#include "MenuPlayerLists.h"
#include "MenuAimBot.h"
#include "MenuItem.h"
#include "MenuESP.h"
#include "MenuWebRadar.h"


static void Clipboard(const char* data) {
    const TCHAR szOperation[] = _T("open");
    //const TCHAR szAddress[] = data;
    HINSTANCE hRslt = ShellExecute(NULL, szOperation,
        data, NULL, NULL, SW_SHOWNORMAL);
    assert(hRslt > (HINSTANCE)HINSTANCE_ERROR);
}
class Menu
{
public:
    static void DrawMain(const ImFontAtlas* FontAtlas)
    {
        ImGui::PushFont(FontAtlas->Fonts[3]);

        MenuPlayerLists::Render(FontAtlas);

        ImGui::PopFont();
        if (GameData.Config.Window.Setting)
        {
            ImGui::PushFont(FontAtlas->Fonts[2]);
            const ImVec2 Spacing = ImGui::GetStyle().ItemSpacing;
            ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
            ImGui::SetNextWindowSize({ Style::Window::Size.x + Spacing.x, Style::Window::Size.y + Spacing.y });

            ImGui::Begin("MENU", &GameData.Config.Window.Setting, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            {
                ImVec2 Pos = ImGui::GetWindowPos();
                ImVec2 OPos = Pos;
                Pos.x += Spacing.x / 2;
                Pos.y += Spacing.y / 2;

                ImGui::GetWindowDrawList()->AddRectFilled(Pos, ImVec2(Pos.x + Style::Window::Size.x, Pos.y + Style::Window::Size.y), ImGui::GetColorU32(Style::Window::Background), Style::Window::Rounding);
                //ImGui::GetWindowDrawList()->AddRect(Pos, ImVec2(Pos.x + Style::Window::Size.x, Pos.y + Style::Window::Size.y), ImGui::GetColorU32(Style::Window::OutlineBackground), Style::Window::Rounding);
                
                ImGui::SetCursorPos(ImVec2(Style::Padding + Spacing.x / 2, Style::Padding));
                //std::string IconUrl = "Assets/Icons/Radar/T_1vs99_Indicator.png";
                //ImGui::Image(GImGuiTextureMap[IconUrl].Texture, ImVec2(950 * 0.8f, 65 * 0.8f));
                ImGui::PushFont(FontAtlas->Fonts[1]);
                ImGui::Text(U8("                          PUBG-Stay"));
                ImGui::PopFont();

                const char* NameTabs[] = { U8("视觉效果"), U8("自动瞄准"), U8("物品分组"), U8("网页雷达"), U8("全局设置")};
                static int SelectedTab = 0;
                ImGui::SetCursorPos(ImVec2(Style::Padding + Spacing.x / 2, 70));
                ImGui::BeginGroup();
                {
   /*                 for (int i = 0; i < sizeof(NameTabs) / sizeof(NameTabs[0]); i++)
                        if (ImGui::Tab(i == SelectedTab, NameTabs[i], Style::Tab::Size)) SelectedTab = i;
                        if (i < sizeof(NameTabs) / sizeof(NameTabs[0]) - 1) ImGui::SameLine();*/



                        for (int i = 0; i < sizeof(NameTabs) / sizeof(NameTabs[0]); i++) {
                            if (ImGui::SubTab(i == SelectedTab, NameTabs[i], Style::SubTab::Size)) SelectedTab = i;
                            if (i < sizeof(NameTabs) / sizeof(NameTabs[0]) - 1) ImGui::SameLine();
                        }






                    
                }
                //ImGui::EndGroup();

                //ImGui::SetCursorPos(ImVec2(Style::Tab::Size.x + (Style::Padding + Spacing.x / 2) * 3 - 3 - 7, Style::Padding * 2));

                ImGui::BeginGroup();
                {
                    //Pos.x += Style::Tab::Size.x + (Style::Padding + Spacing.x / 2) * 2 - 8;
                    //Pos.y += Style::Padding;
                    //ImGui::GetWindowDrawList()->AddRectFilled(Pos, ImVec2(Pos.x + Style::Window::Size.x - Style::Tab::Size.x - Style::Padding * 2 - Style::Padding + 3, Pos.y + Style::Window::Size.y - Style::Padding * 2), ImGui::GetColorU32(Style::Layout::Background), Style::Window::Rounding);

         /*           ImGui::PushFont(FontAtlas->Fonts[1]);
                    ImGui::Text(NameTabs[SelectedTab]);
                    ImGui::PopFont();

                    ImGui::Dummy(ImVec2(0, 10.f));*/

                    if (SelectedTab == 0)
                    {
                        MenuESP::Render();
                    }

                    if (SelectedTab == 1) {
                        MenuAimBot::Render();
                    }

                    if (SelectedTab == 2) {
                        MenuItem::Render();
                    }

                    //if (SelectedTab == 3) {
                    //    MenuItem::Render;

                    //  //  MenuHelp::Render;
                    //}
                    if (SelectedTab == 3) {
                      //  MenuWebRadar::Render();
                        ImGuiStyle* ImStyle = &ImGui::GetStyle();
                        ImGui::BeginGroup();
                        {
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

                    }

                    if (SelectedTab == 4) {
                        ImGuiStyle* ImStyle = &ImGui::GetStyle();
                        
                        ImGui::BeginGroup();
                        {
                           
                            
                            ImGui::BeginChild("O", U8("设置"), ImVec2(276, 170));
                            {
                                ImGui::Keybind(U8("战斗模式"), &GameData.Config.ESP.FocusModeKey, true);
                                ImGui::Keybind(U8("刷新缓存"), &GameData.Config.Function.ClearKey, true);
                                ImGui::NewCheckbox(U8("文字阴影"), &GameData.Config.ESP.Stroke);
                                ImGui::Text(U8("Home 显/隐"));
                            }
                            ImGui::EndChild();
                        }
                        ImGui::EndGroup();

                        ImGui::SameLine();
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y));

                        ImGui::BeginGroup();
                        {
                            ImGui::BeginChild("O", U8("配置"), ImVec2(276, 170));
                            {
                                if (ImGui::Button(U8("载入配置"), ImVec2(ImGui::GetContentRegionMax().x - ImStyle->WindowPadding.x, 25))) 
                                {
                                    Config::Load();
                                }
                                if (ImGui::Button(U8("保存配置"), ImVec2(ImGui::GetContentRegionMax().x - ImStyle->WindowPadding.x, 25))) 
                                {
                                    Config::Save();
                                }
                            }
                            ImGui::EndChild();
                        }
                        ImGui::EndGroup();
                    }



                }
                ImGui::EndGroup();
                
            }
            ImGui::End();
            ImGui::PopFont();
        }
    }
};
