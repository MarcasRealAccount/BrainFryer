#pragma once

#include <cstddef>
#include <cstdint>

namespace Brainfryer::Input
{
	namespace Devices
	{
		class Keyboard final
		{
		public:
			static constexpr std::size_t c_MaxKeys = 512;

		public:
			Keyboard()  = default;
			~Keyboard() = default;

			void update();

			void keyPressed(std::uint32_t key);
			void keyRepeated(std::uint32_t key);
			void keyReleased(std::uint32_t key);

			std::uint8_t getState(std::uint32_t key) const;

			bool isKeyPressed(std::uint32_t key) const;
			bool isKeyRepeated(std::uint32_t key) const;
			bool isKeyReleased(std::uint32_t key) const;
			bool isKeyDown(std::uint32_t key) const;

		private:
			std::uint8_t m_Keys[c_MaxKeys] { 0 };
		};
	} // namespace Devices

	namespace ButtonIndices
	{
		static constexpr std::uint32_t KeySpace        = 32;
		static constexpr std::uint32_t KeyApostrophe   = 39;
		static constexpr std::uint32_t KeyComma        = 44;
		static constexpr std::uint32_t KeyMinus        = 45;
		static constexpr std::uint32_t KeyPeriod       = 46;
		static constexpr std::uint32_t KeySlash        = 47;
		static constexpr std::uint32_t Key0            = 48;
		static constexpr std::uint32_t Key1            = 49;
		static constexpr std::uint32_t Key2            = 50;
		static constexpr std::uint32_t Key3            = 51;
		static constexpr std::uint32_t Key4            = 52;
		static constexpr std::uint32_t Key5            = 53;
		static constexpr std::uint32_t Key6            = 54;
		static constexpr std::uint32_t Key7            = 55;
		static constexpr std::uint32_t Key8            = 56;
		static constexpr std::uint32_t Key9            = 57;
		static constexpr std::uint32_t KeySemicolon    = 59;
		static constexpr std::uint32_t KeyEqual        = 61;
		static constexpr std::uint32_t KeyA            = 65;
		static constexpr std::uint32_t KeyB            = 66;
		static constexpr std::uint32_t KeyC            = 67;
		static constexpr std::uint32_t KeyD            = 68;
		static constexpr std::uint32_t KeyE            = 69;
		static constexpr std::uint32_t KeyF            = 70;
		static constexpr std::uint32_t KeyG            = 71;
		static constexpr std::uint32_t KeyH            = 72;
		static constexpr std::uint32_t KeyI            = 73;
		static constexpr std::uint32_t KeyJ            = 74;
		static constexpr std::uint32_t KeyK            = 75;
		static constexpr std::uint32_t KeyL            = 76;
		static constexpr std::uint32_t KeyM            = 77;
		static constexpr std::uint32_t KeyN            = 78;
		static constexpr std::uint32_t KeyO            = 79;
		static constexpr std::uint32_t KeyP            = 80;
		static constexpr std::uint32_t KeyQ            = 81;
		static constexpr std::uint32_t KeyR            = 82;
		static constexpr std::uint32_t KeyS            = 83;
		static constexpr std::uint32_t KeyT            = 84;
		static constexpr std::uint32_t KeyU            = 85;
		static constexpr std::uint32_t KeyV            = 86;
		static constexpr std::uint32_t KeyW            = 87;
		static constexpr std::uint32_t KeyX            = 88;
		static constexpr std::uint32_t KeyY            = 89;
		static constexpr std::uint32_t KeyZ            = 90;
		static constexpr std::uint32_t KeyLeftBracket  = 91;
		static constexpr std::uint32_t KeyBackslash    = 92;
		static constexpr std::uint32_t KeyRightBracket = 93;
		static constexpr std::uint32_t KeyGraveAccent  = 96;
		static constexpr std::uint32_t KeyWorld1       = 161;
		static constexpr std::uint32_t KeyWorld2       = 162;
		static constexpr std::uint32_t KeyEscape       = 256;
		static constexpr std::uint32_t KeyEnter        = 257;
		static constexpr std::uint32_t KeyTab          = 258;
		static constexpr std::uint32_t KeyBackspace    = 259;
		static constexpr std::uint32_t KeyInsert       = 260;
		static constexpr std::uint32_t KeyDelete       = 261;
		static constexpr std::uint32_t KeyRight        = 262;
		static constexpr std::uint32_t KeyLeft         = 263;
		static constexpr std::uint32_t KeyDown         = 264;
		static constexpr std::uint32_t KeyUp           = 265;
		static constexpr std::uint32_t KeyPageUp       = 266;
		static constexpr std::uint32_t KeyPageDown     = 267;
		static constexpr std::uint32_t KeyHome         = 268;
		static constexpr std::uint32_t KeyEnd          = 269;
		static constexpr std::uint32_t KeyCapsLock     = 280;
		static constexpr std::uint32_t KeyScrollLock   = 281;
		static constexpr std::uint32_t KeyNumLock      = 282;
		static constexpr std::uint32_t KeyPrintScreen  = 283;
		static constexpr std::uint32_t KeyPause        = 284;
		static constexpr std::uint32_t KeyF1           = 290;
		static constexpr std::uint32_t KeyF2           = 291;
		static constexpr std::uint32_t KeyF3           = 292;
		static constexpr std::uint32_t KeyF4           = 293;
		static constexpr std::uint32_t KeyF5           = 294;
		static constexpr std::uint32_t KeyF6           = 295;
		static constexpr std::uint32_t KeyF7           = 296;
		static constexpr std::uint32_t KeyF8           = 297;
		static constexpr std::uint32_t KeyF9           = 298;
		static constexpr std::uint32_t KeyF10          = 299;
		static constexpr std::uint32_t KeyF11          = 300;
		static constexpr std::uint32_t KeyF12          = 301;
		static constexpr std::uint32_t KeyF13          = 302;
		static constexpr std::uint32_t KeyF14          = 303;
		static constexpr std::uint32_t KeyF15          = 304;
		static constexpr std::uint32_t KeyF16          = 305;
		static constexpr std::uint32_t KeyF17          = 306;
		static constexpr std::uint32_t KeyF18          = 307;
		static constexpr std::uint32_t KeyF19          = 308;
		static constexpr std::uint32_t KeyF20          = 309;
		static constexpr std::uint32_t KeyF21          = 310;
		static constexpr std::uint32_t KeyF22          = 311;
		static constexpr std::uint32_t KeyF23          = 312;
		static constexpr std::uint32_t KeyF24          = 313;
		static constexpr std::uint32_t KeyF25          = 314;
		static constexpr std::uint32_t KeyKp0          = 320;
		static constexpr std::uint32_t KeyKp1          = 321;
		static constexpr std::uint32_t KeyKp2          = 322;
		static constexpr std::uint32_t KeyKp3          = 323;
		static constexpr std::uint32_t KeyKp4          = 324;
		static constexpr std::uint32_t KeyKp5          = 325;
		static constexpr std::uint32_t KeyKp6          = 326;
		static constexpr std::uint32_t KeyKp7          = 327;
		static constexpr std::uint32_t KeyKp8          = 328;
		static constexpr std::uint32_t KeyKp9          = 329;
		static constexpr std::uint32_t KeyKpDecimal    = 330;
		static constexpr std::uint32_t KeyKpDivide     = 331;
		static constexpr std::uint32_t KeyKpMultiply   = 332;
		static constexpr std::uint32_t KeyKpSubtract   = 333;
		static constexpr std::uint32_t KeyKpAdd        = 334;
		static constexpr std::uint32_t KeyKpEnter      = 335;
		static constexpr std::uint32_t KeyKpEqual      = 336;
		static constexpr std::uint32_t KeyLeftShift    = 340;
		static constexpr std::uint32_t KeyLeftControl  = 341;
		static constexpr std::uint32_t KeyLeftAlt      = 342;
		static constexpr std::uint32_t KeyLeftSuper    = 343;
		static constexpr std::uint32_t KeyRightShift   = 344;
		static constexpr std::uint32_t KeyRightControl = 345;
		static constexpr std::uint32_t KeyRightAlt     = 346;
		static constexpr std::uint32_t KeyRightSuper   = 347;
		static constexpr std::uint32_t KeyMenu         = 348;
	} // namespace ButtonIndices
} // namespace Brainfryer::Input