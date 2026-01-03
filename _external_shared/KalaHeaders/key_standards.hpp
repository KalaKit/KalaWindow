//------------------------------------------------------------------------------
// key_standards.hpp
//
// Copyright (C) 2026 Lost Empire Entertainment
//
// This is free source code, and you are welcome to redistribute it under certain conditions.
// Read LICENSE.md for more information.
//
// Provides:
//   - standard layout with enums for mouse buttons, keyboard keys and gamepad actions
//   - standard layout for typography, math and currency symbols
//   - standard layout for latin and cyrillic alphabet letters
//   - standard layout for emojis and kaomojis
//------------------------------------------------------------------------------

#pragma once

#include <array>
#include <vector>
#include <string>
#include <algorithm>

//static_cast
#ifndef scast
	#define scast static_cast
#endif

namespace KalaHeaders::KalaKeyStandards
{	
	using std::array;
	using std::vector;
	using std::string;
	using std::string_view;
	using std::lower_bound;
	using std::find_if;
	
	using u32 = uint32_t;
	
	struct KeyValue
	{
		u32 key{};
		u32 utf{};
		string_view value{};
	};
	
	//
	// KEY AND MOUSE ACTIONS
	//
	
	enum class MouseButton : u32
	{
		M_INVALID = 0,
		
		M_LEFT   = 1,
		M_RIGHT  = 2,
		M_MIDDLE = 3,
		
		M_X1 = 4,
		M_X2 = 5,
		
		M_WHEEL_UP    = 6,
		M_WHEEL_DOWN  = 7,
		M_WHEEL_LEFT  = 8,
		M_WHEEL_RIGHT = 9,
		
		M_LEFT_DOUBLE   = 10,
		M_RIGHT_DOUBLE  = 11,
		M_MIDDLE_DOUBLE = 12,
		M_X1_DOUBLE     = 13,
		M_X2_DOUBLE     = 14
	};
	
	//Common mouse actions. 15-30 is reserved for future use.
	inline constexpr array<KeyValue, 15> mouseButtons
	{{
		{scast<u32>(MouseButton::M_LEFT),   0, "Left"},
		{scast<u32>(MouseButton::M_RIGHT),  0, "Right"}, //scrollwheel click
		{scast<u32>(MouseButton::M_MIDDLE), 0, "Middle"},
		
		{scast<u32>(MouseButton::M_X1), 0, "X1"}, //left side button
		{scast<u32>(MouseButton::M_X2), 0, "X2"}, //right side button
		
		{scast<u32>(MouseButton::M_WHEEL_UP),    0, "WheelUp"},    //scroll up
		{scast<u32>(MouseButton::M_WHEEL_DOWN),  0, "WheelDown"},  //scroll down
		{scast<u32>(MouseButton::M_WHEEL_LEFT),  0, "WheelLeft"},  //scrollwheel left click
		{scast<u32>(MouseButton::M_WHEEL_RIGHT), 0, "WheelRight"}, //scrollwheel right click
		
		{scast<u32>(MouseButton::M_LEFT_DOUBLE),   0, "LeftDouble"},
		{scast<u32>(MouseButton::M_RIGHT_DOUBLE),  0, "RightDouble"},
		{scast<u32>(MouseButton::M_MIDDLE_DOUBLE), 0, "MiddleDouble"},
		{scast<u32>(MouseButton::M_X1_DOUBLE),     0, "X1Double"},
		{scast<u32>(MouseButton::M_X2_DOUBLE),     0, "X2Double"}
	}};
	
	inline constexpr MouseButton GetMouseButton(const string& v)
	{
		for (const auto& _b : mouseButtons)
		{
			if (_b.value == v) return scast<MouseButton>(_b.key);
		}
		return scast<MouseButton>(0);
	}
	inline constexpr string_view GetMouseButtonValue(MouseButton b) 
	{ 
		for (const auto& _b : mouseButtons)
		{
			if (_b.key == scast<u32>(b)) return _b.value;
		}
		return "";
	}
	
	enum class GamepadButton : u32
	{
		G_INVALID = 0,
		
		G_A = 31,
		G_B = 32,
		G_X = 33,
		G_Y = 34,
		
		G_L1 = 35,
		G_R1 = 36,
		G_L2 = 37,
		G_R2 = 38,
		G_L3 = 39,
		G_R3 = 40,
		
		G_DPAD_UP    = 41,
		G_DPAD_DOWN  = 42,
		G_DPAD_LEFT  = 43,
		G_DPAD_RIGHT = 44,
		
		G_START    = 45,
		G_SELECT   = 46,
		G_HOME     = 47,
		G_CAPTURE  = 48,
		G_TOUCHPAD = 49
	};
	
	//Common gamepad/controller/switch-like actions. 50-60 is reserved for future use.
	inline constexpr array<KeyValue, 19> gamepadButtons
	{{
		{scast<u32>(GamepadButton::G_A), 0, "A"}, 
		{scast<u32>(GamepadButton::G_B), 0, "B"}, 
		{scast<u32>(GamepadButton::G_X), 0, "X"}, 
		{scast<u32>(GamepadButton::G_Y), 0, "Y"},
		
		{scast<u32>(GamepadButton::G_L1), 0, "L1"}, 
		{scast<u32>(GamepadButton::G_R1), 0, "R1"},
		{scast<u32>(GamepadButton::G_L2), 0, "L2"}, 
		{scast<u32>(GamepadButton::G_R2), 0, "R2"},
		{scast<u32>(GamepadButton::G_L3), 0, "L3"}, 
		{scast<u32>(GamepadButton::G_R3), 0, "R3"},
		
		{scast<u32>(GamepadButton::G_DPAD_UP),    0, "DPad Up"}, 
		{scast<u32>(GamepadButton::G_DPAD_DOWN),  0, "DPad Down"}, 
		{scast<u32>(GamepadButton::G_DPAD_LEFT),  0, "DPad Left"}, 
		{scast<u32>(GamepadButton::G_DPAD_RIGHT), 0, "DPad Right"},
		
		{scast<u32>(GamepadButton::G_START),    0, "Start"}, 
		{scast<u32>(GamepadButton::G_SELECT),   0, "Select"}, 
		{scast<u32>(GamepadButton::G_HOME),     0, "Home"}, 
		{scast<u32>(GamepadButton::G_CAPTURE),  0, "Capture"}, 
		{scast<u32>(GamepadButton::G_TOUCHPAD), 0, "Touchpad"}
	}};
	
	inline constexpr GamepadButton GetGamepadButton(const string& v)
	{
		for (const auto& _b : gamepadButtons)
		{
			if (_b.value == v) return scast<GamepadButton>(_b.key);
		}
		return scast<GamepadButton>(0);
	}
	inline constexpr string_view GetGamepadValue(GamepadButton b) 
	{ 
		for (const auto& _b : gamepadButtons)
		{
			if (_b.key == scast<u32>(b)) return _b.value;
		}
		return "";
	}
	
	enum class KeyboardButton : u32
	{
		K_INVALID = 0,
		
