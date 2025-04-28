#include "imgui.h"
#include "imgui_internal.h"

//namespace c
//{
//	inline ImVec4 accent_color = ImColor(149, 1, 247);
//	inline ImVec4 accent_low_color = ImColor(149, 1, 247,255/2);
//	inline ImVec4 accent_low = ImColor(174, 197, 255, 255 / 2);
//
//	inline ImVec4 accent_text_color = ImColor(245, 245, 255);
//
//	inline ImVec4 accent_text_low_color = ImColor(245, 245, 255, 255 / 2);
//
//	inline ImVec4 notify = ImColor(43, 48, 54);
//
//
//	namespace bg
//	{
//		inline ImVec4 background = ImColor(27, 24, 29, 255);
//		inline ImVec4 outline_background = ImColor(27, 29, 32, 255);
//		inline ImVec2 size = ImVec2(650, 475);
//		inline float rounding = 6;
//	}
//
//	namespace child
//	{
//		inline ImVec4 background = ImColor(33, 31, 36, 250);
//		inline ImVec4 outline_background = ImColor(27, 29, 32, 255);
//		inline float rounding = 6;
//	}
//
//	namespace checkbox
//	{
//		inline ImVec4 circle_inactive = ImColor(43, 48, 54, 255);
//
//		inline ImVec4 background = ImColor(27, 29, 32, 255);
//		inline ImVec4 outline_background = ImColor(30, 32, 36, 255);
//		inline float rounding = 30;
//	}
//
//	namespace slider
//	{
//		inline ImVec4 circle_inactive = ImColor(43, 48, 54, 255);
//
//		inline ImVec4 background = ImColor(27, 29, 32, 255);
//		inline ImVec4 outline_background = ImColor(30, 32, 36, 255);
//		inline float rounding = 30;
//	}
//
//	namespace combo
//	{
//		inline ImVec4 background = ImColor(27, 29, 32, 255);
//		inline ImVec4 outline_background = ImColor(30, 32, 36, 255);
//		inline float rounding = 3;
//	}
//
//	namespace picker
//	{
//		inline ImVec4 background = ImColor(27, 29, 32, 255);
//		inline ImVec4 outline_background = ImColor(30, 32, 36, 255);
//		inline float rounding = 2;
//	}
//
//	namespace button
//	{
//		inline ImVec4 background = ImColor(149, 1, 247);
//		inline ImVec4 outline_background = ImColor(30, 32, 36, 255);
//		inline float rounding = 4;
//	}
//
//	namespace input
//	{
//		inline ImVec4 background = ImColor(27, 29, 32, 255);
//		inline ImVec4 outline_background = ImColor(30, 32, 36, 255);
//		inline float rounding = 4;
//	}
//
//	namespace keybind
//	{
//		inline ImVec4 background = ImColor(27, 29, 32, 255);
//		inline ImVec4 outline_background = ImColor(30, 32, 36, 255);
//		inline float rounding = 3;
//	}
//
//
//	namespace text
//	{
//		inline ImVec4 text_hov = ImColor(245, 245, 255);
//		inline ImVec4 text = ImColor(90, 93, 100);
//		inline ImVec4 text2 = ImColor(90, 93, 100,0);
//		inline ImVec4 hide_text = ImColor(43, 48, 54, 255);
//
//	}
//
//	namespace tab
//	{
//		inline ImVec4 background = ImColor(35, 35, 35, 255);
//		inline float rounding = 10.f;
//	}
//}

namespace Style
{
    inline ImVec4 AccentColor = ImColor(255, 215, 0); // PUBG�Ļ�ɫ
    inline ImVec4 AccentLowColor = ImColor(0, 215, 0, 127); // ��͸���Ļ�ɫ
    inline ImVec4 AccentLow = ImColor(0, 215, 0, 127); // ��͸���Ļ�ɫ

    inline ImVec4 AccentTextColor = ImColor(0, 255, 0); // ��ɫ����
    inline ImVec4 AccentTextLowColor = ImColor(0, 255, 0, 127); // ��͸���İ�ɫ����
    inline ImVec4 Notify = ImColor(0, 255, 0); // PUBG�����֪ͨ��ɫ

    inline float Padding = 20; // �趨һ�µ��ڱ߾�

