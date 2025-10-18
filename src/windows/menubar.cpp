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
#include "core/core.hpp"

using namespace KalaHeaders;

using KalaWindow::Graphics::Window;
using KalaWindow::Graphics::TargetType;
using KalaWindow::Graphics::WindowData;
using KalaWindow::Core::globalID;

using std::ostringstream;
using std::to_string;
using std::unique_ptr;
using std::make_unique;
using std::wstring;

static wstring ToWide(const string& str);

constexpr u8 MAX_LABEL_LENGTH = 64;

namespace KalaWindow::Windows
{
	MenuBar* MenuBar::CreateMenuBar(u32 windowID)
	{
		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot create menu bar because its window reference is invalid!",
				"MENUBAR",
				LogType::LOG_ERROR);

			return nullptr;
		}

		u32 newID = ++globalID;
		unique_ptr<MenuBar> newMenu = make_unique<MenuBar>();
		MenuBar* menuPtr = newMenu.get();

		Log::Print(
			"Creating new menu bar for window '" + window->GetTitle() + "' with ID '" + to_string(newID) + "'.",
			"MENUBAR",
			LogType::LOG_DEBUG);

		HWND hwnd = ToVar<HWND>(window->GetWindowData().hwnd);

		WindowData wData = window->GetWindowData();

		HMENU hMenu = CreateMenu();
		wData.hMenu = FromVar(hMenu);
		window->SetWindowData(wData);

		SetMenu(hwnd, hMenu);
		DrawMenuBar(hwnd);

		registry.AddContent(newID, move(newMenu));
		window->AddValue(TargetType::TYPE_MENU_BAR, newID);

		menuPtr->isInitialized = true;
		menuPtr->isEnabled = true;

		Log::Print(
			"Created new menu bar for window '" + window->GetTitle() + "' with ID '" + to_string(newID) + "'!",
			"MENUBAR",
			LogType::LOG_SUCCESS);

		return menuPtr;
	}
	bool MenuBar::IsInitialized() const
	{
		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot check if menu bar is initialized because its window reference is invalid!",
				"MENUBAR",
				LogType::LOG_ERROR);

			return false;
		}

		return isInitialized
			&& window->GetWindowData().hMenu != NULL;
	}

	void MenuBar::SetMenuBarState(bool state)
	{
		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot set menu bar state because its window reference is invalid!",
				"MENUBAR",
				LogType::LOG_ERROR);

			return;
		}

		HWND hwnd = ToVar<HWND>(window->GetWindowData().hwnd);
		if (!window) return;

		HMENU storedHMenu = ToVar<HMENU>(window->GetWindowData().hMenu);

		if (!isInitialized)
		{
			Log::Print(
				"Failed to set menu bar state for window '" + window->GetTitle() + "' because it has not yet created a menu bar!",
				"MENUBAR",
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
				"MENUBAR",
				LogType::LOG_SUCCESS);
		}
	}
	bool MenuBar::IsEnabled() const
	{
		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot check if menu bar is enabled because its window reference is invalid!",
				"MENUBAR",
				LogType::LOG_ERROR);

			return false;
		}

		HWND hwnd = ToVar<HWND>(window->GetWindowData().hwnd);
		HMENU attached = GetMenu(hwnd);

		return
			attached != NULL
			&& window->GetWindowData().hMenu != NULL
			&& isEnabled;
	}

	void MenuBar::CreateLabel(
		LabelType type,
		const string& parentRef,
		const string& labelRef,
		const function<void()> func)
	{
		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot create label for menu bar because its window reference is invalid!",
				"MENUBAR",
				LogType::LOG_ERROR);

			return;
		}

		HWND hwnd = ToVar<HWND>(window->GetWindowData().hwnd);

		string typeName = type == LabelType::LABEL_LEAF ? "leaf" : "branch";

		string parentName = parentRef;
		if (parentName.empty()) parentName = "root";

		if (!isInitialized)
		{
			ostringstream oss{};
			oss << "Failed to add " << typeName << " to window '" << window->GetTitle() << "' because no menu bar was created!";

			Log::Print(
				oss.str(),
				"MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		if (labelRef.empty())
		{
			ostringstream oss{};
			oss << "Failed to add " << typeName << " to window '" << window->GetTitle() << "' because the label name is empty!";

			Log::Print(
				oss.str(),
				"MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}
		if (labelRef.length() > MAX_LABEL_LENGTH)
		{
			ostringstream oss{};
			oss << "Failed to add " << typeName << " '" << labelRef
				<< "' to window '" << window->GetTitle() << "' because the label length '"
				<< labelRef.length() << "' is too long! You can only use label length up to '"
				<< to_string(MAX_LABEL_LENGTH) << "' characters long.";

			Log::Print(
				oss.str(),
				"MENUBAR",
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
				<< "' in window '" << window->GetTitle() << "' because the leaf has an empty function!";

			Log::Print(
				oss.str(),
				"MENUBAR",
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
					ostringstream oss{};
					oss << "Failed to add leaf '" << labelRef << "' under parent '" << parentRef
						<< "' in window '" << window->GetTitle() << "' because the parent is also a leaf!";

					Log::Print(
						oss.str(),
						"MENUBAR",
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
				ostringstream oss{};
				oss << "Failed to add " << typeName << " '" << labelRef << "' to window '" << window->GetTitle()
					<< "' because the " << typeName << " already exists!";

				Log::Print(
					oss.str(),
					"MENUBAR",
					LogType::LOG_ERROR,
					2);

				return;
			}
			else if (parentRef == parent
				&& labelRef == label)
			{
				ostringstream oss{};
				oss << "Failed to add " << typeName << " '" << labelRef << "' under parent '" << parentName
					<< "' in window '" << window->GetTitle()
					<< "' because the " << typeName << " and its parent already exists!";

				Log::Print(
					oss.str(),
					"MENUBAR",
					LogType::LOG_ERROR,
					2);

				return;
			}
		}

		HMENU hMenu = GetMenu(hwnd);
		u32 newID = ++globalID;

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
					ostringstream oss{};
					oss << "Added " << typeName << " '" << labelRef << "' with ID '" << to_string(newID)
						<< "' under parent '" << parentName
						<< "' in window '" << window->GetTitle() << "'!";

					Log::Print(
						oss.str(),
						"MENUBAR",
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
				ostringstream oss{};
				oss << "Failed to create " << typeName << " '" << labelRef << "' under parent '" << parentName
					<< "' in window '" << window->GetTitle() << "' because the parent does not exist!";

				Log::Print(
					oss.str(),
					"MENUBAR",
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
		const string& parentRef,
		const string& labelRef) const
	{
		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot add separator to menu bar because its window reference is invalid!",
				"MENUBAR",
				LogType::LOG_ERROR);

			return;
		}

		HWND hwnd = ToVar<HWND>(window->GetWindowData().hwnd);

		if (!isInitialized)
		{
			ostringstream oss{};
			oss << "Failed to add separator to menu label '" << labelRef << "' in window '" << window->GetTitle()
				<< "' because it has no menu bar!";

			Log::Print(
				oss.str(),
				"MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		HMENU hMenu = GetMenu(hwnd);

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
						ostringstream oss{};
						oss << "Failed to add separator at the end of parent '" << parentRef
							<< "' in window '" << window->GetTitle() << "' because the parent does not exist!";

						Log::Print(
							oss.str(),
							"MENUBAR",
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
							"Placed separator to the end of parent label '" + parentRef + "' in window '" + window->GetTitle() + "'!",
							"MENUBAR",
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
						ostringstream oss{};
						oss << "Failed to add separator under parent '" << parentRef << "' after label '" << labelRef
							<< "' in window '" << window->GetTitle() << "' because the label does not exist!";

						Log::Print(
							oss.str(),
							"MENUBAR",
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
									"MENUBAR",
									LogType::LOG_SUCCESS);
							}

							DrawMenuBar(hwnd);

							return;
						}
					}
				}
			}
		}

		ostringstream oss{};
		oss << "Failed to add separator at the end of parent '" << parentRef << "' or after label '" << labelRef
			<< "' in window '" + window->GetTitle() + "' because parent or label does not exist!";

		Log::Print(
			oss.str(),
			"MENUBAR",
			LogType::LOG_ERROR,
			2);
	}

	MenuBar::~MenuBar()
	{
		if (!isInitialized)
		{
			Log::Print(
				"Cannot destroy menu bar with ID '" + to_string(ID) + "' because it is not initialized!",
				"MENUBAR",
				LogType::LOG_ERROR,
				2);

			return;
		}

		Window* window = Window::registry.GetContent(windowID);

		if (!window
			|| !window->IsInitialized())
		{
			Log::Print(
				"Cannot destroy menu bar with ID '" + to_string(ID) + "' because its window was not found!",
				"MENUBAR",
				LogType::LOG_ERROR);

			return;
		}

		HWND hwnd = ToVar<HWND>(window->GetWindowData().hwnd);

		events.clear();

		HMENU hMenu = GetMenu(hwnd);

		//detach the menu bar from the window first
		SetMenu(hwnd, nullptr);
		DrawMenuBar(hwnd);

		//and finally destroy the menu handle itself
		DestroyMenu(hMenu);
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