		K_NUM_0 = 61, K_NUM_1 = 62, K_NUM_2 = 63, K_NUM_3 = 64, K_NUM_4 = 65,
		K_NUM_5 = 66, K_NUM_6 = 67, K_NUM_7 = 68, K_NUM_8 = 69, K_NUM_9 = 70,
		
		K_NUM_LOCK = 71, K_NUM_RETURN = 72, 
		K_NUM_DIVIDE = 73, K_NUM_MULTIPLY = 74,
		K_NUM_ADD = 75, K_NUM_SUBTRACT = 76,
		
		K_0 = 77, K_1 = 78, K_2 = 79, K_3 = 80, K_4 = 81,
		K_5 = 82, K_6 = 83, K_7 = 84, K_8 = 85, K_9 = 86,
		
		K_F1 = 87, K_F2 = 88, K_F3 = 89, K_F4 = 90, 
		K_F5 = 91, K_F6 = 92, K_F7 = 93, K_F8 = 94, 
		K_F9 = 95, K_F10 = 96, K_F11 = 97, K_F12 = 98, 
		
		K_RETURN = 99, K_BACKSPACE = 100, K_SPACE = 101,
		K_ESC = 102, K_TAB = 103, K_CAPS_LOCK = 104,
		
		K_LEFT_SHIFT = 105, K_LEFT_CTRL = 106, K_LEFT_ALT = 107, 
		K_RIGHT_SHIFT = 108, K_RIGHT_CTRL = 109, K_RIGHT_ALT = 110, 
		
		K_PRINT_SCREEN = 111, K_SCROLL_LOCK = 112, K_PAUSE = 113, 
		K_INSERT = 114, K_DELETE = 115,
		K_HOME = 116, K_END = 117, 
		K_PAGE_UP = 118, K_PAGE_DOWN = 119, 
		
		K_ARROW_UP = 120, K_ARROW_DOWN = 121, 
		K_ARROW_LEFT = 122, K_ARROW_RIGHT = 123, 
		
		K_ALTERNATE_GRAPHIC = 124, 
		K_FUNCTION = 125, 
		K_SYSTEM = 126, 
		K_MENU = 127
	};
	
	//Common keyboard actions. 128-140 is reserved.
	inline constexpr array<KeyValue, 67> keyboard_actions
	{{
		{scast<u32>(KeyboardButton::K_NUM_0), 0, "Num 0"}, 
		{scast<u32>(KeyboardButton::K_NUM_1), 0, "Num 1"}, 
		{scast<u32>(KeyboardButton::K_NUM_2), 0, "Num 2"}, 
		{scast<u32>(KeyboardButton::K_NUM_3), 0, "Num 3"}, 
		{scast<u32>(KeyboardButton::K_NUM_4), 0, "Num 4"},
		{scast<u32>(KeyboardButton::K_NUM_5), 0, "Num 5"}, 
		{scast<u32>(KeyboardButton::K_NUM_6), 0, "Num 6"}, 
		{scast<u32>(KeyboardButton::K_NUM_7), 0, "Num 7"}, 
		{scast<u32>(KeyboardButton::K_NUM_8), 0, "Num 8"}, 
		{scast<u32>(KeyboardButton::K_NUM_9), 0, "Num 9"}, 
		
		{scast<u32>(KeyboardButton::K_NUM_LOCK),     0, "Num lock"}, 
		{scast<u32>(KeyboardButton::K_NUM_RETURN),   0, "Num return"}, 
		{scast<u32>(KeyboardButton::K_NUM_DIVIDE),   0, "Num /"}, 
		{scast<u32>(KeyboardButton::K_NUM_MULTIPLY), 0, "Num *"}, 
		{scast<u32>(KeyboardButton::K_NUM_ADD),      0, "Num +"}, 
		{scast<u32>(KeyboardButton::K_NUM_SUBTRACT), 0, "Num -"},
		
		{scast<u32>(KeyboardButton::K_0), 0, "0"}, 
		{scast<u32>(KeyboardButton::K_1), 0, "1"}, 
		{scast<u32>(KeyboardButton::K_2), 0, "2"}, 
		{scast<u32>(KeyboardButton::K_3), 0, "3"}, 
		{scast<u32>(KeyboardButton::K_4), 0, "4"},
		{scast<u32>(KeyboardButton::K_5), 0, "5"}, 
		{scast<u32>(KeyboardButton::K_6), 0, "6"}, 
		{scast<u32>(KeyboardButton::K_7), 0, "7"}, 
		{scast<u32>(KeyboardButton::K_8), 0, "8"}, 
		{scast<u32>(KeyboardButton::K_9), 0, "9"},
		
		{scast<u32>(KeyboardButton::K_F1), 0, "F1"}, 
		{scast<u32>(KeyboardButton::K_F2), 0, "F2"}, 
		{scast<u32>(KeyboardButton::K_F3), 0, "F3"}, 
		{scast<u32>(KeyboardButton::K_F4), 0, "F4"}, 
		{scast<u32>(KeyboardButton::K_F5), 0, "F5"}, 
		{scast<u32>(KeyboardButton::K_F6), 0, "F6"}, 
		{scast<u32>(KeyboardButton::K_F7), 0, "F7"}, 
		{scast<u32>(KeyboardButton::K_F8), 0, "F8"},
		{scast<u32>(KeyboardButton::K_F9), 0, "F9"}, 
		{scast<u32>(KeyboardButton::K_F10), 0, "F10"}, 
		{scast<u32>(KeyboardButton::K_F11), 0, "F11"}, 
		{scast<u32>(KeyboardButton::K_F12), 0, "F12"},
		
		{scast<u32>(KeyboardButton::K_RETURN),    0, "Return"}, 
		{scast<u32>(KeyboardButton::K_BACKSPACE), 0, "Backspace"}, 
		{scast<u32>(KeyboardButton::K_SPACE),     0, "Space"},
		{scast<u32>(KeyboardButton::K_ESC),       0, "Esc"}, 
		{scast<u32>(KeyboardButton::K_TAB),       0, "Tab"}, 
		{scast<u32>(KeyboardButton::K_CAPS_LOCK), 0, "Caps lock"},
		
		{scast<u32>(KeyboardButton::K_LEFT_SHIFT),  0, "Left shift"}, 
		{scast<u32>(KeyboardButton::K_LEFT_CTRL),   0, "Left control"}, 
		{scast<u32>(KeyboardButton::K_LEFT_ALT),    0, "Left alt"},
		{scast<u32>(KeyboardButton::K_RIGHT_SHIFT), 0, "Right shift"}, 
		{scast<u32>(KeyboardButton::K_RIGHT_CTRL),  0, "Right control"}, 
		{scast<u32>(KeyboardButton::K_RIGHT_ALT),   0, "Right alt"},
		
		{scast<u32>(KeyboardButton::K_PRINT_SCREEN), 0, "Print screen"}, 
		{scast<u32>(KeyboardButton::K_SCROLL_LOCK),  0, "Scroll lock"}, 
		{scast<u32>(KeyboardButton::K_PAUSE),        0, "Pause"},
		{scast<u32>(KeyboardButton::K_INSERT),       0, "Insert"}, 
		{scast<u32>(KeyboardButton::K_DELETE),       0, "Delete"},
		{scast<u32>(KeyboardButton::K_HOME),         0, "Home"}, 
		{scast<u32>(KeyboardButton::K_END),          0, "End"},
		{scast<u32>(KeyboardButton::K_PAGE_UP),      0, "Page up"}, 
		{scast<u32>(KeyboardButton::K_PAGE_DOWN),    0, "Page down"},
		
		{scast<u32>(KeyboardButton::K_ARROW_UP),    0, "Arrow up"}, 
		{scast<u32>(KeyboardButton::K_ARROW_DOWN),  0, "Arrow down"}, 
		{scast<u32>(KeyboardButton::K_ARROW_LEFT),  0, "Arrow left"}, 
		{scast<u32>(KeyboardButton::K_ARROW_RIGHT), 0, "Arrow right"},
		
		{scast<u32>(KeyboardButton::K_ALTERNATE_GRAPHIC), 0, "Alternate graphic"}, 
		{scast<u32>(KeyboardButton::K_FUNCTION),          0, "Function"}, 
		{scast<u32>(KeyboardButton::K_SYSTEM),            0, "System"}, 
		{scast<u32>(KeyboardButton::K_MENU),              0, "Menu"}
	}};
	
