enum Axis
{
	X = 0,
	Y,
	Z,
	rX,
	rY,
	rZ,
	Slider,
  Dial,
  Wheel,
	MouseX,
	MouseY,
};

enum Hat
{
  CENTER = 0,
  UP,
  UP_RIGHT,
  RIGHT,
  DOWN_RIGHT,
  DOWN,
  DOWN_LEFT,
  LEFT,
  UP_LEFT
};


void press(report *_report, uint8_t b)
{
  _report->buttons |= (uint64_t)1<<(b-1);
}

void release(report *_report, uint8_t b)
{
  _report->buttons &= ~((uint64_t)1<<(b-1));
}

void releaseAll(report *_report)
{
  memset(&_report, 0x00, sizeof(_report));
}

void buttons(report *_report, uint64_t b)
{
  _report->buttons = b;
}

void dpad(report *_report, int dpad, hid_gamepad_hat_t direction)
{
  // Calculate the hat direction based on the given value
  // Assuming you have 9 directions (0-8), where 0 is center

  const uint8_t numDirections = 8;

  // Map the value to a range from 0 to numDirections
  uint8_t mappedValue = (direction % (numDirections + 1));

  if(dpad==1)
    // Adjust the direction to start from 1 (UP) instead of 0 (CENTER)
    _report->Dpad1 = (mappedValue == 0) ? numDirections : mappedValue;
  else if (dpad==2)
    // Adjust the direction to start from 1 (UP) instead of 0 (CENTER)
    _report->Dpad2 = (mappedValue == 0) ? numDirections : mappedValue;

}
