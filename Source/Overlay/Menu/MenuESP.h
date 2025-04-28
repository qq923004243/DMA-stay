#pragma once

#include <DMALibrary/Memory/Memory.h>
#include <Common/Data.h>
#include <Common/Entitys.h>
#include <Utils/Utils.h>
#include <Utils/Throttler.h>
#include <imgui/imgui.h>
#include <Overlay/Texture.h>

DWORD PickerFlags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

class MenuESP
{
public:
    static void Item()
    {
        ImGui::SetNextWindowSize({ 580, 371 });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::Begin("物品设置", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoBackground);
        {
            ImGui::BeginChild("O", U8("显示"), ImVec2(276, 220));
            {
                ImGui::NewCheckbox(U8("物品透视"), &GameData.Config.Item.Enable);
                ImGui::Keybind(U8("切换热键"), &GameData.Config.Item.GroupKey, true);
                ImGui::NewCheckbox(U8("分组显示"), &GameData.Config.Item.Combination);
                ImGui::NewCheckbox(U8("显示图标"), &GameData.Config.Item.ShowIcon);
                ImGui::NewCheckbox(U8("显示距离"), &GameData.Config.Item.ShowDistance);
                ImGui::NewCheckbox(U8("配件过滤"), &GameData.Config.Item.AccessoriesFilter);
            }
            ImGui::EndChild();

            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y - 30));

            ImGui::BeginChild("O", U8("调节"), ImVec2(276, 220));
            {
                ImGui::SliderInt(U8("显示距离"), &GameData.Config.Item.DistanceMax, 0, 150);
                ImGui::SliderInt(U8("分组X轴"), &GameData.Config.Item.ThresholdX, 1, 100);
                ImGui::SliderInt(U8("分组Y轴"), &GameData.Config.Item.ThresholdY, 1, 100);
                ImGui::SliderInt(U8("字体比例"), &GameData.Config.Item.FontSize, 10, 20);
            }
            ImGui::EndChild();