	inline constexpr KeyboardButton GetKeyButton(const string& v)
	{
		for (const auto& _b : keyboard_actions)
		{
			if (_b.value == v) return scast<KeyboardButton>(_b.key);
		}
		return scast<KeyboardButton>(0);
	}
	inline constexpr string_view GetKeyValue(KeyboardButton b) 
	{ 
		for (const auto& _b : keyboard_actions)
		{
			if (_b.key == scast<u32>(b)) return _b.value;
		}
		return "";
	}
	
	//
	// KEY SYMBOLS
	//
	
	//Common symbols used for typing and UI. 186-200 is reserved for future use.
	inline constexpr array<KeyValue, 45> typography_symbols
	{{
		{141, 0x0030, "0"}, {142, 0x0031, "1"}, {143, 0x0032, "2"}, {144, 0x0033, "3"}, {145, 0x0034, "4"},
		{146, 0x0035, "5"}, {147, 0x0036, "6"}, {148, 0x0037, "7"}, {149, 0x0038, "8"}, {150, 0x0039, "9"},
		
		{151, 0x003F, "?"}, {152, 0x0021, "!"}, {153, 0x005F, "_"}, 
		{154, 0x0040, "@"}, {155, 0x003B, ";"}, {156, 0x002C, ","}, {157, 0x002E, "."}, 
		
		{158, 0x0028, "("}, {159, 0x0029, ")"}, 
		{160, 0x007B, "{"}, {161, 0x007D, "}"}, 
		{162, 0x005B, "["}, {163, 0x005D, "]"},  
		
		{164, 0x0020, "Space"}, {165, 0x00A0, "Non-breaking space"}, 
		
		{166, 0x005C, "\\"}, //backslash
		
		{167, 0x2013, "–"}, //endash 
		{168, 0x2014, "—"}, //emdash
		{169, 0x2026, "…"}, //ellipsis
		
		{170, 0x0027, "'"}, //straight single quote
		{171, 0x0022, "\""}, //straight double quote
		{172, 0x02C7, "ˇ"}, //caron
		{173, 0x0060, "`"}, //grave accent
		{174, 0x00B4, "´"}, //acute accent
		{175, 0x2018, "‘"}, //left single quote
		{176, 0x2019, "’"}, //right single quote
		{177, 0x201C, "“"}, //left double quote
		{178, 0x201D, "”"}, //right double quote
		
		{179, 0x00B0, "°"}, //degree
		{180, 0x00A7, "§"}, //section
		{181, 0x2022, "•"}, //bullet
		{182, 0x00B7, "·"}, //middot
		
		{183, 0x00A9, "©"}, //copyright
		{184, 0x00AE, "®"}, //registered
		{185, 0x2122, "™"}  //trademark
	}};
	
	//Common symbols used for math and technical operations. 236-250 is reserved for future use.
	inline constexpr array<KeyValue, 37> math_symbols
	{{
		{201, 0x002B, "+"}, {202, 0x002D, "-"}, 
		{203, 0x002A, "*"}, {204, 0x00D7, "×"}, 
		{205, 0x002F, "/"}, {206, 0x00F7, "÷"},
		{207, 0x003A, ":"}, {208, 0x2237, "∷"}, 
		
		{209, 0x0025, "%"}, //modulus
		{210, 0x007C, "|"}, //bitwise or logical OR
		
		{211, 0x007E, "~"}, //bitwise NOT
		{212, 0x2248, "≈"}, //approximately equal
		{213, 0x003D, "="}, //equality
		{214, 0x2260, "≠"}, //not equal
		{215, 0x2261, "≡"}, //identical to
		{216, 0x221E, "∞"}, //infinity
		{217, 0x221A, "√"}, //square root
		{218, 0x00B0, "°"}, //degree
		{219, 0x00B5, "µ"}, //micro
		{220, 0x2126, "Ω"}, //ohm
		
		{221, 0x003C, "<"}, 
		{222, 0x003E, ">"}, 
		{223, 0x2264, "≤"}, 
		{224, 0x2265, "≥"},
		{225, 0x00B1, "±"}, //plus-minus
		
		{226, 0x2190, "←"},  
		{227, 0x2191, "↑"},  
		{228, 0x2192, "→"},  
		{229, 0x2193, "↓"},  
		{230, 0x21D2, "⇒"},  //implies  
		{231, 0x21D4, "⇔"},  //if and only if  
		{232, 0x27F5, "⟵"}, //long left arrow  
		{233, 0x27F6, "⟶"}, //long right arrow 
		
		{234, 0x00B9, "¹"}, //superscript one
		{235, 0x00B2, "²"}, //superscript two
		{236, 0x00B3, "³"}  //superscript three
	}};
	
	//Common currency symbols. 264-270 is reserved for future use.
	inline constexpr array<KeyValue, 13> currency_symbols
	{{
		{251, 0x20AC, "€"},  //euro
		{252, 0x00A3, "£"},  //pound
		{253, 0x0024, "$"},  //dollar
		{254, 0x20B4, "₴"},  //ukrainian hryvnia
		{255, 0x20BD, "₽"},  //russian rouble
		{256, 0x20A8, "₨"},  //old indian rupee
		{257, 0x20B9, "₹"},  //indian rupee
		{258, 0x00A5, "¥"},  //japanese yen
		{259, 0x5186, "円"}, //japanese yen (kanji)
		{260, 0x5713, "圓"}, //taiwanese/hong kong yuan
		{261, 0x20A9, "₩"},  //korean won
		{262, 0x20AA, "₪"},  //israeli shekel
		{263, 0x20B1, "₱"}   //philippine peso
	}};
	
	//
	// LATIN LETTERS
	//
	
