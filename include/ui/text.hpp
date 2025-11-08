//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "KalaHeaders/math_utils.hpp"
#include "KalaHeaders/import_ktf.hpp"

#include "ui/widget.hpp"
#include "utils/registry.hpp"

namespace KalaWindow::UI
{
	using KalaHeaders::kclamp;
	using KalaHeaders::GlyphBlock;
	
	using KalaWindow::Utils::Registry;

	class LIB_API Text : public Widget
	{
	public:
		static inline Registry<Text> registry{};

		//Initialize a new Text widget.
		//Parent widget and texture are optional
		static Text* Initialize(
			const string& name,
			u32 windowID,
			u32 glyphIndex,
			u32 fontID,
			const vec2 pos,
			const float rot,
			const vec2 size,
			Widget* parentWidget,
			OpenGL_Texture* texture,
			OpenGL_Shader* shader);
			
		//Render the text
		virtual bool Render(const mat4& projection) override;

		inline void AddChar(u32 newValue) { text.push_back(newValue); }
		inline void AddTab()
		{ 
			for (int i = 0; i < 4; ++i)
			{
				text.push_back(static_cast<u32>(' '));
			}
		}
		inline void AddNewLine() { text.push_back(static_cast<u32>('\n')); }
		inline void RemoveCharFromBack() { if (!text.empty()) text.pop_back(); }
		
		inline void SetText(const vector<u32>& newValue) { text = newValue; }
		inline vector<u32>& GetText() { return text; }
		
		inline void SetColor(const vec3& newValue) 
		{ 
			vec3 clamped = kclamp(newValue, vec3(0), vec3(1));
			color = clamped;
		}
		inline const vec3& GetColor() const { return color; }
		
		inline void SetOpacity(float newValue) 
		{ 
			f32 clamped = clamp(newValue, 0.0f, 1.0f);
			opacity = clamped;
		}
		inline f32 GetOpacity() const { return opacity; }
		
		inline void SetUnderline(bool newValue) { underline = newValue; }
		inline bool IsUnderlineEnabled() const { return underline; }
		
		inline void SetStrikethrough(bool newValue) { strikethrough = newValue; }
		inline bool IsStrikethroughEnabled() const { return strikethrough; }

		void SetFontID(u32 newValue);
		inline u32 GetFontID() const { return fontID; }

		//Do not destroy manually, erase from registry instead
		virtual ~Text() override;
	private:
		vector<u32> text{};            //the text typed by the user
		vector<GlyphBlock*> letters{}; //all the letters that have been typed to this text
		
		vec3 color{};         //what color the visible letters are
		f32 opacity{};        //how see-through the visible letters are
		bool underline{};     //should this text have an underline
		bool strikethrough{}; //should this text be striked through
		
		u32 fontID{};
		u32 textureID{};
	};
}