#pragma once


#include <Common/Data.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <Mosquitto/mosquitto.h>
#include <TlHelp32.h>
#pragma comment(lib, "mosquitto.lib")
//�������QQȺ��964504044
static struct mosquitto* mosq;
static int rc = -1;

class WebRadar {
public:
	//MQTT���ӻص�����
	static void on_connect(struct mosquitto* mosq, void*, int rc) {
		if (rc == 0) {
			GameData.Config.WebRadar.isWebRadarConnect = true;
		}
		else {
			std::cerr << "Failed to connect to MQTT broker with code " << rc << std::endl;
		}
	}

	//MQTT��Ϣ���ջص�����
	static void on_message(struct mosquitto* mosq, void* userdata, const struct mosquitto_message* message) {
		std::cout << "Received message on topic " << message->topic << ": " << (char*)message->payload << std::endl;
	}
	static void on_disconnec(struct mosquitto* mosq, void*, int rc) {
		while (true)
		{
			if (mosquitto_reconnect(mosq) == MOSQ_ERR_SUCCESS)
				break;
			Sleep(5000);
		}


	}

	static void Init() {



		while (true)
		{
			try
			{
				if (!GameData.Config.WebRadar.isWebRadarEnable && !GameData.Config.WebRadar.isWebRadarConnect) {
					Sleep(500);
					continue;
				}
				if (GameData.Config.WebRadar.isWebRadarEnable == false && GameData.Config.WebRadar.isWebRadarConnect)
				{
					Destroy();
				}

				if (GameData.Config.WebRadar.isWebRadarConnect == false && GameData.Config.WebRadar.isWebRadarEnable)
				{
					Connect();
				}
				nlohmann::json Config;
				if (GameData.Scene == Scene::Gaming) {
					Config["Map"] = GameData.MapName;
					Config["MapX"] = GameData.Radar.WorldOriginLocation.X;
					Config["MapY"] = GameData.Radar.WorldOriginLocation.Y;

					Config["Game"][0][0] = GameData.Radar.SafetyZonePosition.X;
					Config["Game"][0][1] = GameData.Radar.SafetyZonePosition.Y;
					Config["Game"][0][2] = GameData.Radar.SafetyZoneRadius;
					Config["Game"][1][0] = GameData.Radar.BlueZonePosition.X;
					Config["Game"][1][1] = GameData.Radar.BlueZonePosition.Y;
					Config["Game"][1][2] = GameData.Radar.BlueZoneRadius;

					auto Players = Data::GetPlayers();
					int i = 0;
					for (auto Item : Players) {
						auto Player = Item.second;
						if (Player.InFog || (Player.State == CharacterState::Dead))
						{
							continue;
						}
						Config["Player"][i][0] = Player.Location.X;
						Config["Player"][i][1] = Player.Location.Y;
						Config["Player"][i][2] = Player.Distance;
						Config["Player"][i][3] = Player.TeamID;
						Config["Player"][i][4] = Player.Health;
						Config["Player"][i][5] = Player.KillCount;
						Config["Player"][i][6] = Player.SpectatedCount;
						Config["Player"][i][7] = Player.AimOffsets.Yaw;
						Config["Player"][i][8] = Player.IsMyTeam;
						Config["Player"][i][9] = Player.Type == EntityType::AI;
						Config["Player"][i][10] = Player.IsVisible;
						Config["Player"][i][11] = Player.IsMe;
						Config["Player"][i][12] = Player.IsAimMe;
						Config["Player"][i][13] = 0;
						Config["Player"][i][14] = Player.WeaponName;
						Config["Player"][i][15] = 0;
						Config["Player"][i][16] = Player.Name;
						int* teamNumberColor = GetColorForNumber(Player.TeamID);
						Config["Player"][i][17] = Player.Name;


						auto infoColor = GameData.Config.ESP.Color.Default.Info;
						Config["Player"][i][17] = rgb2hex(teamNumberColor[0], teamNumberColor[1], teamNumberColor[2], true);
						i++;
					}

					std::unordered_map<uint64_t, VehicleInfo> Vehicles = Data::GetVehicles();
					i = 0;
					for (auto item : Vehicles)
					{
						auto Vehicle = item.second;
						Config["Car"][i][0] = "Car";
						Config["Car"][i][1] = Vehicle.Location.X;
						Config["Car"][i][2] = Vehicle.Location.Y;
						std::cerr <<"car-->X" << Vehicle.Location.X << "car-->Y" << Vehicle.Location.Y << std::endl;
						i++;
					}

					Send(Config.dump());
				}
				Sleep(50);
			}
			catch (const std::exception&)
			{
			}
		}
		Destroy();
	}
	static void Connect() {

		try
		{
			OpenMqtt();
			// ��ʼ��mosquitto��
			mosquitto_lib_init();
			// ����һ���µ�mosquitto�ͻ���ʵ��
			mosq = mosquitto_new(NULL, true, NULL);
			if (!mosq) {
				std::cerr << "Failed to create Mosquitto client instance" << std::endl;
				return;
			}
			//mosquitto_int_option(mosq, MOSQ_OPT_PROTOCOL_VERSION, MQTT_PROTOCOL_V5);
			// �������ӻص�����
			mosquitto_connect_callback_set(mosq, on_connect);
			// ������Ϣ���ջص�����
			//mosquitto_message_callback_set(mosq, on_message);
			mosquitto_disconnect_callback_set(mosq, on_disconnec);
			// ���ӵ�MQTT����
			while (rc != MOSQ_ERR_SUCCESS)
			{
				rc = mosquitto_connect(mosq, GameData.Config.WebRadar.IP, std::atoi(GameData.Config.WebRadar.Port), 600);
				if (rc != MOSQ_ERR_SUCCESS) {
					std::cerr << "Unable to connect to MQTT broker: " << mosquitto_strerror(rc) << std::endl;
				}
				GameData.Config.WebRadar.isWebRadarConnect = true;
				//������IP�����߱���IPhttp://118.25.178.244
				//�˿���Ҫ���ռ�������ռ���˾ͻ���cmd netstat -a|findstr �˿ںţ� 8098��8083
				//std::string dynamicValue = "niubi";
				//std::string idValue = "ycnc_h";
				//GameData.Config.WebRadar.RadarUrl = "http://5201314.6655.la:8098/?" + std::string(GameData.Config.WebRadar.IP) + ":8083";
			    //GameData.Config.WebRadar.RadarUrl = "http://5201314.6655.la:8098/?" + dynamicValue + "&addr=" + std::string(GameData.Config.WebRadar.IP) + "&id=" + idValue;
				//GameData.Config.WebRadar.RadarUrl = "http://110.42.41.119:8888/?" + std::string(GameData.Config.WebRadar.IP) + "&sub=" + GameData.Config.WebRadar.SubTitle;
				GameData.Config.WebRadar.RadarUrl = "http://110.42.41.119:8888/?" + std::string(GameData.Config.WebRadar.IP) + "&sub=" + GameData.Config.WebRadar.SubTitle;

			}

		}
		catch (const std::exception&)
		{

		}
	}