	inline constexpr array<KeyValue, 52> latin_standard
	{{
		{271, 0x0041, "A"}, {272, 0x0042, "B"}, {273, 0x0043, "C"}, {274, 0x0044, "D"}, {275, 0x0045, "E"},
		{276, 0x0046, "F"}, {277, 0x0047, "G"}, {278, 0x0048, "H"}, {279, 0x0049, "I"}, {280, 0x004A, "J"},
		{281, 0x004B, "K"}, {282, 0x004C, "L"}, {283, 0x004D, "M"}, {284, 0x004E, "N"}, {285, 0x004F, "O"},
		{286, 0x0050, "P"}, {287, 0x0051, "Q"}, {288, 0x0052, "R"}, {289, 0x0053, "S"}, {290, 0x0054, "T"},
		{291, 0x0055, "U"}, {292, 0x0056, "V"}, {293, 0x0057, "W"}, {294, 0x0058, "X"}, {295, 0x0059, "Y"},
		{296, 0x005A, "Z"},
		
		{297, 0x0061, "a"}, {298, 0x0062, "b"}, {299, 0x0063, "c"}, {300, 0x0064, "d"}, {301, 0x0065, "e"},
		{302, 0x0066, "f"}, {303, 0x0067, "g"}, {304, 0x0068, "h"}, {305, 0x0069, "i"}, {306, 0x006A, "j"},
		{307, 0x006B, "k"}, {308, 0x006C, "l"}, {309, 0x006D, "m"}, {310, 0x006E, "n"}, {311, 0x006F, "o"},
		{312, 0x0070, "p"}, {313, 0x0071, "q"}, {314, 0x0072, "r"}, {315, 0x0073, "s"}, {316, 0x0074, "t"},
		{317, 0x0075, "u"}, {318, 0x0076, "v"}, {319, 0x0077, "w"}, {320, 0x0078, "x"}, {321, 0x0079, "y"},
		{322, 0x007A, "z"}
	}};

	inline constexpr array<KeyValue, 124> latin_extra
	{{
		//simple uppercase diacritic and regional extensions
		
		{323, 0x00C0, "À"}, {324, 0x00C1, "Á"}, {325, 0x00C2, "Â"}, {326, 0x00C3, "Ã"}, {327, 0x00C4, "Ä"},
		{328, 0x00C5, "Å"}, {329, 0x00C7, "Ç"}, {330, 0x00C8, "È"}, {331, 0x00C9, "É"}, {332, 0x00CA, "Ê"},
		{333, 0x00CB, "Ë"}, {334, 0x00CC, "Ì"}, {335, 0x00CD, "Í"}, {336, 0x00CE, "Î"}, {337, 0x00CF, "Ï"},
		{338, 0x00D1, "Ñ"}, {339, 0x00D2, "Ò"}, {340, 0x00D3, "Ó"}, {341, 0x00D4, "Ô"}, {342, 0x00D5, "Õ"},
		{343, 0x00D6, "Ö"}, {344, 0x00D8, "Ø"}, {345, 0x00D9, "Ù"}, {346, 0x00DA, "Ú"}, {347, 0x00DB, "Û"},
		{348, 0x00DC, "Ü"}, {349, 0x00DD, "Ý"}, {350, 0x0179, "Ź"}, {351, 0x017B, "Ż"}, {352, 0x017D, "Ž"},
		{353, 0x0102, "Ă"}, {354, 0x0104, "Ą"}, {355, 0x010C, "Č"}, {356, 0x010E, "Ď"}, {357, 0x0118, "Ę"},
		{358, 0x011A, "Ě"}, {359, 0x011E, "Ğ"}, {360, 0x0141, "Ł"}, {361, 0x0143, "Ń"}, {362, 0x0147, "Ň"},
		{363, 0x0150, "Ő"}, {364, 0x0158, "Ř"}, {365, 0x015A, "Ś"}, {366, 0x015E, "Ş"}, {367, 0x0160, "Š"},
		{368, 0x0162, "Ţ"}, {369, 0x0164, "Ť"}, {370, 0x016E, "Ů"}, {371, 0x0170, "Ű"}, {372, 0x0172, "Ų"},
		{373, 0x0176, "Ŷ"}, {374, 0x0178, "Ÿ"},

		//uppercase morphological / ligature / historical letters
		
		{375, 0x00C6, "Æ"}, //AE ligature - Nordic/French
		{376, 0x00D0, "Ð"}, //Eth - Icelandic/Faroese
		{377, 0x0152, "Œ"}, //OE ligature - French/Latin
		{378, 0x0132, "Ĳ"}, //IJ ligature - Dutch
		{379, 0x00DE, "Þ"}, //Thorn - Old English/Icelandic
		{380, 0x1E9E, "ẞ"}, //Capital Sharp S - German
		{381, 0x017F, "ſ"}, //Long s - Historical typography
		
		//simple lowercase diacritic and regional extensions

		{382, 0x00E0, "à"}, {383, 0x00E1, "á"}, {384, 0x00E2, "â"}, {385, 0x00E3, "ã"}, {386, 0x00E4, "ä"},
		{387, 0x00E5, "å"}, {388, 0x00E7, "ç"}, {389, 0x00E8, "è"}, {390, 0x00E9, "é"}, {391, 0x00EA, "ê"},
		{392, 0x00EB, "ë"}, {393, 0x00EC, "ì"}, {394, 0x00ED, "í"}, {395, 0x00EE, "î"}, {396, 0x00EF, "ï"},
		{397, 0x00F1, "ñ"}, {398, 0x00F2, "ò"}, {399, 0x00F3, "ó"}, {400, 0x00F4, "ô"}, {401, 0x00F5, "õ"},
		{402, 0x00F6, "ö"}, {403, 0x00F8, "ø"}, {404, 0x00F9, "ù"}, {405, 0x00FA, "ú"}, {406, 0x00FB, "û"},
		{407, 0x00FC, "ü"}, {408, 0x00FD, "ý"}, {409, 0x017A, "ź"}, {410, 0x017C, "ż"}, {411, 0x017E, "ž"},
		{412, 0x0103, "ă"}, {413, 0x0105, "ą"}, {414, 0x010D, "č"}, {415, 0x010F, "ď"}, {416, 0x0119, "ę"},
		{417, 0x011B, "ě"}, {418, 0x011F, "ğ"}, {419, 0x0142, "ł"}, {420, 0x0144, "ń"}, {421, 0x0148, "ň"},
		{422, 0x0151, "ő"}, {423, 0x0159, "ř"}, {424, 0x015B, "ś"}, {425, 0x015F, "ş"}, {426, 0x0161, "š"},
		{427, 0x0163, "ţ"}, {428, 0x0165, "ť"}, {429, 0x016F, "ů"}, {430, 0x0171, "ű"}, {431, 0x0173, "ų"},
		{432, 0x0177, "ŷ"}, {433, 0x00FF, "ÿ"},

		//lowercase morphological / ligature / historical letters

		{434, 0x00E6, "æ"}, //AE ligature - Nordic/French
		{435, 0x00F0, "ð"}, //Eth - Icelandic/Faroese
		{436, 0x0153, "œ"}, //OE ligature - French/Latin
		{437, 0x0133, "ĳ"}, //IJ ligature - Dutch
		{438, 0x00FE, "þ"}, //Thorn - Old English/Icelandic
		{439, 0x00DF, "ß"}, //Sharp S - German
		{440, 0x017F, "ſ"}  //Long s - Historical typography
	}};
	
