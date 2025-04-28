#pragma once
#include <DMALibrary/Memory/Memory.h>
#include <Common/Data.h>
#include <Common/Entitys.h>
#include <Utils/Utils.h>
#include <Utils/Throttler.h>
#include <imgui/imgui.h>
#include <Utils/KmBox.h>
#include <Utils/KmBoxNet.h>
#include <Utils/Lurker.h>
#include <Utils/MoBox.h>

class MenuAimBot
{
public:
    static void Setting(std::string type)
    {
        auto& Config = GameData.Config.AimBot.Configs[GameData.Config.AimBot.ConfigIndex].Weapon[type];

        ImGui::BeginChild("O", U8("热键"), ImVec2(276, 230));
        {
            ImGui::Keybind(U8("主要热键"), &Config.First.Key, true);
            ImGui::Keybind(U8("次要热键"), &Config.Second.Key, true);
            ImGui::NewCheckbox(U8("热键合并"), &Config.HotkeyMerge);
            ImGui::Keybind(U8("仅限倒地"), &Config.Groggy.Key, true);

            const char* MultiItems[17] = {
                U8("头顶"),
                U8("头部"),
                U8("脖子"),
                U8("胸部"),
                U8("裆部"),
                U8("左肩"),
                U8("左肘"),
                U8("右肩"),
                U8("右肘"),
                U8("左手"),
                U8("右手"),
                U8("左骨盆"),
                U8("左腿骨"),
                U8("右骨盆"),
                U8("右腿骨"),
                U8("左脚"),
                U8("右脚")
            };

            ImGui::NewMultiCombo(U8("主要部位"), Config.First.Bones, MultiItems, 17);
            ImGui::NewMultiCombo(U8("次要部位"), Config.Second.Bones, MultiItems, 17);
            ImGui::NewMultiCombo(U8("倒地部位"), Config.Groggy.Bones, MultiItems, 17);
        }
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y - 30));
        ImGui::BeginChild("O", U8("调节"), ImVec2(276, 230));
        {
            ImGui::SliderInt(U8("最大距离"), &Config.AimDistance, 0, 1000);
            ImGui::SliderInt(U8("FOV"), &Config.FOV, 1, 360);
            ImGui::SliderFloat(U8("Y轴平滑"), &Config.YSpeed, 0.1, 100);
            ImGui::SliderFloat(U8("X轴平滑"), &Config.XSpeed, 0.1, 100);
            ImGui::SliderInt(U8("平滑比例"), &Config.InitialValue, 100, 1500);
            ImGui::SliderFloat(U8("吸附强度"), &Config.AimSpeedMaxFactor, 0.1, 1);
            ImGui::SliderInt(U8("保持压枪"), &Config.RecoilTime, 0, 20);
            ImGui::SliderInt(U8("FPS限制"), &Config.FPS, 30, 1500);
            ImGui::SliderInt(U8("切换延迟"), &Config.SwitchingDelay, 0, 10);
        }
        ImGui::EndChild();

        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + Style::Padding - 3));
        ImGui::BeginChild("O", U8("选项"), ImVec2(276, 240));
        {
            ImGui::NewCheckbox(U8("切换目标"), &Config.FOVenable);
            //ImGui::NewCheckbox(U8("算法漏打"), &Config.LineTraceSingle);
            if(!Config.LineTraceSingle) ImGui::NewCheckbox(U8("掩体不瞄"), &Config.VisibleCheck);
            ImGui::NewCheckbox(U8("后座抑制"), &Config.NoRecoil);
            if (!Config.NoRecoil)
            {
                ImGui::NewCheckbox(U8("原始回弹"), &Config.OriginalRecoil);
            }
            ImGui::NewCheckbox(U8("弹道预判"), &Config.Prediction);
            if (Config.Prediction)
            {
                const char* PredictionItems[2]{ U8("弹道模拟"), U8("算法模拟") };
                ImGui::NewCombo(U8("预判算法"), &Config.PredictionMode, PredictionItems, IM_ARRAYSIZE(PredictionItems), 2);
            }
            ImGui::NewCheckbox(U8("动态FOV"), &Config.DynamicFov);
            ImGui::NewCheckbox(U8("跳过倒地"), &Config.IgnoreGroggy);
            ImGui::NewCheckbox(U8("击杀切换"), &Config.AutoSwitch);
            ImGui::NewCheckbox(U8("空弹不锁"), &Config.NoBulletNotAim);
        }
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y - 30));
        ImGui::BeginChild("O", U8("锁胎"), ImVec2(276, 240));
        {
            ImGui::NewCheckbox(U8("启用锁胎"), &Config.AimWheel);
            ImGui::Keybind(U8("锁胎热键"), &Config.Wheel.Key, true);
            const char* configItems[5]{ U8("顶部"), U8("偏上"), U8("中间"), U8("偏下"), U8("底部") };
            ImGui::NewCombo(U8("锁胎部位 (单选)"), &Config.AimWheelBone, configItems, IM_ARRAYSIZE(configItems), 5);
            ImGui::SliderInt(U8("锁胎FOV"), &Config.WheelFOV, 1, 360);
            ImGui::SliderFloat(U8("锁胎平滑"), &Config.AimWheelSpeed, 0.1, 100);
        }
        ImGui::EndChild();
    }

    static void Render()
    {
        const std::string WeapType[7] = { "AR", "DMR", "SR", "LMG", "HG", "SG", "SMG" };
        const char* SubNameTabs[8] = { U8("设置"), U8("步枪"), U8("连狙"), U8("栓狙"), U8("机枪"), U8("手枪"), U8("霰弹枪"), U8("冲锋枪") };
        static int SubSelectedTab = 0;
        ImGuiStyle* ImStyle = &ImGui::GetStyle();
        auto io = ImGui::GetIO();
        auto pIO = ImGui::GetPlatformIO();
        if (GetAsyncKeyState(VK_F2) & 0x8000)
            GameData.Config.AimBot.Enable = !GameData.Config.AimBot.Enable;

        ImGui::BeginGroup();
        {
            for (int i = 0; i < sizeof(SubNameTabs) / sizeof(SubNameTabs[0]); i++) {
                if (ImGui::SubTab(i == SubSelectedTab, SubNameTabs[i], Style::SubTab::Size)) SubSelectedTab = i;
                if (i < sizeof(SubNameTabs) / sizeof(SubNameTabs[0]) - 1) ImGui::SameLine();
            }

            if (SubSelectedTab == 0)
            {
                ImGui::BeginChild("O", U8("自瞄配置"), ImVec2(276, 310));
                {
                    ImGui::NewCheckbox(U8("启用自瞄"), &GameData.Config.AimBot.Enable);
                    const char* ModeItems[2]{ U8("硬件自瞄"), U8("内存自瞄 (不安全)") };
                    ImGui::NewCombo(U8("自瞄方式"), &GameData.Config.AimBot.Mode, ModeItems, IM_ARRAYSIZE(ModeItems), 2);
                    ImGui::CheckPicker(U8("显示FOV"), U8("FOV颜色"), &GameData.Config.AimBot.ShowFOV, GameData.Config.AimBot.FOVColor);
                    ImGui::CheckPicker(U8("锁胎FOV"), U8("锁胎FOV颜色"), &GameData.Config.AimBot.ShowWheelFOV, GameData.Config.AimBot.WheelFOVColor);
                    ImGui::CheckPicker(U8("显示预瞄"), U8("预瞄颜色"), &GameData.Config.AimBot.ShowPoint, GameData.Config.AimBot.PointColor);
                    ImGui::SliderInt(U8("预瞄比例"), &GameData.Config.AimBot.PointSize, 1, 20);
                    const char* configItems[2]{ U8("配置1"), U8("配置2") };
                    ImGui::NewCombo(U8("当前配置"), &GameData.Config.AimBot.ConfigIndex, configItems, IM_ARRAYSIZE(configItems), 2);
                    ImGui::Keybind(U8("配置热键"), &GameData.Config.AimBot.Configs[GameData.Config.AimBot.ConfigIndex].Key, true);
                }
                ImGui::EndChild();

                if (GameData.Config.AimBot.Mode == 0)
                {
                    int SizeY = 145;
                    if (GameData.Config.AimBot.Controller == 0)
                    {
                        SizeY = 200;
                    }
                    if (GameData.Config.AimBot.Controller == 1)
                    {
                        SizeY = 200;
                    }
                    ImGui::SameLine();
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 7, ImGui::GetCursorPos().y - 30));
                    ImGui::BeginChild("O", U8("硬件设备"), ImVec2(276, SizeY));
                    {
                        const char* items[] = { U8("KMBOX"), U8("KMBOX 网络版"), U8("双头"), U8("魔盒") };
                        ImGui::NewCombo(U8("控制器"), &GameData.Config.AimBot.Controller, items, IM_ARRAYSIZE(items));

                        if (GameData.Config.AimBot.Controller == 1)
                        {
                            ImGui::NewInputText("IP", GameData.Config.AimBot.IP, IM_ARRAYSIZE(GameData.Config.AimBot.IP));
                            ImGui::NewInputText(U8("端口"), GameData.Config.AimBot.Port, IM_ARRAYSIZE(GameData.Config.AimBot.Port));
                            ImGui::NewInputText("UUID", GameData.Config.AimBot.UUID, IM_ARRAYSIZE(GameData.Config.AimBot.UUID));
                        }

                        static std::vector<std::string> ports = Utils::GetCOMPorts();

                        if (GameData.Config.AimBot.Controller != 1)
                        {
                            std::vector<std::pair<std::string, std::string>> portVector;
                            for (const auto& port : ports)
                            {
                                portVector.push_back({ port, Utils::StringToUTF8(port) });
                            }
                            std::vector<const char*> items;
                            for (const auto& port : portVector) {
                                items.push_back(port.second.c_str());
                            }
                            
                            if (items.size() > 0)
                            {
                                ImGui::NewCombo(U8("COM端口"), &GameData.Config.AimBot.COM, &items[0], items.size());
                            }
                        }

                        if (GameData.Config.AimBot.Controller == 0)
                        {
                            ImGui::SliderInt(U8("执行延迟"), &GameData.Config.AimBot.Delay, 0, 20);
                        }

                        bool NeedPushStyleColor = false;
                        bool ConnectedNeedPushStyleColor = false;

                        if (ImGui::Button(GameData.Config.AimBot.Connected ? U8("断开连接") : U8("连接"), ImVec2(ImGui::GetContentRegionMax().x - ImStyle->WindowPadding.x, 25))) {
                            bool Connected = false;
                            std::string extractedStr;
                            if (ports.size() > 0)
                            {
                                extractedStr = Utils::ExtractSubstring(ports[GameData.Config.AimBot.COM], R"(COM(\d+))");
                            }
                            int COM = 0;
                            int number = 0;
                            if (extractedStr != "")
                            {
                                COM = std::stoi(extractedStr);
                                number = std::stoi(extractedStr);
                            }

                            switch (GameData.Config.AimBot.Controller)
                            {
                            case 0:
                                if (!GameData.Config.AimBot.Connected)
                                {
                                    Connected = KmBox::Init(COM);
                                    if (!Connected)
                                    {
                                        ImGui::OpenPopup(U8("连接失败"));
                                    }
                                }
                                else {
                                    KmBox::Close();
                                    ConnectedNeedPushStyleColor = true;
                                    Connected = false;
                                }
                                break;
                            case 1:
                                if (!GameData.Config.AimBot.Connected)
                                {
                                    Connected = KmBoxNet::Init(GameData.Config.AimBot.IP, GameData.Config.AimBot.Port, GameData.Config.AimBot.UUID);
                                    if (!Connected)
                                    {
                                        ImGui::OpenPopup(U8("连接失败"));
                                    }
                                }
                                else {
                                    KmBoxNet::Close();
                                    ConnectedNeedPushStyleColor = true;
                                    Connected = false;
                                }
                                break;
                            case 2:
                                if (!GameData.Config.AimBot.Connected)
                                {
                                    Connected = Lurker::Init(COM);
                                    if (!Connected)
                                    {
                                        ImGui::OpenPopup(U8("连接失败"));
                                    }
                                }
                                else {
                                    Lurker::Close();
                                    ConnectedNeedPushStyleColor = true;
                                    Connected = false;
                                }
                                break;
                            case 3:
                                if (!GameData.Config.AimBot.Connected)
                                {
                                    Connected = MoBox::Init(COM);
                                    if (!Connected)
                                    {
                                        ImGui::OpenPopup(U8("连接失败"));
                                    }
                                }
                                else {
                                    MoBox::Close();
                                    ConnectedNeedPushStyleColor = true;
                                    Connected = false;
                                }
                                break;
                            default:
                                break;
                            }

                            GameData.Config.AimBot.Connected = Connected;
                        }

                        if (ImGui::BeginPopupModal(U8("连接失败"), NULL, ImGuiWindowFlags_AlwaysAutoResize))
                        {
                            ImGui::Text(U8("请检查COM端口或控制器状态"));

                            if (ImGui::Button(U8("关闭")))
                            {
                                ImGui::CloseCurrentPopup();
                            }

                            ImGui::EndPopup();
                        }

                    }
                    ImGui::EndChild();
                }
            }
            else {
                ImGui::SetNextWindowSize({ 580, 371 });
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
                ImGui::Begin("自瞄设置", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoBackground);
                MenuAimBot::Setting(WeapType[SubSelectedTab - 1]);
                ImGui::PopStyleVar();
                ImGui::Dummy(ImVec2(0, Style::Padding - 5));
                ImGui::End();
            }
        }
        ImGui::EndGroup();
    }
};