	static void Destroy() {
		GameData.Config.WebRadar.isWebRadarConnect = false;
		mosquitto_lib_cleanup();
		mosquitto_destroy(mosq);
	}
	static bool Send(std::string message) {
		auto ret = mosquitto_pub_topic_check(GameData.Config.WebRadar.SubTitle);
		if (mosquitto_publish(mosq, NULL, GameData.Config.WebRadar.SubTitle, strlen(message.c_str()), message.c_str(), 0, false) == MOSQ_ERR_SUCCESS) {
			return true;
		}
		return false;
	}


	static bool getProcess(const char* procressName)                //�˺��������������ִ�Сд
	{
		char pName[MAX_PATH];                                //��PROCESSENTRY32�ṹ���е�szExeFile�ַ����鱣��һ�£����ڱȽ�
		strcpy(pName, procressName);                            //��������
		CharLowerBuff(pName, MAX_PATH);                        //������ת��ΪСд
		PROCESSENTRY32 currentProcess;                        //��ſ��ս�����Ϣ��һ���ṹ��
		currentProcess.dwSize = sizeof(currentProcess);        //��ʹ������ṹ֮ǰ�����������Ĵ�С
		HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//��ϵͳ�ڵ����н�����һ������

		if (hProcess == INVALID_HANDLE_VALUE)
		{
			printf("CreateToolhelp32Snapshot()����ʧ��!\n");
			return false;
		}

		bool bMore = Process32First(hProcess, &currentProcess);        //��ȡ��һ��������Ϣ
		while (bMore)
		{
			CharLowerBuff(currentProcess.szExeFile, MAX_PATH);        //��������ת��ΪСд
			if (strcmp(currentProcess.szExeFile, pName) == 0)            //�Ƚ��Ƿ���ڴ˽���
			{
				CloseHandle(hProcess);                                //���hProcess���
				return true;
			}
			bMore = Process32Next(hProcess, &currentProcess);            //������һ��
		}

		CloseHandle(hProcess);    //���hProcess���
		return false;
	}