	//
	// CYRILLIC LETTERS
	//
	
	inline constexpr array<KeyValue, 66> cyrillic_standard
	{{
		{441, 0x0410, "А"}, {442, 0x0411, "Б"}, {443, 0x0412, "В"}, {444, 0x0413, "Г"}, {445, 0x0414, "Д"},
		{446, 0x0415, "Е"}, {447, 0x0401, "Ё"}, {448, 0x0416, "Ж"}, {449, 0x0417, "З"}, {450, 0x0418, "И"},
		{451, 0x0419, "Й"}, {452, 0x041A, "К"}, {453, 0x041B, "Л"}, {454, 0x041C, "М"}, {455, 0x041D, "Н"},
		{456, 0x041E, "О"}, {457, 0x041F, "П"}, {458, 0x0420, "Р"}, {459, 0x0421, "С"}, {460, 0x0422, "Т"},
		{461, 0x0423, "У"}, {462, 0x0424, "Ф"}, {463, 0x0425, "Х"}, {464, 0x0426, "Ц"}, {465, 0x0427, "Ч"},
		{466, 0x0428, "Ш"}, {467, 0x0429, "Щ"}, {468, 0x042A, "Ъ"}, {469, 0x042B, "Ы"}, {470, 0x042C, "Ь"},
		{471, 0x042D, "Э"}, {472, 0x042E, "Ю"}, {473, 0x042F, "Я"},
		
		{474, 0x0430, "а"}, {475, 0x0431, "б"}, {476, 0x0432, "в"}, {477, 0x0433, "г"}, {478, 0x0434, "д"},
		{479, 0x0435, "е"}, {480, 0x0451, "ё"}, {481, 0x0436, "ж"}, {482, 0x0437, "з"}, {483, 0x0438, "и"},
		{484, 0x0439, "й"}, {485, 0x043A, "к"}, {486, 0x043B, "л"}, {487, 0x043C, "м"}, {488, 0x043D, "н"},
		{489, 0x043E, "о"}, {490, 0x043F, "п"}, {491, 0x0440, "р"}, {492, 0x0441, "с"}, {493, 0x0442, "т"},
		{494, 0x0443, "у"}, {495, 0x0444, "ф"}, {496, 0x0445, "х"}, {497, 0x0446, "ц"}, {498, 0x0447, "ч"},
		{499, 0x0448, "ш"}, {500, 0x0449, "щ"}, {501, 0x044A, "ъ"}, {502, 0x044B, "ы"}, {503, 0x044C, "ь"},
		{504, 0x044D, "э"}, {505, 0x044E, "ю"}, {506, 0x044F, "я"}
	}};

	inline constexpr array<KeyValue, 20> cyrillic_extra
	{{
		{507, 0x0490, "Ґ"}, {508, 0x0404, "Є"}, {509, 0x0406, "І"}, {510, 0x0407, "Ї"},
		{511, 0x0408, "Ј"}, {512, 0x0409, "Љ"}, {513, 0x040A, "Њ"}, {514, 0x040B, "Ћ"}, 
		{515, 0x0402, "Ђ"}, {516, 0x040F, "Џ"},
		
		{517, 0x0491, "ґ"}, {518, 0x0454, "є"}, {519, 0x0456, "і"}, {520, 0x0457, "ї"},
		{521, 0x0458, "ј"}, {522, 0x0459, "љ"}, {523, 0x045A, "њ"}, {524, 0x045B, "ћ"},
		{525, 0x0452, "ђ"}, {526, 0x045F, "џ"}
	}};
	
	//
	// EMOJIS
	//
	
