#pragma once

#include <windows.h>
#include "common/Data.h"
//#include <OS-ImGui/imgui/imgui.h>
#include "common/VectorHelper.h"
#include "RenderHelper.h"
#include <imgui/imgui.h>
#include <DMALibrary/Memory/Memory.h>
#include "Texture.h"
#include "HealthBar.h"
#include <Hack/Radar.h>

Throttler PlayerThrottlered;
std::string UTF8ToAnsi(const std::string& utf8_str) {
	// 1. 将 UTF-8 字符串转换为宽字符串 (UTF-16)
	int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);
	if (wlen == 0) {
		// 错误处理
		return "";
	}

	std::wstring wide_str(wlen, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wide_str[0], wlen);

	// 2. 将宽字符串 (UTF-16) 转换为 ANSI 字符串
	int alen = WideCharToMultiByte(CP_ACP, 0, wide_str.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (alen == 0) {
		// 错误处理
		return "";
	}

	std::string ansi_str(alen, '\0');
	WideCharToMultiByte(CP_ACP, 0, wide_str.c_str(), -1, &ansi_str[0], alen, nullptr, nullptr);

	return ansi_str;
}
class ESP {
public:
	static void DrawFPS(ImGuiIO& io)
	{
		if (!GameData.Config.Overlay.ShowFPS)
		{
			return;
		}
		auto GameScene = GameData.Scene;
		std::string Scene = "Unknown";
		std::string Items = "Hide";

		if (GameScene == Scene::FindProcess) {
			Scene = "FindProcess";
		}
		else if (GameScene == Scene::Lobby) {
			Scene = "Lobby";
		}
		else if (GameScene == Scene::Gaming) {
			Scene = "Gaming";
		}

		if (GameData.Config.ESP.FocusMode)
		{
			Items = "战斗模式";
		}
		else {
			if (GameData.Config.Item.ShowGroup == 2)
			{
				Items = "A";
			}
			else if (GameData.Config.Item.ShowGroup == 3) {
				Items = "B";
			}
			else if (GameData.Config.Item.ShowGroup == 4) {
				Items = "C";
			}
			else if (GameData.Config.Item.ShowGroup == 5) {
				Items = "D";
			}
			else if (GameData.Config.Item.ShowGroup == 6) {
				Items = "E";
			}
			else if (GameData.Config.Item.ShowGroup == 1) {
				Items = "All";
			}

			Items = "物品模式: " + Items;
		}

		/*char FormattedText[256];
		snprintf(FormattedText, sizeof(FormattedText), U8("%s %.1f\nAimConfig: Profile%d\n%s\n观战 (%d)"), Scene, io.Framerate, GameData.Config.AimBot.ConfigIndex + 1, Items, GameData.FogPlayerCount, GameData.LocalPlayerInfo.SpectatedCount);
		*/
		auto TextPosition = ImVec2(5, 20);
		RenderHelper::Text(Utils::StringToUTF8(std::format("{} {}\n自瞄配置: Profile{}\n{}\n迷雾数量: {}\n观战数量 ({})\n自瞄开关F2 ({})", Scene, (int)io.Framerate, GameData.Config.AimBot.ConfigIndex + 1, Items, GameData.FogPlayerCount, GameData.LocalPlayerInfo.SpectatedCount, GameData.Config.AimBot.Enable ? "开" : "关")).c_str(), TextPosition, IM_COL32(255, 255, 255, 255), 16, false, false);
	










	};

	static void DrawSkeleton(Player Player, const ImU32 SkeletonUseColor, const float Thickness)
	{
		//ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
		/*auto DrawList = ImGui::GetWindowDrawList();
		auto CursorScreenPos = ImGui::GetCursorScreenPos();
		CursorScreenPos.x -= 8;
		CursorScreenPos.y -= 8;*/

		FVector2D neckpos = Player.Skeleton.ScreenBones[EBoneIndex::Neck_01];
		FVector2D pelvispos = Player.Skeleton.ScreenBones[EBoneIndex::Pelvis];
		std::vector<std::pair<ImVec2, ImVec2>> Bones;
		bool failed = false;

		for (const auto& a : SkeletonLists::Skeleton)
		{
			FVector2D previous = FVector2D(0, 0);
			FVector2D current, p1, c1;


			for (EBoneIndex bone : a)
			{
				current = (bone == EBoneIndex::Neck_01) ? neckpos : (bone == EBoneIndex::Pelvis) ? pelvispos : Player.Skeleton.ScreenBones[bone];

				if (previous.X != 0.f && previous.Y != 0.f)
				{
					p1 = previous;
					c1 = current;

					Bones.emplace_back(std::make_pair(ImVec2(p1.X, p1.Y), ImVec2(c1.X, c1.Y)));

					if (p1.X == INFINITY || p1.Y == INFINITY || c1.X == INFINITY || c1.Y == INFINITY) {
						failed = true;
						Bones.clear();
						break;
					}

				}
				previous = current;
			}

			if (failed)
				break;
		}

		//std::vector<ImVec2> Points;

		for (auto& line : Bones)
		{
			/*std::vector<ImVec2> Points;
			Points.emplace_back(line.first);
			Points.emplace_back(line.second);
			RenderHelper::PolyLine(Points, SkeletonUseColor, Thickness);*/
			RenderHelper::Line(line.first, line.second, SkeletonUseColor, Thickness);
		}

		//RenderHelper::PolyLine(Points, SkeletonUseColor, Thickness);
	}

	static void DrawFOV()
	{
		AimBotConfig Config = GameData.Config.AimBot.Configs[GameData.Config.AimBot.ConfigIndex].Weapon[WeaponTypeToString[GameData.LocalPlayerInfo.WeaponEntityInfo.WeaponType]];
		
		if (Config.DynamicFov) {
			Config.FOV = Config.FOV * (90.0f / GameData.Camera.FOV);
			Config.WheelFOV = Config.WheelFOV * (90.0f / GameData.Camera.FOV);
		}

		ImVec2 CenterPoint = { (float)GameData.Config.Overlay.ScreenWidth / 2, (float)GameData.Config.Overlay.ScreenHeight / 2 };

		if (GameData.Config.AimBot.ShowFOV && !GameData.AimBot.Lock && GameData.AimBot.Type == EntityType::Player)
		{
			if (Config.FOV <= 0) return;
			RenderHelper::Circle(CenterPoint, Config.FOV, Utils::FloatToImColor(GameData.Config.AimBot.FOVColor), 2, 360);
		} 
		
		if (GameData.Config.AimBot.ShowWheelFOV && !GameData.AimBot.Lock)
		{
			if (Config.WheelFOV <= 0) return;
			RenderHelper::Circle(CenterPoint, Config.WheelFOV, Utils::FloatToImColor(GameData.Config.AimBot.WheelFOVColor), 2, 360);
		}
	}

	static void DrawAimBotPoint()
	{
		if (GameData.Config.AimBot.ShowPoint && GameData.LocalPlayerInfo.IsScoping && !Utils::ValidPtr(GameData.AimBot.TargetPlayerInfo.Entity))
		{
			ImVec2 CenterPoint = { (float)GameData.Config.Overlay.ScreenWidth / 2, (float)GameData.Config.Overlay.ScreenHeight / 2 };
			if (GameData.AimBot.PredictedPos.X == 0 && GameData.AimBot.PredictedPos.Y == 0 && GameData.AimBot.PredictedPos.Z == 0) return;
			
			FVector2D PointPos = VectorHelper::WorldToScreen(GameData.AimBot.PredictedPos);
			
			ImVec2 ImPointPos = { PointPos.X, PointPos.Y };

			RenderHelper::CircleFilled(ImPointPos, GameData.Config.AimBot.PointSize, Utils::FloatToImColor(GameData.Config.AimBot.PointColor), GameData.Config.AimBot.PointSize * 2);
		}
	}

	static void DrawHealthBar(DWORD Sign, float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size, ImColor Color, bool Horizontal, bool ShowBackupHealth = true)
	{
		static std::map<DWORD, HealthBar> HealthBarMap;
		if (!HealthBarMap.count(Sign))
		{
			HealthBarMap.insert({ Sign,HealthBar() });
		}
		if (HealthBarMap.count(Sign))
		{
			if (Horizontal)
				HealthBarMap[Sign].DrawHealthBar_Horizontal(MaxHealth, CurrentHealth, Pos, Size, Color, ShowBackupHealth);
			else
				HealthBarMap[Sign].DrawHealthBar_Vertical(MaxHealth, CurrentHealth, Pos, Size, Color, ShowBackupHealth);
		}
	}

