//Copyright(C) 2026 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <windows.h>
#include <string>
#include <memory>

#include "log_utils.hpp"

#include "graphics/kw_menubar_windows.hpp"
#include "graphics/kw_window.hpp"
#include "core/kw_core.hpp"

using KalaHeaders::KalaCore::ToVar;
using KalaHeaders::KalaCore::FromVar;
using KalaHeaders::KalaLog::Log;
using KalaHeaders::KalaLog::LogType;

using KalaWindow::Graphics::ProcessWindow;
using KalaWindow::Graphics::WindowData;
using KalaWindow::Core::KalaWindowCore;

using std::string;
using std::string_view;
using std::to_string;
using std::unique_ptr;
using std::make_unique;
using std::wstring;

static wstring ToWide(string_view str);

constexpr u8 MAX_LABEL_LENGTH = 64;

namespace KalaWindow::Graphics
{
	static KalaWindowRegistry<MenuBar> registry{};

	static bool isMenuBarVerboseLoggingEnabled{};

	KalaWindowRegistry<MenuBar>& MenuBar::GetRegistry() { return registry; }

	void MenuBar::SetVerboseLoggingState(bool newState) { isMenuBarVerboseLoggingEnabled = newState; }
	bool MenuBar::IsVerboseLoggingEnabled() { return isMenuBarVerboseLoggingEnabled; }

	MenuBar* MenuBar::CreateMenuBar(u32 windowID)
	{
		ProcessWindow* window = ProcessWindow::GetRegistry().GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot create menu bar because its window reference is invalid!",
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return nullptr;
		}

		u32 newID = KalaWindowCore::GetGlobalID() + 1;
		KalaWindowCore::SetGlobalID(newID);

		unique_ptr<MenuBar> newMenu = make_unique<MenuBar>();
		MenuBar* menuPtr = newMenu.get();