	//Common face, hand and gesture emojis. 1226-1500 is reserved for future use. 
	inline constexpr array<KeyValue, 226> emojis
	{{
		//face - smiling
		
		{1001, 0x1F600, "grinning face"},
		{1002, 0x1F603, "grinning face with big eyes"},
		{1003, 0x1F604, "grinning face with smiling eyes"},
		{1004, 0x1F601, "beaming face with smiling eyes"},
		{1005, 0x1F606, "grinning squinting face"},
		{1006, 0x1F605, "grinning face with sweat"},
		{1008, 0x1F923, "rolling on the floor laughing"},
		{1009, 0x1F602, "face with tears of joy"},
		{1010, 0x1F642, "slightly smiling face"},
		{1011, 0x1F643, "upside-down face"},
		{1012, 0x1FAE0, "melting face"},
		{1013, 0x1F609, "winking face"},
		{1014, 0x1F60A, "smiling face with smiling eyes"},
		{1015, 0x1F607, "smiling face with halo"},
		
		//face - affection
		
		{1016, 0x1F970, "smiling face with hearts"},
		{1017, 0x1F60D, "smiling face with heart-eyes"},
		{1018, 0x1F929, "star-struck"},
		{1019, 0x1F618, "face blowing a kiss"},
		{1020, 0x1F617, "kissing face"},
		{1021, 0x263A, "smiling face"},
		{1022, 0x1F61A, "kissing face with closed eyes"},
		{1023, 0x1F619, "kissing face with smiling eyes"},
		{1024, 0x1F972, "smiling face with tear"},
		
		//face - tongue
		
		{1025, 0x1F60B, "face savoring food"},
		{1026, 0x1F61B, "face with tongue"},
		{1027, 0x1F61C, "winking face with tongue"},
		{1028, 0x1F92A, "zany face"},
		{1029, 0x1F61D, "squinting face with tongue"},
		{1030, 0x1F911, "money-mouth face"},
		
		//face - hand
		
		{1031, 0x1F917, "smiling face with open hands"},
		{1032, 0x1F92D, "face with hand over mouth"},
		{1033, 0x1FAE2, "face with open eyes and hand over mouth"},
		{1034, 0x1FAE3, "face with peeking eye"},
		{1035, 0x1F92B, "shushing face"},
		{1036, 0x1F914, "thinking face"},
		{1037, 0x1FAE1, "saluting face"},
		
		//face - neutral/skeptical
		
		//the following ones were not included:
		//  - face in clouds
		//  - face exhaling
		//  - head shaking horizontally
		//  - head shaking vertically
		
		{1038, 0x1F910, "zipper-mouth face"},
		{1039, 0x1F928, "face with raised eyebrow"},
		{1040, 0x1F610, "neutral face"},
		{1041, 0x1F611, "expressionless face"},
		{1042, 0x1F636, "face without mouth"},
		{1043, 0x1FAE5, "dotted line face"},
		{1044, 0x1F60F, "smirking face"},
		{1045, 0x1F612, "unamused face"},
		{1046, 0x1F644, "face with rolling eyes"},
		{1047, 0x1F62C, "grimacing face"},
		{1048, 0x1F925, "lying face"},
		{1049, 0x1FAE8, "shaking face"},
		
		//face - sleepy
		
		{1050, 0x1F60C, "relieved face"},
		{1051, 0x1F614, "pensive face"},
		{1052, 0x1F62A, "sleepy face"},
		{1053, 0x1F924, "drooling face"},
		{1054, 0x1F634, "sleeping face"},
		{1055, 0x1FAE9, "face with bags under eyes"},
		
		//face - unwell
		
		//the following one was not included:
		//  - face with spiral eyes
		
		{1056, 0x1F637, "face with medical mask"},
		{1057, 0x1F912, "face with thermometer"},
		{1058, 0x1F915, "face with head-bandage"},
		{1059, 0x1F922, "nauseated face"},
		{1060, 0x1F92E, "face vomiting"},
		{1061, 0x1F927, "sneezing face"},
		{1062, 0x1F975, "hot face"},
		{1063, 0x1F976, "cold face"},
		{1064, 0x1F974, "woozy face"},
		{1065, 0x1F635, "face with crossed-out eyes"},
		{1066, 0x1F92F, "exploding head"},
		
		//face - hat
		
		{1067, 0x1F920, "cowboy hat face"},
		{1068, 0x1F973, "partying face"},
		{1069, 0x1F978, "disguised face"},
		
		//face - glasses
		
		{1070, 0x1F60E, "smiling face with sunglasses"},
		{1071, 0x1F913, "nerd face"},
		{1072, 0x1F9D0, "face with monocle"},
		
		//face - concerned
		
		{1073, 0x1F615, "confused face"},
		{1074, 0x1FAE4, "face with diagonal mouth"},
		{1075, 0x1F61F, "worried face"},
		{1076, 0x1F641, "slightly frowning face"},
		{1077, 0x1F639, "frowning face"},
		{1078, 0x2639, "face with open mouth"},
		{1079, 0x1F62E, "hushed face"},
		{1080, 0x1F62F, "astonished face"},
		{1081, 0x1F632, "flushed face"},
		{1082, 0x1F633, "distorted face"},
		{1083, 0x1FAEA, "pleading face"},
		{1084, 0x1F97A, "face holding back tears"},
		{1085, 0x1F979, "frowing face with open mouth"},
		{1086, 0x1F626, "anguished face"},
		{1087, 0x1F627, "fearful face"},
		{1088, 0x1F628, "anxious face with sweat"},
		{1089, 0x1F630, "sad but relieved face"},
		{1090, 0x1F625, "crying face"},
		{1091, 0x1F622, "loudly crying face"},
		{1092, 0x1F62D, "face screaming in fear"},
		{1093, 0x1F631, "confounded face"},
		{1094, 0x1F616, "persevering face"},
		{1095, 0x1F623, "dissapointed face"},
		{1096, 0x1F61E, "downcast face with sweat"},
		{1097, 0x1F613, "weary face"},
		{1098, 0x1F62B, "tired face"},
		{1099, 0x1F971, "yawning face"},
		
		//face - negative
		
		{1100, 0x1F624, "face with steam from nose"},
		{1101, 0x1F621, "enraged face"},
		{1102, 0x1F620, "angry face"},
		{1103, 0x1F92C, "face with symbols on mouth"},
		{1104, 0x1F608, "smiling face with horns"},
		{1105, 0x1F47F, "angry face with horns"},
		{1106, 0x1F480, "skull"},
		{1107, 0x2620, "skull and crossbones"},
		
		//face - costume
		
		{1108, 0x1F4A9, "pile of poo"},
		{1109, 0x1F921, "clown face"},
		{1110, 0x1F479, "ogre"},
		{1111, 0x1F47A, "goblin"},
		{1112, 0x1F47B, "ghost"},
		{1113, 0x1F47D, "alien"},
		{1114, 0x1F47E, "alien monster"},
		{1115, 0x1F916, "robot"},
		
		//face - cat
		
		{1116, 0x1F63A, "grinning cat"},
		{1117, 0x1F638, "grinning cat with smiling eyes"},
		{1118, 0x1F639, "cat with tears of joy"},
		{1119, 0x1F63B, "smiling cat with heart-eyes"},
		{1120, 0x1F63C, "cat with wry smile"},
		{1121, 0x1F63D, "kissing cat"},
		{1122, 0x1F640, "weary cat"},
		{1123, 0x1F63F, "crying cat"},
		{1124, 0x1F63E, "pouting cat"},
		
		//face - monkey
		
		{1125, 0x1F648, "see-no-evil monkey"},
		{1126, 0x1F649, "hear-no-evil monkey"},
		{1127, 0x1F64A, "speak-no-evil monkey"},
		
		//heart
		
		//the following ones were not included:
		//  - heart on fire
		//  - mending heart
		
		{1128, 0x1F48C, "love letter"},
		{1129, 0x1F498, "heart with arrow"},
		{1130, 0x1F49D, "heart with ribbon"},
		{1131, 0x1F496, "sparkling heart"},
		{1132, 0x1F497, "growing heart"},
		{1133, 0x1F493, "beating heart"},
		{1134, 0x1F49E, "revolving hearts"},
		{1135, 0x1F495, "two hearts"},
		{1136, 0x1F49F, "heart decoration"},
		{1137, 0x2763, "heart exclamation"},
		{1138, 0x1F494, "broken heart"},
		{1139, 0x1F764, "red heart"},
		{1140, 0x1FA77, "pink heart"},
		{1141, 0x1F9E1, "orange heart"},
		{1142, 0x1F49B, "yellow heart"},
		{1143, 0x1F49A, "green heart"},
		{1144, 0x1F499, "blue heart"},
		{1145, 0x1FA75, "light blue heart"},
		{1146, 0x1F49C, "purple heart"},
		{1147, 0x1F90E, "brown heart"},
		{1148, 0x1F5A4, "black heart"},
		{1149, 0x1FA76, "grey heart"},
		{1150, 0x1F90D, "white heart"},
		
		//emotion
		
		//the following one was not included:
		//  - eye in speech bubble
		
		{1151, 0x1F48B, "kiss mark"},
		{1152, 0x1F4AF, "hundred points"},
		{1153, 0x1F4A2, "anger symbol"},
		{1154, 0x1FAEF, "fight cloud"},
		{1155, 0x1F4A5, "collision"},
		{1156, 0x1F4AB, "dizzy"},
		{1157, 0x1F4A6, "sweat droplets"},
		{1158, 0x1F4A8, "dashing away"},
		{1159, 0x1F573, "hole"},
		{1160, 0x1F4AC, "speech balloon"},
		{1161, 0x1F5E8, "left speech bubble"},
		{1162, 0x1F5EF, "right angle bubble"},
		{1163, 0x1F4AD, "thought balloon"},
		{1164, 0x1F4A4, "zzz"},
		
		//hand - fingers open
		
		{1165, 0x1F44B, "waving hand"},
		{1166, 0x1F91A, "raised back of hand"},
		{1167, 0x1F590, "hand with fingers splayed"},
		{1168, 0x270B, "raised hand"},
		{1169, 0x1F596, "vulcan salute"},
		{1170, 0x1FAF1, "rightwards hand"},
		{1171, 0x1FAF2, "leftwards hand"},
		{1172, 0x1FAF3, "palm down hand"},
		{1173, 0x1FAF4, "palm up hand"},
		{1174, 0x1FAF7, "leftwards pushing hand"},
		{1175, 0x1FAF8, "rightwards pushing hand"},
		
		//hand - fingers partial
		
		{1176, 0x1F44C, "ok hand"},
		{1177, 0x1F90C, "pinched fingers"},
		{1178, 0x1F90F, "pinching hand"},
		{1179, 0x270C, "victory hand"},
		{1180, 0x1F91E, "crossed fingers"},
		{1181, 0x1FAF0, "hand with index finger and thumb crossed"},
		{1182, 0x1F91F, "love-you gesture"},
		{1183, 0x1F918, "sign of the horns"},
		{1184, 0x1F919, "call me hand"},
		
		//hand - single finger
		
		{1185, 0x1F448, "backhand index pointing left"},
		{1186, 0x1F449, "backhand index pointing right"},
		{1187, 0x1F446, "backhand index pointing up"},
		{1188, 0x1F595, "middle finger"},
		{1189, 0x1F447, "backhand index pointing down"},
		{1190, 0x261D, "index pointing up"},
		{1191, 0x1FAF5, "index pointing at the viewer"},
		
		//hand - fingers closed
		
		{1192, 0x1F44D, "thumbs up"},
		{1193, 0x1F44E, "thumbs down"},
		{1194, 0x270A, "raised fist"},
		{1195, 0x1F44A, "oncoming fist"},
		{1196, 0x1F91B, "left-facing fist"},
		{1197, 0x1F91C, "right-facing fist"},
		
		//hands
		
		{1198, 0x1F44F, "clapping hands"},
		{1199, 0x1F64C, "raising hands"},
		{1200, 0x1FAF6, "heart hands"},
		{1201, 0x1F450, "open hands"},
		{1202, 0x1F932, "palms up together"},
		{1203, 0x1F91D, "handshake"},
		{1204, 0x1F64F, "folded hands"},
		
		//hand - prop
		
		{1205, 0x270D, "writing hand"},
		{1206, 0x1F485, "nail polish"},
		{1207, 0x1F933, "selfie"},
		
		//body parts
		
		{1208, 0x1F4AA, "flexed biceps"},
		{1209, 0x1F9BE, "mechanical arm"},
		{1210, 0x1F9BF, "mechanical leg"},
		{1211, 0x1F9B5, "leg"},
		{1212, 0x1F9B6, "foot"},
		{1213, 0x1F442, "ear"},
		{1214, 0x1F9BB, "ear with hearing aid"},
		{1215, 0x1F443, "nose"},
		{1216, 0x1F9E0, "brain"},
		{1217, 0x1FAC0, "anatomical heart"},
		{1218, 0x1FAC1, "lungs"},
		{1219, 0x1F9B7, "tooth"},
		{1220, 0x1F9B4, "bone"},
		{1221, 0x1F440, "eyes"},
		{1222, 0x1F441, "eye"},
		{1223, 0x1F445, "tongue"},
		{1224, 0x1F444, "mouth"},
		{1225, 0x1FAE6, "biting lip"}
	}};
	