	static void OpenMqtt() {
		if (getProcess("mqtt.exe"))
		{
			return;
		}

		TCHAR pBuf[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, pBuf);
		char str3[MAX_PATH];
		strcpy(str3, pBuf);
		strcat(str3, "\\mqtt");
		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.lpVerb = "open";
		sei.lpFile = "mqtt.exe";
		sei.lpParameters = "-c mqtt.conf";
		sei.lpDirectory = str3;
		sei.nShow = SW_HIDE;
		ShellExecuteEx(&sei);
		//WaitForSingleObject(sei.hProcess, INFINITE);//�ȴ�exe���н���
		CloseHandle(sei.hProcess);

	}
	//static void  Inti() {
	//	GameData.Config.WebRadar.IP = "127.0.0.1";
	//	GameData.Config.WebRadar.Port = 1472;
	//	GameData.Config.WebRadar.SubTitle = "/mqtt_backend";

	//	SOCKET sock;
	//	struct sockaddr addr;
	//	int errCode;
	//	char msg[255] = { 0 };

	//	WORD wVersionRequested;
	//	WSADATA wsaData;

	//	SetConsoleTitle("�ͻ���");
	//	wVersionRequested = MAKEWORD(2, 0);
	//	errCode = WSAStartup(wVersionRequested, &wsaData);

	//	if (errCode)
	//	{
	//		printf("û��Windows Socket��̬��!\n");
	//		//return WSAGetLastError();
	//	}

	//	if (LOBYTE(wsaData.wVersion) != 2 ||
	//		HIBYTE(wsaData.wVersion) != 0)
	//	{
	//		printf("��ҪWindows Socket 2!\n");
	//		WSACleanup();
	//		//return WSAGetLastError();
	//	}

	//	sock = socket(AF_INET, SOCK_STREAM, 0);


	//	if (sock == INVALID_SOCKET)
	//	{
	//		printf("���ܴ���Socket!\n");
	//		getch();
	//		WSACleanup();
	//		//return WSAGetLastError();
	//	}

	//	((sockaddr_in*)&addr)->sin_family = AF_INET;

	//	((sockaddr_in*)&addr)->sin_addr.s_addr = inet_addr("127.0.0.1");

	//	((sockaddr_in*)&addr)->sin_port = htons(5001);


	//	errCode = connect(sock, (sockaddr*)&addr, sizeof(addr));
	//	if (errCode == SOCKET_ERROR)
	//	{
	//		WSACleanup();
	//		return;
	//	}



	//	nlohmann::json Config;
	//	//std::thread (Init).detach();
	//	while (true)
	//	{
	//		Config["_"][0][0] = (int)GameData.Scene;

	//		if (GameData.Scene == Scene::Gaming) {
	//			Config["-"][0][1] = GameData.MapName;
	//			Config["_"][0][2] = GameData.Radar.BlueZonePosition.X;
	//			Config["_"][0][3] = GameData.Radar.BlueZonePosition.Y;
	//			Config["_"][0][4] = GameData.Radar.BlueZoneRadius;
	//			Config["_"][0][5] = GameData.Radar.SafetyZonePosition.X;
	//			Config["_"][0][6] = GameData.Radar.SafetyZonePosition.Y;
	//			Config["_"][0][7] = GameData.Radar.SafetyZoneRadius;

	//			auto Players = Data::GetPlayers();
	//			for (auto Item : Players) {
	//				auto Player = Item.second;
	//				Config["_"][1][0] = Player.ClanName;
	//				Config["_"][1][1] = Player.Name;
	//				Config["_"][1][2] = Player.Location.X;
	//				Config["_"][1][3] = Player.Location.Y;
	//				Config["_"][1][4] = 0;
	//				Config["_"][1][5] = 0;

	//				auto teamNumberColor = GetColorForNumber(Player.TeamID);
	//				auto infoColor = GameData.Config.ESP.Color.Default.Info;

	//				if (Player.Health == 0 and Player.GroggyHealth <= 100)
	//				{
	//					infoColor = GameData.Config.ESP.Color.Groggy.Info;
	//				}
	//				else if (Player.ListType == 1)
	//				{
	//					infoColor = GameData.Config.ESP.Color.Blacklist.Info;
	//				}
	//				else if (Player.ListType == 2)
	//				{
	//					infoColor = GameData.Config.ESP.Color.Whitelist.Info;
	//				}
	//				else if (Player.PartnerLevel > 0)
	//				{
	//					infoColor = GameData.Config.ESP.Color.Partner.Info;
	//				}
	//				else if (Player.KillCount > 4)
	//				{
	//					infoColor = GameData.Config.ESP.Color.Dangerous.Info;
	//				}
	//				else if (Player.Type == EntityType::AI)
	//				{
	//					infoColor = GameData.Config.ESP.Color.AI.Info;
	//				}
	//				else if (Player.IsVisible && GameData.Config.ESP.VisibleCheck)
	//				{
	//					infoColor = GameData.Config.ESP.Color.Visible.Info;
	//				}
	//				Config["_"][1][6] = infoColor[0];
	//				Config["_"][1][7] = infoColor[1];
	//				Config["_"][1][8] = infoColor[2];

