KalaInput is a lightweight C++ 20 library for Windows that handles input detection, this copy of KalaInput source code is natively built into KalaWindow and does not need to be separately built.

---

# Runtime loop input functions

Call these functions AFTER you call KalaInput::Update().

Pass one of any of the keys in KalaInput Key enum as a parameter for most of these functions where Key is requested.

```cpp
//the variable of Key that can be declared anywhere
KalaKit::Key yourKey;

//detect which key is currently held
bool isKeyDown = KalaInput::IsKeyHeld(yourKey);

//detect which key is currently held
bool isKeyPressed = KalaInput::IsKeyPressed(yourKey);

//detect if a combination of keys is pressed
//you must hold each key in order of the initializer list
//and once you press the last key the combo returns as true
static const std::initializer_list<Key> saveCombo
{
    KalaKit::Key::LeftControl,
    KalaKit::Key::S
};
bool isComboPressed = KalaInput::IsComboPressed(saveCombo);

//detect if either left or right mouse key was double-clicked.
//this does not need a reference to any Key
bool isDoubleClicked = KalaInput::IsMouseKeyDoubleClicked();

//detect if either left or right mouse key is held 
//and mouse is dragged in any direction.
//this does not need a reference to any Key
bool isMouseDragging = KalaInput::IsMouseDragging();

//get current mouse position relative to the client area (top-left = 0,0).
//coordinates are in pixels
POINT mousePos = KalaInput::GetMousePosition();

//get how much the cursor moved on screen (in client space) since the last frame.
//this uses absolute screen-based movement, affected by OS acceleration and DPI
POINT mouseDelta = KalaInput::GetMouseDelta();

//get raw, unfiltered mouse movement from the hardware since the last frame.
//not affected by DPI, sensitivity, or OS mouse settings, ideal for game camera control
POINT rawMouseDelta = KalaInput::GetRawMouseDelta();

//get how many scroll steps the mouse wheel moved since the last frame.
//positive = scroll up, negative = scroll down
int mouseWheelDelta = KalaInput::GetMouseWheelDelta();

//returns true if cursor is not hidden
bool isMouseVisible = KalaInput::IsMouseVisible();

//allows to set the visibility state of the cursor,
//if true, then the cursor is visible
bool visibilityState = true;
KalaInput::SetMouseVisibility(visibilityState);

//returns true if cursor is locked
bool isMouseLocked = KalaInput::IsMouseLocked();

//allows to set the lock state of the cursor,
//if true, then the cursor is locked
bool lockState = true;
KalaInput::SetMouseLockState(lockState);
```