	//
	// KAOMOJIS
	//
	
	//Common kaomojis. 1591 to 1700 is reserved for future use.
	inline constexpr array<KeyValue, 90> kaomojis
	{{
		//joy
		
		{1501, 0x003F, "(* ^ ω ^)"},
		{1502, 0x003F, "٩(◕‿◕｡)۶"},
		{1503, 0x003F, "(￣ω￣)"},
		{1504, 0x003F, "(* ^ ω ^)"},
		{1505, 0x003F, "(≧◡≦)"},
		{1506, 0x003F, "(─‿‿─)"},
		{1507, 0x003F, "(´• ω •`)"},
		{1508, 0x003F, "(o･ω･o)"},
		{1509, 0x003F, "＼(≧▽≦)／"},
		{1510, 0x003F, "(☆ω☆)"},
		{1511, 0x003F, "(๑>◡<๑)"},
		{1512, 0x003F, "(╯✧▽✧)╯"},
		{1513, 0x003F, "o(>ω<)o"},
		{1514, 0x003F, "(ﾉ◕ヮ◕)ﾉ*:･ﾟ✧"},
		{1515, 0x003F, "(๑˘︶˘๑)"},
		{1516, 0x003F, "( = ⩊ = )"},
		{1517, 0x003F, "(ᵔ⩊ᵔ)"},
		{1518, 0x003F, "(„• ᴗ •„)"},
		{1519, 0x003F, "(´｡• ᵕ •｡`)"},
		{1520, 0x003F, "°˖✧◝(⁰▿⁰)◜✧˖°"},
		
		//love
		
		{1521, 0x003F, "(≧◡≦) ♡"},
		{1522, 0x003F, "╰(*´︶`*)╯♡"},
		{1523, 0x003F, "(´• ω •`) ♡"},
		{1524, 0x003F, "(ﾉ´ з `)ノ"},
		{1525, 0x003F, "(´｡• ᵕ •｡`) ♡"},
		{1526, 0x003F, "♡( ◡‿◡ )"},
		{1527, 0x003F, "(*˘︶˘*).｡.:*♡"},
		{1528, 0x003F, "(´ ε ` )♡"},
		{1529, 0x003F, "(´,,•ω•,,)♡"},
		{1530, 0x003F, "( ´ ꒳ ` ) ♡"},
		{1531, 0x003F, "σ(≧ε≦σ) ♡"},
		{1532, 0x003F, "(*^^*)♡"},
		{1533, 0x003F, "ヽ(♡‿♡)ノ"},
		{1534, 0x003F, "(◕‿◕)♡"},
		{1535, 0x003F, "(´｡• ω •｡`) ♡"},
		{1536, 0x003F, "(♡˙︶˙♡)"},
		{1537, 0x003F, "(*¯ ³¯*)♡"},
		{1538, 0x003F, "♡ (￣З￣)"},
		{1539, 0x003F, "♡(>ᴗ•)"},
		{1540, 0x003F, "(｡・//ε//・｡)"},
		
		//embarrassment
		
		{1541, 0x003F, "(⌒_⌒;)	"},
		{1542, 0x003F, "(//ω//)"},
		{1543, 0x003F, "(„ಡωಡ„)"},
		{1544, 0x003F, "(///￣ ￣///)"},
		{1545, 0x003F, "(*ﾉωﾉ)"},
		{1546, 0x003F, "(⁄ ⁄•⁄ω⁄•⁄ ⁄)"},
		{1547, 0x003F, "(*/ω＼)"},
		{1548, 0x003F, "( 〃▽〃)"},
		{1549, 0x003F, "(*ﾉ▽ﾉ)"},
		{1550, 0x003F, "(⁄ ⁄>⁄ ▽ ⁄<⁄ ⁄)"},
		
		//dissatisfaction
		
		{1551, 0x003F, "(＃＞＜)"},
		{1552, 0x003F, "(；￣Д￣)"},
		{1553, 0x003F, "(￢_￢;)"},
		{1554, 0x003F, "(ᗒᗣᗕ)՞"},
		{1555, 0x003F, "(＞﹏＜)"},
		{1556, 0x003F, "(＃￣0￣)"},
		{1557, 0x003F, "(￣ヘ￣)"},
		{1558, 0x003F, "(＾＾＃)	"},
		{1559, 0x003F, "(〃＞＿＜;〃)"},
		{1560, 0x003F, "(￣ ￣|||)"},
		
		//anger
		
		{1561, 0x003F, "(＃`Д´)"},
		{1562, 0x003F, "↑_(ΦwΦ)Ψ"},
		{1563, 0x003F, "(ﾉಥ益ಥ)ﾉ"},
		{1564, 0x003F, "ψ( ` ∇ ´ )ψ	"},
		{1565, 0x003F, "(╬ Ò﹏Ó)	"},
		{1566, 0x003F, "ヾ(`ヘ´)ﾉﾞ"},
		{1567, 0x003F, "୧((#Φ益Φ#))୨	"},
		{1568, 0x003F, "＼＼٩(๑`^´๑)۶／／"},
		{1569, 0x003F, "ヽ( `д´*)ノ"},
		{1570, 0x003F, "((╬◣﹏◢))"},
		
		//sadness
		
		{1571, 0x003F, "(ノ_<。)"},
		{1572, 0x003F, "o(TヘTo)"},
		{1573, 0x003F, "( ╥ω╥ )"},
		{1574, 0x003F, "o(〒﹏〒)o"},
		{1575, 0x003F, "( ; ω ; )"},
		{1576, 0x003F, "(｡T ω T｡)"},
		{1577, 0x003F, "(╯︵╰,)"},
		{1578, 0x003F, "(╥﹏╥)"},
		{1579, 0x003F, "(μ_μ)"},
		{1580, 0x003F, "(ಡ‸ಡ)"},
		
		//cat
		
		{1581, 0x003F, "(=^･ω･^=)"},
		{1582, 0x003F, "(＾• ω •＾)"},
		{1583, 0x003F, "(^◔ᴥ◔^)"},
		{1584, 0x003F, "(=`ω´=)"},
		{1585, 0x003F, "ଲ(ⓛ ω ⓛ)ଲ"},
		{1586, 0x003F, "(=^-ω-^=)"},
		{1587, 0x003F, "ฅ(•ㅅ•❀)ฅ"},
		{1588, 0x003F, "ヾ(=`ω´=)ノ”"},
		{1589, 0x003F, "(=^ ◡ ^=)"},
		{1590, 0x003F, "(^◕ᴥ◕^)"}
	}};

