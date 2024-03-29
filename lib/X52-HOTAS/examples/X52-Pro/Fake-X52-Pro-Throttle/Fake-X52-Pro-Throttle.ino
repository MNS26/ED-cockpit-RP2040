// This example is for the X52 Pro. It isn't compatible with the non-Pro hardware.
//
// This firmware queries the state of the X52 Pro joystick through the PS/2 cable
// and can do various things with it (for example print it to the serial console).
//
// If you use a board that can appear to the PC as a generic USB game controller
// then you can flip the USB_JOYSTICK_TEENSY or USB_JOYSTICK_MHEIRONIMUS switch to
// emulate a USB game controller with the state of the physical X52 Pro joystick.
// These switches are optional just like the two libraries they depend on.
// These aren't the only USB game controller libraries out there.
// I picked them as examples because they cover a wide range of popular boards.
// Your Arduino may require the use of a different board-specific library.
//
// My X52 Pro joystick doesn't have a power rating printed on it and the X52 Pro
// throttle powers it with an odd 4.1-4.2V. It may work with 5V but I didn't
// want to take risks so tried it only with 3.3V that works well.
//
// The C02 input pin of my X52 Pro joystick has extremely low impedance. I don't
// know if this is by design (in all production units) or partial (ESD) damage
// affecting only my joystick. My old X52 non-Pro joystick doesn't have this "issue".
// This has at least two important implications in my case:
//
// - I can interface with my X52 Pro joystick's C02 input pin only using my
//   teensy boards that have MCUs with low enough output pin impedance.
//   I tried an ATmega32U4 based 3.3V board too but its output pins couldn't
//   handle the joystick's low impedance C02 input pin (without additional
//   external components).
//
// - The current flowing through the C02 pin is relatively high (a few milliamps)
//   instead of "almost zero" (micro- or nanoamps) that I'd expect from a
//   general-purpose high impedance digital input pin. An update rate of 250
//   frames/sec results in a current of 4mA with the teensy 3.2 and about 6mA
//   with the high drive pins of the teensy LC. The teensy 3.2 output pins are
//   rated for 10mA while the teensy LC pins are rated for 5mA. The teensy LC
//   has four special high drive pins rated for 20mA: 5, 16, 17, 21. Connect the
//   joystick's input pins (C01, C02) to those just to be on the safe side.
//   https://www.pjrc.com/teensy/pinout.html
//
// Tested with:
// - Hardware: PJRC teensy 3.2, PJRC teensy LC
// - Arduino IDE 1.8.13
// - Teensyduino 1.53 (add-on for the Arduino IDE)
// - https://github.com/MHeironimus/ArduinoJoystickLibrary 2.0.7

#define X52_DEBUG 1
#define X52_PRO_IMPROVED_JOYSTICK_CLIENT_DESYNC_DETECTION 1
#include <x52_hotas.h>


// LOG_JOYSTICK_STATE periodically logs the joystick state to serial if X52_DEBUG=1.
#define LOG_JOYSTICK_STATE 1

// USB_JOYSTICK_TEENSY uses the teensy joystick library to emulate a USB joystick.
// You can use this option only with a PJRC teensy board. The Arduino IDE must
// have the teensyduino addon that contains the teensy joystick library.
#define USB_JOYSTICK_TEENSY 0

// USB_JOYSTICK_MHEIRONIMUS emulates a USB joystick with the help of the
// following library: https://github.com/MHeironimus/ArduinoJoystickLibrary
// https://github.com/MHeironimus/ArduinoJoystickLibrary/wiki/Supported-Boards
// You have to install that library before building with this switch enabled.
#define USB_JOYSTICK_MHEIRONIMUS 0

// MAX_UPDATES_PER_SECOND puts a max limit on the number of updates the
// joystick can push to your board in one second. My X52 Pro joystick is capable
// of 250-400 updates per second. If the joystick is idle then the rate
// is around 400. As soon as I start pressing buttons on the joystick
// handle the number dips down close to 250. The reason: there are 2 MCUs
// in the joystick: one in the base (the mainboard) and one in the handle.
// The handle-MCU sends messages to the mainboard-MCU in an event-driven way
// only when the button states change.
// I prefer a stable update rate - that's why I limit it to 250.
// Set MAX_UPDATES_PER_SECOND to zero for unlimited update rate: it will
// fluctuate wildly between 250 and 400.
#define MAX_UPDATES_PER_SECOND 250

// RATE_LOG_PERIOD_MILLIS defines the period for the update rate logger.
// Rate logging is disabled if X52_DEBUG or RATE_LOG_PERIOD_MILLIS is zero.
#define RATE_LOG_PERIOD_MILLIS 3000


// TODO: Choose your favorite digital pins on your board.
x52::pro::JoystickClient<16, 5, 3, 2> joystick_client;