	//				Config["_"][1][9] = teamNumberColor[0];
	//				Config["_"][1][10] = teamNumberColor[1];
	//				Config["_"][1][11] = teamNumberColor[2];
	//				Config["_"][1][12] = Player.TeamID;
	//				Config["_"][1][13] = Player.Health;
	//				Config["_"][1][14] = Player.GroggyHealth;
	//				Config["_"][1][15] = Player.IsMe;
	//				Config["_"][1][16] = Player.KillCount;
	//				Config["_"][1][17] = Player.SpectatedCount;
	//				Config["_"][1][18] = Player.SquadMemberIndex;
	//			}


	//		

	//	


	//		
	//	/*		recv(sock, msg, 200, 0);*/
	//			//printf("������:%s", msg);

	//		}
	//		std::string mesg = Config.dump();
	//		send(sock, mesg.c_str(), strlen(mesg.c_str()) + 1, 0);
	//		Sleep(64);

	//	}
	//	closesocket(sock);
	//	printf("\n�Ự����������һ������\n");
	//	getch();
	//	WSACleanup();
	//	return;
	//}
	//static void Run() {

	//	//WSAStartup();
	//	nlohmann::json Config;
	//	GameData.Config.WebRadar.IP = "127.0.0.1";
	//	GameData.Config.WebRadar.Port = 8888;
	//	GameData.Config.WebRadar.SubTitle = "/mqtt_backend";


	//	//std::thread (Init).detach();
	//	while (true)
	//	{
	//		Config["_"][0][0] = (int)GameData.Scene;

	//		if (GameData.Scene == Scene::Gaming) {
	//			Config["Game"][0][1] = GameData.MapName;
	//			Config["_"][0][2] = GameData.Radar.BlueZonePosition.X;
	//			Config["_"][0][3] = GameData.Radar.BlueZonePosition.Y;
	//			Config["_"][0][4] = GameData.Radar.BlueZoneRadius;
	//			Config["_"][0][5] = GameData.Radar.SafetyZonePosition.X;
	//			Config["_"][0][6] = GameData.Radar.SafetyZonePosition.Y;
	//			Config["_"][0][7] = GameData.Radar.SafetyZoneRadius;

	//			auto Players = Data::GetPlayers();
	//			for (auto Item : Players) {
	//				auto Player = Item.second;
	//				Config["_"][1][0] = Player.ClanName;
	//				Config["_"][1][1] = Player.Name;
	//				Config["_"][1][2] = Player.Location.X;
	//				Config["_"][1][3] = Player.Location.Y;
	//				Config["_"][1][4] = 0;
	//				Config["_"][1][5] = 0;

	//				auto teamNumberColor = GetColorForNumber(Player.TeamID);
	//				auto infoColor = GameData.Config.ESP.Color.Default.Info;

	//				if (Player.Health == 0 and Player.GroggyHealth <= 100)
	//				{
	//					infoColor = GameData.Config.ESP.Color.Groggy.Info;
	//				}
	//				else if (Player.ListType == 1)
	//				{
	//					infoColor = GameData.Config.ESP.Color.Blacklist.Info;
	//				}
	//				else if (Player.ListType == 2)
	//				{
	//					infoColor = GameData.Config.ESP.Color.Whitelist.Info;
	//				}
	//				else if (Player.PartnerLevel > 0)
	//				{
	//					infoColor = GameData.Config.ESP.Color.Partner.Info;
	//				}
	//				else if (Player.KillCount > 4)
	//				{
	//					infoColor = GameData.Config.ESP.Color.Dangerous.Info;
	//				}
	//				else if (Player.Type == EntityType::AI)
	//				{
	//					infoColor = GameData.Config.ESP.Color.AI.Info;
	//				}
	//				else if (Player.IsVisible && GameData.Config.ESP.VisibleCheck)
	//				{
	//					infoColor = GameData.Config.ESP.Color.Visible.Info;
	//				}
	//				Config["_"][1][6] = infoColor[0];
	//				Config["_"][1][7] = infoColor[1];
	//				Config["_"][1][8] = infoColor[2];