	static ImVec4 Get2DBox(const Player& Player)
	{
		FVector2D Head = Player.Skeleton.ScreenBones.at(EBoneIndex::Head);
		FVector2D Root = Player.Skeleton.ScreenBones.at(EBoneIndex::Root);

		ImVec2 Size, Pos;
		Size.y = (Root.Y - Head.Y) * 1.09;
		Size.x = Size.y * 0.6;

		Pos = ImVec2(Root.X - Size.x / 2, Head.Y - Size.y * 0.08);

		return ImVec4{ Pos.x,Pos.y,Size.x,Size.y };
	}

	static std::unordered_map<int, std::list<ItemInfo>> GroupItems(std::vector<std::pair<uint64_t, ItemInfo>> Items, float ThresholdX, float ThresholdY) {
		static const auto IsNear = [&](const FVector2D& A, const FVector2D& B) {
			return std::abs(A.X - B.X) < ThresholdX && std::abs(A.Y - B.Y) < ThresholdY;
		};

		std::unordered_map<int, std::list<ItemInfo>> Groups;
		int GroupId = 0;

		for (auto& Key : Items) {
			auto& Item = Key.second;

			bool FoundGroup = false;
			Item.ScreenLocation = VectorHelper::WorldToScreen(Item.Location);

			for (auto& Group : Groups) {
				for (const auto& GroupItem : Group.second) {
					if (IsNear(Item.ScreenLocation, GroupItem.ScreenLocation)) {
						Group.second.push_back(Item);
						FoundGroup = true;
						break;
					}
				}
				if (FoundGroup) break;
			}

			if (!FoundGroup) {
				Groups[GroupId++] = std::list<ItemInfo>{ Item };
			}
		}
		return Groups;
	}