#if USB_JOYSTICK_MHEIRONIMUS
#include <Joystick.h>
Joystick_ joystick;
#endif


void setup() {
#if X52_DEBUG
	Serial.begin(9600);
#endif

#if USB_JOYSTICK_TEENSY
	Joystick.useManualSend(true);
#endif

#if USB_JOYSTICK_MHEIRONIMUS
	joystick.begin(false);
#endif

	joystick_client.Setup();

	// TODO: Deal with unused/floating input pins if you want to do it by the book.
}

// Rhodes island
// 262mm x 126mm x 79mm (1/2500)
// 655m x 315m x 197.5m (1/1)

void loop() {
	// Calling PrepareForPoll is optional (this firmware works without it)
	// but it eliminates almost all jitter from the timing of the updates.
	// With PrepareForPoll we ask the joystick to prepare for the next
	// update so it can serve our PollJoystickState call immediately
	// after the delay introduced by the rate limiter.
	// Without PrepareForPoll it can take up to 1500ms for the joystick
	// to respond to our PollJoystickState call.
	joystick_client.PrepareForPoll();

#if MAX_UPDATES_PER_SECOND
	static x52::util::RateLimiter<MAX_UPDATES_PER_SECOND,MAX_UPDATES_PER_SECOND> rate_limiter;
	unsigned long d = rate_limiter.MicrosTillNextUpdate();
	if (d > 0) {
#if !X52_BUSY_WAIT
		delayMicroseconds(d);
#endif
		return;
	}
#endif

	if (update_joystick()) {
#if X52_DEBUG && RATE_LOG_PERIOD_MILLIS
		static x52::util::RateLogger<RATE_LOG_PERIOD_MILLIS> rate_logger;
		rate_logger.OnUpdate();
#endif
	}
}


// Query the state of the X52 Pro joystick via the PS/2 connection and send it to
// the PC as the state of the USB joystick emulated by the Arduino-compatible board.
bool update_joystick() {
	x52::pro::JoystickState state;
	x52::pro::JoystickConfig cfg;

	// ~50% brightness because that's what I prefer
	cfg.led_brightness = x52::pro::JoystickConfig::MAX_LED_BRIGHTNESS / 2;

	// TODO: set other JoystickConfig values if you want

	auto timeout_micros = joystick_client.PollJoystickState(state, cfg);
	if (timeout_micros) {
		X52DebugPrintln("PollJoystickState failed");
		delayMicroseconds(timeout_micros);
		return false;
	}

#if X52_DEBUG && LOG_JOYSTICK_STATE
	serial_log_joystick_state(state);
#endif

#if USB_JOYSTICK_TEENSY
	send_teensy_usb_joystick_state(state);
#endif

#if USB_JOYSTICK_MHEIRONIMUS
	send_mheironimus_usb_joystick_state(state);
#endif
	return true;
}

#if X52_DEBUG && LOG_JOYSTICK_STATE

void serial_log_joystick_state(const x52::pro::JoystickState& state) {
	// DebugPrinting after every 100th call
	static int counter = 0;
	if (counter++ % 100 != 0)
		return;

	X52DebugPrint("x:");
	X52DebugPrint(state.x);
	X52DebugPrint(" y:");
	X52DebugPrint(state.y);
	X52DebugPrint(" z:");
	X52DebugPrint(state.z);

	X52DebugPrint(" pov1:");
	if (state.pov_1 & x52::Up)
		X52DebugPrint("Up");
	if (state.pov_1 & x52::Down)
		X52DebugPrint("Down");
	if (state.pov_1 & x52::Left)
		X52DebugPrint("Left");
	if (state.pov_1 & x52::Right)
		X52DebugPrint("Right");

	X52DebugPrint(" pov2:");
	if (state.pov_2 & x52::Up)
		X52DebugPrint("Up");
	if (state.pov_2 & x52::Down)
		X52DebugPrint("Down");
	if (state.pov_2 & x52::Left)
		X52DebugPrint("Left");
	if (state.pov_2 & x52::Right)
		X52DebugPrint("Right");

	X52DebugPrint(" mode:");
	switch (state.mode) {
		case x52::ModeUndefined: X52DebugPrint("Undefined"); break;
		case x52::Mode1: X52DebugPrint("Mode1"); break;
		case x52::Mode2: X52DebugPrint("Mode2"); break;
		case x52::Mode3: X52DebugPrint("Mode3"); break;
	}

	X52DebugPrint("\ntrigger_stage_1:");
	X52DebugPrint(state.trigger_stage_1);
	X52DebugPrint(" trigger_stage_2:");
	X52DebugPrint(state.trigger_stage_2);
	X52DebugPrint(" pinkie:");
	X52DebugPrint(state.pinkie_switch);
	X52DebugPrint(" fire:");
	X52DebugPrint(state.button_fire);

	X52DebugPrint("\nA:");
	X52DebugPrint(state.button_a);
	X52DebugPrint(" B:");
	X52DebugPrint(state.button_b);
	X52DebugPrint(" C:");
	X52DebugPrint(state.button_c);

	X52DebugPrint(" T1:");
	X52DebugPrint(state.button_t1);
	X52DebugPrint(" T2:");
	X52DebugPrint(state.button_t2);
	X52DebugPrint(" T3:");
	X52DebugPrint(state.button_t3);
	X52DebugPrint(" T4:");
	X52DebugPrint(state.button_t4);
	X52DebugPrint(" T5:");
	X52DebugPrint(state.button_t5);
	X52DebugPrint(" T6:");
	X52DebugPrint(state.button_t6);

	X52DebugPrint("\n");
}