    namespace Text
    {
        inline ImVec4 TextHov = ImColor(0, 255, 0); // ������ͣ��ɫ
        inline ImVec4 Text = ImColor(0, 255, 0); // ��ɫ����
        inline ImVec4 Text2 = ImColor(0, 255, 0, 0); // ͸��������
        inline ImVec4 HideText = ImColor(0, 255, 0, 255); // ����������ɫ
    }

    namespace Window
    {
        inline ImVec2 Size = ImVec2(800, 500); // ���ڴ�С
        inline ImVec2 PlayerListsSize = ImVec2(736, 473); // ����б��С
        inline ImVec4 Background = ImColor(20, 20, 20, 220); // ������ɫ
        inline ImVec4 OutlineBackground = ImColor(0, 255, 0, 255); // �ⲿ������ɫ
        inline float Rounding = 10; // Բ��
    }

    namespace Keybind
    {
        inline ImVec4 Background = ImColor(35, 35, 35, 255); // �����󶨱���ɫ
        inline ImVec4 OutlineBackground = ImColor(0, 255, 0, 255); // �������ⲿ����ɫ
        inline float Rounding = 3; // Բ��
    }

    namespace Tab
    {
        inline ImVec2 Size = ImVec2(150, 35); // ��ǩ��С
        inline ImVec4 Background = ImColor(35, 35, 35, 255); // ��ǩ����ɫ
        inline float Rounding = 10.f; // Բ��
    }

    namespace SubTab
    {
        inline ImVec2 Size = ImVec2(25, 25); // �ӱ�ǩ��С
        inline ImVec4 Background = ImColor(40, 40, 40, 255); // �ӱ�ǩ����ɫ
        inline ImVec4 DefBackground = ImColor(23, 23, 23, 255); // Ĭ���ӱ�ǩ����ɫ
        inline float Rounding = 50; // �ӱ�ǩԲ��
        inline ImVec4 AccentTextColor = ImColor(255, 255, 0, 255); // �ӱ�ǩ������ɫ
    }

    namespace Layout
    {
        inline ImVec4 Background = ImColor(25, 25, 25, 255); // ���ֱ���ɫ
    }

    namespace Input
    {
        inline ImVec4 Background = ImColor(30, 30, 30, 255); // ����򱳾�ɫ
        inline ImVec4 OutlineBackground = ImColor(0, 255, 0, 255); // ������ⲿ����ɫ
        inline float Rounding = 4; // Բ��
    }

    namespace Child
    {
        inline ImVec4 Background = ImColor(25, 25, 25, 255); // �Ӵ��ڱ���ɫ
        inline ImVec4 OutlineBackground = ImColor(0, 255, 0, 255); // �Ӵ����ⲿ����ɫ
        inline float Rounding = 6; // Բ��
    }

    namespace Checkbox
    {
        inline ImVec4 CircleInactive = ImColor(43, 48, 54, 255); // δѡ�еĸ�ѡ��ԲȦ��ɫ
        inline ImVec4 Background = ImColor(30, 30, 30, 255); // ��ѡ�򱳾�ɫ
        inline ImVec4 OutlineBackground = ImColor(0, 255, 0, 255); // ��ѡ���ⲿ����ɫ
        inline float Rounding = 30; // Բ��
    }

    namespace Slider
    {
        inline ImVec4 CircleInactive = ImColor(255, 255, 255, 255); // δѡ�еĻ���ԲȦ��ɫ
        inline ImVec4 Background = ImColor(30, 30, 30, 255); // ���鱳��ɫ
        inline ImVec4 OutlineBackground = ImColor(0, 255, 0, 255); // �����ⲿ����ɫ
        inline float Rounding = 30; // Բ��
    }

    namespace Combo
    {
        inline ImVec4 Background = ImColor(35, 35, 35, 255); // �����򱳾�ɫ
        inline ImVec4 OutlineBackground = ImColor(0, 255, 0, 255); // �������ⲿ����ɫ
        inline float Rounding = 3; // Բ��
    }

    namespace Picker
    {
        inline ImVec4 Background = ImColor(35, 35, 35, 255); // ��ɫѡ��������ɫ
        inline ImVec4 OutlineBackground = ImColor(0, 255, 0, 255); // ��ɫѡ�����ⲿ����ɫ
        inline float Rounding = 2; // Բ��
    }
}