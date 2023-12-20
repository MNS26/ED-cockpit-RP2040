typedef union __attribute__((packed, aligned(1))) {
  // 64 buttons, 9 axis, 2 D-pads
  uint8_t whole8[0];
  uint16_t whole16[0];
  uint32_t whole32[0];
  uint32_t buttons;
  struct 
  {
    uint8_t  button0:1;
    uint8_t  button1:1;
    uint8_t  button2:1;
    uint8_t  button3:1;
    uint8_t  button4:1;
    uint8_t  button5:1;
    uint8_t  button6:1;
    uint8_t  button7:1;
    uint8_t  button8:1;
    uint8_t  button9:1;
    uint8_t  button10:1;
    uint8_t  button11:1;
    uint8_t  button12:1;
    uint8_t  button13:1;
    uint8_t  button14:1;
    uint8_t  button15:1;
    uint8_t  button16:1;
    uint8_t  button17:1;
    uint8_t  button18:1;
    uint8_t  button19:1;
    uint8_t  button20:1;
    uint8_t  button21:1;
    uint8_t  button22:1;
    uint8_t  button23:1;
    uint8_t  button24:1;
    uint8_t  button25:1;
    uint8_t  button26:1;
    uint8_t  button27:1;
    uint8_t  button28:1;
    uint8_t  button29:1;
    uint8_t  button30:1;
    uint8_t  button31:1;

    uint16_t  X:11;
    uint16_t  Y:11;
    uint16_t  Z:11;     // The X,Y, Z axis position

    uint16_t  rX:11;
    uint16_t  rY:11;
    uint16_t  rZ:11;  // The Z, Rx, Ry position

    uint16_t  Slider:11;
    uint16_t  Dial:11;  // The slider, Dial, Wheel position
    
    uint8_t Dpad1:4;
    uint8_t Dpad2:4;
    uint8_t Dpad3:4;
} __attribute__((packed, aligned(1)));//

} report;