	//
	// GET KEY, UTF OR VALUE
	//
	
	inline vector<KeyValue> keyValues{};
	
	inline void FillKeyValues()
	{
		if (!keyValues.empty()) return;
		
		keyValues.reserve(
			mouseButtons.size()
			+ gamepadButtons.size()
			+ keyboard_actions.size()
			
			+ typography_symbols.size()
			+ math_symbols.size()
			+ currency_symbols.size()
				
			+ latin_standard.size()
			+ latin_extra.size()
				
			+ cyrillic_standard.size()
			+ cyrillic_extra.size()
			
			+ emojis.size()
			+ kaomojis.size());
				
		keyValues.insert(keyValues.end(), mouseButtons.begin(), mouseButtons.end());
		keyValues.insert(keyValues.end(), gamepadButtons.begin(), gamepadButtons.end());
		keyValues.insert(keyValues.end(), keyboard_actions.begin(), keyboard_actions.end());
		
		keyValues.insert(keyValues.end(), typography_symbols.begin(), typography_symbols.end());
		keyValues.insert(keyValues.end(), math_symbols.begin(), math_symbols.end());
		keyValues.insert(keyValues.end(), currency_symbols.begin(), currency_symbols.end());
			
		keyValues.insert(keyValues.end(), latin_standard.begin(), latin_standard.end());
		keyValues.insert(keyValues.end(), latin_extra.begin(), latin_extra.end());
			
		keyValues.insert(keyValues.end(), cyrillic_standard.begin(), cyrillic_standard.end());
		keyValues.insert(keyValues.end(), cyrillic_extra.begin(), cyrillic_extra.end());
		
		keyValues.insert(keyValues.end(), emojis.begin(), emojis.end());
		keyValues.insert(keyValues.end(), kaomojis.begin(), kaomojis.end());
	}
	
	inline u32 GetKeyByUTF(const u32 utf)
	{
		FillKeyValues();
		
		//prevent empty searches
		if (utf == 0) return 0;
		
		const auto it = find_if(
			keyValues.begin(), keyValues.end(),
			[&](const KeyValue& kv) { return kv.utf == utf; });
			
		return (
			it != keyValues.end()
			&& it->utf != 0)
			? it->utf
			: u32{};
	}
	inline u32 GetKeyByValue(const string_view& value)
	{
		FillKeyValues();
		
		//prevent empty searches
		if (value.empty()) return 0;
		
		const auto it = find_if(
			keyValues.begin(), keyValues.end(),
			[&](const KeyValue& kv) { return kv.value == value; });
			
		return (
			it != keyValues.end()
			&& it->key != 0)
			? it->key
			: u32{};
	}
	
	inline u32 GetUTFByKey(u32 key)
	{
		FillKeyValues();
		
		//prevent empty searches
		if (key == 0) return 0x003F;
		
		const auto it = lower_bound(
			keyValues.begin(), keyValues.end(), key,
			[](const KeyValue& kv, u32 k) { return kv.key < k; });
			
		return (
			it != keyValues.end()
			&& it->key == key
			&& it->utf != 0)
			? it->utf
			: 0x003F; //returns ? as fallback
	}
	inline u32 GetUTFByValue(const string_view& value)
	{
		FillKeyValues();
		
		//prevent empty searches
		if (value.empty()) return 0x003F;
		
		const auto it = find_if(
			keyValues.begin(), keyValues.end(),
			[&](const KeyValue& kv) { return kv.value == value; });
			
		return (
			it != keyValues.end()
			&& it->utf != 0)
			? it->utf
			: 0x003F; //returns ? as fallback
	}
	
	inline string_view GetValueByKey(u32 key)
	{
		FillKeyValues();
	
		//prevent empty searches
		if (key == 0) return "?";
	
		const auto it = lower_bound(
			keyValues.begin(), keyValues.end(), key,
			[](const KeyValue& kv, u32 k) { return kv.key < k; });
			
		return (
			it != keyValues.end()
			&& it->key == key)
			? it->value
			: "?";
	}
	inline string_view GetValueByUTF(u32 utf)
	{
		FillKeyValues();
		
		//prevent empty searches
		if (utf == 0) return "?";
		
		const auto it = find_if(
			keyValues.begin(), keyValues.end(),
			[&](const KeyValue& kv) { return kv.utf == utf; });
			
		return (
			it != keyValues.end()
			&& it->utf == utf)
			? it->value
			: "?";
	}
}