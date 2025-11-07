//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include "ui/widget.hpp"
#include "utils/registry.hpp"

namespace KalaWindow::UI
{
	using KalaWindow::Utils::Registry;

	class LIB_API Text : public Widget
	{
	public:
		static inline Registry<Text> registry{};
		static inline vector<Text*> runtimeText{};

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

		void SetFontID(u32 newValue);
		inline u32 GetFontID() const { return fontID; }

		//Do not destroy manually, erase from registry instead
		virtual ~Text() override;
	private:
		u32 fontID{};
		u32 textureID{};
	};
}