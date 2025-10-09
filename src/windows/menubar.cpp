//Copyright(C) 2025 Lost Empire Entertainment
//This program comes with ABSOLUTELY NO WARRANTY.
//This is free software, and you are welcome to redistribute it under certain conditions.
//Read LICENSE.md for more information.

#ifdef _WIN32

#include <Windows.h>
#include <sstream>
#include <memory>

#include "KalaHeaders/log_utils.hpp"

#include "windows/menubar.hpp"
#include "graphics/window.hpp"
#include "core/containers.hpp"

using namespace KalaHeaders;

using KalaWindow::Graphics::WindowData;
using KalaWindow::Core::globalID;
using KalaWindow::Core::createdMenuBarEvents;
using KalaWindow::Core::runtimeMenuBarEvents;

using std::ostringstream;
using std::to_string;
using std::unique_ptr;
using std::make_unique;
using std::wstring;

static wstring ToWide(const string& str);

constexpr u8 MAX_LABEL_LENGTH = 64;

namespace KalaWindow::Windows
{
	void MenuBar::CreateMenuBar(Window* windowRef)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);
		if (!window) return;

		WindowData wData = windowRef->GetWindowData();

		if (IsInitialized(windowRef))
		{
			Log::Print(
				"Failed to add menu bar to window '" + windowRef->GetTitle() + "' because the window already has one!",
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		HMENU hMenu = CreateMenu();
		wData.hMenu = FromVar(hMenu);
		windowRef->SetWindowData(wData);

		SetMenu(window, hMenu);
		DrawMenuBar(window);

		isEnabled = true;

		ostringstream oss{};
		oss << "Created new menu bar in window '" << windowRef->GetTitle() << "'!";

		Log::Print(
			oss.str(),
			"MENU_BAR",
			LogType::LOG_SUCCESS);
	}
	bool MenuBar::IsInitialized(Window* windowRef)
	{
		return (windowRef->GetWindowData().hMenu != NULL);
	}

	void MenuBar::SetMenuBarState(
		bool state,
		Window* windowRef)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);
		if (!window) return;

		HMENU storedHMenu = ToVar<HMENU>(windowRef->GetWindowData().hMenu);

		if (!IsInitialized(windowRef))
		{
			Log::Print(
				"Failed to set menu bar state for window '" + windowRef->GetTitle() + "' because it has not yet created a menu bar!",
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		SetMenu(window, state ? storedHMenu : nullptr);
		DrawMenuBar(window);

		string val = state ? "true" : "false";
		isEnabled = state;

		if (MenuBar::IsVerboseLoggingEnabled())
		{
			Log::Print(
				"Set window '" + windowRef->GetTitle() + "' menu bar state to '" + val + "'",
				"MENU_BAR",
				LogType::LOG_SUCCESS);
		}
	}
	bool MenuBar::IsEnabled(Window* window)
	{
		HWND hwnd = ToVar<HWND>(window->GetWindowData().hwnd);
		HMENU attached = GetMenu(hwnd);

		return
			attached != NULL
			&& window->GetWindowData().hMenu != NULL
			&& isEnabled;
	}

	void MenuBar::CreateLabel(
		Window* windowRef,
		LabelType type,
		const string& parentRef,
		const string& labelRef,
		const function<void()> func)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);
		if (!window) return;

		string typeName = type == LabelType::LABEL_LEAF ? "leaf" : "branch";

		string parentName = parentRef;
		if (parentName.empty()) parentName = "root";

		if (!IsInitialized(windowRef))
		{
			ostringstream oss{};
			oss << "Failed to add " << typeName << " to window '" << windowRef->GetTitle() << "' because no menu bar was created!";

			Log::Print(
				oss.str(),
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (labelRef.empty())
		{
			ostringstream oss{};
			oss << "Failed to add " << typeName << " to window '" << windowRef->GetTitle() << "' because the label name is empty!";

			Log::Print(
				oss.str(),
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

			return;
		}
		if (labelRef.length() > MAX_LABEL_LENGTH)
		{
			ostringstream oss{};
			oss << "Failed to add " << typeName << " '" << labelRef
				<< "' to window '" << windowRef->GetTitle() << "' because the label length '"
				<< labelRef.length() << "' is too long! You can only use label length up to '"
				<< to_string(MAX_LABEL_LENGTH) << "' characters long.";

			Log::Print(
				oss.str(),
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		//leaf requires valid function
		if (type == LabelType::LABEL_LEAF
			&& !func)
		{
			ostringstream oss{};
			oss << "Failed to add leaf '" << labelRef << "' under parent '" << parentRef
				<< "' in window '" << windowRef->GetTitle() << "' because the leaf has an empty function!";

			Log::Print(
				oss.str(),
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		//leaf cant have parent that is also a leaf
		if (type == LabelType::LABEL_LEAF
			&& !parentRef.empty())
		{
			for (const auto& e : runtimeMenuBarEvents)
			{
				if (e->label == parentRef
					&& e->labelID != 0)
				{
					ostringstream oss{};
					oss << "Failed to add leaf '" << labelRef << "' under parent '" << parentRef
						<< "' in window '" << windowRef->GetTitle() << "' because the parent is also a leaf!";

					Log::Print(
						oss.str(),
						"MENU_BAR",
						LogType::LOG_ERROR,
						2);

					return;
				}
			}
		}

		//check if label or the parent of the label already exists or not
		for (const auto& e : runtimeMenuBarEvents)
		{
			const string& parent = e->parentLabel;
			const string& label = e->label;
			if (parent.empty()
				&& labelRef == label)
			{
				ostringstream oss{};
				oss << "Failed to add " << typeName << " '" << labelRef << "' to window '" << windowRef->GetTitle()
					<< "' because the " << typeName << " already exists!";

				Log::Print(
					oss.str(),
					"MENU_BAR",
					LogType::LOG_ERROR,
					2);

				return;
			}
			else if (parentRef == parent
				&& labelRef == label)
			{
				ostringstream oss{};
				oss << "Failed to add " << typeName << " '" << labelRef << "' under parent '" << parentName
					<< "' in window '" << windowRef->GetTitle()
					<< "' because the " << typeName << " and its parent already exists!";

				Log::Print(
					oss.str(),
					"MENU_BAR",
					LogType::LOG_ERROR,
					2);

				return;
			}
		}

		HMENU hMenu = GetMenu(window);
		u32 newID = ++globalID;

		unique_ptr<MenuBarEvent> newEvent = make_unique<MenuBarEvent>();
		newEvent->parentLabel = parentRef;
		newEvent->label = labelRef;

		if (type == LabelType::LABEL_LEAF)
		{
			newEvent->function = func;
			newEvent->labelID = newID;
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

					newEvent->hMenu = FromVar(thisMenu);
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
					ostringstream oss{};
					oss << "Added " << typeName << " '" << labelRef << "' with ID '" << to_string(newID)
						<< "' under parent '" << parentName
						<< "' in window '" << windowRef->GetTitle() << "'!";

					Log::Print(
						oss.str(),
						"MENU_BAR",
						LogType::LOG_SUCCESS);
				}
			};

		if (parentRef.empty()) NewLabel(hMenu);
		else
		{
			HMENU parentMenu{};

			for (const auto& value : runtimeMenuBarEvents)
			{
				if (value->label == parentRef)
				{
					parentMenu = ToVar<HMENU>(value->hMenu);
					break;
				}
			}

			if (!parentMenu)
			{
				ostringstream oss{};
				oss << "Failed to create " << typeName << " '" << labelRef << "' under parent '" << parentName
					<< "' in window '" << windowRef->GetTitle() << "' because the parent does not exist!";

				Log::Print(
					oss.str(),
					"MENU_BAR",
					LogType::LOG_ERROR,
					2);

				return;
			}

			NewLabel(parentMenu);
		}

		DrawMenuBar(window);

		createdMenuBarEvents[newID] = move(newEvent);

		MenuBarEvent* storedEvent = createdMenuBarEvents[newID].get();
		runtimeMenuBarEvents.push_back(storedEvent);
	}

	void MenuBar::AddSeparator(
		Window* windowRef,
		const string& parentRef,
		const string& labelRef)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);
		if (!window) return;

		if (!IsInitialized(windowRef))
		{
			ostringstream oss{};
			oss << "Failed to add separator to menu label '" << labelRef << "' in window '" << windowRef->GetTitle()
				<< "' because it has no menu bar!";

			Log::Print(
				oss.str(),
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		HMENU hMenu = GetMenu(window);

		for (const auto& e : runtimeMenuBarEvents)
		{
			const string& parent = e->parentLabel;
			const string& label = e->label;
			if (label.empty())
			{
				if (parent == parentRef)
				{
					HMENU parentMenu{};

					for (const auto& value : runtimeMenuBarEvents)
					{
						if (value->label == parentRef)
						{
							parentMenu = ToVar<HMENU>(value->hMenu);
							break;
						}
					}

					if (!parentMenu)
					{
						ostringstream oss{};
						oss << "Failed to add separator at the end of parent '" << parentRef
							<< "' in window '" << windowRef->GetTitle() << "' because the parent does not exist!";

						Log::Print(
							oss.str(),
							"MENU_BAR",
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
							"Placed separator to the end of parent label '" + parentRef + "' in window '" + windowRef->GetTitle() + "'!",
							"MENU_BAR",
							LogType::LOG_SUCCESS);
					}

					DrawMenuBar(window);

					return;
				}
			}
			else
			{
				if (parent == parentRef
					&& label == labelRef)
				{
					HMENU parentMenu{};

					for (const auto& value : runtimeMenuBarEvents)
					{
						if (value->label == parentRef)
						{
							parentMenu = ToVar<HMENU>(value->hMenu);
							break;
						}
					}

					if (!parentMenu)
					{
						ostringstream oss{};
						oss << "Failed to add separator under parent '" << parentRef << "' after label '" << labelRef
							<< "' in window '" << windowRef->GetTitle() << "' because the label does not exist!";

						Log::Print(
							oss.str(),
							"MENU_BAR",
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
									"Placed separator after label '" + labelRef + "' in window '" + windowRef->GetTitle() + "'!",
									"MENU_BAR",
									LogType::LOG_SUCCESS);
							}

							DrawMenuBar(window);

							return;
						}
					}
				}
			}
		}

		ostringstream oss{};
		oss << "Failed to add separator at the end of parent '" << parentRef << "' or after label '" << labelRef
			<< "' in window '" + windowRef->GetTitle() + "' because parent or label does not exist!";

		Log::Print(
			oss.str(),
			"MENU_BAR",
			LogType::LOG_ERROR,
			2);
	}

	void MenuBar::DestroyMenuBar(Window* windowRef)
	{
		HWND window = ToVar<HWND>(windowRef->GetWindowData().hwnd);
		if (!window) return;

		if (!IsInitialized(windowRef))
		{
			Log::Print(
				"Cannot destroy menu bar for window '" + windowRef->GetTitle() + "' because it hasn't created one!",
				"MENU_BAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		HMENU hMenu = GetMenu(window);

		//detach the menu bar from the window first
		SetMenu(window, nullptr);
		DrawMenuBar(window);

		//and finally destroy the menu handle itself
		DestroyMenu(hMenu);

		ostringstream oss{};
		oss << "Destroyed menu bar in window '" << windowRef->GetTitle() << "'!";

		Log::Print(
			oss.str(),
			"MENU_BAR",
			LogType::LOG_SUCCESS);
	}
}

wstring ToWide(const string& str)
{
	if (str.empty()) return wstring();

	int size_needed = MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		-1,
		nullptr,
		0);

	wstring wstr(size_needed - 1, 0);

	MultiByteToWideChar(
		CP_UTF8,
		0,
		str.c_str(),
		-1,
		wstr.data(),
		size_needed);

	return wstr;
}

#endif //_WIN32