#pragma once

#include <DMALibrary/Memory/Memory.h>
#include <Common/Data.h>
#include <Common/Entitys.h>
#include <Utils/Utils.h>
#include <Utils/Throttler.h>
#include <imgui/imgui.h>
#include <Overlay/Texture.h>

inline std::vector<std::string> MenuItemTypes = {
        U8("全部"),
        U8("步枪"),
        U8("连狙"),
        U8("栓狙"),
        U8("散弹"),
        U8("冲锋"),
        U8("机枪"),
        U8("手枪"),
        U8("药品"),
        U8("装备"),
        U8("枪口"),
        U8("握把"),
        U8("弹匣"),
        U8("枪托"),
        U8("瞄具"),
        U8("子弹"),
        U8("投掷"),
        U8("其它")
};

inline std::vector<WeaponType> MenuItemTypeIndexs = {
       WeaponType::AR,
       WeaponType::AR,
       WeaponType::DMR,
       WeaponType::SR,
       WeaponType::SG,
       WeaponType::SMG,
       WeaponType::LMG,
       WeaponType::HG,
       WeaponType::Drug,
       WeaponType::Armor,
       WeaponType::Muzzle,
       WeaponType::Grip,
       WeaponType::Magazine,
       WeaponType::GunButt,
       WeaponType::Sight,
       WeaponType::Bullet,
       WeaponType::Grenade,
       WeaponType::Other
};

inline static int SelectedItem = 0;

class MenuItem
{
public:
    static void Setting(std::string type)
    {

    }

    static void Render()
    {
        for (int i = 0; i < MenuItemTypes.size(); ++i)
        {
            if (ImGui::SubTab(i == SelectedItem, MenuItemTypes[i].c_str(), Style::SubTab::Size)) SelectedItem = i;
            if (i != 9 && i < MenuItemTypes.size() - 1) ImGui::SameLine();
        }

        ImGui::BeginChild("O", U8("物品列表"), ImVec2(568, 320));
        {
            static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(5.0f, 4.0f));
            if (ImGui::BeginTable(U8("物品列表"), 3, flags, ImVec2(0.0f, 0.0f)))
            {
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableSetupColumn(U8("   图片"), ImGuiTableColumnFlags_WidthFixed, 38.0f);
                ImGui::TableSetupColumn(U8("名称"), ImGuiTableColumnFlags_WidthFixed, 360);
                ImGui::TableSetupColumn(U8("分组"), ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableHeadersRow();

                for (auto& Item : GameData.Config.Item.Lists)
                {
                    if (SelectedItem > 0 && Item.second.Type != MenuItemTypeIndexs[SelectedItem])
                    {
                        continue;
                    }

                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    {
                        std::string Space = "";
                        ImGui::TextUnformatted(Space.c_str());
                        ImGui::SameLine();
                        std::string ItemName = Item.first;
                        std::string IconUrl = "Assets/Icons/Item/All/" + ItemName + ".png";

                        if (GImGuiTextureMap[IconUrl].Width > 0) {
                            ImGui::Image(GImGuiTextureMap[IconUrl].Texture, ImVec2(20, 20));
                        }
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(Utils::StringToUTF8(Item.second.DisplayName).c_str());

                    ImGui::TableSetColumnIndex(2);
                    {
                        static const char* items[] = { U8("未分组"), U8("A组"), U8("B组"), U8("C组"), U8("D组"), U8("E组") };
                        
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 4));

                        ImGui::NewCombo(Item.second.Name.c_str(), &Item.second.Group, items, IM_ARRAYSIZE(items), 6, true);
                    }
                }

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
    }
};