		const WindowData& windowData = window->GetWindowData();

		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Menu bar error",
				"Failed to create menu bar because the attached window was invalid!");
		}

		HWND hwnd = ToVar<HWND>(windowData.window);

		WindowData wData = window->GetWindowData();

		HMENU hMenu = CreateMenu();
		wData.hMenu = FromVar(hMenu);
		window->SetWindowData(wData);

		SetMenu(hwnd, hMenu);
		DrawMenuBar(hwnd);

		registry.AddContent(newID, std::move(newMenu));
		window->SetMenuBarID(newID);

		menuPtr->isInitialized = true;
		menuPtr->isEnabled = true;

		Log::Print(
			"Created new menu bar with ID '" + to_string(newID) + "' for window '" + window->GetTitle() + "''!",
			"KW_MENUBAR",
			LogType::LOG_SUCCESS);

		return menuPtr;
	}
	bool MenuBar::IsInitialized() const
	{
		ProcessWindow* window = ProcessWindow::GetRegistry().GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot check if menu bar is initialized because its window reference is invalid!",
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		return isInitialized
			&& window->GetWindowData().hMenu != NULL;
	}

	u32 MenuBar::GetID() const { return ID; }
	u32 MenuBar::GetWindowID() const { return windowID; }

	void MenuBar::SetMenuBarState(bool state)
	{
		ProcessWindow* window = ProcessWindow::GetRegistry().GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot set menu bar state because its window reference is invalid!",
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		const WindowData& windowData = window->GetWindowData();

		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Menu bar error",
				"Failed to set menu bar state because the attached window was invalid!");
		}

		HWND hwnd = ToVar<HWND>(windowData.window);

		HMENU storedHMenu = ToVar<HMENU>(window->GetWindowData().hMenu);

		if (!isInitialized)
		{
			Log::Print(
				"Failed to set menu bar state for window '" + window->GetTitle() + "' because it has not yet created a menu bar!",
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		SetMenu(hwnd, state ? storedHMenu : nullptr);
		DrawMenuBar(hwnd);

		string val = state ? "true" : "false";
		isEnabled = state;

		if (MenuBar::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + window->GetTitle() + "' menu bar state to '" + val + "'",
				"KW_MENUBAR",
				LogType::LOG_SUCCESS);
		}
	}
	bool MenuBar::IsEnabled() const
	{
		ProcessWindow* window = ProcessWindow::GetRegistry().GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot check if menu bar is enabled because its window reference is invalid!",
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return false;
		}

		const WindowData& windowData = window->GetWindowData();

		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Menu bar error",
				"Failed to check if menu bar is enabled because the attached window was invalid!");
		}

		HWND hwnd = ToVar<HWND>(windowData.window);
		HMENU attached = GetMenu(hwnd);

		return
			attached != NULL
			&& window->GetWindowData().hMenu != NULL
			&& isEnabled;
	}

	void MenuBar::CreateLabel(
		LabelType type,
		string_view parentRef,
		string_view labelRef,
		const function<void()> func)
	{
		ProcessWindow* window = ProcessWindow::GetRegistry().GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot create label for menu bar because its window reference is invalid!",
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		const WindowData& windowData = window->GetWindowData();

		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Menu bar error",
				"Failed to create menu bar label because the attached window was invalid!");
		}

		HWND hwnd = ToVar<HWND>(windowData.window);

		string typeName = type == LabelType::LABEL_LEAF ? "leaf" : "branch";

		string parentName = string(parentRef);
		if (parentName.empty()) parentName = "root";

		if (!isInitialized)
		{
			Log::Print(
				"Failed to add " + typeName + " to window '" + window->GetTitle() + "' because no menu bar was created!",
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (labelRef.empty())
		{
			Log::Print(
				"Failed to add " + typeName + " to window '" + window->GetTitle() + "' because the label name is empty!";,
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}
		if (labelRef.length() > MAX_LABEL_LENGTH)
		{
			Log::Print(
				"Failed to add " + typeName + " '" + labelRef
				"' to window '" + window->GetTitle() + "' because the label length '"
				to_string(labelRef.length()) + "' is too long! You can only use label length up to '"
				+ to_string(MAX_LABEL_LENGTH) + "' characters long.",
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		//leaf requires valid function
		if (type == LabelType::LABEL_LEAF
			&& !func)
		{
			Log::Print(
				"Failed to add leaf '" + labelRef + "' under parent '" + parentRef
				+ "' in window '" + window->GetTitle() + "' because the leaf has an empty function!",
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		//leaf cant have parent that is also a leaf
		if (type == LabelType::LABEL_LEAF
			&& !parentRef.empty())
		{
			for (const auto& e : events)
			{
				if (e.label == parentRef
					&& e.labelID != 0)
				{
					Log::Print(
						"Failed to add leaf '" + labelRef + "' under parent '" + parentRef
						+ "' in window '" + window->GetTitle() + "' because the parent is also a leaf!",
						"KW_MENUBAR",
						LogType::LOG_ERROR,
						2);

					return;
				}
			}
		}

		//check if label or the parent of the label already exists or not
		for (const auto& e : events)
		{
			const string& parent = e.parentLabel;
			const string& label = e.label;
			if (parent.empty()
				&& labelRef == label)
			{
				Log::Print(
					"Failed to add " + typeName + " '" + labelRef + "' to window '" + window->GetTitle()
					+ "' because the " + typeName + " already exists!",
					"KW_MENUBAR",
					LogType::LOG_ERROR,
					2);

				return;
			}
			else if (parentRef == parent
				&& labelRef == label)
			{
				Log::Print(
					"Failed to add " + typeName + " '" + labelRef + "' under parent '" + parentName
					+ "' in window '" + window->GetTitle() + "' because the " + typeName + " and its parent already exists!",
					"KW_MENUBAR",
					LogType::LOG_ERROR,
					2);

				return;
			}
		}

		HMENU hMenu = GetMenu(hwnd);

		u32 newID = KalaWindowCore::GetGlobalID() + 1;
		KalaWindowCore::SetGlobalID(newID);

		MenuBarEvent newEvent{};

		newEvent.parentLabel = parentRef;
		newEvent.label = labelRef;

		if (type == LabelType::LABEL_LEAF)
		{
			newEvent.function = func;
			newEvent.labelID = newID;
		}

		auto NewLabel = [&](HMENU parentMenu)
			{
				if (type == LabelType::LABEL_BRANCH)
				{
					HMENU thisMenu = CreatePopupMenu();
					AppendMenu(
						parentMenu,
						MF_POPUP,
						(UINT_PTR)thisMenu,
						ToWide(labelRef).c_str());

					newEvent.hMenu = FromVar(thisMenu);
				}
				else
				{
					AppendMenu(
						parentMenu,
						MF_STRING,
						newID,
						ToWide(labelRef).c_str());
				}

				if (MenuBar::IsVerboseLoggingEnabled())
				{
					Log::Print(
						"Added " + typeName + " '" + labelRef + "' '" + to_string(newID)
						+ "' under parent '" + parentName
						+ "' in window '" + window->GetTitle() + "'!",
						"KW_MENUBAR",
						LogType::LOG_SUCCESS);
				}
			};

		if (parentRef.empty()) NewLabel(hMenu);
		else
		{
			HMENU parentMenu{};

			for (const auto& value : events)
			{
				if (value.label == parentRef)
				{
					parentMenu = ToVar<HMENU>(value.hMenu);
					break;
				}
			}

			if (!parentMenu)
			{
				Log::Print(
					"Failed to create " + typeName + " '" + labelRef + "' under parent '" + parentName
					+ "' in window '" + window->GetTitle() + "' because the parent does not exist!",
					"KW_MENUBAR",
					LogType::LOG_ERROR,
					2);

				return;
			}

			NewLabel(parentMenu);
		}

		DrawMenuBar(hwnd);

		events.push_back(newEvent);
	}

	void MenuBar::AddSeparator(
		string_view parentRef,
		const string& labelRef) const
	{
		ProcessWindow* window = ProcessWindow::GetRegistry().GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot add separator to menu bar because its window reference is invalid!",
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		const WindowData& windowData = window->GetWindowData();

		if (!windowData.window)
		{
			KalaWindowCore::ForceClose(
				"Menu bar error",
				"Failed to add menu bar separator because the attached window was invalid!");
		}

		HWND hwnd = ToVar<HWND>(windowData.window);

		if (!isInitialized)
		{
			Log::Print(
				"Failed to add separator to menu label '" + labelRef + "' in window '" + window->GetTitle()
				+ "' because it has no menu bar!",
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		for (const auto& e : events)
		{
			const string& parent = e.parentLabel;
			const string& label = e.label;
			if (label.empty())
			{
				if (parent == parentRef)
				{
					HMENU parentMenu{};

					for (const auto& value : events)
					{
						if (value.label == parentRef)
						{
							parentMenu = ToVar<HMENU>(value.hMenu);
							break;
						}
					}

					if (!parentMenu)
					{
						Log::Print(
							"Failed to add separator at the end of parent '" + parentRef
							+ "' in window '" + window->GetTitle() + "' because the parent does not exist!",
							"KW_MENUBAR",
							LogType::LOG_ERROR,
							2);

						return;
					}

					AppendMenu(
						parentMenu,
						MF_SEPARATOR,
						0,
						nullptr);

					if (MenuBar::IsVerboseLoggingEnabled())
					{
						Log::Print(
							"Placed separator to the end of parent label '" + string(parentRef) + "' in window '" + window->GetTitle() + "'!",
							"KW_MENUBAR",
							LogType::LOG_SUCCESS);
					}

					DrawMenuBar(hwnd);

					return;
				}
			}
			else
			{
				if (parent == parentRef
					&& label == labelRef)
				{
					HMENU parentMenu{};

					for (const auto& value : events)
					{
						if (value.label == parentRef)
						{
							parentMenu = ToVar<HMENU>(value.hMenu);
							break;
						}
					}

					if (!parentMenu)
					{
						Log::Print(
							"Failed to add separator under parent '" + parentRef + "' after label '" + labelRef
							+ "' in window '" + window->GetTitle() + "' because the label does not exist!",
							"KW_MENUBAR",
							LogType::LOG_ERROR,
							2);

						return;
					}

					int pos = GetMenuItemCount(parentMenu);
					for (int i = 0; i < pos; ++i)
					{
						wchar_t buffer[MAX_LABEL_LENGTH + 1]{};
						GetMenuStringW(
							parentMenu,
							i,
							buffer,
							MAX_LABEL_LENGTH + 1,
							MF_BYPOSITION);

						if (ToWide(labelRef) == buffer)
						{
							InsertMenuW(
								parentMenu,
								i + 1, //insert after this item
								MF_BYPOSITION
								| MF_SEPARATOR,
								0,
								nullptr);

							if (MenuBar::IsVerboseLoggingEnabled())
							{
								Log::Print(
									"Placed separator after label '" + labelRef + "' in window '" + window->GetTitle() + "'!",
									"KW_MENUBAR",
									LogType::LOG_SUCCESS);
							}

							DrawMenuBar(hwnd);

							return;
						}
					}
				}
			}
		}

		Log::Print(
			"Failed to add separator at the end of parent '" + parentRef + "' or after label '" + labelRef
			+ "' in window '" + window->GetTitle() + "' because parent or label does not exist!",
			"KW_MENUBAR",
			LogType::LOG_ERROR,
			2);
	}

	const vector<MenuBarEvent>& MenuBar::GetEvents() const { return events; }

	MenuBar::~MenuBar()
	{
		ProcessWindow* window = ProcessWindow::GetRegistry().GetContent(windowID);
		if (!window)
		{
			Log::Print(
				"Cannot shut down menu bar context because its window was not found!",
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		const WindowData& windowData = window->GetWindowData();

		if (!windowData.window)
		{
			Log::Print(
				"Failed to destroy menu bar because its window was invalid!",
				"KW_MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		Log::Print(
			"Destroying menu bar context with ID '" + to_string(ID) + "' for window '" + w->GetTitle() + "'.",
			"KW_MENUBAR",
			LogType::LOG_INFO);

		HWND hwnd = ToVar<HWND>(windowData.window);

		events.clear();

		HMENU hMenu = GetMenu(hwnd);

		//detach the menu bar from the window first
		SetMenu(hwnd, nullptr);
		DrawMenuBar(hwnd);

		//and finally destroy the menu handle itself
		DestroyMenu(hMenu);
	}
}

wstring ToWide(string_view input)
{
	if (input.empty()) return wstring();

	int size_needed = MultiByteToWideChar(
		CP_UTF8,
		0,
		input.data(),
		scast<int>(input.size()),
		nullptr,
		0);

	wstring wstr(size_needed, 0);

	MultiByteToWideChar(
		CP_UTF8,
		0,
		input.data(),
		scast<int>(input.size()),
		wstr.data(),
		size_needed);

	return wstr;
}

#endif //_WIN32