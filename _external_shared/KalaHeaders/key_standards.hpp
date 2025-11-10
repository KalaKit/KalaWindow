//------------------------------------------------------------------------------
// key_standards.hpp
//
// Copyright (C) 2025 Lost Empire Entertainment
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

namespace KalaHeaders
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
	
	enum class MouseActions : u32
	{
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
	
	inline constexpr u32 MA(MouseActions m) { return static_cast<u32>(m); }
	
	//Common mouse actions. 15-30 is reserved for future use.
	inline constexpr array<KeyValue, 15> mouse_actions
	{{
		{MA(MouseActions::M_LEFT),   0, "Left"},
		{MA(MouseActions::M_RIGHT),  0, "Right"}, //scrollwheel click
		{MA(MouseActions::M_MIDDLE), 0, "Middle"},
		
		{MA(MouseActions::M_X1), 0, "X1"}, //left side button
		{MA(MouseActions::M_X2), 0, "X2"}, //right side button
		
		{MA(MouseActions::M_WHEEL_UP),    0, "WheelUp"},    //scroll up
		{MA(MouseActions::M_WHEEL_DOWN),  0, "WheelDown"},  //scroll down
		{MA(MouseActions::M_WHEEL_LEFT),  0, "WheelLeft"},  //scrollwheel left click
		{MA(MouseActions::M_WHEEL_RIGHT), 0, "WheelRight"}, //scrollwheel right click
		
		{MA(MouseActions::M_LEFT_DOUBLE),   0, "LeftDouble"},
		{MA(MouseActions::M_RIGHT_DOUBLE),  0, "RightDouble"},
		{MA(MouseActions::M_MIDDLE_DOUBLE), 0, "MiddleDouble"},
		{MA(MouseActions::M_X1_DOUBLE),     0, "X1Double"},
		{MA(MouseActions::M_X2_DOUBLE),     0, "X2Double"}
	}};
	
	enum class GamepadActions : u32
	{
		GP_A = 31,
		GP_B = 32,
		GP_X = 33,
		GP_Y = 34,
		
		GP_L1 = 35,
		GP_R1 = 36,
		GP_L2 = 37,
		GP_R2 = 38,
		GP_L3 = 39,
		GP_R3 = 40,
		
		GP_DPAD_UP    = 41,
		GP_DPAD_DOWN  = 42,
		GP_DPAD_LEFT  = 43,
		GP_DPAD_RIGHT = 44,
		
		GP_START    = 45,
		GP_SELECT   = 46,
		GP_HOME     = 47,
		GP_CAPTURE  = 48,
		GP_TOUCHPAD = 49
	};
	
	inline constexpr u32 GA(GamepadActions m) { return static_cast<u32>(m); }
	
	//Common gamepad/controller/switch-like actions. 50-60 is reserved for future use.
	inline constexpr array<KeyValue, 19> gamepad_actions
	{{
		{GA(GamepadActions::GP_A), 0, "A"}, 
		{GA(GamepadActions::GP_B), 0, "B"}, 
		{GA(GamepadActions::GP_X), 0, "X"}, 
		{GA(GamepadActions::GP_Y), 0, "Y"},
		
		{GA(GamepadActions::GP_L1), 0, "L1"}, 
		{GA(GamepadActions::GP_R1), 0, "R1"},
		{GA(GamepadActions::GP_L2), 0, "L2"}, 
		{GA(GamepadActions::GP_R2), 0, "R2"},
		{GA(GamepadActions::GP_L3), 0, "L3"}, 
		{GA(GamepadActions::GP_R3), 0, "R3"},
		
		{GA(GamepadActions::GP_DPAD_UP),    0, "DPad Up"}, 
		{GA(GamepadActions::GP_DPAD_DOWN),  0, "DPad Down"}, 
		{GA(GamepadActions::GP_DPAD_LEFT),  0, "DPad Left"}, 
		{GA(GamepadActions::GP_DPAD_RIGHT), 0, "DPad Right"},
		
		{GA(GamepadActions::GP_START),    0, "Start"}, 
		{GA(GamepadActions::GP_SELECT),   0, "Select"}, 
		{GA(GamepadActions::GP_HOME),     0, "Home"}, 
		{GA(GamepadActions::GP_CAPTURE),  0, "Capture"}, 
		{GA(GamepadActions::GP_TOUCHPAD), 0, "Touchpad"}
	}};
	
	enum class KeyboardActions : u32
	{
		KB_NUM_0 = 61, KB_NUM_1 = 62, KB_NUM_2 = 63, KB_NUM_3 = 64, KB_NUM_4 = 65,
		KB_NUM_5 = 66, KB_NUM_6 = 67, KB_NUM_7 = 68, KB_NUM_8 = 69, KB_NUM_9 = 70,
		
		KB_NUM_LOCK = 71, KB_NUM_RETURN = 72, 
		KB_NUM_DIVIDE = 73, KB_NUM_MULTIPLY = 74,
		KB_NUM_ADD = 75, KB_NUM_SUBTRACT = 76,
		
		KB_0 = 77, KB_1 = 78, KB_2 = 79, KB_3 = 80, KB_4 = 81,
		KB_5 = 82, KB_6 = 83, KB_7 = 84, KB_8 = 85, KB_9 = 86,
		
		KB_F1 = 87, KB_F2 = 88, KB_F3 = 89, KB_F4 = 90, 
		KB_F5 = 91, KB_F6 = 92, KB_F7 = 93, KB_F8 = 94, 
		KB_F9 = 95, KB_F10 = 96, KB_F11 = 97, KB_F12 = 98, 
		
		KB_RETURN = 99, KB_BACKSPACE = 100, KB_SPACE = 101,
		KB_ESC = 102, KB_TAB = 103, KB_CAPS_LOCK = 104,
		
		KB_LEFT_SHIFT = 105, KB_LEFT_CTRL = 106, KB_LEFT_ALT = 107, 
		KB_RIGHT_SHIFT = 108, KB_RIGHT_CTRL = 109, KB_RIGHT_ALT = 110, 
		
		KB_PRINT_SCREEN = 111, KB_SCROLL_LOCK = 112, KB_PAUSE = 113, 
		KB_INSERT = 114, KB_DELETE = 115,
		KB_HOME = 116, KB_END = 117, 
		KB_PAGE_UP = 118, KB_PAGE_DOWN = 119, 
		
		KB_ARROW_UP = 120, KB_ARROW_DOWN = 121, 
		KB_ARROW_LEFT = 122, KB_ARROW_RIGHT = 123, 
		
		KB_ALTERNATE_GRAPHIC = 124, 
		KB_FUNCTION = 125, 
		KB_SYSTEM = 126, 
		KB_MENU = 127, 
	};
	
	inline constexpr u32 KA(KeyboardActions m) { return static_cast<u32>(m); }
	
	//Common keyboard actions. 128-140 is reserved.
	inline constexpr array<KeyValue, 67> keyboard_actions
	{{
		{KA(KeyboardActions::KB_NUM_0), 0, "Num 0"}, 
		{KA(KeyboardActions::KB_NUM_1), 0, "Num 1"}, 
		{KA(KeyboardActions::KB_NUM_2), 0, "Num 2"}, 
		{KA(KeyboardActions::KB_NUM_3), 0, "Num 3"}, 
		{KA(KeyboardActions::KB_NUM_4), 0, "Num 4"},
		{KA(KeyboardActions::KB_NUM_5), 0, "Num 5"}, 
		{KA(KeyboardActions::KB_NUM_6), 0, "Num 6"}, 
		{KA(KeyboardActions::KB_NUM_7), 0, "Num 7"}, 
		{KA(KeyboardActions::KB_NUM_8), 0, "Num 8"}, 
		{KA(KeyboardActions::KB_NUM_9), 0, "Num 9"}, 
		
		{KA(KeyboardActions::KB_NUM_LOCK),     0, "Num lock"}, 
		{KA(KeyboardActions::KB_NUM_RETURN),   0, "Num return"}, 
		{KA(KeyboardActions::KB_NUM_DIVIDE),   0, "Num /"}, 
		{KA(KeyboardActions::KB_NUM_MULTIPLY), 0, "Num *"}, 
		{KA(KeyboardActions::KB_NUM_ADD),      0, "Num +"}, 
		{KA(KeyboardActions::KB_NUM_SUBTRACT), 0, "Num -"},
		
		{KA(KeyboardActions::KB_0), 0, "0"}, 
		{KA(KeyboardActions::KB_1), 0, "1"}, 
		{KA(KeyboardActions::KB_2), 0, "2"}, 
		{KA(KeyboardActions::KB_3), 0, "3"}, 
		{KA(KeyboardActions::KB_4), 0, "4"},
		{KA(KeyboardActions::KB_5), 0, "5"}, 
		{KA(KeyboardActions::KB_6), 0, "6"}, 
		{KA(KeyboardActions::KB_7), 0, "7"}, 
		{KA(KeyboardActions::KB_8), 0, "8"}, 
		{KA(KeyboardActions::KB_9), 0, "9"},
		
		{KA(KeyboardActions::KB_F1), 0, "F1"}, 
		{KA(KeyboardActions::KB_F2), 0, "F2"}, 
		{KA(KeyboardActions::KB_F3), 0, "F3"}, 
		{KA(KeyboardActions::KB_F4), 0, "F4"}, 
		{KA(KeyboardActions::KB_F5), 0, "F5"}, 
		{KA(KeyboardActions::KB_F6), 0, "F6"}, 
		{KA(KeyboardActions::KB_F7), 0, "F7"}, 
		{KA(KeyboardActions::KB_F8), 0, "F8"},
		{KA(KeyboardActions::KB_F9), 0, "F9"}, 
		{KA(KeyboardActions::KB_F10), 0, "F10"}, 
		{KA(KeyboardActions::KB_F11), 0, "F11"}, 
		{KA(KeyboardActions::KB_F12), 0, "F12"},
		
		{KA(KeyboardActions::KB_RETURN),    0, "Return"}, 
		{KA(KeyboardActions::KB_BACKSPACE), 0, "Backspace"}, 
		{KA(KeyboardActions::KB_SPACE),     0, "Space"},
		{KA(KeyboardActions::KB_ESC),       0, "Esc"}, 
		{KA(KeyboardActions::KB_TAB),       0, "Tab"}, 
		{KA(KeyboardActions::KB_CAPS_LOCK), 0, "Caps lock"},
		
		{KA(KeyboardActions::KB_LEFT_SHIFT),  0, "Left shift"}, 
		{KA(KeyboardActions::KB_LEFT_CTRL),   0, "Left control"}, 
		{KA(KeyboardActions::KB_LEFT_ALT),    0, "Left alt"},
		{KA(KeyboardActions::KB_RIGHT_SHIFT), 0, "Right shift"}, 
		{KA(KeyboardActions::KB_RIGHT_CTRL),  0, "Right control"}, 
		{KA(KeyboardActions::KB_RIGHT_ALT),   0, "Right alt"},
		
		{KA(KeyboardActions::KB_PRINT_SCREEN), 0, "Print screen"}, 
		{KA(KeyboardActions::KB_SCROLL_LOCK),  0, "Scroll lock"}, 
		{KA(KeyboardActions::KB_PAUSE),        0, "Pause"},
		{KA(KeyboardActions::KB_INSERT),       0, "Insert"}, 
		{KA(KeyboardActions::KB_DELETE),       0, "Delete"},
		{KA(KeyboardActions::KB_HOME),         0, "Home"}, 
		{KA(KeyboardActions::KB_END),          0, "End"},
		{KA(KeyboardActions::KB_PAGE_UP),      0, "Page up"}, 
		{KA(KeyboardActions::KB_PAGE_DOWN),    0, "Page down"},
		
		{KA(KeyboardActions::KB_ARROW_UP),    0, "Arrow up"}, 
		{KA(KeyboardActions::KB_ARROW_DOWN),  0, "Arrow down"}, 
		{KA(KeyboardActions::KB_ARROW_LEFT),  0, "Arrow left"}, 
		{KA(KeyboardActions::KB_ARROW_RIGHT), 0, "Arrow right"},
		
		{KA(KeyboardActions::KB_ALTERNATE_GRAPHIC), 0, "Alternate graphic"}, 
		{KA(KeyboardActions::KB_FUNCTION),          0, "Function"}, 
		{KA(KeyboardActions::KB_SYSTEM),            0, "System"}, 
		{KA(KeyboardActions::KB_MENU),              0, "Menu"}
	}};
	
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
	
	inline constexpr array<KeyValue, 219> emojis
	{{
		//mood: joy / cheer
		
		{1001, 0x1F600, "grinning_face"},
		{1002, 0x1F603, "grinning_face_with_big_eyes"},
		{1003, 0x1F604, "grinning_face_with_smiling_eyes"},
		{1004, 0x1F601, "beaming_face_with_smiling_eyes"},
		{1005, 0x1F606, "grinning_squinting_face"},
		{1006, 0x1F605, "grinning_face_with_sweat"},
		{1007, 0x1F602, "face_with_tears_of_joy"},
		{1008, 0x1F923, "rolling_on_the_floor_laughing"},
		{1009, 0x1F60A, "smiling_face_with_smiling_eyes"},
		{1010, 0x1F642, "slightly_smiling_face"},
		{1011, 0x1F609, "winking_face"},
		{1012, 0x1F607, "smiling_face_with_halo"},
		{1013, 0x1F970, "smiling_face_with_hearts"},
		{1014, 0x1F917, "hugging_face"},

		//mood: affection / warmth
		
		{1015, 0x1F60D, "smiling_face_with_heart_eyes"},
		{1016, 0x1F618, "face_blowing_a_kiss"},
		{1017, 0x1F617, "kissing_face"},
		{1018, 0x1F619, "kissing_face_with_smiling_eyes"},
		{1019, 0x1F61A, "kissing_face_with_closed_eyes"},
		{1020, 0x1F60B, "face_savoring_food"},

		//mood: playful / mischief
		
		{1021, 0x1F61C, "winking_face_with_tongue"},
		{1022, 0x1F61D, "squinting_face_with_tongue"},
		{1023, 0x1F61B, "face_with_tongue"},
		{1024, 0x1F92A, "zany_face"},
		{1025, 0x1F60F, "smirking_face"},
		{1026, 0x1F608, "smiling_face_with_horns"},
		{1027, 0x1F92B, "shushing_face"},
		{1028, 0x1F92D, "face_with_hand_over_mouth"},

		//mood: thoughtful / neutral
		
		{1029, 0x1F914, "thinking_face"},
		{1030, 0x1F928, "face_with_raised_eyebrow"},
		{1031, 0x1F610, "neutral_face"},
		{1032, 0x1F611, "expressionless_face"},
		{1033, 0x1F636, "face_without_mouth"},
		{1034, 0x1FAE1, "saluting_face"},
		{1035, 0x1FAE3, "peeking_face"},

		//mood: sadness / exhaustion
		
		{1036, 0x1F62D, "loudly_crying_face"},
		{1037, 0x1F97A, "pleading_face"},
		{1038, 0x1F614, "pensive_face"},
		{1039, 0x1F62A, "sleepy_face"},
		{1040, 0x1F634, "sleeping_face"},
		{1041, 0x1F60C, "relieved_face"},

		//mood: anger / frustration
		
		{1042, 0x1F620, "angry_face"},
		{1043, 0x1F621, "pouting_face"},
		{1044, 0x1F92C, "swearing_face"},
		{1045, 0x1F62C, "grimacing_face"},
		{1046, 0x1F624, "face_with_steam"},
		{1047, 0x1F47F, "angry_face_with_horns"},

		//mood: shock / confusion
		
		{1048, 0x1F633, "flushed_face"},
		{1049, 0x1FAE2, "gasping_face"},
		{1050, 0x1F615, "confused_face"},
		{1051, 0x1F92F, "exploding_head"},
		{1052, 0x1F974, "woozy_face"},
		{1053, 0x1F925, "lying_face"},
		{1054, 0x1F635, "dizzy_face"},
		{1055, 0x1F644, "face_with_rolling_eyes"},

		//mood: illness / discomfort
		
		{1056, 0x1F637, "face_with_medical_mask"},
		{1057, 0x1F912, "face_with_thermometer"},
		{1058, 0x1F915, "face_with_head_bandage"},
		{1059, 0x1F922, "nauseated_face"},
		{1060, 0x1F92E, "face_vomiting"},
		{1061, 0x1F927, "sneezing_face"},
		
		
		//people and gestures
		
		{1062, 0x1F645, "person_no_gesture"},
		{1063, 0x1F646, "person_ok_gesture"},
		{1064, 0x1F64B, "person_raising_hand"},
		{1065, 0x1F921, "clown_face"},
		{1066, 0x1F47B, "ghost"},
		{1067, 0x1F47D, "alien"},
		{1068, 0x1F49A, "green_heart"},
		{1069, 0x2764, "red_heart"},
		{1070, 0x1F499, "blue_heart"},
		{1071, 0x1F480, "skull"},
		{1072, 0x2620, "skull_and_crossbones"},
		{1073, 0x1F4A9, "poop"},
		{1074, 0x1F44B, "waving_hand"},
		{1075, 0x1F91A, "raised_back_of_hand"},
		{1076, 0x1F590, "hand_with_fingers_splayed"},
		{1077, 0x270B, "raised_hand"},
		{1078, 0x1F596, "vulcan_salute"},
		{1079, 0x1F44A, "fist_bump"},
		{1080, 0x270A, "raised_fist"},
		{1081, 0x1F91D, "handshake"},
		{1082, 0x1FAF6, "heart_hands"},
		{1083, 0x1F4AA, "flexed_biceps"},
		{1084, 0x1F44C, "ok_hand"},
		{1085, 0x1F90C, "pinched_fingers"},
		{1086, 0x1F90F, "pinching_hand"},
		{1087, 0x270C, "victory_hand"},
		{1088, 0x1F91E, "crossed_fingers"},
		{1089, 0x1F91F, "rock_on_hand"},
		{1090, 0x1F918, "sign_of_the_horns"},
		{1091, 0x1F919, "call_me_hand"},
		{1092, 0x1F44D, "thumbs_up"},
		{1093, 0x1F44E, "thumbs_down"},
		{1094, 0x1F44F, "clapping_hands"},
		{1095, 0x1F64C, "raising_hands"},
		{1096, 0x1F450, "open_hands"},
		{1097, 0x1F932, "praying_hands"},
		{1098, 0x1F64F, "folded_hands"},
		
		//animals and nature
		
		{1099, 0x1F436, "dog_face"},
		{1100, 0x1F431, "cat_face"},
		{1101, 0x1F98A, "fox_face"},
		{1102, 0x1F43B, "bear_face"},
		{1103, 0x1F43C, "panda_face"},
		{1104, 0x1F981, "lion_face"},
		{1105, 0x1F42F, "tiger_face"},
		{1106, 0x1F438, "frog_face"},
		{1107, 0x1F435, "monkey_face"},
		{1108, 0x1F648, "see_no_evil_monkey"},
		{1109, 0x1F649, "hear_no_evil_monkey"},
		{1110, 0x1F64A, "speak_no_evil_monkey"},
		{1111, 0x1F427, "penguin"},
		{1112, 0x1F426, "bird"},
		{1113, 0x1F986, "duck"},
		{1114, 0x1F40D, "snake"},
		{1115, 0x1F98B, "butterfly"},
		
		//nature
		
		{1116, 0x1F338, "cherry_blossom"},
		{1117, 0x1F341, "maple_leaf"},
		{1118, 0x1F342, "fallen_leaves"},
		{1119, 0x1F339, "rose"},
		{1120, 0x1F33C, "daisy"},
		{1121, 0x1F940, "withered_rose"},
		{1122, 0x1F33B, "sunflower"},
		{1123, 0x1F335, "cactus"},
		{1124, 0x1F332, "evergreen_tree"},
		
		//weather
		
		{1125, 0x1F315, "full_moon"},
		{1126, 0x1F319, "crescent_moon"},
		{1127, 0x1F31E, "sun_with_face"},
		{1128, 0x2600, "sun"},
		{1129, 0x1F311, "new_moon"},
		{1130, 0x1F308, "rainbow"},
		{1131, 0x1F4A8, "wind"},
		{1132, 0x1F32A, "tornado"},
		{1133, 0x2601, "cloud"},
		{1134, 0x1F327, "cloud_with_rain"},
		{1135, 0x26C8, "thunderstorm_cloud"},
		{1136, 0x1F4A7, "droplet"},
		{1137, 0x1F4A6, "sweat_droplets"},
		
		//audio
		
		{1138, 0x1F3A4, "microphone"},
		{1139, 0x1F399, "studio_microphone"},
		{1140, 0x1F508, "speaker_low"},
		{1141, 0x1F509, "speaker_medium"},
		{1142, 0x1F50A, "speaker_high"},
		{1143, 0x1F507, "speaker_mute"},
		{1144, 0x1F3A7, "headphones"},
		{1145, 0x1F3B5, "musical_note"},
		{1146, 0x1F3B6, "musical_notes"},
		
		//money
		
		{1147, 0x1F4B0, "money_bag"},
		{1148, 0x1F4B8, "money_with_wings"},
		{1149, 0x1F4B5, "dollar_banknote"},
		{1150, 0x1F4B3, "credit_card"},
		{1151, 0x1FA99, "coin"},
		{1152, 0x1F48E, "gem"},
		{1153, 0x1F9FE, "receipt"},
		
		//writing and reading
		
		{1154, 0x1F4D6, "open_book"},
		{1155, 0x1F4D5, "red_book"},
		{1156, 0x1F4D7, "green_book"},
		{1157, 0x1F4D8, "blue_book"},
		{1158, 0x1F4D9, "orange_book"},
		{1159, 0x1F4DD, "memo"},
		{1160, 0x1F58A, "pen"},
		{1161, 0x270F, "pencil"},
		{1162, 0x1F58B, "fountain_pen"},
		{1163, 0x1F4DC, "scroll"},
		
		//home
		
		{1164, 0x1F3E0, "house"},
		{1165, 0x1F3E1, "house_with_garden"},
		{1166, 0x1F9F1, "brick"},
		{1167, 0x1FA9E, "mirror"},
		{1168, 0x1FA9F, "window"},
		{1169, 0x1F3FA, "vase"},
		{1170, 0x1F56F, "candle"},
		{1171, 0x1F512, "locked"},
		{1172, 0x1F513, "unlocked"},
		{1173, 0x1F511, "key"},
		{1174, 0x1F5DD, "old_key"},
		{1175, 0x1F9E9, "puzzle_piece"},
		{1176, 0x1F4E6, "box"},
		{1177, 0x1F5C2, "file_folder"},
		
		//science and technology
		
		{1178, 0x1F4A1, "light_bulb"},
		{1179, 0x1F4BB, "laptop"},
		{1180, 0x1F5A5, "desktop_computer"},
		{1181, 0x1F4F8, "camera"},
		{1182, 0x2328, "keyboard"},
		{1183, 0x1F5B1, "computer_mouse"},
		{1184, 0x1F4F1, "mobile_phone"},
		{1185, 0x1F50B, "battery"},
		{1186, 0x1FAAB, "low_battery"},
		{1187, 0x1F3AE, "video_game"},
		{1188, 0x1F579, "joystick"},
		{1189, 0x231B, "hourglass"},
		{1190, 0x23F3, "hourglass_done"},
		{1191, 0x23F0, "alarm_clock"},
		{1192, 0x1F570, "mantelpiece_clock"},
		{1193, 0x23F1, "stopwatch"},
		{1194, 0x23F2, "timer_clock"},
		{1195, 0x1F9ED, "compass"},
		{1196, 0x2696, "balance_scale"},
		{1197, 0x1F9F2, "magnet"},
		{1198, 0x2697, "alembic"},
		{1199, 0x1F9EC, "dna"},
		{1200, 0x1F9EA, "test_tube"},
		{1201, 0x1F9E0, "brain"},
		{1202, 0x2699, "gear"},
		{1203, 0x1F9F0, "toolbox"},
		
		//misc
		
		{1204, 0x1F4AD, "thought_cloud"},
		{1205, 0x1F4AC, "speech_balloon"},
		{1206, 0x1F5EF, "anger_bubble"},
		{1207, 0x26A1, "high_voltage"},
		{1208, 0x1F525, "fire"},
		{1209, 0x1FAA8, "rock"},
		{1210, 0x1FA84, "magic_wand"},
		{1211, 0x1FAB6, "feather"},
		{1212, 0x1FAA9, "mirror_ball"},
		{1213, 0x1F386, "fireworks"},
		{1214, 0x1F4A5, "explosion"},
		{1215, 0x26B0, "coffin"},
		{1216, 0x1F573, "hole"},
		{1217, 0x2694, "crossed_swords"},
		{1218, 0x1F6E1, "shield"},
		{1219, 0x1FA93, "axe"}
	}};
	
	//
	// KAOMOJIS
	//
	
	inline constexpr array<KeyValue, 200> kaomojis
	{{
		//mood: joy / cheer

		{1501, 0x003F, "(＾▽＾)"},
		{1502, 0x003F, "(≧◡≦)"},
		{1503, 0x003F, "(*^‿^*)"},
		{1504, 0x003F, "(￣▽￣)ノ"},
		{1505, 0x003F, "(✿◠‿◠)"},
		{1506, 0x003F, "(^‿^)"},
		{1507, 0x003F, "(๑˃ᴗ˂)ﻭ"},
		{1508, 0x003F, "(⁀ᗢ⁀)"},
		{1509, 0x003F, "(ﾉ◕ヮ◕)ﾉ*:･ﾟ✧"},
		{1510, 0x003F, "(づ｡◕‿‿◕｡)づ"},
		{1511, 0x003F, "ヽ(・∀・)ﾉ"},
		{1512, 0x003F, "＼(^o^)／"},
		{1513, 0x003F, "(⌒▽⌒)☆"},
		{1514, 0x003F, "(o^▽^o)"},
		{1515, 0x003F, "(☆▽☆)"},

		//mood: affection / warmth

		{1516, 0x003F, "(´▽`ʃ♡ƪ)"},
		{1517, 0x003F, "(♡‿♡)"},
		{1518, 0x003F, "(ღ˘⌣˘ღ)"},
		{1519, 0x003F, "(づ￣ ³￣)づ"},
		{1520, 0x003F, "(´ε｀ )♡"},
		{1521, 0x003F, "(´｡• ᵕ •｡`)"},
		{1522, 0x003F, "(⁄ ⁄•⁄ω⁄•⁄ ⁄)⁄"},
		{1523, 0x003F, "(•ө•)♡"},
		{1524, 0x003F, "(⌒‿⌒)"},
		{1525, 0x003F, "(´▽`ʃ♡ƪ)"},
		{1526, 0x003F, "(♥ω♥*)"},
		{1527, 0x003F, "(っ˘з(˘⌣˘ )"},
		{1528, 0x003F, "(⁎⁍̴̆Ɛ⁍̴̆⁎)"},
		{1529, 0x003F, "(〃￣ω￣〃ゞ)"},
		{1530, 0x003F, "(˘ ³˘)♥"},

		//mood: playful / mischief

		{1531, 0x003F, "(¬‿¬)"},
		{1532, 0x003F, "(͡° ͜ʖ ͡°)"},
		{1533, 0x003F, "(ʘ‿ʘ)"},
		{1534, 0x003F, "(☞ﾟヮﾟ)☞"},
		{1535, 0x003F, "(⌐■_■)"},
		{1536, 0x003F, "(¬‿¬ )"},
		{1537, 0x003F, "(•̀ᴗ•́)و ̑̑"},
		{1538, 0x003F, "(ง'̀-'́)ง"},
		{1539, 0x003F, "(ง •̀_•́)ง"},
		{1540, 0x003F, "(•_•) ( •_•)>⌐■-■"},
		{1541, 0x003F, "ヘ(￣ω￣ヘ)"},
		{1542, 0x003F, "(☞⌐■_■)☞"},
		{1543, 0x003F, "(＾ω＾)"},
		{1544, 0x003F, "(っಠ‿ಠ)っ"},
		{1545, 0x003F, "(๑˃̵ᴗ˂̵)"},

		//mood: sadness / exhaustion

		{1546, 0x003F, "(╯︵╰,)"},
		{1547, 0x003F, "(｡•́︿•̀｡)"},
		{1548, 0x003F, "(ಥ﹏ಥ)"},
		{1549, 0x003F, "(╥﹏╥)"},
		{1550, 0x003F, "(T_T)"},
		{1551, 0x003F, "(つ﹏⊂)"},
		{1552, 0x003F, "(´；ω；`)"},
		{1553, 0x003F, "(>_<)"},
		{1554, 0x003F, "(︶︹︺)"},
		{1555, 0x003F, "(´-﹏-`；)"},
		{1556, 0x003F, "(ᗒᗣᗕ)՞"},
		{1557, 0x003F, "(｡╯︵╰｡)"},
		{1558, 0x003F, "(つд⊂)"},
		{1559, 0x003F, "(；へ：)"},
		{1560, 0x003F, "(ノ_<。)"},

		//mood: anger / frustration

		{1561, 0x003F, "(｀皿´＃)"},
		{1562, 0x003F, "(ノಠ益ಠ)ノ彡┻━┻"},
		{1563, 0x003F, "(╬ಠ益ಠ)"},
		{1564, 0x003F, "(ง'̀-'́)ง"},
		{1565, 0x003F, "(╯°□°）╯︵ ┻━┻"},
		{1566, 0x003F, "(＞﹏＜)"},
		{1567, 0x003F, "(҂⌣̀_⌣́)"},
		{1568, 0x003F, "(ノ｀Д´)ノ"},
		{1569, 0x003F, "(≧ヘ≦ )"},
		{1570, 0x003F, "(•̀o•́)ง"},
		{1571, 0x003F, "(ಠ_ಠ)"},
		{1572, 0x003F, "(¬_¬)"},
		{1573, 0x003F, "(ノω<。)ノ))"},
		{1574, 0x003F, "(≧△≦)"},
		{1575, 0x003F, "(╬ Ò﹏Ó)"},

		//mood: shock / confusion

		{1576, 0x003F, "(⊙_☉)"},
		{1577, 0x003F, "(°ロ°)"},
		{1578, 0x003F, "(⊙﹏⊙✿)"},
		{1579, 0x003F, "(・_・ヾ"},
		{1580, 0x003F, "(゜ロ゜)"},
		{1581, 0x003F, "(O_O;)"},
		{1582, 0x003F, "(๑•﹏•)"},
		{1583, 0x003F, "(⁀ᗢ⁀)"},
		{1584, 0x003F, "( ﾟoﾟ)"},
		{1585, 0x003F, "(⊙_◎)"},
		{1586, 0x003F, "(°□°)"},
		{1587, 0x003F, "(⊙_⊙)"},
		{1588, 0x003F, "(O.O)"},
		{1589, 0x003F, "(๑•᎑•๑)"},
		{1590, 0x003F, "(°ー°〃)"},

		//mood: sleepy / tired

		{1591, 0x003F, "(－_－) zzZ"},
		{1592, 0x003F, "(￣o￣) zzZ"},
		{1593, 0x003F, "(≚ᄌ≚)ƶƶƶ"},
		{1594, 0x003F, "(-_-)zzz"},
		{1595, 0x003F, "(￣ρ￣)..zzZZ"},
		{1596, 0x003F, "(=_=)"},
		{1597, 0x003F, "(´～`)"},
		{1598, 0x003F, "(－－)zzz"},
		{1599, 0x003F, "(∪｡∪)｡｡｡zzZ"},
		{1600, 0x003F, "(－ω－)／zzZ"},
		
		//mood: shrug / unsure
		
		{1601, 0x003F, "¯\\_(._.)_/¯"},
		{1602, 0x003F, "┐( -_-)┌"},
		{1603, 0x003F, "\\_(o_o)_/"},
		{1604, 0x003F, "┐(._.)┌"},
		{1605, 0x003F, "(?_?)"},
		{1606, 0x003F, "(o_O)?"},
		{1607, 0x003F, "(-_-)?"},
		{1608, 0x003F, "(._.)?"},
		{1609, 0x003F, "(O_o)?"},
		{1610, 0x003F, "(>_>)?"},

		//mood: embarrassment / awkward
		
		{1611, 0x003F, "(^_^;)"},
		{1612, 0x003F, "(*^_^*)"},
		{1613, 0x003F, "(>_<)"},
		{1614, 0x003F, "(//_//)"},
		{1615, 0x003F, "(*_*)"},
		{1616, 0x003F, "(-_-;)"},
		{1617, 0x003F, "(^_^')"},
		{1618, 0x003F, "(^///^)"},
		{1619, 0x003F, "(^_~)"},
		{1620, 0x003F, "(x_x;)"},
		
		//mood: confidence / cool
		
		{1621, 0x003F, "(^_^)b"},
		{1622, 0x003F, "(>_^)"},
		{1623, 0x003F, "(^_-)"},
		{1624, 0x003F, "(^_^)v"},
		{1625, 0x003F, "(^_^)7"},
		{1626, 0x003F, "(^_^)y-~"},
		{1627, 0x003F, "(-_^)b"},
		{1628, 0x003F, "(^o^)b"},
		{1629, 0x003F, "(^_^)/"},
		{1630, 0x003F, "(>_^)b"},

		//mood: celebration / excitement
		
		{1631, 0x003F, "\\(^o^)/"},
		{1632, 0x003F, "(^O^)／"},
		{1633, 0x003F, "\\(^^)/"},
		{1634, 0x003F, "\\(^-^)/"},
		{1635, 0x003F, "\\(>o<)/"},
		{1636, 0x003F, "\\(^▽^)/"},
		{1637, 0x003F, "(*^o^*)"},
		{1638, 0x003F, "(^0^)/"},
		{1639, 0x003F, "(^_^)/~"},
		{1640, 0x003F, "(^O^)/☆"},
		
		//mood: determination / strength
		
		{1641, 0x003F, "(>_<)9"},
		{1642, 0x003F, "(^o^)9"},
		{1643, 0x003F, "(^_^)9"},
		{1644, 0x003F, "(>_^)9"},
		{1645, 0x003F, "(^o^)/9"},
		{1646, 0x003F, "(o^-^)9"},
		{1647, 0x003F, "(^_^)/9"},
		{1648, 0x003F, "(^_^)v"},
		{1649, 0x003F, "(o^-^)b"},
		{1650, 0x003F, "(>_<)!!"},

		//mood: surprise / shock
		
		{1651, 0x003F, "(O_O)"},
		{1652, 0x003F, "(0_0)"},
		{1653, 0x003F, "(o_O)"},
		{1654, 0x003F, "(O_o)"},
		{1655, 0x003F, "(0_o)"},
		{1656, 0x003F, "(;O;)"},
		{1657, 0x003F, "(O_O;)"},
		{1658, 0x003F, "(o_O;)"}, 
		{1659, 0x003F, "(0_0;)"}, 
		{1660, 0x003F, "(O_O')"}, 
		
		//mood: sleepy / tired
		
		{1661, 0x003F, "(-_-)zzz"},
		{1662, 0x003F, "(=_=)"},
		{1663, 0x003F, "(u_u)"},
		{1664, 0x003F, "(-o-)"},
		{1665, 0x003F, "(-_-)/~~~"},
		{1666, 0x003F, "(^_^)zzz"},
		{1667, 0x003F, "(o_o)zzZ"},
		{1668, 0x003F, "(=_=)y-~"},
		{1669, 0x003F, "(Z_z)"},
		{1670, 0x003F, "(=_=)..."},

		//mood: thinking / neutral
		
		{1671, 0x003F, "(._.)"},
		{1672, 0x003F, "(^_^?)"},
		{1673, 0x003F, "(-_-)"},
		{1674, 0x003F, "(._. )"},
		{1675, 0x003F, "(>_>)"},
		{1676, 0x003F, "(<_<)"},
		{1677, 0x003F, "(^_^;)"},
		{1678, 0x003F, "(-_-)?"}, 
		{1679, 0x003F, "(._. )?"}, 
		{1680, 0x003F, "(?_?)"},

		//mood: special / misc
		
		{1681, 0x003F, "(>^_^)>"},
		{1682, 0x003F, "<(^_^<)"},
		{1683, 0x003F, "(^_^)/"},
		{1684, 0x003F, "(^o^)/"},
		{1685, 0x003F, "(^.^)/"},
		{1686, 0x003F, "(/^-^)/"},
		{1687, 0x003F, "(~_^)/"},
		{1688, 0x003F, "(^_^)/~"},
		{1689, 0x003F, "(^-^*)/"},
		{1690, 0x003F, "\\(^.^\\)"},
		
		//mood: celebration / party
		
		{1691, 0x003F, "\\(^o^)/Yay!"},
		{1692, 0x003F, "\\(^_^)/*"},
		{1693, 0x003F, "(^_^)v♪"},
		{1694, 0x003F, "(^_^)y"},
		{1695, 0x003F, "(^O^)b"},
		{1696, 0x003F, "\\(^.^)/"},
		{1697, 0x003F, "(^-^)/♪"},
		{1698, 0x003F, "(^_^)ノ"},
		{1699, 0x003F, "(o^-^)b"},
		{1700, 0x003F, "\\(^_^)/Cheers!"}
	}};

	//
	// GET KEY, UTF OR VALUE
	//
	
	inline vector<KeyValue> keyValues{};
	
	inline void FillKeyValues()
	{
		if (!keyValues.empty()) return;
		
		keyValues.reserve(
			mouse_actions.size()
			+ gamepad_actions.size()
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
				
		keyValues.insert(keyValues.end(), mouse_actions.begin(), mouse_actions.end());
		keyValues.insert(keyValues.end(), gamepad_actions.begin(), gamepad_actions.end());
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
	
	inline string_view GetValueByKey(u32 key)
	{
		FillKeyValues();
		
		const auto it = lower_bound(
			keyValues.begin(), keyValues.end(), key,
			[](const KeyValue& kv, u32 k) { return kv.key < k; });
			
		return (
			it != keyValues.end()
			&& it->key == key)
			? it->value
			: string_view{};
	}
	
	inline u32 GetUTFByKey(u32 key)
	{
		FillKeyValues();
		
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
	
	inline u32 GetKeyByValue(const string_view& value)
	{
		FillKeyValues();
		
		const auto it = find_if(
			keyValues.begin(), keyValues.end(),
			[&](const KeyValue& kv) { return kv.value == value; });
			
		return (
			it != keyValues.end()
			&& it->key != 0)
			? it->key
			: u32{};
	}
	
	inline u32 GetUTFByValue(const string_view& value)
	{
		FillKeyValues();
		
		const auto it = find_if(
			keyValues.begin(), keyValues.end(),
			[&](const KeyValue& kv) { return kv.value == value; });
			
		return (
			it != keyValues.end()
			&& it->utf != 0)
			? it->utf
			: 0x003F; //returns ? as fallback
	}
}