//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#pragma once

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <memory>

#include "KalaHeaders/core_utils.hpp"
#include "KalaHeaders/math_utils.hpp"

#include "utils/kw_registry.hpp"

namespace KalaWindow::Graphics
{
	using std::string;
	using std::function;
	using std::unordered_map;
	using std::vector;
	using std::unique_ptr;

	using KalaWindow::Utils::KalaWindowRegistry;

	enum class LabelType
	{
		LABEL_LEAF,  //Clickable with required function, can't have children
		LABEL_BRANCH //Not clickable, won't work if function is added, can have children
	};
	struct MenuBarEvent
	{
		string parentLabel{};        //Name of parent label, leave empty if root

		string label{};              //Name of this label
		u32 labelID{};               //ID assigned to leaves, used for interaction
		function<void()> function{}; //Function assigned to leaves

		uintptr_t hMenu{};           //Branch HMENU handle for fast lookup
	};

	//Windows-only native menu bar. All leaf and and branch interactions are handled by the message loop.
	//Attach a function in CreateLabel for leaves, leave empty for functions so that the message loop
	//calls your function so that the menu bar interactions call your chosen functions.
	class LIB_API MenuBar
	{
	public:
		static inline KalaWindowRegistry<MenuBar> registry{};

		//Create a new empty menu bar at the top of the window.
		//Only one menu bar can be added to a window
		static MenuBar* CreateMenuBar(u32 windowID);

		bool IsInitialized() const;

		inline u32 GetID() const { return ID; }
		inline u32 GetWindowID() const { return windowID; }

		//Toggle verbose logging. If true, then usually frequently updated runtime values like
		//branch and leaf creation will dump their logs into the console.
		static inline void SetVerboseLoggingState(bool newState) { isMenuBarVerboseLoggingEnabled = newState; }
		static inline bool IsVerboseLoggingEnabled() { return isMenuBarVerboseLoggingEnabled; }

		//If true, then menu bar is shown
		void SetMenuBarState(bool state);
		//If true, then menu bar is shown
		bool IsEnabled() const;

		//Create a menu bar label. Leaves must have functions, branches can't.
		//Leave parentRef empty if you want this label to be root
		void CreateLabel(
			LabelType type,
			const string& parentRef,
			const string& labelRef,
			const function<void()> func = nullptr);

		//Add a horizontal separator line to the menu label.
		//If itemLabel isn't empty and exists then the separator is placed after the item label,
		//otherwise it is placed at the end of the menu label
		void AddSeparator(
			const string& parentRef,
			const string& labelRef = "") const;

		inline const vector<MenuBarEvent>& GetEvents() const { return events; }

		//Destroy the existing menu bar inside the window
		~MenuBar();
	private:
		static inline bool isMenuBarVerboseLoggingEnabled{};

		bool isInitialized{};
		bool isEnabled{};

		u32 ID{};
		u32 windowID{};

		vector<MenuBarEvent> events{};
	};
}