#endif

#if USB_JOYSTICK_TEENSY

// This function is specific to teensy boards and uses the joystick library
// of the Teensyduno addon so it's unlikely to work with other types of boards.
void send_teensy_usb_joystick_state(const x52::pro::JoystickState& state) {
	Joystick.X(state.x);
	Joystick.Y(state.y);
	Joystick.Zrotate(state.z);

	static int hat_angles[16] = {
		-1, 90, 180, 135, 270, -1, 225, 180, 0, 45, -1, 90, 315, 0, 270, -1
	};
	Joystick.hat(hat_angles[state.pov_1]);

	// These button numbers may look random but I just tried
	// to increase compatibility with xbox and ps controllers.

	// The teensy joystick library supports only 1 hat switch
	// so pov_2 is used as 4 separate buttons:
	Joystick.button(13, bool(state.pov_2 & x52::Up));
	Joystick.button(16, bool(state.pov_2 & x52::Right));
	Joystick.button(14, bool(state.pov_2 & x52::Down));
	Joystick.button(15, bool(state.pov_2 & x52::Left));

	// This library uses one-based button indexes [1..32]
	Joystick.button(8, state.trigger_stage_1);
	Joystick.button(29, state.trigger_stage_2);
	Joystick.button(7, state.pinkie_switch);
	Joystick.button(4, state.button_fire);
	Joystick.button(1, state.button_a);
	Joystick.button(2, state.button_b);
	Joystick.button(3, state.button_c);
	Joystick.button(9, state.button_t1);
	Joystick.button(5, state.button_t2);
	Joystick.button(10, state.button_t3);
	Joystick.button(6, state.button_t4);
	Joystick.button(11, state.button_t5);
	Joystick.button(12, state.button_t6);
	Joystick.button(30, state.mode == x52::Mode1);
	Joystick.button(31, state.mode == x52::Mode2);
	Joystick.button(32, state.mode == x52::Mode3);

	Joystick.send_now();
}

#endif

#if USB_JOYSTICK_MHEIRONIMUS

void send_mheironimus_usb_joystick_state(const x52::pro::JoystickState& state) {
	joystick.setXAxis(state.x);
	joystick.setYAxis(state.y);
	joystick.setRzAxis(state.z);

	static int hat_angles[16] = {
		-1, 90, 180, 135, 270, -1, 225, 180, 0, 45, -1, 90, 315, 0, 270, -1
	};
	joystick.setHatSwitch(0, hat_angles[state.pov_1]);

	// This joystick library supports two hat switches so you can decide
	// whether to use pov_2 as the second hat switch or 4 separate buttons:

#if 0
	joystick.setHatSwitch(1, hat_angles[state.pov_2]);
#else
	joystick.setButton(12, bool(state.pov_2 & x52::Up));
	joystick.setButton(15, bool(state.pov_2 & x52::Right));
	joystick.setButton(13, bool(state.pov_2 & x52::Down));
	joystick.setButton(14, bool(state.pov_2 & x52::Left));
#endif

	// This library uses zero-based button indexes [0..31]
	joystick.setButton(7, state.trigger_stage_1);
	joystick.setButton(28, state.trigger_stage_2);
	joystick.setButton(6, state.pinkie_switch);
	joystick.setButton(3, state.button_fire);
	joystick.setButton(0, state.button_a);
	joystick.setButton(1, state.button_b);
	joystick.setButton(2, state.button_c);
	joystick.setButton(8, state.button_t1);
	joystick.setButton(4, state.button_t2);
	joystick.setButton(9, state.button_t3);
	joystick.setButton(5, state.button_t4);
	joystick.setButton(10, state.button_t5);
	joystick.setButton(11, state.button_t6);
	joystick.setButton(29, state.mode == x52::Mode1);
	joystick.setButton(30, state.mode == x52::Mode2);
	joystick.setButton(31, state.mode == x52::Mode3);

	joystick.sendState();
}

#endif
