#include <winsock2.h>
#include <windows.h>
#include <Overlay/Overlay.h>
#include <Common/Data.h>
#include <Utils/Utils.h>
#include <Hack/Hack.h>
#include <Common/Offset.h>
#include <thread>
#include <DMALibrary/Memory/Memory.h>
#include <cstdint>
#include <Common/Config.h>
#include <Auth/Auth.h>
#include <Utils/MachineCodeGenerator.h>
#include <Utils/w3c/w3c.h>


FGameData GameData;

using namespace std;


void deleteCardKey() {
	remove("cardKey.txt");
}





void SetConsoleStyle()
{
	SetConsoleOutputCP(CP_UTF8);

	SetConsoleTitle("Stay-PUBG");

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;
	cfi.dwFontSize.Y = 14;

	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);

	auto hStdin = ::GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	::GetConsoleMode(hStdin, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE;
	::SetConsoleMode(hStdin, mode);
}

int Refresh()
{
	while (true)
	{
		mem.RefreshAll();
		Sleep(1000 * 60 * 15);
	}
}


void ConvertGBKToUTF8(const char* gbkStr, std::string& utf8Str) {
	int wideStrSize = MultiByteToWideChar(CP_ACP, 0, gbkStr, -1, NULL, 0);
	if (wideStrSize == 0) {
		cerr << "Failed to convert GBK to wide string with error: " << GetLastError() << endl;
		return;
	}
	wchar_t* wideStr = new wchar_t[wideStrSize];
	MultiByteToWideChar(CP_ACP, 0, gbkStr, -1, wideStr, wideStrSize);
	int utf8StrSize = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
	if (utf8StrSize == 0) {
		cerr << "Failed to convert wide string to UTF-8 with error: " << GetLastError() << endl;
		delete[] wideStr;
		return;
	}
	char* utf8StrC = new char[utf8StrSize];
	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8StrC, utf8StrSize, NULL, NULL);

	utf8Str = std::string(utf8StrC);

	delete[] wideStr;
	delete[] utf8StrC;
}

int main() {

	SetConsoleStyle();



	Auth::Init();

	std::thread HackThread(Hack::Init);

	std::thread OverlayThread(Overlay::Init);


	while (true)
	{
		if (GameData.Config.Window.Main) {
			Sleep(500);
			continue;
		}
		break;
	}


	return 0;

}

