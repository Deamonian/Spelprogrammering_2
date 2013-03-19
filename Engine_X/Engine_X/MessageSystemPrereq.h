// MessageSystemPrereq.h

#ifndef MESSAGESYSTEMPREREQ_H_INCLUDED
#define MESSAGESYSTEMPREREQ_H_INCLUDED

enum EEventType 
{
	EVT_QUIT,
	EVT_RESIZE,
	EVT_MOUSEMOVE,
	EVT_MOUSEBUTTON,
	EVT_MOUSEWHEEL,
	EVT_CHARACTER,
	EVT_KEYBOARD,

	EVT_MAX
};

enum EMouseButton 
{
	MB_Left,
	MB_Middle,
	MB_Right,
	MB_Xtra0,
	MB_Xtra1,
};

enum EKeyCode 
{
	KC_Invalid,

	KC_Backspace,
	KC_Tab,
	KC_Return,
	KC_Pause,
	KC_Escape,
	KC_Space,
	KC_Pageup,
	KC_Pagedown,
	KC_End,
	KC_Home,
	KC_Left,
	KC_Up,
	KC_Right,
	KC_Down,
	KC_Printscreen,
	KC_Insert,
	KC_Delete,
	KC_Shift,
	KC_Control,
	KC_Alt,
	
	KC_0,
	KC_1,
	KC_2,
	KC_3,
	KC_4,
	KC_5,
	KC_6,
	KC_7,
	KC_8,
	KC_9,

	KC_a,
	KC_b,
	KC_c,
	KC_d,
	KC_e,
	KC_f,
	KC_g,
	KC_h,
	KC_i,
	KC_j,
	KC_k,
	KC_l,
	KC_m,
	KC_n,
	KC_o,
	KC_p,
	KC_q,
	KC_r,
	KC_s,
	KC_t,
	KC_u,
	KC_v,
	KC_w,
	KC_x,
	KC_y,
	KC_z,

	KC_LeftWin,
	KC_RightWin,
	KC_Apps,

	KC_Numpad0,
	KC_Numpad1,
	KC_Numpad2,
	KC_Numpad3,
	KC_Numpad4,
	KC_Numpad5,
	KC_Numpad6,
	KC_Numpad7,
	KC_Numpad8,
	KC_Numpad9,
	KC_NumpadMultiply,
	KC_NumpadAdd,
	KC_NumpadSeparator,
	KC_NumpadSubtract,
	KC_NumpadDecimal,
	KC_NumpadDivide,

	KC_F1,
	KC_F2,
	KC_F3,
	KC_F4,
	KC_F5,
	KC_F6,
	KC_F7,
	KC_F8,
	KC_F9,
	KC_F10,
	KC_F11,
	KC_F12,
	KC_F13,
	KC_F14,
	KC_F15,

	KC_Numlock,
	KC_ScrollLock,

	KC_LeftShift,
	KC_RightShift,
	KC_LeftControl,
	KC_RightControl,
	KC_LeftAlt,
	KC_RightAlt,
};

struct IEvent 
{
	EEventType mType;
};

struct ResizeEvent : IEvent 
{
	ResizeEvent() { mType = EVT_RESIZE; }
	int mWidth, mHeight;
};

struct MouseMoveEvent : IEvent 
{
	MouseMoveEvent() { mType = EVT_MOUSEMOVE; }
	int mX, mY;
};

struct MouseButtonEvent : IEvent 
{
	MouseButtonEvent() { mType = EVT_MOUSEBUTTON; }
	EMouseButton mButton;
	bool mState;
};

struct MouseWheelEvent : IEvent 
{
	MouseWheelEvent() { mType = EVT_MOUSEWHEEL; }
	int mX, mY;
	int mDelta;
};

struct CharacterEvent : IEvent 
{
	CharacterEvent() { mType = EVT_CHARACTER; }
	wchar_t mCode;
};

struct KeyboardEvent : IEvent 
{
	KeyboardEvent() { mType = EVT_KEYBOARD; }
	EKeyCode mKeycode;
	int mMods;
	bool mState;
};

EKeyCode Convert(int vk); 

#endif // MESSAGESYSTEMPREREQ_H_INCLUDED