            {
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + Style::Padding - 3));

                ImGui::BeginChild("O", U8("A组"), ImVec2(276, 110));
                {
                    ImGui::Keybind(U8("A组热键"), &GameData.Config.Item.GroupAKey, true);
                    ImGui::NewColorEdit4(U8("A组颜色"), GameData.Config.Item.GroupAColor, PickerFlags);
                }
                ImGui::EndChild();

                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y - 30));

                ImGui::BeginChild("O", U8("B组"), ImVec2(276, 110));
                {
                    ImGui::Keybind(U8("B组热键"), &GameData.Config.Item.GroupBKey, true);
                    ImGui::NewColorEdit4(U8("B组颜色"), GameData.Config.Item.GroupBColor, PickerFlags);
                }
                ImGui::EndChild();

                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + Style::Padding - 3));

                ImGui::BeginChild("O", U8("C组"), ImVec2(276, 110));
                {
                    ImGui::Keybind(U8("C组热键"), &GameData.Config.Item.GroupCKey, true);
                    ImGui::NewColorEdit4(U8("C组颜色"), GameData.Config.Item.GroupCColor, PickerFlags);
                }
                ImGui::EndChild();

                ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y - 30));

                ImGui::BeginChild("O", U8("D组"), ImVec2(276, 110));
                {
                    ImGui::Keybind(U8("D组热键"), &GameData.Config.Item.GroupDKey, true);
                    ImGui::NewColorEdit4(U8("D组颜色"), GameData.Config.Item.GroupDColor, PickerFlags);
                }
                ImGui::EndChild();

                //ImGui::SameLine();
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + Style::Padding - 3));

                ImGui::BeginChild("O", U8("E组"), ImVec2(276, 110));
                {
                    ImGui::Keybind(U8("E组热键"), &GameData.Config.Item.GroupEKey, true);
                    ImGui::NewColorEdit4(U8("E组颜色"), GameData.Config.Item.GroupEColor, PickerFlags);
                }
                ImGui::EndChild();
            }
        }
        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(0, Style::Padding - 2));
        ImGui::End();
    }

    static void Radar()
    {
        ImGui::SetNextWindowSize({ 580, 371 });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::Begin("雷达设置", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoBackground);
        {
            ImGui::BeginChild("O", U8("大地图"), ImVec2(276, 220));
            {
                ImGui::NewCheckbox(U8("显示玩家"), &GameData.Config.Radar.Main.ShowPlayer);
                ImGui::NewCheckbox(U8("显示载具"), &GameData.Config.Radar.Main.ShowVehicle);
                ImGui::NewCheckbox(U8("显示空投"), &GameData.Config.Radar.Main.ShowAirDrop);
                ImGui::NewCheckbox(U8("显示死亡"), &GameData.Config.Radar.Main.ShowDeadBox);
                ImGui::SliderInt(U8("图标比例"), &GameData.Config.Radar.Main.FontSize, 5, 20);
            }
            ImGui::EndChild();

            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y - 30));

            ImGui::BeginChild("O", U8("小地图"), ImVec2(276, 220));
            {
                ImGui::NewCheckbox(U8("显示玩家"), &GameData.Config.Radar.Mini.ShowPlayer);
                ImGui::NewCheckbox(U8("显示载具"), &GameData.Config.Radar.Mini.ShowVehicle);
                ImGui::NewCheckbox(U8("显示空投"), &GameData.Config.Radar.Mini.ShowAirDrop);
                ImGui::NewCheckbox(U8("显示死亡"), &GameData.Config.Radar.Mini.ShowDeadBox);
                ImGui::SliderInt(U8("图标比例"), &GameData.Config.Radar.Mini.FontSize, 5, 20);
            }
            ImGui::EndChild();
        }
        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(0, Style::Padding - 2));
        ImGui::End();
    }

    static void Vehicle()
    {
        ImGui::SetNextWindowSize({ 580, 371 });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::Begin("载具设置", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoBackground);
        {
            ImGui::BeginChild("O", U8("显示"), ImVec2(276, 220));
            {
                ImGui::NewCheckbox(U8("显示载具"), &GameData.Config.Vehicle.Enable);
                ImGui::Keybind(U8("快捷热键"), &GameData.Config.Vehicle.EnableKey, true);
                ImGui::NewColorEdit4(U8("文字颜色"), GameData.Config.Vehicle.Color, PickerFlags);
                ImGui::SliderInt(U8("文字大小"), &GameData.Config.Vehicle.FontSize, 10, 20);
            }
            ImGui::EndChild();
        }
        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(0, Style::Padding - 2));
        ImGui::End();
    }

    static void Project()
    {
        ImGui::SetNextWindowSize({ 580, 371 });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::Begin("投掷物设置", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoBackground);
        {
            ImGui::BeginChild("O", U8("显示"), ImVec2(276, 220));
            { 
                ImGui::NewCheckbox(U8("显示投掷物"), &GameData.Config.Project.Enable);
                ImGui::NewCheckbox(U8("投掷物范围"), &GameData.Config.Project.EnableRange);
                ImGui::SliderInt(U8("最大距离"), &GameData.Config.Project.DistanceMax, 10, 1000);
                ImGui::NewColorEdit4(U8("显示颜色"), GameData.Config.Project.Color, PickerFlags);
                ImGui::SliderInt(U8("比例大小"), &GameData.Config.Project.FontSize, 10, 20);
            }
            ImGui::EndChild();
        }
        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(0, Style::Padding - 2));
        ImGui::End();
    }

    static void AirDrop()
    {
        ImGui::SetNextWindowSize({ 580, 371 });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::Begin("空投设置", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoBackground);
        {
            ImGui::BeginChild("O", U8("显示"), ImVec2(276, 220));
            {
                ImGui::NewCheckbox(U8("显示空投"), &GameData.Config.AirDrop.Enable);
                ImGui::Keybind(U8("快捷热键"), &GameData.Config.AirDrop.EnableKey, true);
                ImGui::NewCheckbox(U8("显示物品"), &GameData.Config.AirDrop.ShowItems);

                ImGui::SliderInt(U8("最大距离"), &GameData.Config.AirDrop.DistanceMax, 10, 1500);
                ImGui::NewColorEdit4(U8("文字颜色"), GameData.Config.AirDrop.Color, PickerFlags);
                ImGui::SliderInt(U8("比例大小"), &GameData.Config.AirDrop.FontSize, 10, 20);
            }
            ImGui::EndChild();
        }
        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(0, Style::Padding - 2));
        ImGui::End();
    }

    static void DeadBox()
    {
        ImGui::SetNextWindowSize({ 580, 371 });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::Begin("死亡盒设置", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoBackground);
        {
            ImGui::BeginChild("O", U8("显示"), ImVec2(276, 220));
            {
                ImGui::NewCheckbox(U8("显示死亡盒"), &GameData.Config.DeadBox.Enable);
                ImGui::Keybind(U8("快捷热键"), &GameData.Config.DeadBox.EnableKey, true);
                ImGui::NewCheckbox(U8("显示物品"), &GameData.Config.DeadBox.ShowItems);

                ImGui::SliderInt(U8("最大距离"), &GameData.Config.DeadBox.DistanceMax, 10, 1500);
                ImGui::NewColorEdit4(U8("文字颜色"), GameData.Config.DeadBox.Color, PickerFlags);
                ImGui::SliderInt(U8("比例大小"), &GameData.Config.DeadBox.FontSize, 10, 20);
            }
            ImGui::EndChild();
        }
        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(0, Style::Padding - 2));
        ImGui::End();
    }

    static void Other()
    {
        ImGui::SetNextWindowSize({ 580, 371 });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::Begin("其它设置", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoBackground);
        {
            ImGui::BeginChild("O", U8("恰雷"), ImVec2(276, 220));
            {
                ImGui::NewCheckbox(U8("显示倒计时"), &GameData.Config.Project.ShowChareTime);
                ImGui::NewColorEdit4(U8("显示颜色"), GameData.Config.Project.ChareColor, PickerFlags);
                ImGui::SliderInt(U8("比例大小"), &GameData.Config.Project.ChareFontSize, 5, 20);
            }
            ImGui::EndChild();

            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y - 30));

            ImGui::BeginChild("O", U8("预警雷达"), ImVec2(276, 220));
            {
                ImGui::NewCheckbox(U8("显示预警"), &GameData.Config.Early.Enable);
                ImGui::NewCheckbox(U8("显示距离"), &GameData.Config.Early.ShowDistance);
                ImGui::SliderInt(U8("最大距离"), &GameData.Config.Early.DistanceMax, 10, 1000);
                ImGui::SliderInt(U8("图标比例"), &GameData.Config.Early.FontSize, 5, 20);
            }
            ImGui::EndChild();
        }
        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(0, Style::Padding - 2));
        ImGui::End();
    }

    static void Render()
    {
        const char* SubNameTabs[9] = { U8("设置"), U8("玩家"), U8("物品"), U8("载具"), U8("空投"), U8("死亡盒"), U8("雷达"), U8("投掷物"), U8("其它") };
        static int SubSelectedTab = 0;
        ImGuiStyle* ImStyle = &ImGui::GetStyle();
        auto io = ImGui::GetIO();
        auto pIO = ImGui::GetPlatformIO();

        ImGui::BeginGroup();
        {
            for (int i = 0; i < sizeof(SubNameTabs) / sizeof(SubNameTabs[0]); i++) {
                if (ImGui::SubTab(i == SubSelectedTab, SubNameTabs[i], Style::SubTab::Size)) SubSelectedTab = i;
                if (i < sizeof(SubNameTabs) / sizeof(SubNameTabs[0]) - 1) ImGui::SameLine();
            }

            if (SubSelectedTab == 0)
            {
                ImGui::BeginChild("O", U8("覆盖"), ImVec2(276, 235));
                {
                    ImGui::NewCheckbox(U8("垂直同步"), &GameData.Config.Overlay.VSync);
                    ImGui::NewCheckbox(U8("融合模式"), &GameData.Config.Overlay.FusionMode);
                    ImGui::NewCheckbox(U8("备用相机"), &GameData.Config.Overlay.UseLastFrameCameraCache);
                    ImGui::NewCheckbox(U8("独立线程"), &GameData.Config.Overlay.UseThread);

                    GameData.Config.Overlay.ScreenWidth = pIO.Monitors[GameData.Config.Overlay.MonitorCurrentIdx].MainSize.x;
                    GameData.Config.Overlay.ScreenHeight = pIO.Monitors[GameData.Config.Overlay.MonitorCurrentIdx].MainSize.y;

                    std::string comboPreview = Utils::StringToUTF8(std::format("{}x{} (屏幕{})", GameData.Config.Overlay.ScreenWidth, GameData.Config.Overlay.ScreenHeight, GameData.Config.Overlay.MonitorCurrentIdx));

                    if (ImGui::NewBeginCombo(U8("屏幕列表"), comboPreview.c_str())) {
                        for (int i = 0; i < pIO.Monitors.size(); i++) {
                            auto pMonitor = pIO.Monitors[i];
                            const bool isSelected = (GameData.Config.Overlay.MonitorCurrentIdx == i);
                            auto monitorName = Utils::StringToUTF8(std::format("{}x{} (屏幕{})", pMonitor.MainSize.x, pMonitor.MainSize.y, i));
                            if (ImGui::Selectable(monitorName.c_str(), isSelected)) {
                                GameData.Config.Overlay.MonitorCurrentIdx = i;
                            }
                            if (isSelected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }

                    if (GameData.Config.Overlay.Enable)
                    {
                        if (ImGui::Button(U8("关闭透视"), ImVec2(ImGui::GetContentRegionMax().x - ImStyle->WindowPadding.x, 25)))
                            GameData.Config.Overlay.Enable = false;

                        //ImGui::Text("FPS: %.1f", io.Framerate);
                    }
                    else {
                        if (ImGui::Button(U8("启用透视"), ImVec2(ImGui::GetContentRegionMax().x - ImStyle->WindowPadding.x, 25))) {
                            GameData.Config.Overlay.Enable = true;
                        }
                    }

                }
                ImGui::EndChild();
            }

            if (SubSelectedTab == 1)
            {
                ImGui::SetNextWindowSize({ 580, 371 });
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
                ImGui::Begin("玩家设置", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoBackground);
                {
                    ImGui::BeginChild("O", U8("显示"), ImVec2(276, 220));
                    {
                        ImGui::NewCheckbox(U8("玩家透视"), &GameData.Config.ESP.Enable);

                        static const char* ShowMultiItems[17] = {
                            U8("骨骼"), // 0
                            U8("战队"), // 1
                            U8("名称"), // 2
                            U8("血条"), // 3
                            U8("距离"), // 4
                            U8("队标"), // 5
                            U8("击杀"), // 6
                            U8("观战"), // 7
                            U8("伤害"), // 8
                            U8("掉线"), // 9
                            U8("合作者"), // 10
                            U8("段位"), // 11
                            U8("段位分"), // 12
                            U8("KD"), // 13
                            U8("手持图标"), // 14
                            U8("手持武器"), // 15
                            U8("等级"), // 16
                        };

                        ImGui::NewMultiCombo(U8("显示信息"), GameData.Config.ESP.ShowInfos, ShowMultiItems, 17);
                        static const char* DistanceStyleItems[2]{ U8("顶部"), U8("底部") };
                        ImGui::NewCombo(U8("距离位置"), &GameData.Config.ESP.DistanceStyle, DistanceStyleItems, IM_ARRAYSIZE(DistanceStyleItems), 2);
                        static const char* HealthBarStyleItems[2]{ U8("顶部"), U8("左侧") };
                        ImGui::NewCombo(U8("血条位置"), &GameData.Config.ESP.HealthBarStyle, HealthBarStyleItems, IM_ARRAYSIZE(HealthBarStyleItems), 2);

                        ImGui::NewCheckbox(U8("可视检查"), &GameData.Config.ESP.VisibleCheck);
                        ImGui::NewCheckbox(U8("瞄准信息"), &GameData.Config.ESP.AimExpandInfo);
                        ImGui::NewCheckbox(U8("渐变血条"), &GameData.Config.ESP.DynamicHealthColor);
                        ImGui::CheckPicker(U8("被瞄射线"), U8("射线颜色"), &GameData.Config.ESP.TargetedRay, GameData.Config.ESP.Color.Ray.Line);
                        ImGui::NewCheckbox(U8("可视射线"), &GameData.Config.ESP.VisibleCheckRay);
                        ImGui::NewCheckbox(U8("隐藏骨骼 (锁定时)"), &GameData.Config.ESP.LockedHiddenBones);
                    }
                    ImGui::EndChild();

                    ImGui::SameLine();
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y - 30));

                    ImGui::BeginChild("O", U8("调节"), ImVec2(276, 220));
                    {
                        ImGui::SliderInt(U8("显示距离"), &GameData.Config.ESP.DistanceMax, 0, 1000);
                        ImGui::SliderInt(U8("信息距离"), &GameData.Config.ESP.InfoDistanceMax, 0, 1000);
                        ImGui::SliderInt(U8("手持距离"), &GameData.Config.ESP.WeaponDistanceMax, 0, 1000);
                        ImGui::SliderInt(U8("骨骼粗细"), &GameData.Config.ESP.SkeletonWidth, 1, 3);
                        ImGui::SliderInt(U8("射线粗细"), &GameData.Config.ESP.RayWidth, 1, 3);
                        ImGui::SliderInt(U8("字体比例"), &GameData.Config.ESP.FontSize, 10, 20);
                    }
                    ImGui::EndChild();

                    {
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + Style::Padding - 3));

                        ImGui::BeginChild("O", U8("默认状态"), ImVec2(276, 110));
                        {
                            ImGui::NewColorEdit4(U8("默认信息"), GameData.Config.ESP.Color.Default.Info, PickerFlags);
                            ImGui::NewColorEdit4(U8("默认骨骼"), GameData.Config.ESP.Color.Default.Skeleton, PickerFlags);
                        }
                        ImGui::EndChild();

                        ImGui::SameLine();
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y - 30));

                        ImGui::BeginChild("O", U8("可视状态"), ImVec2(276, 110));
                        {
                            ImGui::NewColorEdit4(U8("信息颜色"), GameData.Config.ESP.Color.Visible.Info, PickerFlags);
                            ImGui::NewColorEdit4(U8("骨骼颜色"), GameData.Config.ESP.Color.Visible.Skeleton, PickerFlags);
                        }
                        ImGui::EndChild();

                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + Style::Padding - 3));

                        ImGui::BeginChild("O", U8("危险玩家"), ImVec2(276, 110));
                        {
                            ImGui::NewColorEdit4(U8("信息颜色"), GameData.Config.ESP.Color.Dangerous.Info, PickerFlags);
                            ImGui::NewColorEdit4(U8("骨骼颜色"), GameData.Config.ESP.Color.Dangerous.Skeleton, PickerFlags);
                        }
                        ImGui::EndChild();

                        ImGui::SameLine();
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y - 30));

                        ImGui::BeginChild("O", U8("合作者"), ImVec2(276, 110));
                        {
                            ImGui::NewColorEdit4(U8("信息颜色"), GameData.Config.ESP.Color.Partner.Info, PickerFlags);
                            ImGui::NewColorEdit4(U8("骨骼颜色"), GameData.Config.ESP.Color.Partner.Skeleton, PickerFlags);
                        }
                        ImGui::EndChild();

                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + Style::Padding - 3));

                        ImGui::BeginChild("O", U8("黑名单"), ImVec2(276, 110));
                        {
                            ImGui::NewColorEdit4(U8("信息颜色"), GameData.Config.ESP.Color.Blacklist.Info, PickerFlags);
                            ImGui::NewColorEdit4(U8("骨骼颜色"), GameData.Config.ESP.Color.Blacklist.Skeleton, PickerFlags);
                        }
                        ImGui::EndChild();

                        ImGui::SameLine();
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y - 30));

                        ImGui::BeginChild("O", U8("白名单"), ImVec2(276, 110));
                        {
                            ImGui::NewColorEdit4(U8("信息颜色"), GameData.Config.ESP.Color.Whitelist.Info, PickerFlags);
                            ImGui::NewColorEdit4(U8("骨骼颜色"), GameData.Config.ESP.Color.Whitelist.Skeleton, PickerFlags);
                        }
                        ImGui::EndChild();

                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + Style::Padding - 3));

                        ImGui::BeginChild("O", U8("人机"), ImVec2(276, 110));
                        {
                            ImGui::NewColorEdit4(U8("信息颜色"), GameData.Config.ESP.Color.AI.Info, PickerFlags);
                            ImGui::NewColorEdit4(U8("骨骼颜色"), GameData.Config.ESP.Color.AI.Skeleton, PickerFlags);
                        }
                        ImGui::EndChild();

                        ImGui::SameLine();
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y - 30));

                        ImGui::BeginChild("O", U8("倒地"), ImVec2(276, 110));
                        {
                            ImGui::NewColorEdit4(U8("信息颜色"), GameData.Config.ESP.Color.Groggy.Info, PickerFlags);
                            ImGui::NewColorEdit4(U8("骨骼颜色"), GameData.Config.ESP.Color.Groggy.Skeleton, PickerFlags);
                        }
                        ImGui::EndChild();
                    }
                }
                ImGui::PopStyleVar();
                ImGui::Dummy(ImVec2(0, Style::Padding - 2));
                ImGui::End();
            }

            if (SubSelectedTab == 2)
            {
                Item();
            }

            if (SubSelectedTab == 3)
            {
                Vehicle();
            }

            if (SubSelectedTab == 4)
            {
                AirDrop();
            }
            
            if (SubSelectedTab == 5)
            {
                DeadBox();
            }

            if (SubSelectedTab == 6)
            {
                Radar();
            }

            if (SubSelectedTab == 7)
            {
                Project();
            }

            if (SubSelectedTab == 8)
            {
                Other();
            }
        }

        ImGui::EndGroup();
    }
};