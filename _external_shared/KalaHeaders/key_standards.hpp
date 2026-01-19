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
//   - standard layout for emojis
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
		{scast<u32>(MouseButton::M_LEFT),   0, "left"},
		{scast<u32>(MouseButton::M_RIGHT),  0, "right"}, //scrollwheel click
		{scast<u32>(MouseButton::M_MIDDLE), 0, "middle"},
		
		{scast<u32>(MouseButton::M_X1), 0, "x1"}, //left side button
		{scast<u32>(MouseButton::M_X2), 0, "x2"}, //right side button
		
		{scast<u32>(MouseButton::M_WHEEL_UP),    0, "wheel-up"},    //scroll up
		{scast<u32>(MouseButton::M_WHEEL_DOWN),  0, "wheel-down"},  //scroll down
		{scast<u32>(MouseButton::M_WHEEL_LEFT),  0, "wheel-left"},  //scrollwheel left click
		{scast<u32>(MouseButton::M_WHEEL_RIGHT), 0, "wheel-right"}, //scrollwheel right click
		
		{scast<u32>(MouseButton::M_LEFT_DOUBLE),   0, "left-double"},
		{scast<u32>(MouseButton::M_RIGHT_DOUBLE),  0, "right-double"},
		{scast<u32>(MouseButton::M_MIDDLE_DOUBLE), 0, "middle-double"},
		{scast<u32>(MouseButton::M_X1_DOUBLE),     0, "x1-double"},
		{scast<u32>(MouseButton::M_X2_DOUBLE),     0, "x2-double"}
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
		{scast<u32>(GamepadButton::G_A), 0, "a"}, 
		{scast<u32>(GamepadButton::G_B), 0, "b"}, 
		{scast<u32>(GamepadButton::G_X), 0, "x"}, 
		{scast<u32>(GamepadButton::G_Y), 0, "y"},
		
		{scast<u32>(GamepadButton::G_L1), 0, "l1"}, 
		{scast<u32>(GamepadButton::G_R1), 0, "r1"},
		{scast<u32>(GamepadButton::G_L2), 0, "l2"}, 
		{scast<u32>(GamepadButton::G_R2), 0, "r2"},
		{scast<u32>(GamepadButton::G_L3), 0, "l3"}, 
		{scast<u32>(GamepadButton::G_R3), 0, "r3"},
		
		{scast<u32>(GamepadButton::G_DPAD_UP),    0, "dpad-up"}, 
		{scast<u32>(GamepadButton::G_DPAD_DOWN),  0, "dpad-down"}, 
		{scast<u32>(GamepadButton::G_DPAD_LEFT),  0, "dpad-left"}, 
		{scast<u32>(GamepadButton::G_DPAD_RIGHT), 0, "dpad-right"},
		
		{scast<u32>(GamepadButton::G_START),    0, "start"}, 
		{scast<u32>(GamepadButton::G_SELECT),   0, "select"}, 
		{scast<u32>(GamepadButton::G_HOME),     0, "home"}, 
		{scast<u32>(GamepadButton::G_CAPTURE),  0, "capture"}, 
		{scast<u32>(GamepadButton::G_TOUCHPAD), 0, "touchpad"}
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

		K_A = 61, K_B = 62, K_C = 63, K_D = 64, K_E = 65, K_F = 66, K_G = 67,
		K_H = 68, K_I = 69, K_J = 70, K_K = 71, K_L = 72, K_M = 73, K_N = 74,
		K_O = 75, K_P = 76, K_Q = 77, K_R = 78, K_S = 79, K_T = 80, K_U = 81,
		K_V = 82, K_W = 83, K_X = 84, K_Y = 85, K_Z = 86,

		K_NUM_0 = 87, K_NUM_1 = 88, K_NUM_2 = 89, K_NUM_3 = 90, K_NUM_4 = 91,
		K_NUM_5 = 92, K_NUM_6 = 93, K_NUM_7 = 94, K_NUM_8 = 95, K_NUM_9 = 96,

		K_NUM_LOCK = 97, K_NUM_DECIMAL = 98, K_NUM_RETURN = 99,
		K_NUM_DIVIDE = 100, K_NUM_MULTIPLY = 101,
		K_NUM_ADD = 102, K_NUM_SUBTRACT = 103,

		K_0 = 104, K_1 = 105, K_2 = 106, K_3 = 107, K_4 = 108,
		K_5 = 109, K_6 = 110, K_7 = 111, K_8 = 112, K_9 = 113,

		K_F1 = 114, K_F2 = 115, K_F3 = 116, K_F4 = 117,
		K_F5 = 118, K_F6 = 119, K_F7 = 120, K_F8 = 121,
		K_F9 = 122, K_F10 = 123, K_F11 = 124, K_F12 = 125,

		K_RETURN = 126, K_BACKSPACE = 127, K_SPACE = 128,
		K_ESC = 129, K_TAB = 130, K_CAPS_LOCK = 131,

		K_LEFT_SHIFT = 132, K_LEFT_CTRL = 133, K_LEFT_ALT = 134,
		K_RIGHT_SHIFT = 135, K_RIGHT_CTRL = 136, K_RIGHT_ALT = 137,

		K_PRINT_SCREEN = 138, K_SCROLL_LOCK = 139, K_PAUSE = 140,
		K_INSERT = 141, K_DELETE = 142,
		K_HOME = 143, K_END = 144,
		K_PAGE_UP = 145, K_PAGE_DOWN = 146,

		K_ARROW_UP = 147, K_ARROW_DOWN = 148,
		K_ARROW_LEFT = 149, K_ARROW_RIGHT = 150,

		K_ALTERNATE_GRAPHIC = 151,
		K_FUNCTION = 152,
		K_SYSTEM = 153,
		K_MENU = 154,
		K_SUPERLEFT = 155, K_SUPERRIGHT = 156
	};
	
	//Common keyboard actions. 157-200 is reserved.
	inline constexpr array<KeyValue, 96> keyboard_actions
	{{
		{scast<u32>(KeyboardButton::K_A), 0, "a"},
		{scast<u32>(KeyboardButton::K_B), 0, "b"},
		{scast<u32>(KeyboardButton::K_C), 0, "c"},
		{scast<u32>(KeyboardButton::K_D), 0, "d"},
		{scast<u32>(KeyboardButton::K_E), 0, "e"},
		{scast<u32>(KeyboardButton::K_F), 0, "f"},
		{scast<u32>(KeyboardButton::K_G), 0, "g"},
		{scast<u32>(KeyboardButton::K_H), 0, "h"},
		{scast<u32>(KeyboardButton::K_I), 0, "i"},
		{scast<u32>(KeyboardButton::K_J), 0, "j"},
		{scast<u32>(KeyboardButton::K_K), 0, "k"},
		{scast<u32>(KeyboardButton::K_L), 0, "l"},
		{scast<u32>(KeyboardButton::K_M), 0, "m"},
		{scast<u32>(KeyboardButton::K_N), 0, "n"},
		{scast<u32>(KeyboardButton::K_O), 0, "o"},
		{scast<u32>(KeyboardButton::K_P), 0, "p"},
		{scast<u32>(KeyboardButton::K_Q), 0, "q"},
		{scast<u32>(KeyboardButton::K_R), 0, "r"},
		{scast<u32>(KeyboardButton::K_S), 0, "s"},
		{scast<u32>(KeyboardButton::K_T), 0, "t"},
		{scast<u32>(KeyboardButton::K_U), 0, "u"},
		{scast<u32>(KeyboardButton::K_V), 0, "v"},
		{scast<u32>(KeyboardButton::K_W), 0, "w"},
		{scast<u32>(KeyboardButton::K_X), 0, "x"},
		{scast<u32>(KeyboardButton::K_Y), 0, "y"},
		{scast<u32>(KeyboardButton::K_Z), 0, "z"},

		{scast<u32>(KeyboardButton::K_NUM_0), 0, "num-0"}, 
		{scast<u32>(KeyboardButton::K_NUM_1), 0, "num-1"}, 
		{scast<u32>(KeyboardButton::K_NUM_2), 0, "num-2"}, 
		{scast<u32>(KeyboardButton::K_NUM_3), 0, "num-3"}, 
		{scast<u32>(KeyboardButton::K_NUM_4), 0, "num-4"},
		{scast<u32>(KeyboardButton::K_NUM_5), 0, "num-5"}, 
		{scast<u32>(KeyboardButton::K_NUM_6), 0, "num-6"}, 
		{scast<u32>(KeyboardButton::K_NUM_7), 0, "num-7"}, 
		{scast<u32>(KeyboardButton::K_NUM_8), 0, "num-8"}, 
		{scast<u32>(KeyboardButton::K_NUM_9), 0, "num-9"}, 
		
		{scast<u32>(KeyboardButton::K_NUM_LOCK),     0, "num-lock"}, 
		{scast<u32>(KeyboardButton::K_NUM_RETURN),   0, "mun-return"}, 
		{scast<u32>(KeyboardButton::K_NUM_DIVIDE),   0, "num-slash"}, 
		{scast<u32>(KeyboardButton::K_NUM_MULTIPLY), 0, "num-multiply"}, 
		{scast<u32>(KeyboardButton::K_NUM_ADD),      0, "num-add"}, 
		{scast<u32>(KeyboardButton::K_NUM_SUBTRACT), 0, "num-subtract"},
		
		{scast<u32>(KeyboardButton::K_0), 0, "top-0"}, 
		{scast<u32>(KeyboardButton::K_1), 0, "top-1"}, 
		{scast<u32>(KeyboardButton::K_2), 0, "top-2"}, 
		{scast<u32>(KeyboardButton::K_3), 0, "top-3"}, 
		{scast<u32>(KeyboardButton::K_4), 0, "top-4"},
		{scast<u32>(KeyboardButton::K_5), 0, "top-5"}, 
		{scast<u32>(KeyboardButton::K_6), 0, "top-6"}, 
		{scast<u32>(KeyboardButton::K_7), 0, "top-7"}, 
		{scast<u32>(KeyboardButton::K_8), 0, "top-8"}, 
		{scast<u32>(KeyboardButton::K_9), 0, "top-9"},
		
		{scast<u32>(KeyboardButton::K_F1), 0, "f1"}, 
		{scast<u32>(KeyboardButton::K_F2), 0, "f2"}, 
		{scast<u32>(KeyboardButton::K_F3), 0, "f3"}, 
		{scast<u32>(KeyboardButton::K_F4), 0, "f4"}, 
		{scast<u32>(KeyboardButton::K_F5), 0, "f5"}, 
		{scast<u32>(KeyboardButton::K_F6), 0, "f6"}, 
		{scast<u32>(KeyboardButton::K_F7), 0, "f7"}, 
		{scast<u32>(KeyboardButton::K_F8), 0, "f8"},
		{scast<u32>(KeyboardButton::K_F9), 0, "f9"}, 
		{scast<u32>(KeyboardButton::K_F10), 0, "f10"}, 
		{scast<u32>(KeyboardButton::K_F11), 0, "f11"}, 
		{scast<u32>(KeyboardButton::K_F12), 0, "f12"},
		
		{scast<u32>(KeyboardButton::K_RETURN),    0, "return"}, 
		{scast<u32>(KeyboardButton::K_BACKSPACE), 0, "backspace"}, 
		{scast<u32>(KeyboardButton::K_SPACE),     0, "space"},
		{scast<u32>(KeyboardButton::K_ESC),       0, "esc"}, 
		{scast<u32>(KeyboardButton::K_TAB),       0, "tab"}, 
		{scast<u32>(KeyboardButton::K_CAPS_LOCK), 0, "caps-lock"},
		
		{scast<u32>(KeyboardButton::K_LEFT_SHIFT),  0, "left-shift"}, 
		{scast<u32>(KeyboardButton::K_LEFT_CTRL),   0, "left-control"}, 
		{scast<u32>(KeyboardButton::K_LEFT_ALT),    0, "left-alt"},
		{scast<u32>(KeyboardButton::K_RIGHT_SHIFT), 0, "right-shift"}, 
		{scast<u32>(KeyboardButton::K_RIGHT_CTRL),  0, "right-control"}, 
		{scast<u32>(KeyboardButton::K_RIGHT_ALT),   0, "right-alt"},
		
		{scast<u32>(KeyboardButton::K_PRINT_SCREEN), 0, "print-screen"}, 
		{scast<u32>(KeyboardButton::K_SCROLL_LOCK),  0, "scroll-lock"}, 
		{scast<u32>(KeyboardButton::K_PAUSE),        0, "pause"},
		{scast<u32>(KeyboardButton::K_INSERT),       0, "insert"}, 
		{scast<u32>(KeyboardButton::K_DELETE),       0, "delete"},
		{scast<u32>(KeyboardButton::K_HOME),         0, "home"}, 
		{scast<u32>(KeyboardButton::K_END),          0, "end"},
		{scast<u32>(KeyboardButton::K_PAGE_UP),      0, "page-up"}, 
		{scast<u32>(KeyboardButton::K_PAGE_DOWN),    0, "page-down"},
		
		{scast<u32>(KeyboardButton::K_ARROW_UP),    0, "arrow-up"}, 
		{scast<u32>(KeyboardButton::K_ARROW_DOWN),  0, "arrow-down"}, 
		{scast<u32>(KeyboardButton::K_ARROW_LEFT),  0, "arrow-left"}, 
		{scast<u32>(KeyboardButton::K_ARROW_RIGHT), 0, "arrow-right"},
		
		{scast<u32>(KeyboardButton::K_ALTERNATE_GRAPHIC), 0, "alternate-graphic"}, 
		{scast<u32>(KeyboardButton::K_FUNCTION),          0, "function"}, 
		{scast<u32>(KeyboardButton::K_SYSTEM),            0, "system"}, 
		{scast<u32>(KeyboardButton::K_MENU),              0, "menu"},
		{scast<u32>(KeyboardButton::K_SUPERLEFT),         0, "super-left"},
		{scast<u32>(KeyboardButton::K_SUPERRIGHT),        0, "super-right"}
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
	
	//Common symbols used for typing and UI. 247-300 is reserved for future use.
	inline constexpr array<KeyValue, 46> typography_symbols
	{{
		{201, 0x0030, "0"}, {202, 0x0031, "1"}, {203, 0x0032, "2"}, {204, 0x0033, "3"}, {205, 0x0034, "4"},
		{206, 0x0035, "5"}, {207, 0x0036, "6"}, {208, 0x0037, "7"}, {209, 0x0038, "8"}, {210, 0x0039, "9"},

		{211, 0x003F, "?"}, {212, 0x0021, "!"}, {213, 0x005F, "_"},
		{214, 0x0040, "@"}, {215, 0x003B, ";"}, {216, 0x002C, ","}, {217, 0x002E, "."},

		{218, 0x0028, "("}, {219, 0x0029, ")"},
		{220, 0x007B, "{"}, {221, 0x007D, "}"},
		{222, 0x005B, "["}, {223, 0x005D, "]"},

		{224, 0x003A, ":"},
		{225, 0x2237, "double-colon"}, // ∷

		{226, 0x0020, "space"}, {227, 0x00A0, "non-breaking-space"},

		{228, 0x005C, "backslash"}, // \

		{229, 0x2013, "endash"},   // –
		{230, 0x2014, "emdash"},   // —
		{231, 0x2026, "ellipsis"}, // …

		{232, 0x0027, "straight-single-quote"}, // '
		{233, 0x0022, "straight-double-quote"}, // "
		{234, 0x02C7, "caron"},                 // ˇ
		{235, 0x0060, "grave-accent"},          // `
		{236, 0x00B4, "acute-accent"},          // ´
		{237, 0x2018, "left-single-quote"},     // ‘
		{238, 0x2019, "right-single-quote"},    // ’
		{239, 0x201C, "left-double-quote"},     // “
		{240, 0x201D, "right-double-quote"},    // ”

		{241, 0x00A7, "section"}, // §
		{242, 0x2022, "bullet"},  // •
		{243, 0x00B7, "middot"},  // ·

		{244, 0x00A9, "copyright"},  // ©
		{245, 0x00AE, "registered"}, // ®
		{246, 0x2122, "trademark"}   // ™
	}};
	
	//Common symbols used for math and technical operations. 335-350 is reserved for future use.
	inline constexpr array<KeyValue, 35> math_symbols
	{{
		{301, 0x002B, "+"},            // add
		{302, 0x002D, "-"},            // subtract
		{303, 0x002A, "*"},            // multiply asterisk
		{304, 0x00D7, "multiply-x"},   // ×
		{305, 0x002F, "/"},            // divide
		{306, 0x00F7, "divide"},       // ÷

		{307, 0x0025, "%"}, // modulus
		{308, 0x007C, "|"}, // bitwise or logical OR

		{309, 0x007E, "bitwise-not"},         // ~
		{310, 0x2248, "approximately-equal"}, // ≈
		{311, 0x003D, "="},                   // equal
		{312, 0x2260, "not-equal"},           // ≠
		{313, 0x2261, "identical-to"},        // ≡
		{314, 0x221E, "infinity"},            // ∞
		{315, 0x221A, "square-root"},         // √
		{316, 0x00B0, "degree"},              // °
		{317, 0x00B5, "micro"},               // µ
		{318, 0x2126, "ohm"},                 // Ω

		{319, 0x003C, "<"},                      // less than
		{320, 0x003E, ">"},                      // more than
		{321, 0x2264, "less-than-and-equal-to"}, // ≤
		{322, 0x2265, "more-than-and-equal-to"}, // ≥
		{323, 0x00B1, "plus-minus"},             // ±

		{324, 0x2190, "arrow-left"},       // ←
		{325, 0x2191, "arrow-up"},         // ↑
		{326, 0x2192, "arrow-right"},      // →
		{327, 0x2193, "arrow-down"},       // ↓
		{328, 0x21D2, "implies"},          // ⇒
		{329, 0x21D4, "if-and-only-if"},   // ⇔
		{330, 0x27F5, "long-left-arrow"},  // ⟵
		{331, 0x27F6, "long-right-arrow"}, // ⟶

		{332, 0x00B9, "superscript-one"},   // ¹
		{333, 0x00B2, "superscript-two"},   // ²
		{334, 0x00B3, "superscript-three"}  // ³
	}};
	
	//Common currency symbols. 364-370 is reserved for future use.
	inline constexpr array<KeyValue, 13> currency_symbols
	{{
		{351, 0x20AC, "euro"},               //€
		{352, 0x00A3, "pound"},              //£
		{353, 0x0024, "dollar"},             //$
		{354, 0x20B4, "ukrainian-hryvnia"},  //₴
		{355, 0x20BD, "russian-rouble"},     //₽
		{356, 0x20A8, "old-indian-rupee"},   //₨
		{357, 0x20B9, "indian-rupee"},       //₹
		{358, 0x00A5, "japanese-yen"},       //¥
		{359, 0x5186, "japanese-kanji-yen"}, //円
		{360, 0x5713, "taiwanese-yuan"},     //圓
		{361, 0x20A9, "korean-won"},         //₩
		{362, 0x20AA, "israeli-shekel"},     //₪
		{363, 0x20B1, "philippine-peso"}     //₱
	}};
	
	//
	// LATIN LETTERS
	//
	
	inline constexpr array<KeyValue, 52> latin_standard
	{{
		{371, 0x0041, "A"}, {372, 0x0042, "B"}, {373, 0x0043, "C"}, {374, 0x0044, "D"}, {375, 0x0045, "E"},
		{376, 0x0046, "F"}, {377, 0x0047, "G"}, {378, 0x0048, "H"}, {379, 0x0049, "I"}, {380, 0x004A, "J"},
		{381, 0x004B, "K"}, {382, 0x004C, "L"}, {383, 0x004D, "M"}, {384, 0x004E, "N"}, {385, 0x004F, "O"},
		{386, 0x0050, "P"}, {387, 0x0051, "Q"}, {388, 0x0052, "R"}, {389, 0x0053, "S"}, {390, 0x0054, "T"},
		{391, 0x0055, "U"}, {392, 0x0056, "V"}, {393, 0x0057, "W"}, {394, 0x0058, "X"}, {395, 0x0059, "Y"},
		{396, 0x005A, "Z"},

		{397, 0x0061, "a"}, {398, 0x0062, "b"}, {399, 0x0063, "c"}, {400, 0x0064, "d"}, {401, 0x0065, "e"},
		{402, 0x0066, "f"}, {403, 0x0067, "g"}, {404, 0x0068, "h"}, {405, 0x0069, "i"}, {406, 0x006A, "j"},
		{407, 0x006B, "k"}, {408, 0x006C, "l"}, {409, 0x006D, "m"}, {410, 0x006E, "n"}, {411, 0x006F, "o"},
		{412, 0x0070, "p"}, {413, 0x0071, "q"}, {414, 0x0072, "r"}, {415, 0x0073, "s"}, {416, 0x0074, "t"},
		{417, 0x0075, "u"}, {418, 0x0076, "v"}, {419, 0x0077, "w"}, {420, 0x0078, "x"}, {421, 0x0079, "y"},
		{422, 0x007A, "z"}
	}};

	inline constexpr array<KeyValue, 124> latin_extra
	{{
		//simple uppercase diacritic and regional extensions

		{423, 0x00C0, "À"}, {424, 0x00C1, "Á"}, {425, 0x00C2, "Â"}, {426, 0x00C3, "Ã"}, {427, 0x00C4, "Ä"},
		{428, 0x00C5, "Å"}, {429, 0x00C7, "Ç"}, {430, 0x00C8, "È"}, {431, 0x00C9, "É"}, {432, 0x00CA, "Ê"},
		{433, 0x00CB, "Ë"}, {434, 0x00CC, "Ì"}, {435, 0x00CD, "Í"}, {436, 0x00CE, "Î"}, {437, 0x00CF, "Ï"},
		{438, 0x00D1, "Ñ"}, {439, 0x00D2, "Ò"}, {440, 0x00D3, "Ó"}, {441, 0x00D4, "Ô"}, {442, 0x00D5, "Õ"},
		{443, 0x00D6, "Ö"}, {444, 0x00D8, "Ø"}, {445, 0x00D9, "Ù"}, {446, 0x00DA, "Ú"}, {447, 0x00DB, "Û"},
		{448, 0x00DC, "Ü"}, {449, 0x00DD, "Ý"}, {450, 0x0179, "Ź"}, {451, 0x017B, "Ż"}, {452, 0x017D, "Ž"},
		{453, 0x0102, "Ă"}, {454, 0x0104, "Ą"}, {455, 0x010C, "Č"}, {456, 0x010E, "Ď"}, {457, 0x0118, "Ę"},
		{458, 0x011A, "Ě"}, {459, 0x011E, "Ğ"}, {460, 0x0141, "Ł"}, {461, 0x0143, "Ń"}, {462, 0x0147, "Ň"},
		{463, 0x0150, "Ő"}, {464, 0x0158, "Ř"}, {465, 0x015A, "Ś"}, {466, 0x015E, "Ş"}, {467, 0x0160, "Š"},
		{468, 0x0162, "Ţ"}, {469, 0x0164, "Ť"}, {470, 0x016E, "Ů"}, {471, 0x0170, "Ű"}, {472, 0x0172, "Ų"},
		{473, 0x0176, "Ŷ"}, {474, 0x0178, "Ÿ"},

		//uppercase morphological / ligature / historical letters

		{475, 0x00C6, "Æ"}, //AE ligature - Nordic/French
		{476, 0x00D0, "Ð"}, //Eth - Icelandic/Faroese
		{477, 0x0152, "Œ"}, //OE ligature - French/Latin
		{478, 0x0132, "Ĳ"}, //IJ ligature - Dutch
		{479, 0x00DE, "Þ"}, //Thorn - Old English/Icelandic
		{480, 0x1E9E, "ẞ"}, //Capital Sharp S - German
		{481, 0x017F, "ſ"}, //Long s - Historical typography

		//simple lowercase diacritic and regional extensions

		{482, 0x00E0, "à"}, {483, 0x00E1, "á"}, {484, 0x00E2, "â"}, {485, 0x00E3, "ã"}, {486, 0x00E4, "ä"},
		{487, 0x00E5, "å"}, {488, 0x00E7, "ç"}, {489, 0x00E8, "è"}, {490, 0x00E9, "é"}, {491, 0x00EA, "ê"},
		{492, 0x00EB, "ë"}, {493, 0x00EC, "ì"}, {494, 0x00ED, "í"}, {495, 0x00EE, "î"}, {496, 0x00EF, "ï"},
		{497, 0x00F1, "ñ"}, {498, 0x00F2, "ò"}, {499, 0x00F3, "ó"}, {500, 0x00F4, "ô"}, {501, 0x00F5, "õ"},
		{502, 0x00F6, "ö"}, {503, 0x00F8, "ø"}, {504, 0x00F9, "ù"}, {505, 0x00FA, "ú"}, {506, 0x00FB, "û"},
		{507, 0x00FC, "ü"}, {508, 0x00FD, "ý"}, {509, 0x017A, "ź"}, {510, 0x017C, "ż"}, {511, 0x017E, "ž"},
		{512, 0x0103, "ă"}, {513, 0x0105, "ą"}, {514, 0x010D, "č"}, {515, 0x010F, "ď"}, {516, 0x0119, "ę"},
		{517, 0x011B, "ě"}, {518, 0x011F, "ğ"}, {519, 0x0142, "ł"}, {520, 0x0144, "ń"}, {521, 0x0148, "ň"},
		{522, 0x0151, "ő"}, {523, 0x0159, "ř"}, {524, 0x015B, "ś"}, {525, 0x015F, "ş"}, {526, 0x0161, "š"},
		{527, 0x0163, "ţ"}, {528, 0x0165, "ť"}, {529, 0x016F, "ů"}, {530, 0x0171, "ű"}, {531, 0x0173, "ų"},
		{532, 0x0177, "ŷ"}, {533, 0x00FF, "ÿ"},

		//lowercase morphological / ligature / historical letters

		{534, 0x00E6, "æ"}, //AE ligature - Nordic/French
		{535, 0x00F0, "ð"}, //Eth - Icelandic/Faroese
		{536, 0x0153, "œ"}, //OE ligature - French/Latin
		{537, 0x0133, "ĳ"}, //IJ ligature - Dutch
		{538, 0x00FE, "þ"}, //Thorn - Old English/Icelandic
		{539, 0x00DF, "ß"}, //Sharp S - German
		{540, 0x017F, "ſ"}  //Long s - Historical typography
	}};
	
	//
	// CYRILLIC LETTERS
	//
	
	inline constexpr array<KeyValue, 66> cyrillic_standard
	{{
		{541, 0x0410, "А"}, {542, 0x0411, "Б"}, {543, 0x0412, "В"}, {544, 0x0413, "Г"}, {545, 0x0414, "Д"},
		{546, 0x0415, "Е"}, {547, 0x0401, "Ё"}, {548, 0x0416, "Ж"}, {549, 0x0417, "З"}, {550, 0x0418, "И"},
		{551, 0x0419, "Й"}, {552, 0x041A, "К"}, {553, 0x041B, "Л"}, {554, 0x041C, "М"}, {555, 0x041D, "Н"},
		{556, 0x041E, "О"}, {557, 0x041F, "П"}, {558, 0x0420, "Р"}, {559, 0x0421, "С"}, {560, 0x0422, "Т"},
		{561, 0x0423, "У"}, {562, 0x0424, "Ф"}, {563, 0x0425, "Х"}, {564, 0x0426, "Ц"}, {565, 0x0427, "Ч"},
		{566, 0x0428, "Ш"}, {567, 0x0429, "Щ"}, {568, 0x042A, "Ъ"}, {569, 0x042B, "Ы"}, {570, 0x042C, "Ь"},
		{571, 0x042D, "Э"}, {572, 0x042E, "Ю"}, {573, 0x042F, "Я"},

		{574, 0x0430, "а"}, {575, 0x0431, "б"}, {576, 0x0432, "в"}, {577, 0x0433, "г"}, {578, 0x0434, "д"},
		{579, 0x0435, "е"}, {580, 0x0451, "ё"}, {581, 0x0436, "ж"}, {582, 0x0437, "з"}, {583, 0x0438, "и"},
		{584, 0x0439, "й"}, {585, 0x043A, "к"}, {586, 0x043B, "л"}, {587, 0x043C, "м"}, {588, 0x043D, "н"},
		{589, 0x043E, "о"}, {590, 0x043F, "п"}, {591, 0x0440, "р"}, {592, 0x0441, "с"}, {593, 0x0442, "т"},
		{594, 0x0443, "у"}, {595, 0x0444, "ф"}, {596, 0x0445, "х"}, {597, 0x0446, "ц"}, {598, 0x0447, "ч"},
		{599, 0x0448, "ш"}, {600, 0x0449, "щ"}, {601, 0x044A, "ъ"}, {602, 0x044B, "ы"}, {603, 0x044C, "ь"},
		{604, 0x044D, "э"}, {605, 0x044E, "ю"}, {606, 0x044F, "я"}
	}};

	inline constexpr array<KeyValue, 20> cyrillic_extra
	{{
		{607, 0x0490, "Ґ"}, {608, 0x0404, "Є"}, {609, 0x0406, "І"}, {610, 0x0407, "Ї"},
		{611, 0x0408, "Ј"}, {612, 0x0409, "Љ"}, {613, 0x040A, "Њ"}, {614, 0x040B, "Ћ"},
		{615, 0x0402, "Ђ"}, {616, 0x040F, "Џ"},

		{617, 0x0491, "ґ"}, {618, 0x0454, "є"}, {619, 0x0456, "і"}, {620, 0x0457, "ї"},
		{621, 0x0458, "ј"}, {622, 0x0459, "љ"}, {623, 0x045A, "њ"}, {624, 0x045B, "ћ"},
		{625, 0x0452, "ђ"}, {626, 0x045F, "џ"}
	}};
	
	//
	// EMOJIS
	//
	
	//Common face, hand and gesture emojis. 1226-1500 is reserved for future use. 
	inline constexpr array<KeyValue, 226> emojis
	{{
		//face - smiling
		
		{1001, 0x1F600, "grinning-face"},
		{1002, 0x1F603, "grinning-face-with-big-eyes"},
		{1003, 0x1F604, "grinning-face-with-smiling-eyes"},
		{1004, 0x1F601, "beaming-face-with-smiling-eyes"},
		{1005, 0x1F606, "grinning-squinting-face"},
		{1006, 0x1F605, "grinning-face-with-sweat"},
		{1008, 0x1F923, "rolling-on-the-floor-laughing"},
		{1009, 0x1F602, "face-with-tears-of-joy"},
		{1010, 0x1F642, "slightly-smiling-face"},
		{1011, 0x1F643, "upside-down-face"},
		{1012, 0x1FAE0, "melting-face"},
		{1013, 0x1F609, "winking-face"},
		{1014, 0x1F60A, "smiling-face-with-smiling-eyes"},
		{1015, 0x1F607, "smiling-face-with-halo"},

		//face - affection
		
		{1016, 0x1F970, "smiling-face-with-hearts"},
		{1017, 0x1F60D, "smiling-face-with-heart-eyes"},
		{1018, 0x1F929, "star-struck"},
		{1019, 0x1F618, "face-blowing-a-kiss"},
		{1020, 0x1F617, "kissing-face"},
		{1021, 0x263A, "smiling-face"},
		{1022, 0x1F61A, "kissing-face-with-closed-eyes"},
		{1023, 0x1F619, "kissing-face-with-smiling-eyes"},
		{1024, 0x1F972, "smiling-face-with-tear"},

		//face - tongue
		
		{1025, 0x1F60B, "face-savoring-food"},
		{1026, 0x1F61B, "face-with-tongue"},
		{1027, 0x1F61C, "winking-face-with-tongue"},
		{1028, 0x1F92A, "zany-face"},
		{1029, 0x1F61D, "squinting-face-with-tongue"},
		{1030, 0x1F911, "money-mouth-face"},

		//face - hand
		
		{1031, 0x1F917, "smiling-face-with-open-hands"},
		{1032, 0x1F92D, "face-with-hand-over-mouth"},
		{1033, 0x1FAE2, "face-with-open-eyes-and-hand-over-mouth"},
		{1034, 0x1FAE3, "face-with-peeking-eye"},
		{1035, 0x1F92B, "shushing-face"},
		{1036, 0x1F914, "thinking-face"},
		{1037, 0x1FAE1, "saluting-face"},

		//face - neutral/skeptical
		
		//the following ones were not included:
		//  - face in clouds
		//  - face exhaling
		//  - head shaking horizontally
		//  - head shaking vertically
		
		{1038, 0x1F910, "zipper-mouth-face"},
		{1039, 0x1F928, "face-with-raised-eyebrow"},
		{1040, 0x1F610, "neutral-face"},
		{1041, 0x1F611, "expressionless-face"},
		{1042, 0x1F636, "face-without-mouth"},
		{1043, 0x1FAE5, "dotted-line-face"},
		{1044, 0x1F60F, "smirking-face"},
		{1045, 0x1F612, "unamused-face"},
		{1046, 0x1F644, "face-with-rolling-eyes"},
		{1047, 0x1F62C, "grimacing-face"},
		{1048, 0x1F925, "lying-face"},
		{1049, 0x1FAE8, "shaking-face"},

		//face - sleepy
		
		{1050, 0x1F60C, "relieved-face"},
		{1051, 0x1F614, "pensive-face"},
		{1052, 0x1F62A, "sleepy-face"},
		{1053, 0x1F924, "drooling-face"},
		{1054, 0x1F634, "sleeping-face"},
		{1055, 0x1FAE9, "face-with-bags-under-eyes"},

		//face - unwell
		
		//the following one was not included:
		//  - face with spiral eyes
		
		{1056, 0x1F637, "face-with-medical-mask"},
		{1057, 0x1F912, "face-with-thermometer"},
		{1058, 0x1F915, "face-with-head-bandage"},
		{1059, 0x1F922, "nauseated-face"},
		{1060, 0x1F92E, "face-vomiting"},
		{1061, 0x1F927, "sneezing-face"},
		{1062, 0x1F975, "hot-face"},
		{1063, 0x1F976, "cold-face"},
		{1064, 0x1F974, "woozy-face"},
		{1065, 0x1F635, "face-with-crossed-out-eyes"},
		{1066, 0x1F92F, "exploding-head"},
		
		//face - hat

		{1067, 0x1F920, "cowboy-hat-face"},
		{1068, 0x1F973, "partying-face"},
		{1069, 0x1F978, "disguised-face"},

		//face - glasses

		{1070, 0x1F60E, "smiling-face-with-sunglasses"},
		{1071, 0x1F913, "nerd-face"},
		{1072, 0x1F9D0, "face-with-monocle"},
		
		//face - concerned
		
		{1073, 0x1F615, "confused-face"},
		{1074, 0x1FAE4, "face-with-diagonal-mouth"},
		{1075, 0x1F61F, "worried-face"},
		{1076, 0x1F641, "slightly-frowning-face"},
		{1077, 0x1F639, "frowning-face"},
		{1078, 0x2639, "face-with-open-mouth"},
		{1079, 0x1F62E, "hushed-face"},
		{1080, 0x1F62F, "astonished-face"},
		{1081, 0x1F632, "flushed-face"},
		{1082, 0x1F633, "distorted-face"},
		{1083, 0x1FAEA, "pleading-face"},
		{1084, 0x1F97A, "face-holding-back-tears"},
		{1085, 0x1F979, "frowing-face-with-open-mouth"},
		{1086, 0x1F626, "anguished-face"},
		{1087, 0x1F627, "fearful-face"},
		{1088, 0x1F628, "anxious-face-with-sweat"},
		{1089, 0x1F630, "sad-but-relieved-face"},
		{1090, 0x1F625, "crying-face"},
		{1091, 0x1F622, "loudly-crying-face"},
		{1092, 0x1F62D, "face-screaming-in-fear"},
		{1093, 0x1F631, "confounded-face"},
		{1094, 0x1F616, "persevering-face"},
		{1095, 0x1F623, "dissapointed-face"},
		{1096, 0x1F61E, "downcast-face-with-sweat"},
		{1097, 0x1F613, "weary-face"},
		{1098, 0x1F62B, "tired-face"},
		{1099, 0x1F971, "yawning-face"},
		
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
		
		{1108, 0x1F4A9, "pile-of-poo"},
		{1109, 0x1F921, "clown-face"},
		{1110, 0x1F479, "ogre"},
		{1111, 0x1F47A, "goblin"},
		{1112, 0x1F47B, "ghost"},
		{1113, 0x1F47D, "alien"},
		{1114, 0x1F47E, "alien-monster"},
		{1115, 0x1F916, "robot"},
		
		//face - cat
		
		{1116, 0x1F63A, "grinning-cat"},
		{1117, 0x1F638, "grinning-cat-with-smiling-eyes"},
		{1118, 0x1F639, "cat-with-tears-of-joy"},
		{1119, 0x1F63B, "smiling-cat-with-heart-eyes"},
		{1120, 0x1F63C, "cat-with-wry-smile"},
		{1121, 0x1F63D, "kissing-cat"},
		{1122, 0x1F640, "weary-cat"},
		{1123, 0x1F63F, "crying-cat"},
		{1124, 0x1F63E, "pouting-cat"},
		
		//face - monkey
		
		{1125, 0x1F648, "see-no-evil-monkey"},
		{1126, 0x1F649, "hear-no-evil-monkey"},
		{1127, 0x1F64A, "speak-no-evil-monkey"},
		
		//heart
		
		//the following ones were not included:
		//  - heart on fire
		//  - mending heart
		
		{1128, 0x1F48C, "love-letter"},
		{1129, 0x1F498, "heart-with-arrow"},
		{1130, 0x1F49D, "heart-with-ribbon"},
		{1131, 0x1F496, "sparkling-heart"},
		{1132, 0x1F497, "growing-heart"},
		{1133, 0x1F493, "beating-heart"},
		{1134, 0x1F49E, "revolving-hearts"},
		{1135, 0x1F495, "two-hearts"},
		{1136, 0x1F49F, "heart-decoration"},
		{1137, 0x2763, "heart-exclamation"},
		{1138, 0x1F494, "broken-heart"},
		{1139, 0x1F764, "red-heart"},
		{1140, 0x1FA77, "pink-heart"},
		{1141, 0x1F9E1, "orange-heart"},
		{1142, 0x1F49B, "yellow-heart"},
		{1143, 0x1F49A, "green-heart"},
		{1144, 0x1F499, "blue-heart"},
		{1145, 0x1FA75, "light-blue-heart"},
		{1146, 0x1F49C, "purple-heart"},
		{1147, 0x1F90E, "brown-heart"},
		{1148, 0x1F5A4, "black-heart"},
		{1149, 0x1FA76, "grey-heart"},
		{1150, 0x1F90D, "white-heart"},
		
		//emotion
		
		//the following one was not included:
		//  - eye in speech bubble
		
		{1151, 0x1F48B, "kiss-mark"},
		{1152, 0x1F4AF, "hundred-points"},
		{1153, 0x1F4A2, "anger-symbol"},
		{1154, 0x1FAEF, "fight-cloud"},
		{1155, 0x1F4A5, "collision"},
		{1156, 0x1F4AB, "dizzy"},
		{1157, 0x1F4A6, "sweat-droplets"},
		{1158, 0x1F4A8, "dashing-away"},
		{1159, 0x1F573, "hole"},
		{1160, 0x1F4AC, "speech-balloon"},
		{1161, 0x1F5E8, "left-speech-bubble"},
		{1162, 0x1F5EF, "right-angle-bubble"},
		{1163, 0x1F4AD, "thought-balloon"},
		{1164, 0x1F4A4, "zzz"},
		
		//hand - fingers open
		
		{1165, 0x1F44B, "waving-hand"},
		{1166, 0x1F91A, "raised-back-of-hand"},
		{1167, 0x1F590, "hand-with-fingers-splayed"},
		{1168, 0x270B, "raised-hand"},
		{1169, 0x1F596, "vulcan-salute"},
		{1170, 0x1FAF1, "rightwards-hand"},
		{1171, 0x1FAF2, "leftwards-hand"},
		{1172, 0x1FAF3, "palm-down-hand"},
		{1173, 0x1FAF4, "palm-up-hand"},
		{1174, 0x1FAF7, "leftwards-pushing-hand"},
		{1175, 0x1FAF8, "rightwards-pushing-hand"},
		
		//hand - fingers partial
		
		{1176, 0x1F44C, "ok-hand"},
		{1177, 0x1F90C, "pinched-fingers"},
		{1178, 0x1F90F, "pinching-hand"},
		{1179, 0x270C, "victory-hand"},
		{1180, 0x1F91E, "crossed-fingers"},
		{1181, 0x1FAF0, "hand-with-index-finger-and-thumb-crossed"},
		{1182, 0x1F91F, "love-you-gesture"},
		{1183, 0x1F918, "sign-of-the-horns"},
		{1184, 0x1F919, "call-me-hand"},
		
		//hand - single finger
		
		{1185, 0x1F448, "backhand-index-pointing-left"},
		{1186, 0x1F449, "backhand-index-pointing-right"},
		{1187, 0x1F446, "backhand-index-pointing-up"},
		{1188, 0x1F595, "middle-finger"},
		{1189, 0x1F447, "backhand-index-pointing-down"},
		{1190, 0x261D, "index-pointing-up"},
		{1191, 0x1FAF5, "index-pointing-at-the-viewer"},
		
		//hand - fingers closed
		
		{1192, 0x1F44D, "thumbs-up"},
		{1193, 0x1F44E, "thumbs-down"},
		{1194, 0x270A, "raised-fist"},
		{1195, 0x1F44A, "oncoming-fist"},
		{1196, 0x1F91B, "left-facing-fist"},
		{1197, 0x1F91C, "right-facing-fist"},
		
		//hands
		
		{1198, 0x1F44F, "clapping-hands"},
		{1199, 0x1F64C, "raising-hands"},
		{1200, 0x1FAF6, "heart-hands"},
		{1201, 0x1F450, "open-hands"},
		{1202, 0x1F932, "palms-up-together"},
		{1203, 0x1F91D, "handshake"},
		{1204, 0x1F64F, "folded-hands"},
		
		//hand - prop
		
		{1205, 0x270D, "writing-hand"},
		{1206, 0x1F485, "nail-polish"},
		{1207, 0x1F933, "selfie"},
		
		//body parts
		
		{1208, 0x1F4AA, "flexed-biceps"},
		{1209, 0x1F9BE, "mechanical-arm"},
		{1210, 0x1F9BF, "mechanical-leg"},
		{1211, 0x1F9B5, "leg"},
		{1212, 0x1F9B6, "foot"},
		{1213, 0x1F442, "ear"},
		{1214, 0x1F9BB, "ear-with-hearing-aid"},
		{1215, 0x1F443, "nose"},
		{1216, 0x1F9E0, "brain"},
		{1217, 0x1FAC0, "anatomical-heart"},
		{1218, 0x1FAC1, "lungs"},
		{1219, 0x1F9B7, "tooth"},
		{1220, 0x1F9B4, "bone"},
		{1221, 0x1F440, "eyes"},
		{1222, 0x1F441, "eye"},
		{1223, 0x1F445, "tongue"},
		{1224, 0x1F444, "mouth"},
		{1225, 0x1FAE6, "biting-lip"}
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
			
			+ emojis.size());
				
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