	static void DrawItems(std::unordered_map<uint64_t, ItemInfo> Items)
	{
		if (!GameData.Config.Item.Enable) return;

		std::vector<std::pair<uint64_t, ItemInfo>> Vectors(Items.begin(), Items.end());

		std::sort(Vectors.begin(), Vectors.end(), [](const std::pair<uint64_t, ItemInfo>& a, const std::pair<uint64_t, ItemInfo>& b) {
			return a.second.Distance > b.second.Distance;
		});

		ImColor FontSize = GameData.Config.Item.FontSize;
		ImColor TextColor = ImColor(255, 255, 255);

		if (GameData.Config.Item.Combination)
		{
			std::unordered_map<int, std::list<ItemInfo>> CombinationItems = GroupItems(Vectors, GameData.Config.Item.ThresholdX, GameData.Config.Item.ThresholdY);
			for (auto& Group : CombinationItems)
			{
				int Index = 0;
				int FistIndex = 0;
				FVector2D FistPos;

				Group.second.sort([](const ItemInfo& A, const ItemInfo& B) {
					return A.ItemType > B.ItemType;
				});

				for (ItemInfo& ItemInfo : Group.second)
				{
					if (ItemInfo.bHidden || ItemInfo.Distance > GameData.Config.Item.DistanceMax) continue;

					std::string Text = Utils::StringToUTF8(std::format("{}", ItemInfo.DisplayName));
					int GroupIndex = GameData.Config.Item.Lists[ItemInfo.Name].Group;
					if (GameData.Config.Item.ShowGroup != 1 && GroupIndex + 1 != GameData.Config.Item.ShowGroup) continue;

					switch (GroupIndex)
					{
					case 1:
						TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupAColor);
						break;
					case 2:
						TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupBColor);
						break;
					case 3:
						TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupCColor);
						break;
					case 4:
						TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupDColor);
						break;
					case 5:
						TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupEColor);
						break;
					default:
						continue;
						break;
					}

					ItemInfo.ScreenLocation = VectorHelper::WorldToScreen(ItemInfo.Location);
					if (!VectorHelper::IsInScreen(ItemInfo.ScreenLocation)) continue;
					ItemInfo.Distance = GameData.Camera.Location.Distance(ItemInfo.Location) / 100.0f;

					if (FistPos.X == 0 && FistPos.Y == 0)
					{
						FistIndex = Index;
						FistPos = ItemInfo.ScreenLocation;
					}

					if (GameData.Config.Item.ShowDistance && Index == FistIndex)
					{
						if (Group.second.size() > 1)
						{
							Text += "\n";
						}

						if (Group.second.size() == 1) {
							Text += " ";
						}

						Text += "[" + std::to_string((int)ItemInfo.Distance) + "M]";
					}

					ImVec2 ScreenLocation = { FistPos.X, FistPos.Y - ((FontSize + 1) * Index) };
					ImVec2 TextSize = RenderHelper::StrokeText(Text.c_str(), { ScreenLocation.x, ScreenLocation.y }, TextColor, FontSize, false, false);
					if (GameData.Config.Item.ShowIcon) {
						float Scale = (FontSize / 14.f);
						std::string IconUrl = "Assets/Icons/Item/All/" + ItemInfo.Name + ".png";
						if (GImGuiTextureMap[IconUrl].Width > 0) {
							float TargetHeight = 14.f * Scale;
							float HeightZoom = TargetHeight / GImGuiTextureMap[IconUrl].Height;
							float IconWidth = GImGuiTextureMap[IconUrl].Width * HeightZoom;
							float IconHeight = TargetHeight;
							ScreenLocation.x -= (FontSize + 4);

							RenderHelper::Image(GImGuiTextureMap[IconUrl].Texture, ScreenLocation, ImVec2(IconWidth, IconHeight), false);
						}
					}

					Index++;
				}

			}
		}
		else {
			for (auto& Item : Vectors)
			{
				ItemInfo& ItemInfo = Item.second;

				if (ItemInfo.bHidden || ItemInfo.Distance > GameData.Config.Item.DistanceMax) continue;
				int GroupIndex = GameData.Config.Item.Lists[ItemInfo.Name].Group;
				if (GameData.Config.Item.ShowGroup != 1 && GroupIndex + 1 != GameData.Config.Item.ShowGroup) continue;

				switch (GroupIndex)
				{
				case 1:
					TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupAColor);
					break;
				case 2:
					TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupBColor);
					break;
				case 3:
					TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupCColor);
					break;
				case 4:
					TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupDColor);
					break;
				case 5:
					TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupEColor);
					break;
				default:
					continue;
					break;
				}

				ItemInfo.ScreenLocation = VectorHelper::WorldToScreen(ItemInfo.Location);
				if (!VectorHelper::IsInScreen(ItemInfo.ScreenLocation)) continue;
				ItemInfo.Distance = GameData.Camera.Location.Distance(ItemInfo.Location) / 100.0f;

				std::string Text = Utils::StringToUTF8(std::format("{}", ItemInfo.DisplayName));
				ImColor InfoColor = ImColor(255, 255, 255);

				ImVec2 InfoSize = RenderHelper::StrokeText(Text.c_str(), { ItemInfo.ScreenLocation.X, ItemInfo.ScreenLocation.Y }, TextColor, FontSize, true, true);
				
				if (GameData.Config.Item.ShowDistance) RenderHelper::StrokeText(Utils::StringToUTF8(std::format("{}M", (int)ItemInfo.Distance)).c_str(), { ItemInfo.ScreenLocation.X, ItemInfo.ScreenLocation.Y }, ImColor(255, 255, 255, 255), FontSize - 1, true, false);

				if (GameData.Config.Item.ShowIcon) {
					float Scale = 1.f;
					if (ItemInfo.ItemType == WeaponType::AR || ItemInfo.ItemType == WeaponType::DMR || ItemInfo.ItemType == WeaponType::SG || ItemInfo.ItemType == WeaponType::SR || ItemInfo.ItemType == WeaponType::LMG)
					{
						Scale = 1.4f;
					}

					std::string IconUrl = "Assets/Icons/Item/All/" + ItemInfo.Name + ".png";
					if (GImGuiTextureMap[IconUrl].Width > 0) {
						{
							float TargetHeight = 24.f * (FontSize / 14.f) * Scale;
							float HeightZoom = TargetHeight / GImGuiTextureMap[IconUrl].Height;
							float IconWidth = GImGuiTextureMap[IconUrl].Width * HeightZoom;
							float IconHeight = TargetHeight;
							ItemInfo.ScreenLocation.X -= IconWidth / 2;
							ItemInfo.ScreenLocation.Y -= (IconHeight + FontSize + 2);

							RenderHelper::Image(GImGuiTextureMap[IconUrl].Texture, { ItemInfo.ScreenLocation.X, ItemInfo.ScreenLocation.Y }, ImVec2(IconWidth, IconHeight), false);
						}
					}
				}
			}
		}
	}
	static void DrawExplosiveRange(FVector center, float radius, ImColor color, float thickness) {

		FVector Vertices[46];
		bool VerticesValid[46]{};
		float angle = 0;
		const float angleStep = (M_PI * 2) / 45;

		for (int i = 0; i <= 45; ++i, angle += angleStep)
		{
			FVector pos = FVector(radius * cosf(angle) + center.X, radius * sinf(angle) + center.Y, center.Z);
			FVector2D w2sPos = VectorHelper::WorldToScreen(pos);
			VerticesValid[i] = w2sPos.X > 0 && w2sPos.X < GameData.Config.Overlay.ScreenWidth && w2sPos.Y > 0 && w2sPos.Y < GameData.Config.Overlay.ScreenHeight;
			Vertices[i] = FVector(w2sPos.X, w2sPos.Y, 0);
		}

		for (int i = 0; i < 45; ++i) {
			if (Vertices[i].Distance(Vertices[i + 1]) < radius)
				if ((VerticesValid[i] && VerticesValid[i + 1]))
					ImGui::GetWindowDrawList()->AddLine({ Vertices[i].X, Vertices[i].Y }, { Vertices[i + 1].X,  Vertices[i + 1].Y }, color, thickness);
		}
	}

	static void DrawProjects(std::unordered_map<uint64_t, ProjectInfo> Items)
	{
		if (!GameData.Config.Project.Enable) return;

		std::vector<std::pair<uint64_t, ProjectInfo>> Vectors(Items.begin(), Items.end());

		std::sort(Vectors.begin(), Vectors.end(), [](const std::pair<uint64_t, ProjectInfo>& a, const std::pair<uint64_t, ProjectInfo>& b) {
			return a.second.Distance > b.second.Distance;
			});

		for (auto& Item : Vectors)
		{
			ProjectInfo& Project = Item.second;

			if (Project.bVisible == 1 || Project.TimeTillExplosion <= 0 || Project.Distance > GameData.Config.Project.DistanceMax) continue;

			Project.ScreenLocation = VectorHelper::WorldToScreen(Project.Location);
			Project.Distance = GameData.Camera.Location.Distance(Project.Location) / 100.0f;

			const int FontSize = GameData.Config.Project.FontSize;
			const float Scale = (float)FontSize / 14.f;

			std::string Text = Utils::StringToUTF8(std::format("{} [{}M]", Project.Name, (int)Project.Distance));
			ImColor InfoColor = Utils::FloatToImColor(GameData.Config.Project.Color);
			Project.ScreenLocation.Y += 5 * Scale;
			/*	Utils::Log(1, "TimeTillExplosion: %s ", Project.EntityName);*/
			ImVec2 InfoSize = RenderHelper::StrokeText(Text.c_str(), { Project.ScreenLocation.X, Project.ScreenLocation.Y }, InfoColor, FontSize, true, false);
			if (Project.EntityName == "ProjGrenade_C")
			{
				/*Utils::Log(1, "TimeTillExplosion: %s ", Project.EntityName);*/
				ImVec2 HealthBarPos, HealthBarSize;
				HealthBarSize = { 26 * Scale, 5 * Scale };
				HealthBarPos = { Project.ScreenLocation.X - (HealthBarSize.x / 2), Project.ScreenLocation.Y + (InfoSize.y + 3 * Scale) };
				DrawHealthBar(Project.Entity, 100.f, Project.TimeTillExplosion / 4.0f * 100.0f, HealthBarPos, HealthBarSize, InfoColor, true, false);
				if (GameData.Config.Project.EnableRange) {
					DrawExplosiveRange(Project.Location, Project.ExplosionRange, InfoColor, 3);
				}
			}
		}
	}





	static void DrawLocalPlayerProject()
	{
		if (!GameData.Config.Project.ShowChareTime) return;

		if (GameData.LocalPlayerInfo.ElapsedCookingTime > 0 && GameData.LocalPlayerInfo.WeaponClass == EWeaponClass::Class_Throwable)
		{
			static bool flashTriggered = false;
			float totalTime = 5.05f;
			if (GameData.LocalPlayerInfo.WeaponName== "闪光弹")
			{
				totalTime /= 2.f;
			}
			float progressValue = GameData.LocalPlayerInfo.ElapsedCookingTime / totalTime;

			if (progressValue >= 0.75f && !flashTriggered) {
				flashTriggered = true;
				progressValue = 0.f;
			}
			else if (flashTriggered) {
				flashTriggered = false;
			}

			ImColor Color = Utils::FloatToImColor(GameData.Config.Project.ChareColor);

			RenderHelper::DrawDashboardProgress(
				ImVec2(GameData.Config.Overlay.ScreenWidth / 2, GameData.Config.Overlay.ScreenHeight / 2),
				120.f * (GameData.Config.Project.ChareFontSize / 14.f),
				2, 
				20,
				8,
				progressValue,
				Color
			);
		}
	}

	static void DrawVehicles(std::unordered_map<uint64_t, VehicleInfo> Vehicles)
	{
		if (!GameData.Config.Vehicle.Enable) return;

		std::vector<std::pair<uint64_t, VehicleInfo>> Vectors(Vehicles.begin(), Vehicles.end());

		std::sort(Vectors.begin(), Vectors.end(), [](const std::pair<uint64_t, VehicleInfo>& a, const std::pair<uint64_t, VehicleInfo>& b) {
			return a.second.Distance > b.second.Distance;
		});

		for (auto& Item : Vectors)
		{
			VehicleInfo& Vehicle = Item.second;

			if (Vehicle.Distance > GameData.Config.Vehicle.DistanceMax) continue;

			Vehicle.ScreenLocation = VectorHelper::WorldToScreen(Vehicle.Location);
			Vehicle.Distance = GameData.Camera.Location.Distance(Vehicle.Location) / 100.0f;
			std::string Text = Utils::StringToUTF8(std::format("{} [{}M]", Vehicle.Name, (int)Vehicle.Distance));
			ImColor InfoColor = Utils::FloatToImColor(GameData.Config.Vehicle.Color);

			ImVec2 HeadInfoSize = RenderHelper::StrokeText(Text.c_str(), { Vehicle.ScreenLocation.X, Vehicle.ScreenLocation.Y }, InfoColor, GameData.Config.Vehicle.FontSize, true, true);

		}
	}

	static void DrawVehicleWheels(std::unordered_map<uint64_t, VehicleWheelInfo> VehicleWheels)
	{
		std::vector<std::pair<uint64_t, VehicleWheelInfo>> Vectors(VehicleWheels.begin(), VehicleWheels.end());

		std::sort(Vectors.begin(), Vectors.end(), [](const std::pair<uint64_t, VehicleWheelInfo>& a, const std::pair<uint64_t, VehicleWheelInfo>& b) {
			return a.second.Distance > b.second.Distance;
		});

		for (auto& Item : Vectors)
		{
			VehicleWheelInfo& Wheel = Item.second;

			Wheel.ScreenLocation = VectorHelper::WorldToScreen(Wheel.Location);
			float Distance = Utils::CalculateDistance(GameData.Config.Overlay.ScreenWidth / 2, GameData.Config.Overlay.ScreenHeight / 2, Wheel.ScreenLocation.X, Wheel.ScreenLocation.Y);

			std::string Text = std::format("{}M {}", (int)Distance, Wheel.DampingRate);
			ImColor InfoColor = ImColor(255, 255, 255);


			ImVec2 HeadInfoSize = RenderHelper::StrokeText(Text.c_str(), { Wheel.ScreenLocation.X, Wheel.ScreenLocation.Y }, InfoColor, 14, true, true);
		}
	}

	static PlayerRankInfo GetPlayerRankInfo(const std::unordered_map<std::string, PlayerRankList> PlayerRankLists, const Player Player)
	{
		PlayerRankInfo PlayerRankData;
		if (PlayerRankLists.count(Player.Name) > 0)
		{
			PlayerRankList PlayerRank = PlayerRankLists.at(Player.Name);

			switch (GameData.Config.PlayerList.RankMode) {
			case 1:
				PlayerRankData = PlayerRank.TPP;
				break;
			case 2:
				PlayerRankData = PlayerRank.SquadTPP;
				break;
			case 3:
				PlayerRankData = PlayerRank.FPP;
				break;
			case 4:
				PlayerRankData = PlayerRank.SquadFPP;
				break;
			default:
				break;
			}
		}
		return PlayerRankData;
	}
	static void DrawSkeleton_Physx(Player Player, const ImU32 SkeletonUseColor, const float Thickness)
	{
		FVector2D neckpos = Player.Skeleton.ScreenBones[EBoneIndex::Neck_01];
		FVector2D pelvispos = Player.Skeleton.ScreenBones[EBoneIndex::Pelvis];
		std::vector<std::tuple<ImVec2, ImVec2, ImU32>> Bones;
		bool failed = false;

		for (const auto& a : SkeletonLists::Skeleton)
		{
			FVector2D previous = FVector2D(0, 0);
			FVector2D current, p1, c1;


			for (EBoneIndex bone : a)
			{
				current = (bone == EBoneIndex::Neck_01) ? neckpos : (bone == EBoneIndex::Pelvis) ? pelvispos : Player.Skeleton.ScreenBones[bone];

				if (previous.X != 0.f && previous.Y != 0.f)
				{
					p1 = previous;
					c1 = current;

					auto visible = Player.Skeleton.VisibleBones[bone];
					ImU32 color = ImGui::ColorConvertFloat4ToU32(Utils::FloatToImColor(GameData.Config.ESP.Color.Visible.Skeleton));
					Bones.emplace_back(std::make_tuple(ImVec2(p1.X, p1.Y), ImVec2(c1.X, c1.Y), visible ? color : SkeletonUseColor));

					if (p1.X == INFINITY || p1.Y == INFINITY || c1.X == INFINITY || c1.Y == INFINITY) {
						failed = true;
						Bones.clear();
						break;
					}

				}
				previous = current;
			}

			if (failed)
				break;
		}

		for (auto& line : Bones)
		{
			RenderHelper::Line(std::get<0>(line), std::get<1>(line), std::get<2>(line), Thickness);
		}
	}
	static void DrawRadars(std::unordered_map<uint64_t, Player> Players, std::unordered_map<uint64_t, VehicleInfo> Vehicles, std::unordered_map<uint64_t, PackageInfo> Packages)
	{
		for (auto& Item : Vehicles)
		{
			VehicleInfo& Vehicle = Item.second;

			std::string IconUrl = "Assets/Icons/Radar/T_Icon_WorldMap_Vehicle_Land.png";

			if (GameData.Radar.Visibility && GameData.Config.Radar.Main.ShowVehicle)
			{
				FVector2D RadarLocation = Radar::WorldToRadarLocation(Vehicle.Location);

				if (GImGuiTextureMap[IconUrl].Width > 0) {
					float IconSize = GImGuiTextureMap[IconUrl].Width * (0.25f * (GameData.Config.Radar.Main.FontSize / 14.f));
					RenderHelper::AddImageRotated(GImGuiTextureMap[IconUrl].Texture, { RadarLocation.X, RadarLocation.Y }, ImVec2(IconSize, IconSize), 0);
				}
			}

			if (GameData.Radar.MiniRadarVisibility && Vehicle.Distance < GameData.Radar.MiniRadarDistance && GameData.Config.Radar.Mini.ShowVehicle)
			{
				FVector2D MiniRadarLocation = Radar::WorldToMiniRadarLocation(Vehicle.Location);

				if (GImGuiTextureMap[IconUrl].Width > 0) {
					float IconSize = GImGuiTextureMap[IconUrl].Width * (0.20f * (GameData.Config.Radar.Mini.FontSize / 14.f));
					RenderHelper::AddImageRotated(GImGuiTextureMap[IconUrl].Texture, { MiniRadarLocation.X, MiniRadarLocation.Y }, ImVec2(IconSize, IconSize), 0);
				}
			}

		}

		for (auto& Item : Packages)
		{
			PackageInfo& Package = Item.second;

			if (Package.Type == EntityType::AirDrop)
			{
				std::string IconUrl = "Assets/Icons/Radar/CarePackage_normal.png";

				if (GameData.Radar.Visibility && GameData.Config.Radar.Main.ShowAirDrop)
				{
					FVector2D RadarLocation = Radar::WorldToRadarLocation(Package.Location);

					if (GImGuiTextureMap[IconUrl].Width > 0) {
						float IconSize = (0.18f * (GameData.Config.Radar.Main.FontSize / 14.f));
						float IconWidth = GImGuiTextureMap[IconUrl].Width * IconSize;
						float IconHeight = GImGuiTextureMap[IconUrl].Height * IconSize;
						RenderHelper::AddImageRotated(GImGuiTextureMap[IconUrl].Texture, { RadarLocation.X, RadarLocation.Y }, ImVec2(IconWidth, IconHeight), 0);
					}
				}

				if (GameData.Radar.MiniRadarVisibility && Package.Distance < GameData.Radar.MiniRadarDistance && GameData.Config.Radar.Mini.ShowAirDrop)
				{
					FVector2D MiniRadarLocation = Radar::WorldToMiniRadarLocation(Package.Location);

					if (GImGuiTextureMap[IconUrl].Width > 0) {
						float IconSize = (0.14f * (GameData.Config.Radar.Main.FontSize / 14.f));
						float IconWidth = GImGuiTextureMap[IconUrl].Width * IconSize;
						float IconHeight = GImGuiTextureMap[IconUrl].Height * IconSize;
						RenderHelper::AddImageRotated(GImGuiTextureMap[IconUrl].Texture, { MiniRadarLocation.X, MiniRadarLocation.Y }, ImVec2(IconWidth, IconHeight), 0);
					}
				}
			}
			else {
				std::string IconUrl = "Assets/Icons/Radar/indicator_onscreen_status_dead.png";

				if (GameData.Radar.Visibility && GameData.Config.Radar.Main.ShowDeadBox)
				{
					FVector2D RadarLocation = Radar::WorldToRadarLocation(Package.Location);

					if (GImGuiTextureMap[IconUrl].Width > 0) {
						float IconSize = GImGuiTextureMap[IconUrl].Width * (0.45f * (GameData.Config.Radar.Main.FontSize / 14.f));
						RenderHelper::AddImageRotated(GImGuiTextureMap[IconUrl].Texture, { RadarLocation.X, RadarLocation.Y }, ImVec2(IconSize, IconSize), 0);
					}
				}

				if (GameData.Radar.MiniRadarVisibility && Package.Distance < GameData.Radar.MiniRadarDistance && GameData.Config.Radar.Mini.ShowDeadBox)
				{
					FVector2D MiniRadarLocation = Radar::WorldToMiniRadarLocation(Package.Location);

					if (GImGuiTextureMap[IconUrl].Width > 0) {
						float IconSize = GImGuiTextureMap[IconUrl].Width * (0.40f * (GameData.Config.Radar.Mini.FontSize / 14.f));
						RenderHelper::AddImageRotated(GImGuiTextureMap[IconUrl].Texture, { MiniRadarLocation.X, MiniRadarLocation.Y }, ImVec2(IconSize, IconSize), 0);
					}
				}
			}
		}

		for (auto& Item : Players)
		{
			Player& Player = Item.second;

			if (Player.InFog || Player.IsMe || Player.IsMyTeam || (Player.State == CharacterState::Dead))
			{
				continue;
			}

			RenderHelper::PlayerColor PlayerColors = RenderHelper::GetPlayerColor(Player);

			FVector AimDirection = FRotator(0.0f, Player.AimOffsets.Yaw, 0.0f).GetUnitVector();
			FVector2D Direction = FVector2D{ AimDirection.X, AimDirection.Y };
			float AngleRadians = atan2(Direction.Y, Direction.X);
			float AngleDegrees = AngleRadians;

			if (GameData.Radar.Visibility && GameData.Config.Radar.Main.ShowPlayer)
			{
				FVector2D RadarLocation = Radar::WorldToRadarLocation(Player.Location);

				//RenderHelper::Line({ RadarLocation.X, RadarLocation.Y }, { Direction.X, Direction.Y }, ImColor(255, 255, 255, 255), 1.f);
				RenderHelper::DrawRadarPlayerCircleWithText(Player.Type == EntityType::AI ? "AI" : std::to_string(Player.TeamID).c_str(), PlayerColors.teamNumberColor, 10 * (GameData.Config.Radar.Main.FontSize / 14.f), { RadarLocation.X, RadarLocation.Y }, AngleDegrees, Player.RadarState);
			}

			if (GameData.Radar.MiniRadarVisibility && Player.Distance < GameData.Radar.MiniRadarDistance && GameData.Config.Radar.Mini.ShowPlayer)
			{
				FVector2D MiniRadarLocation = Radar::WorldToMiniRadarLocation(Player.Location);
				RenderHelper::DrawRadarPlayerCircleWithText(Player.Type == EntityType::AI ? "AI" : std::to_string(Player.TeamID).c_str(), PlayerColors.teamNumberColor, 10 * (GameData.Config.Radar.Mini.FontSize / 14.f), { MiniRadarLocation.X, MiniRadarLocation.Y }, AngleDegrees, Player.RadarState);
			}

		}
	}
	
	static void DrawPlayers(std::unordered_map<uint64_t, Player> Players)
	{
		if (!GameData.Config.ESP.Enable) return;

		std::vector<std::pair<uint64_t, Player>> PlayersVector(Players.begin(), Players.end());

		std::sort(PlayersVector.begin(), PlayersVector.end(), [](const std::pair<uint64_t, Player>& a, const std::pair<uint64_t, Player>& b) {
			return a.second.Distance > b.second.Distance;
			});
		int M200PlayerCount = 0;
		int PalyerBB = 500;//最近敌人
		std::string DisNane = "";
		Utils::Log(1, "length :[%d]", Players.size());
		for (auto& Item : PlayersVector)
		{
			Player& Player = Item.second;
			/*if (Player.IsMe || Player.IsMyTeam || (Player.State == CharacterState::Dead) || (Player.TeamID == 0))
			{
				continue;
			}*/
			Utils::Log(1, "DMA 1");
			Player.Distance = GameData.Camera.Location.Distance(Player.Location) / 100.0f;

			if (Player.Distance > GameData.Config.ESP.DistanceMax)
			{
				continue;
			}
			Utils::Log(1, "DMA 2");
			if (Player.Distance <= 200)
			{
				M200PlayerCount++;
				if (PalyerBB > Player.Distance)
				{
					PalyerBB = Player.Distance;
					DisNane = Player.Name;
				}

			}

			//是否可以显示信息
			bool bShowInfo = (Player.Distance <= GameData.Config.ESP.InfoDistanceMax) ||
				(GameData.Config.ESP.AimExpandInfo && GameData.AimBot.TargetPlayerInfo.Entity == Player.Entity);

			bool bShowWeapon = ((Player.Distance <= GameData.Config.ESP.WeaponDistanceMax && GameData.Config.ESP.Weapon) || bShowInfo) &&
				(Player.WeaponName != "fail" && Player.WeaponName != "Unknown") &&
				(Player.WeaponID > 0);



			// 预先缓存骨骼列表长度，避免重复获取


			const auto& bones = SkeletonLists::Bones;
			for (EBoneIndex Bone : bones) {
				Player.Skeleton.LocationBones[Bone] = VectorHelper::GetBoneWithRotation(Player.Skeleton.Bones[Bone], Player.ComponentToWorld);
				Player.Skeleton.ScreenBones[Bone] = VectorHelper::WorldToScreen(Player.Skeleton.LocationBones[Bone]);

			}


			RenderHelper::PlayerColor PlayerColors = RenderHelper::GetPlayerColor(Player);
			ImColor UseColor = PlayerColors.infoUseColor;


			if (!Player.InScreen)
				continue;

			//是否可以显示骨骼
			if (GameData.Config.ESP.Skeleton) {
				bool isLockedTarget = (GameData.AimBot.Target == Player.Entity && GameData.AimBot.Lock);
				bool shouldDrawSkeleton = !GameData.Config.ESP.LockedHiddenBones || !isLockedTarget;

				if (GameData.Config.ESP.Skeleton && (!GameData.Config.ESP.LockedHiddenBones || (GameData.AimBot.Target != Player.Entity || !GameData.AimBot.Lock)))
				{
					DrawSkeleton_Physx(Player, PlayerColors.skeletonUseColor, GameData.Config.ESP.SkeletonWidth);
				}

			}

			ImVec2 Pos = {
				static_cast<float>(static_cast<int>(Player.Skeleton.ScreenBones[EBoneIndex::Root].X)),
				static_cast<float>(static_cast<int>(Player.Skeleton.ScreenBones[EBoneIndex::ForeHead].Y))
			};

			int FontSize = GameData.Config.ESP.FontSize;

			auto Rect = Get2DBox(Player);

			FVector2D HeadPos = Player.Skeleton.ScreenBones[EBoneIndex::ForeHead];
			//鎖定變色 
			
			if (GameData.Config.ESP.Skeleton && (!GameData.Config.ESP.LockedHiddenBones || (GameData.AimBot.Target != Player.Entity || !GameData.AimBot.Lock))) {
				DrawSkeleton_Physx(Player, PlayerColors.skeletonUseColor, GameData.Config.ESP.SkeletonWidth);
			}
			
			const auto h = (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::Foot_L].Y - (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::ForeHead].Y, w = (h / 4.0f) * 2.5f, x = (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::Root].X - (w / 2.0f), y = (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::ForeHead].Y;

			const float Scale = (float)FontSize / 14.f;
			ImVec2 HealthBarPos, HealthBarSize;

			auto health_xPos = NULL; // 定义变量，存储血条的x坐标

			//绘制血条
			float Health = Player.State == CharacterState::Groggy ? Player.GroggyHealth : Player.Health;
			if (Player.State == CharacterState::Groggy) UseColor = Utils::FloatToImColor(GameData.Config.ESP.Color.Groggy.Info);
			constexpr auto health_bar_width = 20.0f; // 血条的固定宽度
			constexpr auto health_bar_height = 4.0f;  // 血条的固定高度
			constexpr auto example_health_max_value = 100.0f;
			//只有在配置中启动血条时才绘制血条
			if (GameData.Config.ESP.health_bar && bShowInfo) {

				const auto clamped_health = std::clamp(Health, 0.0f, 100.0f);
				const auto health_fill_width = (health_bar_width * clamped_health) / example_health_max_value;
				const auto health_x = x + (w - health_bar_width) / 2.0f;

				const auto health_y = y - 5.0f;
				//检查是否绘制顶部血条
				if (GameData.Config.ESP.XueTiaoWEizhi == 0)//血条位置
				{
					if (GameData.Config.ESP.HealthBarStyle == 0)
					{
						constexpr auto health_bar_width = 20.0f; // 血条的固定宽度
						constexpr auto health_bar_height = 4.0f;  // 血条的固定高度
						constexpr auto example_health_max_value = 100.0f;

						const auto color_1 = ImColor{ 255, 0, 0 };
						const auto color_2 = ImColor{ 255, 128, 0 };
						const auto color_3 = ImColor{ 0, 255, 0 };
						const auto health_x = x + (w - health_bar_width) / 2.0f;
						health_xPos = health_x;

						//绘制血条背景
						RenderHelper::window_filled_rect({ health_x, y - 5.0f }, { health_bar_width,health_bar_height }, { 30, 30, 30 });
						const auto health_fill_width = (health_bar_width * clamped_health) / example_health_max_value;


						//// 根据健康值设置彩虹颜色
						const auto rainbow_bar = [](const ImVec2& position, const ImVec2& size, const ImColor& color1, const ImColor& color2, const ImColor& color3, const ImColor& color4)
							{
								RenderHelper::window_filled_rect_multicolor_horizontal(position, { size.x / 2.0f, size.y }, color1, color2);
								RenderHelper::window_filled_rect_multicolor_horizontal({ position.x + size.x / 2.0f, position.y }, { size.x / 2.0f, size.y }, color3, color4);
							};

						rainbow_bar
						(
							{ health_x + 1.0f, y - 4.0f }, { (health_fill_width - 2.0f),health_bar_height - 1.0f },
							color_1,
							Health < 30.0f ? color_1 : color_2,
							Health < 30.0f ? color_1 : color_2,
							Health < 60.0f ? color_2 : color_3
						);


					}
					else if (GameData.Config.ESP.HealthBarStyle == 1) {
						constexpr auto health_bar_width = 20.0f; // 血条的固定宽度
						constexpr auto health_bar_height = 4.0f;  // 血条的固定高度
						constexpr auto example_health_max_value = 100.0f;

						const auto color_3 = ImColor{ 0, 255, 0 };

						const auto health_x = x + (w - health_bar_width) / 2.0f;
						health_xPos = health_x;

						// 绘制血条背景
						RenderHelper::window_filled_rect({ health_x, y - 5.0f }, { health_bar_width, health_bar_height }, { 30, 30, 30 });

						const auto health_fill_width = (health_bar_width * Health) / example_health_max_value;

						// 根据健康值设置彩虹颜色
						const auto rainbow_bar = [](const ImVec2& position, const ImVec2& size, const ImColor& color_1, const ImColor& color_2, const ImColor& color3, const ImColor& color4) {
							RenderHelper::window_filled_rect_multicolor_horizontal(position, { size.x / 2.0f, size.y }, color_1, color_2);
							RenderHelper::window_filled_rect_multicolor_horizontal({ position.x + size.x / 2.0f, position.y }, { size.x / 2.0f, size.y }, color3, color4);
							};

						rainbow_bar
						(
							{ health_x + 1.0f, y - 4.0f },
							{ health_fill_width - 2.0f, health_bar_height - 1.0f },
							color_3,
							Health < 30.0f ? color_3 : color_3,
							Health < 30.0f ? color_3 : color_3,
							Health < 60.0f ? color_3 : color_3
						);

					}
					else if (GameData.Config.ESP.HealthBarStyle == 2) {

						auto example_health_value = Health;
						constexpr auto example_health_max_value = 100.0f;
						const auto color_1 = ImColor{ 255, 0, 0 };
						const auto green_color = ImColor{ 0, 255, 0 };

						auto nickname_label_size = ImVec2{ w,6.0f * 3.0f };

						const auto health_w = (nickname_label_size.x < w) ? w : nickname_label_size.x;
						const auto health_x = x + (w - health_w) / 2.0f;
						health_xPos = health_x;

						// 绘制血条背景
						RenderHelper::window_filled_rect({ health_x, y - 5.0f }, { health_w, 3.0f }, { 30, 30, 30 });

						RenderHelper::window_filled_rect_multicolor_horizontal(
							{ health_x + 1.0f, y - 4.0f },
							{ (health_w - 2.0f * example_health_value) / example_health_value, 2.0f },
							color_1,
							green_color


						);

					}
					else if (GameData.Config.ESP.HealthBarStyle == 3) {

						auto example_health_value = Health;
						constexpr auto example_health_max_value = 100.0f;

						const auto color_1 = ImColor{ 255, 0, 0 };
						const auto color_2 = ImColor{ 255, 128, 0 };
						const auto color_3 = ImColor{ 0, 255, 0 };

						auto nickname_label_size = ImVec2{ w,6.0f * 3.0f };

						const auto health_w = (nickname_label_size.x < w) ? w : nickname_label_size.x;

						const auto health_x = x + (w - health_w) / 2.0f;

						health_xPos = health_x;

						// 绘制血条背景
						RenderHelper::window_filled_rect({ health_x, y - 5.0f }, { health_w, 3.0f }, { 30, 30, 30 });



						// 根据健康值设置彩虹颜色
						const auto rainbow_bar = [](const ImVec2& position, const ImVec2& size, const ImColor& color1, const ImColor& color2, const ImColor& color3, const ImColor& color4) {
							RenderHelper::window_filled_rect_multicolor_horizontal(position, { size.x / 2.0f, size.y }, color1, color2);
							RenderHelper::window_filled_rect_multicolor_horizontal({ position.x + size.x / 2.0f, position.y }, { size.x / 2.0f, size.y }, color3, color4);
							};

						rainbow_bar
						(
							{ health_x + 1.0f, y - 4.0f },
							{ ((health_w - 2.0f) * example_health_value) / example_health_max_value, 2.0f },
							color_1,
							example_health_value < 30.0f ? color_1 : color_2,
							example_health_value < 30.0f ? color_1 : color_2,
							example_health_value < 60.0f ? color_2 : color_3
						);


					}
				}
				else if (GameData.Config.ESP.XueTiaoWEizhi == 1)
				{
					//绘制左侧血条时不在绘制顶部血条
					HealthBarPos = { Rect.x - 6, Rect.y };
					HealthBarSize = { std::clamp(Rect.w / 4.f, 0.f, 4.f) ,Rect.w - 1 };

					DrawHealthBar(Player.Entity, 100.f, Health, HealthBarPos, HealthBarSize, UseColor, true);
					health_xPos = HealthBarPos.x;
				}

			}
			std::string InfoText = "";

			std::string TeamID = std::to_string(Player.TeamID);

			if (Player.Type == EntityType::AI)
			{
				TeamID = "BOT";
			}
			//队伍编号
			ImVec2 textSize = ImGui::CalcTextSize(Utils::StringToUTF8(InfoText).c_str());
			if (GameData.Config.ESP.TeamID && bShowInfo) {
				//RenderHelper::DrawRadarPlayerCircleWithText(
				//	std::to_string(Player.TeamID).c_str(),
				//	PlayerColors.teamNumberColor,
				//	7 * (14 / 12.f),
				//	/*{ health_xPos - 25.0f, y - 5.0f },*/
				//	{ health_xPos - textSize.x / 7 , Pos.y + 2.0f },
				//	0);
				InfoText += std::format("[{}] ", Player.TeamID);
			}

			
			//名字
			if (GameData.Config.ESP.Nickname && bShowInfo) InfoText += Player.Name + " ";




			Utils::Trim(InfoText);//人名字绘制

			if (InfoText != "") {
				ImVec2 HeadInfoSize = RenderHelper::StrokeText(Utils::StringToUTF8(InfoText).c_str(), { Pos.x, Pos.y - 6.f }, PlayerColors.infoUseColor, FontSize, true, true);
				Pos.y -= HeadInfoSize.y;
			}
			auto Ranky = Pos.y;
			ImVec2 HeadInfoSize;
			PlayerRankInfo PlayerRankData;
			

		
			std::string KDAText = "";
			if (GameData.Config.ESP.KDA && PlayerRankData.KDA > 0.0f)
			{
				std::ostringstream oss;
				oss << std::fixed << std::setprecision(2) << PlayerRankData.KDA;
				KDAText += "KD:" + oss.str();
				ImVec2 HeadInfoSize = RenderHelper::StrokeText(Utils::StringToUTF8(KDAText).c_str(), { Pos.x, Pos.y }, IM_COL32(255, 0, 0, 255), FontSize, true, false);
				Pos.y += HeadInfoSize.y - 2;
			}

			//枪械图标
			if (bShowWeapon)
			{
				std::string IconUrl = "Assets/image/Weapon/" + Player.WeaponName + ".png";
				if (GImGuiTextureMap[IconUrl].Width > 0) {
					float TargetHeight = 16.f * Scale;
					float HeightZoom = TargetHeight / GImGuiTextureMap[IconUrl].Height;
					float IconWidth = GImGuiTextureMap[IconUrl].Width * HeightZoom;
					float IconHeight = TargetHeight;
					Pos.x -= IconWidth / 2;
					Pos.y -= IconHeight + 3;// 
					if (!GameData.Config.PlayerList.RankMode)
						Pos.y += HeadInfoSize.y;
					RenderHelper::Image(GImGuiTextureMap[IconUrl].Texture, Pos, ImVec2(IconWidth, IconHeight), true, PlayerColors.infoUseColor);
				}
			}
			Pos.x = (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::Root].X;
			Pos.y = (float)(int)Player.Skeleton.ScreenBones[EBoneIndex::Root].Y;
			if (GameData.Config.ESP.Dis)
			{
				//距离
				InfoText = "";

				InfoText += std::format("{}M", (int)Player.Distance);

				if (InfoText != "") {
					ImVec2 HeadInfoSize = RenderHelper::StrokeText(Utils::StringToUTF8(InfoText).c_str(), { Pos.x, Pos.y }, PlayerColors.infoUseColor, FontSize, true, false);
					Pos.y += HeadInfoSize.y - 2;
				}
			}

			
			//击杀
			if (Player.KillCount > 0)
			{
				InfoText = "";
				InfoText += std::format("K.{}", (int)Player.KillCount);

				if (InfoText != "") {
					ImVec2 HeadInfoSize = RenderHelper::StrokeText(Utils::StringToUTF8(InfoText).c_str(), { Pos.x, Pos.y }, PlayerColors.infoUseColor, FontSize, true, false);
					Pos.y += HeadInfoSize.y - 2;
				}
			}
			
			//掉线状态
			if (GameData.Config.ESP.ShowInfos && bShowInfo) {
				InfoText = "";
				switch (Player.CharacterState)
				{
				case ECharacterState::Offline:
					InfoText = U8("掉线");
					break;
				default:
					break;
				}
				if (InfoText != "") {
					ImVec2 HeadInfoSize = RenderHelper::StrokeText(InfoText.c_str(), { Pos.x, Pos.y }, PlayerColors.infoUseColor, FontSize, true, false);
					Pos.y += HeadInfoSize.y - 2;
				}
			}
			
			//被瞄射线
			if (Player.IsAimMe && GameData.Config.ESP.TargetedRay)
			{
				auto IsAimMeX = Player.Skeleton.ScreenBones[EBoneIndex::ForeHead].X;
				auto IsAimMeY = Player.Skeleton.ScreenBones[EBoneIndex::ForeHead].Y;

				if (!Player.InScreen)
				{
					FVector2D LoctionToScreen = VectorHelper::WorldToScreen(Player.Location);
					IsAimMeY = LoctionToScreen.Y;
					IsAimMeY = LoctionToScreen.X;
				}

				if (!GameData.Config.ESP.VisibleCheckRay || Player.IsVisible)
				{

					RenderHelper::Line(ImVec2(IsAimMeX, IsAimMeY), ImVec2(GameData.Config.Overlay.ScreenWidth / 2, GameData.Config.Overlay.ScreenHeight / 2), ImGui::ColorConvertFloat4ToU32(Utils::FloatToImColor(GameData.Config.ESP.Color.Ray.Line)), GameData.Config.ESP.RayWidth);
				}

			}

		}
		//危险预警
		if (GameData.Config.ESP.DangerWarning)
		{

			// 计算相对位置（例如，固定在窗口的右下角）
			float posX = GameData.Config.Overlay.ScreenWidth / 2; // 相对于窗口宽度的85%
			float posY = GameData.Config.Overlay.ScreenHeight - 150; // 相对于窗口高度的90%
			int EnemiesBehindCount = 0;
			bool IsBeingAimedFromBehind = false;

			for (auto& Item : Players)
			{
				Player& Player = Item.second;
				if (!Player.IsAimMe) continue;

				float PlayerAngle = atan2(
					Player.Location.Y - GameData.LocalPlayerInfo.Location.Y,
					Player.Location.X - GameData.LocalPlayerInfo.Location.X

				) * 180 / M_PI;

				PlayerAngle = fmod(PlayerAngle - GameData.Camera.Rotation.Yaw + 360, 360);

				if (PlayerAngle >= 90 && PlayerAngle <= 270)
				{
					EnemiesBehindCount++;
					IsBeingAimedFromBehind = true;
				}
			}


			// 绘制文本
			if (M200PlayerCount > 0) {
				RenderHelper::StrokeText(Utils::StringToUTF8(std::format(
					"200米范围内敌人数量: {}  离你最近的敌人: {}米 最近的玩家名字: {}",
					M200PlayerCount,
					PalyerBB,
					DisNane
				)).c_str(), ImVec2(posX, posY), IM_COL32(255, 0, 0, 255), 14.0, true, true);
			}
			if (IsBeingAimedFromBehind)
			{
				std::string warningText = std::format(
					"敌人在背后瞄准你！"
				);

				ImVec2 textSize = ImGui::GetFont()->CalcTextSizeA(24, FLT_MAX, 0.f, warningText.c_str());
				ImVec2 textPos = { GameData.Config.Overlay.ScreenWidth / 2 - textSize.x / 2, 100 }; // 屏幕顶部

				RenderHelper::StrokeText(Utils::StringToUTF8(warningText).c_str(), textPos, IM_COL32(255, 0, 0, 255), 14, true, false);
			}

		}

	}


	static void DrawTakeHitInstigator()
	{
		float Size = 200.f;
		float AngleXSize = 2.f;
		float AngleYSize = 5.f;
		float Angle = atan2(GameData.LocalPlayerInfo.Location.Y - GameData.LocalPlayerInfo.TakeHitInfo.AttackerLocation.Y, GameData.LocalPlayerInfo.Location.X - GameData.LocalPlayerInfo.TakeHitInfo.AttackerLocation.X);
		Angle = Angle * 180 / M_PI;
		Angle = GameData.Camera.Rotation.Yaw + 360 - Angle; // Note: Y is actually X due to storage order differences

		ImVec2 A, B, C;
		A.x = (Size + AngleYSize) * sin(Angle * M_PI / 180) + GameData.Config.Overlay.ScreenWidth / 2;
		A.y = (Size + AngleYSize) * cos(Angle * M_PI / 180) + GameData.Config.Overlay.ScreenHeight / 2;

		ImU32 Color = ImColor(255, 255, 255, 255);

		ImVec2 HeadInfoSize = RenderHelper::StrokeText(std::format("{}", GameData.LocalPlayerInfo.TakeHitInfo.ActualDamage).c_str(), A, Color, 20, true, false);
	}

	static void DrawPlayersEarly(std::unordered_map<uint64_t, Player> Players)
	{
		if (!GameData.Config.Early.Enable) return;

		const float Scale = (float)GameData.Config.Early.FontSize / 14.f;
		float Size = 200.f * Scale;
		float AngleXSize = 2.f * Scale;
		float AngleYSize = 5.f * Scale;

		for (auto& Item : Players)
		{
			Player& Player = Item.second;

			if (Player.IsMe || Player.IsMyTeam || (Player.State == CharacterState::Dead))
			{
				continue;
			}

			float Distance = (GameData.Camera.Location).Distance(Player.Location) / 100;
			float Angle = atan2(GameData.Camera.Location.Y - Player.Location.Y, GameData.Camera.Location.X - Player.Location.X);
			Angle = Angle * 180 / M_PI;
			Angle = GameData.Camera.Rotation.Yaw + 360 - Angle; // Note: Y is actually X due to storage order differences

			ImVec2 A, B, C;
			A.x = (Size + AngleYSize) * sin(Angle * M_PI / 180) + GameData.Config.Overlay.ScreenWidth / 2;
			A.y = (Size + AngleYSize) * cos(Angle * M_PI / 180) + GameData.Config.Overlay.ScreenHeight / 2;

			B.x = (Size - AngleYSize) * sin((Angle - AngleXSize) * M_PI / 180) + GameData.Config.Overlay.ScreenWidth / 2;
			B.y = (Size - AngleYSize) * cos((Angle - AngleXSize) * M_PI / 180) + GameData.Config.Overlay.ScreenHeight / 2;

			C.x = (Size - AngleYSize) * sin((Angle + AngleXSize) * M_PI / 180) + GameData.Config.Overlay.ScreenWidth / 2;
			C.y = (Size - AngleYSize) * cos((Angle + AngleXSize) * M_PI / 180) + GameData.Config.Overlay.ScreenHeight / 2;

			ImU32 Color = ImColor(255, 0, 0, 255);

			float MaxDistance = GameData.Config.Early.DistanceMax;
			float Segment = MaxDistance / 4.0f;

			if (Distance < MaxDistance)
			{
				/*if (Distance < 150.0f)
					Color = ImColor(255, 0, 255, 255);
				else if (Distance < 300.0f)
					Color = ImColor(255, 255, 0, 255);
				else if (Distance < 400.0f)
					Color = ImColor(255, 0, 0, 255);*/

				if (Distance >= Segment && Distance < Segment * 2)
				{
					Color = ImColor(255, 165, 0, 255); // 红色
				}
				else if (Distance >= Segment * 2 && Distance < Segment * 3)
				{
					Color = ImColor(0, 255, 0, 255); // 橙黄色
				}
				else if (Distance >= Segment * 3)
				{
					Color = ImColor(255, 255, 255, 255); // 绿色
				}

				if (GameData.Config.Early.ShowDistance) ImVec2 HeadInfoSize = RenderHelper::StrokeText(std::format("{}M", (int)Distance).c_str(), {A.x + 13 * Scale, A.y}, Color, 13 * Scale, false, true);
				RenderHelper::DrawTriangle(A, B, C, Color, Player.IsAimMe);
			}
		}
	}

	static void DrawPackages(std::unordered_map<uint64_t, PackageInfo> Packages)
	{
		if (!GameData.Config.AirDrop.Enable && !GameData.Config.DeadBox.Enable) return;

		std::vector<std::pair<uint64_t, PackageInfo>> VectorItems(Packages.begin(), Packages.end());

		std::sort(VectorItems.begin(), VectorItems.end(), [](const std::pair<uint64_t, PackageInfo>& a, const std::pair<uint64_t, PackageInfo>& b) {
			return a.second.Distance > b.second.Distance;
		});

		for (auto& Item : VectorItems)
		{
			PackageInfo& Package = Item.second;

			if (Package.Type == EntityType::AirDrop)
			{
				if (Package.Distance > GameData.Config.AirDrop.DistanceMax || !GameData.Config.AirDrop.Enable) continue;

				Package.ScreenLocation = VectorHelper::WorldToScreen(Package.Location);
				Package.Distance = GameData.Camera.Location.Distance(Package.Location) / 100.0f;

				std::string Text = Utils::StringToUTF8(std::format("空投 [{}M]", (int)Package.Distance));

				ImColor InfoColor = Utils::FloatToImColor(GameData.Config.AirDrop.Color);

				ImVec2 HeadInfoSize = RenderHelper::StrokeText(Text.c_str(), { Package.ScreenLocation.X, Package.ScreenLocation.Y }, InfoColor, GameData.Config.AirDrop.FontSize, false, false);
			
				if (GameData.Config.AirDrop.ShowItems)
				{
					int Index = 0;
					int FistIndex = 0;
					FVector2D FistPos;

					for (auto& ItemInfo : Package.Items)
					{
						std::string Text = Utils::StringToUTF8(std::format("{}", ItemInfo.DisplayName));
						int GroupIndex = GameData.Config.Item.Lists[ItemInfo.Name].Group;
						ImColor TextColor = InfoColor;
						float FontSize = GameData.Config.AirDrop.FontSize - 1;

						switch (GroupIndex)
						{
						case 1:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupAColor);
							break;
						case 2:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupBColor);
							break;
						case 3:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupCColor);
							break;
						case 4:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupDColor);
							break;
						case 5:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupEColor);
							break;
						default:
							continue;
							break;
						}

						FistPos = Package.ScreenLocation;
						FistPos.Y += HeadInfoSize.y;

						ImVec2 ScreenLocation = { FistPos.X, FistPos.Y + ((FontSize + 1) * Index) };
						ImVec2 TextSize = RenderHelper::StrokeText(Text.c_str(), { ScreenLocation.x, ScreenLocation.y }, TextColor, FontSize, false, false);
						if (GameData.Config.Item.ShowIcon) {
							float Scale = (FontSize / 14.f);
							std::string IconUrl = "Assets/Icons/Item/All/" + ItemInfo.Name + ".png";
							if (GImGuiTextureMap[IconUrl].Width > 0) {
								float TargetHeight = 14.f * Scale;
								float HeightZoom = TargetHeight / GImGuiTextureMap[IconUrl].Height;
								float IconWidth = GImGuiTextureMap[IconUrl].Width * HeightZoom;
								float IconHeight = TargetHeight;
								ScreenLocation.x -= (FontSize + 4);

								RenderHelper::Image(GImGuiTextureMap[IconUrl].Texture, ScreenLocation, ImVec2(IconWidth, IconHeight), false);
							}
						}

						Index++;
					}
				}
			
			}
			else {

				if (Package.Distance > GameData.Config.DeadBox.DistanceMax || !GameData.Config.DeadBox.Enable) continue;

				Package.ScreenLocation = VectorHelper::WorldToScreen(Package.Location);
				Package.Distance = GameData.Camera.Location.Distance(Package.Location) / 100.0f;

				std::string Text = Utils::StringToUTF8(std::format("死亡盒 [{}M]", (int)Package.Distance));

				ImColor InfoColor = Utils::FloatToImColor(GameData.Config.DeadBox.Color);

				ImVec2 HeadInfoSize = RenderHelper::StrokeText(Text.c_str(), { Package.ScreenLocation.X, Package.ScreenLocation.Y }, InfoColor, GameData.Config.DeadBox.FontSize, false, false);
			
				if (GameData.Config.DeadBox.ShowItems)
				{
					int Index = 0;
					int FistIndex = 0;
					FVector2D FistPos;

					for (auto& ItemInfo : Package.Items)
					{
						std::string Text = Utils::StringToUTF8(std::format("{}", ItemInfo.DisplayName));
						int GroupIndex = GameData.Config.Item.Lists[ItemInfo.Name].Group;
						ImColor TextColor = InfoColor;
						float FontSize = GameData.Config.DeadBox.FontSize - 1;

						switch (GroupIndex)
						{
						case 1:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupAColor);
							break;
						case 2:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupBColor);
							break;
						case 3:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupCColor);
							break;
						case 4:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupDColor);
							break;
						case 5:
							TextColor = Utils::FloatToImColor(GameData.Config.Item.GroupEColor);
							break;
						default:
							continue;
							break;
						}

						FistPos = Package.ScreenLocation;
						FistPos.Y += HeadInfoSize.y + 2;

						ImVec2 ScreenLocation = { FistPos.X, FistPos.Y + ((FontSize + 1) * Index) };
						ImVec2 TextSize = RenderHelper::StrokeText(Text.c_str(), { ScreenLocation.x, ScreenLocation.y }, TextColor, FontSize, false, false);
						if (GameData.Config.Item.ShowIcon) {
							float Scale = (FontSize / 14.f);
							std::string IconUrl = "Assets/Icons/Item/All/" + ItemInfo.Name + ".png";
							if (GImGuiTextureMap[IconUrl].Width > 0) {
								float TargetHeight = 14.f * Scale;
								float HeightZoom = TargetHeight / GImGuiTextureMap[IconUrl].Height;
								float IconWidth = GImGuiTextureMap[IconUrl].Width * HeightZoom;
								float IconHeight = TargetHeight;
								ScreenLocation.x -= (FontSize + 4);

								RenderHelper::Image(GImGuiTextureMap[IconUrl].Texture, ScreenLocation, ImVec2(IconWidth, IconHeight), false);
							}
						}

						Index++;
					}
				}
			}
		}
	}

	static void DrawESP()
	{
		if (GameData.Scene != Scene::Gaming)
		{
			return;
		}

		std::unordered_map<uint64_t, Player> Players = Data::GetPlayers();
		std::unordered_map<uint64_t, VehicleWheelInfo> VehicleWheels = Data::GetVehicleWheels();
		std::unordered_map<uint64_t, VehicleInfo> Vehicles = Data::GetVehicles();
		std::unordered_map<uint64_t, ItemInfo> Items = Data::GetItems();
		std::unordered_map<uint64_t, ProjectInfo> Projects = Data::GetProjects();
		std::unordered_map<uint64_t, PackageInfo> Packages = Data::GetPackages();

		if (!GameData.bShowMouseCursor && !GameData.AimBot.Lock)
		{
			if (!GameData.Config.ESP.FocusMode)
			{
				DrawItems(Items);
				//DrawVehicleWheels(VehicleWheels);
				DrawVehicles(Vehicles);
				DrawPackages(Packages);
			}

			DrawFOV();

			DrawPlayersEarly(Players);
			//DrawTakeHitInstigator();
		}

		DrawPlayers(Players);
		DrawLocalPlayerProject();
		DrawRadars(Players, Vehicles, Packages);
		DrawAimBotPoint();
		DrawProjects(Projects);
	};
};