	//				Config["_"][1][9] = teamNumberColor[0];
	//				Config["_"][1][10] = teamNumberColor[1];
	//				Config["_"][1][11] = teamNumberColor[2];
	//				Config["_"][1][12] = Player.TeamID;
	//				Config["_"][1][13] = Player.Health;
	//				Config["_"][1][14] = Player.GroggyHealth;
	//				Config["_"][1][15] = Player.IsMe;
	//				Config["_"][1][16] = Player.KillCount;
	//				Config["_"][1][17] = Player.SpectatedCount;
	//				Config["_"][1][18] = Player.SquadMemberIndex;
	//			}
	//		}

	//		Sleep(64);
	//	}


	//}
	static std::string rgb2hex(int r, int g, int b, bool with_head)
	{
		std::stringstream ss;
		if (with_head)
			ss << "#";
		ss << std::hex << (r << 16 | g << 8 | b);
		return ss.str();
	}
	static int* SetColor(int a, int b, int c) {
		int* temp = new int[3];
		temp[0] = a;
		temp[1] = b;
		temp[2] = c;
		return temp;
	}
	static int* GetColorForNumber(int number) {


		switch (number) {
		case 1:  return SetColor(247, 248, 19);    // Yellow
		case 2:  return SetColor(250, 127, 73);    // Orange
		case 3:  return SetColor(90, 198, 227);    // Light Blue
		case 4:  return SetColor(90, 189, 77);     // Green
		case 5:  return SetColor(225, 99, 120);    // Pink
		case 6:  return SetColor(115, 129, 168);   // Purple
		case 7:  return SetColor(159, 126, 105);   // Indigo
		case 8:  return SetColor(255, 134, 200);   // Light Cyan
		case 9:  return SetColor(210, 224, 191);   // Pale Green
		case 10: return SetColor(154, 52, 142);    // Violet
		case 11: return SetColor(98, 146, 158);       // Red
		case 12: return SetColor(226, 214, 239);       // Green
		case 13: return SetColor(4, 167, 119);       // Blue
		case 14: return SetColor(115, 113, 252);     // Yellow
		case 15: return SetColor(255, 0, 255);     // Magenta
		case 16: return SetColor(93, 46, 140);     // Cyan
		case 17: return SetColor(0, 255, 0);       // Lime
		case 18: return SetColor(0, 0, 255);       // Blue
		case 19: return SetColor(255, 165, 0);     // Orange
		case 20: return SetColor(128, 0, 128);     // Purple
		case 21: return SetColor(255, 192, 203);   // Pink
		case 22: return SetColor(128, 128, 0);     // Olive
		case 23: return SetColor(255, 215, 0);     // Gold
		case 24: return SetColor(75, 0, 130);      // Indigo
		case 25: return SetColor(0, 191, 255);     // Deep Sky Blue
		case 26: return SetColor(255, 105, 180);   // Hot Pink
		case 27: return SetColor(139, 69, 19);     // Saddle Brown
		case 28: return SetColor(220, 20, 60);     // Crimson
		case 29: return SetColor(0, 255, 127);     // Spring Green
		case 30: return SetColor(0, 250, 154);     // Medium Spring Green
		case 31: return SetColor(72, 61, 139);     // Dark Slate Blue
		case 32: return SetColor(143, 188, 143);   // Dark Sea Green
		case 33: return SetColor(178, 34, 34);     // Firebrick
		case 34: return SetColor(153, 50, 204);    // Dark Orchid
		case 35: return SetColor(233, 150, 122);   // Dark Salmon
		case 36: return SetColor(148, 0, 211);     // Dark Violet
		case 37: return SetColor(95, 158, 160);    // Cadet Blue
		case 38: return SetColor(127, 255, 212);   // Aquamarine
		case 39: return SetColor(218, 112, 214);   // Orchid
		case 40: return SetColor(244, 164, 96);    // Sandy Brown
		case 41: return SetColor(210, 105, 30);    // Chocolate
		case 42: return SetColor(222, 184, 135);   // Burlywood
		case 43: return SetColor(255, 228, 181);   // Moccasin
		case 44: return SetColor(255, 239, 213);   // Papaya Whip
		case 45: return SetColor(175, 238, 238);   // Pale Turquoise
		case 46: return SetColor(100, 149, 237);   // Cornflower Blue
		case 47: return SetColor(219, 112, 147);   // Pale Violet Red
		case 48: return SetColor(173, 216, 230);   // Light Blue
		case 49: return SetColor(240, 128, 128);   // Light Coral
		case 50: return SetColor(255, 248, 220);   // Cornsilk
		default: return SetColor(102, 102, 102);   // Gray
		}
	}




};