#include <WS2tcpip.h>
#include <iostream>
#include <regex>
#include <unordered_map>
#include "cJSON/cJSON.h"
#include <future>
#include <string>
#include <vector>
#include <wininet.h>
#include <map>
//菜鸟基地QQ群：964504044
#include <Common/Data.h>

#pragma comment(lib, "wininet.lib")

class Rank
{
public:
    std::string sendGetRequestWithWininet(const std::string& host, int port, const std::string& path) {
        HINTERNET hInternet = InternetOpenA("WinINet Example", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) {
			return "hInternet error";
        }

        HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (!hConnect) {
            InternetCloseHandle(hInternet);
            return "hConnect error";
        }

        const char* acceptTypes[] = { "*/*", NULL };
        //const char* acceptTypes[] = { "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7", NULL };
        HINTERNET hRequest = HttpOpenRequestA(hConnect, "GET", path.c_str(), NULL, NULL, acceptTypes, INTERNET_FLAG_RELOAD, 0);
        if (!hRequest) {
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return "hRequest error";
        }

        //const char* headers = "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36\r\n";
        const char* headers = "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36\r\n";

        if (!HttpSendRequestA(hRequest, headers, strlen(headers), NULL, 0)) {
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return "headers error";
        }

        std::string response;
        char buffer[4096];
        DWORD bytesRead;
        while (InternetReadFile(hRequest, &buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
            response.append(buffer, bytesRead);
        }

        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        return response;
    }

    std::string sendPostRequestWithWininet(const std::string& host, int port, const std::string& path, const std::string& data) {
        HINTERNET hInternet = InternetOpenA("WinINet Example", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (!hInternet) {
            return "";
        }

        HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (!hConnect) {
            InternetCloseHandle(hInternet);
            return "";
        }

        const char* acceptTypes[] = { "*/*", NULL };
        HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", path.c_str(), NULL, NULL, acceptTypes, INTERNET_FLAG_RELOAD, 0);
        if (!hRequest) {
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return "";
        }

        std::string headers = "Content-Type: application/x-www-form-urlencoded\r\nContent-Length: " + std::to_string(data.length());
        if (!HttpSendRequestA(hRequest, headers.c_str(), headers.length(), (LPVOID)data.c_str(), data.length())) {
            InternetCloseHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return "";
        }

        std::string response;
        char buffer[4096];
        DWORD bytesRead;

        while (InternetReadFile(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
            response.append(buffer, bytesRead);
        }

        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        return response;
    }

    std::string extractOrReturnOriginal(const std::string& input) {
        std::regex pattern(R"(\[.*\]\s*(\S+))");
        std::smatch matches;

        if (std::regex_search(input, matches, pattern) && matches.size() > 1) {
            std::string result = matches[1].str();
            result.erase(remove(result.begin(), result.end(), ' '), result.end());
            return result;
        }
        else {
            return input;
        }
    }

    void fetcahwebget(const std::string& name)
    {
        std::string url = "/user/" + name;
        sendGetRequestWithWininet("pubg.op.gg", 80, url);
    }

    std::string extractSpecificString(const std::string& response) {
        std::regex pattern(R"(<div[^>]*\bid\s*=\s*\"userNickname\"[^>]*\bdata-user_id\s*=\s*\"([^\"]+)\")");
        std::smatch matches;

        if (std::regex_search(response, matches, pattern) && matches.size() > 1) {
            return matches[1].str();
        }
        return "";
    }

    std::string fetchWebGet(const std::string& name) {
        std::string url = "/user/" + name;
        return sendGetRequestWithWininet("pubg.op.gg", 80, url);
    }

    std::string fetchUserRankedStats(const std::string& userId) {
        std::string url = "/api/users/" + userId + GameData.Config.ESP.RankModeUrl;
        return sendGetRequestWithWininet("pubg.op.gg", 80, url);
    }

    void concurrentRequests(const std::vector<std::string>& names, std::unordered_map<std::string, bool>& QuerySuccess, std::unordered_map<std::string, int>& QueryCount) {

        std::map<std::string, std::future<std::string>> futures;
        std::mutex queryMutex;

        for (const auto& name : names) {

            if (QuerySuccess[name])
            {
                continue;
            }

            if (QueryCount[name] >= 2)
            {
                continue;
            }

            futures[name] = std::async(std::launch::async, [this, name]() {
				std::string url = "/user/" + name;
				return this->sendGetRequestWithWininet("pubg.op.gg", 80, url);
                });

            Sleep(2000);
        }

        for (auto& [name, future] : futures) {

            if (QuerySuccess[name])
            {
                continue;
            }

            if (QueryCount[name] >= 2)
            {
                continue;
            }

            std::string response = future.get();
            std::string extractedString = extractSpecificString(response);
            Utils::Log(1, "%s", Utils::StringToUTF8(response).c_str());



            if (!extractedString.empty()) {

                std::string rankedStatsResponse = fetchUserRankedStats(extractedString);

                cJSON* jsonRoot = cJSON_Parse(rankedStatsResponse.c_str());
                if (jsonRoot) {
                    PlayerRankList RankList;
                    {
                        std::lock_guard<std::mutex> lock(queryMutex);
                        QuerySuccess[name] = true;
                    }

                    if (GameData.Config.PlayerList.RankMode == 1)
                    {
                        cJSON* tier = cJSON_GetObjectItem(jsonRoot, "tier");
                        if (tier) {
                            cJSON* title = cJSON_GetObjectItem(tier, "title");
                            if (title) {
                                RankList.TPP.Tier = title->valuestring;
                                std::cout << "RankList.SquadFPP.Tier: " << RankList.TPP.Tier << std::endl;
                            }
                        }

                        cJSON* stats = cJSON_GetObjectItem(jsonRoot, "stats");
                        if (stats) {
                            int kills = 0;
                            int deaths = 0;

                            int matches_cnt = 0;
                            int win_matches_cnt = 0;

                            cJSON* rank_points = cJSON_GetObjectItem(stats, "rank_points");
                            if (rank_points) {
                                RankList.TPP.RankPoint = rank_points->valueint;
                                std::cout << "RankList.SquadFPP.RankPoint: " << RankList.TPP.RankPoint << std::endl;
                            }

                            cJSON* killsSum = cJSON_GetObjectItem(stats, "kills_sum");
                            if (killsSum) {
                                kills = killsSum->valueint;
                            }

                            cJSON* deathsSum = cJSON_GetObjectItem(stats, "deaths_sum");
                            if (deathsSum) {
                                deaths = deathsSum->valueint;
                            }

                            cJSON* matchesSum = cJSON_GetObjectItem(stats, "matches_cnt");
                            if (matchesSum) {
                                matches_cnt = matchesSum->valueint;
                            }

                            cJSON* win_matchesSum = cJSON_GetObjectItem(stats, "win_matches_cnt");
                            if (win_matchesSum) {
                                win_matches_cnt = win_matchesSum->valueint;
                            }

                            RankList.TPP.WinRatio = static_cast<double>(matches_cnt) / win_matches_cnt;

                            RankList.TPP.KDA = static_cast<double>(kills) / deaths;
                            std::cout << "RankList.SquadFPP.KDA: " << RankList.TPP.KDA << std::endl;
                            std::cout << "RankList.WinRatio: \n" << RankList.TPP.WinRatio << std::endl;
                        }
                    }
                    else if (GameData.Config.PlayerList.RankMode == 2)
                    {
                        cJSON* tier = cJSON_GetObjectItem(jsonRoot, "tier");
                        if (tier) {
                            cJSON* title = cJSON_GetObjectItem(tier, "title");
                            if (title) {
                                RankList.SquadTPP.Tier = title->valuestring;
                                std::cout << "RankList.SquadFPP.Tier: " << RankList.SquadTPP.Tier << std::endl;
                            }
                        }

                        cJSON* stats = cJSON_GetObjectItem(jsonRoot, "stats");
                        if (stats) {
                            int kills = 0;
                            int deaths = 0;

                            int matches_cnt = 0;
                            int win_matches_cnt = 0;

                            cJSON* rank_points = cJSON_GetObjectItem(stats, "rank_points");
                            if (rank_points) {
                                RankList.SquadTPP.RankPoint = rank_points->valueint;
                                std::cout << "RankList.SquadFPP.RankPoint: " << RankList.SquadTPP.RankPoint << std::endl;
                            }

                            cJSON* killsSum = cJSON_GetObjectItem(stats, "kills_sum");
                            if (killsSum) {
                                kills = killsSum->valueint;
                            }

                            cJSON* deathsSum = cJSON_GetObjectItem(stats, "deaths_sum");
                            if (deathsSum) {
                                deaths = deathsSum->valueint;
                            }

                            cJSON* matchesSum = cJSON_GetObjectItem(stats, "matches_cnt");
                            if (matchesSum) {
                                matches_cnt = matchesSum->valueint;
                            }

                            cJSON* win_matchesSum = cJSON_GetObjectItem(stats, "win_matches_cnt");
                            if (win_matchesSum) {
                                win_matches_cnt = win_matchesSum->valueint;
                            }

                            RankList.SquadTPP.WinRatio = static_cast<double>(matches_cnt) / win_matches_cnt;

                            RankList.SquadTPP.KDA = static_cast<double>(kills) / deaths;
                            std::cout << "RankList.SquadFPP.KDA: " << RankList.SquadTPP.KDA << std::endl;
                            std::cout << "RankList.WinRatio: \n" << RankList.SquadTPP.WinRatio << std::endl;
                        }
                    }
                    else if (GameData.Config.PlayerList.RankMode == 3)
                    {
                        cJSON* tier = cJSON_GetObjectItem(jsonRoot, "tier");
                        if (tier) {
                            cJSON* title = cJSON_GetObjectItem(tier, "title");
                            if (title) {
                                RankList.FPP.Tier = title->valuestring;
                                std::cout << "RankList.SquadFPP.Tier: " << RankList.FPP.Tier << std::endl;
                            }
                        }

                        cJSON* stats = cJSON_GetObjectItem(jsonRoot, "stats");
                        if (stats) {
                            int kills = 0;
                            int deaths = 0;

                            int matches_cnt = 0;
                            int win_matches_cnt = 0;

                            cJSON* rank_points = cJSON_GetObjectItem(stats, "rank_points");
                            if (rank_points) {
                                RankList.FPP.RankPoint = rank_points->valueint;
                                std::cout << "RankList.SquadFPP.RankPoint: " << RankList.FPP.RankPoint << std::endl;
                            }

                            cJSON* killsSum = cJSON_GetObjectItem(stats, "kills_sum");
                            if (killsSum) {
                                kills = killsSum->valueint;
                            }

                            cJSON* deathsSum = cJSON_GetObjectItem(stats, "deaths_sum");
                            if (deathsSum) {
                                deaths = deathsSum->valueint;
                            }

                            cJSON* matchesSum = cJSON_GetObjectItem(stats, "matches_cnt");
                            if (matchesSum) {
                                matches_cnt = matchesSum->valueint;
                            }

                            cJSON* win_matchesSum = cJSON_GetObjectItem(stats, "win_matches_cnt");
                            if (win_matchesSum) {
                                win_matches_cnt = win_matchesSum->valueint;
                            }

                            RankList.FPP.WinRatio = static_cast<double>(matches_cnt) / win_matches_cnt;

                            RankList.FPP.KDA = static_cast<double>(kills) / deaths;
                            std::cout << "RankList.SquadFPP.KDA: " << RankList.FPP.KDA << std::endl;
                            std::cout << "RankList.WinRatio: " << RankList.FPP.WinRatio << std::endl;
                        }
                    }
                    else if (GameData.Config.PlayerList.RankMode == 4)
                    {
                        cJSON* tier = cJSON_GetObjectItem(jsonRoot, "tier");
                        if (tier) {
                            cJSON* title = cJSON_GetObjectItem(tier, "title");
                            if (title) {
                                RankList.SquadFPP.Tier = title->valuestring;
                                std::cout << "RankList.SquadFPP.Tier: " << RankList.SquadFPP.Tier << std::endl;
                            }
                        }

                        cJSON* stats = cJSON_GetObjectItem(jsonRoot, "stats");
                        if (stats) {
                            int kills = 0;
                            int deaths = 0;

                            int matches_cnt = 0;
                            int win_matches_cnt = 0;

                            cJSON* rank_points = cJSON_GetObjectItem(stats, "rank_points");
                            if (rank_points) {
                                RankList.SquadFPP.RankPoint = rank_points->valueint;
                                std::cout << "RankList.SquadFPP.RankPoint: " << RankList.SquadFPP.RankPoint << std::endl;
                            }

                            cJSON* killsSum = cJSON_GetObjectItem(stats, "kills_sum");
                            if (killsSum) {
                                kills = killsSum->valueint;
                            }

                            cJSON* deathsSum = cJSON_GetObjectItem(stats, "deaths_sum");
                            if (deathsSum) {
                                deaths = deathsSum->valueint;
                            }

                            cJSON* matchesSum = cJSON_GetObjectItem(stats, "matches_cnt");
                            if (matchesSum) {
                                matches_cnt = matchesSum->valueint;
                            }

                            cJSON* win_matchesSum = cJSON_GetObjectItem(stats, "win_matches_cnt");
                            if (win_matchesSum) {
                                win_matches_cnt = win_matchesSum->valueint;
                            }

                            RankList.SquadFPP.WinRatio = static_cast<double>(matches_cnt) / win_matches_cnt;

                            RankList.SquadFPP.KDA = static_cast<double>(kills) / deaths;
                            std::cout << "RankList.SquadFPP.KDA: " << RankList.SquadFPP.KDA << std::endl;
                            std::cout << "RankList.WinRatio:\n " << RankList.SquadFPP.WinRatio << std::endl;
                        }
                    }                  
                    Data::SetPlayerRankListsItem(name, RankList);
                    cJSON_Delete(jsonRoot);
                }
                else {
                    {
                        std::lock_guard<std::mutex> lock(queryMutex);
                        QueryCount[name]++;
                    }
                    std::cerr << "Failed to 2 JSON response." << std::endl;
                }
            }
            else {
                {
                    std::lock_guard<std::mutex> lock(queryMutex);
                    QueryCount[name]++;
                }

                std::cerr << "User opggID not found." << std::endl;
            }
        }
    }

    std::pair<std::string, PlayerRankList> fetchPlayerRank(const std::string& name) {
        std::string url = "/user/" + name;
        std::string response = sendGetRequestWithWininet("pubg.op.gg", 80, url);

        std::string extractedString = extractSpecificString(response);

        //std::cout << "OpggID: " << extractedString << std::endl;

        PlayerRankList playerRankList;
        if (!extractedString.empty()) {
            std::string rankedStatsResponse = fetchUserRankedStats(extractedString);

            cJSON* jsonRoot = cJSON_Parse(rankedStatsResponse.c_str());
            if (jsonRoot) {
                int kills = 0;
                int deaths = 0;
                int rank = 0;

                cJSON* tier = cJSON_GetObjectItem(jsonRoot, "tier");
                if (tier) {
                    cJSON* title = cJSON_GetObjectItem(tier, "title");
                    if (title) {
                        playerRankList.SquadFPP.Tier = title->valuestring;
                    }
                }

                cJSON* stats = cJSON_GetObjectItem(jsonRoot, "stats");
                if (stats) {
                    cJSON* rank_points = cJSON_GetObjectItem(stats, "rank_points");
                    if (rank_points) {
                        playerRankList.SquadFPP.RankPoint = rank_points->valueint;
                    }

                    cJSON* killsSum = cJSON_GetObjectItem(stats, "kills_sum");
                    if (killsSum) {
                        kills = killsSum->valueint;
                    }

                    cJSON* deathsSum = cJSON_GetObjectItem(stats, "deaths_sum");
                    if (deathsSum) {
                        deaths = deathsSum->valueint;
                    }
                    playerRankList.SquadFPP.KDA = static_cast<double>(kills) / deaths;
                }

                cJSON_Delete(jsonRoot);
            }
        }

        return { name, playerRankList };
    }

    void concurrentRequestsPaid(const std::vector<std::string>& names, std::unordered_map<std::string, bool>& QuerySuccess, std::unordered_map<std::string, int>& QueryCount){
		std::map<std::string, std::future<std::string>> futures;
		std::mutex queryMutex;

		for (const auto& name : names) {

			// 检查该玩家是否已经成功查询过
			if (QuerySuccess[name])
			{
				continue;
			}

			if (QueryCount[name] >= 2)
			{
				continue;
			}
            //printf("%s/n", name);
			futures[name] = std::async(std::launch::async, [this, name]() {
				std::string url = "/user/" + name;
				return this->extractPaidInterface(name, "TPP");
				});

			Sleep(2000);
		}


        for (auto& [name, future] : futures) {
        
			// 检查该玩家是否已经成功查询过
			if (QuerySuccess[name]) {
				continue;
			}

			if (QueryCount[name] >= 2) {
				continue;
			}
        
            std::string response = future.get();
            //Utils::Log(1, "response:%s", response.c_str());
			if (!response.empty()) {
                Utils::Log(1, "response:%s", Utils::StringToUTF8( response).c_str());

				PlayerRankList RankList; 
                
                {
					std::lock_guard<std::mutex> lock(queryMutex);
					QuerySuccess[name] = true;
				}

				/*{"code":1,"msg":"段位:大师1|分数:4333|KD:7.4|KDA:12.31|胜率:65.08%|模式:TPP|"}
				RankList.TPP.Tier  = 段位
				RankList.TPP.RankPoint = 分数
				RankList.TPP.KDA = KDA
				RankList.TPP.WinRatio = 胜率*/
                std::regex pattern(R"((段位):(.*?)\|(分数):(.*?)\|KD:(.*?)\|KDA:(.*?)\|胜率:(.*?)\%|模式:(.*?)\|)");
				std::smatch matches;
				if (std::regex_search(response, matches, pattern)) {
					// 由于目前只考虑TPP模式，这里不做模式的检查，直接赋值
					RankList.TPP.Tier = Utils::StringToUTF8(matches.str(2));
					RankList.TPP.RankPoint = std::stoi(matches.str(4));
					RankList.TPP.KDA = std::stof(matches.str(6)); 
					RankList.TPP.WinRatio = std::stof(matches.str(7));

					RankList.FPP.Tier = Utils::StringToUTF8(matches.str(2));
					RankList.FPP.RankPoint = std::stoi(matches.str(4));
					RankList.FPP.KDA = std::stof(matches.str(6));
					RankList.FPP.WinRatio = std::stof(matches.str(7));

					RankList.SquadFPP.Tier = Utils::StringToUTF8(matches.str(2));
					RankList.SquadFPP.RankPoint = std::stoi(matches.str(4));
					RankList.SquadFPP.KDA = std::stof(matches.str(6));
					RankList.SquadFPP.WinRatio = std::stof(matches.str(7));

					RankList.SquadTPP.Tier = Utils::StringToUTF8(matches.str(2));
					RankList.SquadTPP.RankPoint = std::stoi(matches.str(4));
					RankList.SquadTPP.KDA = std::stof(matches.str(6));
					RankList.SquadTPP.WinRatio = std::stof(matches.str(7));


					/*Utils::Log(1, "RankList.TPP.Tier:%s", RankList.TPP.Tier.c_str());
					Utils::Log(1, "RankList.TPP.RankPoint:%d", RankList.TPP.RankPoint);
					Utils::Log(1, "RankList.TPP.KDA:%f", RankList.TPP.KDA);
					Utils::Log(1, "RankList.TPP.WinRatio:%f", RankList.TPP.WinRatio);
					Utils::Log(1, "matches.str(0):%s", Utils::StringToUTF8(matches.str(0)).c_str());
					Utils::Log(1, "matches.str(1):%s", Utils::StringToUTF8(matches.str(1)).c_str());
					Utils::Log(1, "matches.str(2):%s", Utils::StringToUTF8(matches.str(2)).c_str());
					Utils::Log(1, "matches.str(3):%s", Utils::StringToUTF8(matches.str(3)).c_str());
					Utils::Log(1, "matches.str(4):%s", Utils::StringToUTF8(matches.str(4)).c_str());*/

                    Data::SetPlayerRankListsItem(name, RankList);
				}
				else {
					std::cout << "Pattern not matched." << std::endl;
				}

            }
            else {
				
                {
					std::lock_guard<std::mutex> lock(queryMutex);
					QueryCount[name]++;
				}
            }
        }
    }

    std::string extractPaidInterface(const std::string& Name, const std::string& ModeName) {
        //ceshi
        for (int i = 0; i < 15; i++) {
            //std::string url = "?id=2&key=02A87FEF2470861E4D440DC311F7E1E7&mode=" + ModeName + "&name=" + Name;  https://sss.shuziu.com/liebiao/1C89A78454A16A46
            std::string url = "?id=2&key=8D0820F536589C264FDB07B86DAB7112&mode=TPP&queue_size=1&name=" + Name;
            std::string data = sendGetRequestWithWininet("172.247.113.184", 88, url);
            //std::string data_UTF8 = Utils::StringToUTF8(data);
           // printf("url:%s\n", data.c_str());
          //  data = Utils::StringToUTF8(data);
            printf("%s\n", Utils::StringToUTF8(data).c_str());
            //printf("%s\n", Utils::StringToUTF8(data).c_str());*/


            if (strstr(Utils::StringToUTF8(data).c_str(), "KDA:") && strstr(Utils::StringToUTF8(data).c_str(), "KDA:")) {
                //正常KDA返回
               // printf("%s\n", data.c_str());
                return data;
                //break;

            }
            else if (!strstr(Utils::StringToUTF8(data).c_str(), "正在查询"))
            {    
                return "";
              // printf("TEST");
               // break;
                //查询错误返回   
                // 

            }
            Sleep(3000);
        }
        return "";
    }

    static void Update()
    {
        Rank rank;
        std::unordered_map<std::string, PlayerRankList> PlayerRankLists;
        std::vector<std::string> pPlayers;
        std::unordered_map<std::string, bool> QuerySuccess;
        std::unordered_map<std::string, int> QueryCount;


        while (true)
        {
            if (GameData.Scene != Scene::Gaming)
            {
                Sleep(GameData.ThreadSleep);
                continue;
            }

            if (!GameData.Config.PlayerList.RankMode)
            {
                Sleep(GameData.ThreadSleep);
                continue;
            }
            //https://pubg.op.gg/api/users/5e218a2030fa6b00140eee2/ranked-stats?season=pc-2018-31&queue_size=1&mode=competitive-tpp

            if (GameData.Config.PlayerList.RankMode == 1)
            {
                GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=1&mode=competitive-tpp";
            }
            else if (GameData.Config.PlayerList.RankMode == 2)
            {
                GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=4&mode=competitive-tpp";
            }
            else if (GameData.Config.PlayerList.RankMode == 3)
            {
                GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=1&mode=competitive-fpp";
            }
            else if (GameData.Config.PlayerList.RankMode == 4)
            {
                GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=4&mode=competitive-fpp";
            }

            PlayerRankLists = Data::GetPlayerRankLists();

            if (!GameData.Config.ESP.Tier && !GameData.Config.ESP.RankPoint && !GameData.Config.ESP.KDA)
            {
                PlayerRankLists.clear();
                pPlayers.clear();
                QuerySuccess.clear();
                QuerySuccess.clear();
                Sleep(GameData.ThreadSleep);
                continue;
            }

            for (auto& player : PlayerRankLists)
            {
                if (QuerySuccess.find(player.first) == QuerySuccess.end())
                {
                    QuerySuccess[player.first] = false;
                }

                if (QueryCount.find(player.first) == QueryCount.end())
                {
                    QueryCount[player.first] = 0;
                }

                if (QuerySuccess[player.first])
                {
                    continue;
                }

                pPlayers.push_back(player.first);
            }

            if (!pPlayers.empty())
            {
                rank.concurrentRequests(pPlayers, QuerySuccess, QueryCount);
            }


            PlayerRankLists.clear();
            pPlayers.clear();
        }
    }



    //去Hack.h 文件内把 std::thread UpdateRankThread(Rank::Update); 改成 std::thread UpdateRankThread(Rank::UpdatePaid);
    static void UpdatePaid() {

		Rank rank;
		std::unordered_map<std::string, PlayerRankList> PlayerRankLists;
		std::vector<std::string> pPlayers;
		std::unordered_map<std::string, bool> QuerySuccess;
		std::unordered_map<std::string, int> QueryCount;
		//Utils::Log(1, "%s", Utils::StringToUTF8(rank.extractPaidInterface("SSIBHACK", "TPP")).c_str());

        while (true) {

			if (GameData.Scene != Scene::Gaming) {
				Sleep(GameData.ThreadSleep);
				continue;
			}

			if (!GameData.Config.PlayerList.RankMode) {
				Sleep(GameData.ThreadSleep);
				continue;
			}

			if (GameData.Config.PlayerList.RankMode == 1)
			{
				GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=1&mode=competitive-tpp";
			}
			else if (GameData.Config.PlayerList.RankMode == 2)
			{
				GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=4&mode=competitive-tpp";
			}
			else if (GameData.Config.PlayerList.RankMode == 3)
			{
				GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=1&mode=competitive-fpp";
			}
			else if (GameData.Config.PlayerList.RankMode == 4)
			{
				GameData.Config.ESP.RankModeUrl = "/ranked-stats?season=pc-2018-" + std::string(GameData.Config.ESP.RankSize) + "&queue_size=4&mode=competitive-fpp";
			}

            PlayerRankLists = Data::GetPlayerRankLists();
			if (!GameData.Config.ESP.Tier && !GameData.Config.ESP.RankPoint && !GameData.Config.ESP.KDA) {
				PlayerRankLists.clear();
				pPlayers.clear();
				QuerySuccess.clear();
				QuerySuccess.clear();
				Sleep(GameData.ThreadSleep);
				continue;
			}

			for (auto& player : PlayerRankLists) {
				if (QuerySuccess.find(player.first) == QuerySuccess.end()) {
					QuerySuccess[player.first] = false;
				}

				if (QueryCount.find(player.first) == QueryCount.end()) {
					QueryCount[player.first] = 0;
				}

				if (QuerySuccess[player.first]) {
					continue;
				}

				pPlayers.push_back(player.first);
			}

			if (!pPlayers.empty()) {
                rank.concurrentRequestsPaid(pPlayers, QuerySuccess, QueryCount);
			}

			PlayerRankLists.clear();
			pPlayers.clear();
        }
    }
};
