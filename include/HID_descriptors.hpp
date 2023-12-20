// HID device
// - 1st parameter is report id HID_REPORT_ID(n) (optional)
#define TUD_JOYSTICK_REPORT_DESC(...)                   \
HID_USAGE_PAGE        ( HID_USAGE_PAGE_DESKTOP        ),\
HID_USAGE             ( HID_USAGE_DESKTOP_GAMEPAD    ),\
HID_COLLECTION        ( HID_COLLECTION_APPLICATION    ),\
  /* Report ID if any */\
  /*variadic argument*/\
  __VA_ARGS__ \
  HID_USAGE_PAGE      ( HID_USAGE_PAGE_BUTTON         ),\
  HID_USAGE_MIN       ( 1                             ),\
  HID_USAGE_MAX       ( 32                            ),\
  HID_LOGICAL_MIN     ( 0                             ),\
  HID_LOGICAL_MAX     ( 1                             ),\
  HID_REPORT_SIZE     ( 1                             ),\
  HID_REPORT_COUNT    ( 32                            ),\
  HID_INPUT           ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
\
  HID_USAGE_PAGE      ( HID_USAGE_PAGE_DESKTOP       ),\
    HID_USAGE         ( HID_USAGE_DESKTOP_X                    ),\
    HID_USAGE         ( HID_USAGE_DESKTOP_Y                    ),\
    HID_USAGE         ( HID_USAGE_DESKTOP_Z                    ),\
    HID_USAGE         ( HID_USAGE_DESKTOP_RX                   ),\
    HID_USAGE         ( HID_USAGE_DESKTOP_RY                   ),\
    HID_USAGE         ( HID_USAGE_DESKTOP_RZ                   ),\
    HID_USAGE         ( HID_USAGE_DESKTOP_SLIDER               ),\
    HID_USAGE         ( HID_USAGE_DESKTOP_DIAL                 ),\
    HID_LOGICAL_MIN   ( 0                                      ),\
    HID_LOGICAL_MAX_N ( 2047, 2                                ),\
    HID_PHYSICAL_MIN_N( -32768, 2                              ),\
    HID_PHYSICAL_MAX_N( 32767, 2                               ),\
    HID_REPORT_SIZE   ( 11                                     ),\
    HID_REPORT_COUNT  ( 8                                      ),\
    HID_INPUT         ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE | HID_WRAP_NO | HID_LINEAR | HID_PREFERRED_STATE | HID_NO_NULL_POSITION ),\
  HID_USAGE_PAGE      ( HID_USAGE_PAGE_DESKTOP       ),\
  HID_USAGE           ( HID_USAGE_DESKTOP_HAT_SWITCH ),\
  HID_USAGE           ( HID_USAGE_DESKTOP_HAT_SWITCH ),\
  HID_USAGE           ( HID_USAGE_DESKTOP_HAT_SWITCH ),\
  HID_LOGICAL_MIN     ( 0                            ),\
  HID_LOGICAL_MAX     ( 8                            ),\
  HID_PHYSICAL_MIN    ( 0                            ),\
  HID_PHYSICAL_MAX_N  ( 315, 2                       ),\
  HID_REPORT_SIZE     ( 4                            ),\
  HID_REPORT_COUNT    ( 3                            ),\
  HID_INPUT           ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ) ,\
HID_COLLECTION_END

/*

bit layout
  B = buttons (32 x 1 bit)
  X = X (axis 11 bit)
  Y = Y (axis 11 bit)
  Z = Z (axis 11 bit)
  x = Rx (axis 11 bit)
  y = Ry (axis 11 bit)
  z = Rz (axis 11 bit)
  S = slider (axis 11 bit)
  D = dial (axis 11 bit)
  H = HAT (Dpad 3x 4 bit)

|      |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBXXXXXXXXXXXYYYYYYYYYYYZZZZZZZZZZZxxxxxxxxxxxyyyyyyyyyyyzzzzzzzzzzzSSSSSSSSSSDDDDDDDDDDDHHHHHHHHHHHH     
|  |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
1      8      16      24      32      40      48      56      64      72      80      88      96      104     112     120     128     136

*/
// HID Generic Input & Output
// - 1st parameter is report size (mandatory)
// - 2nd parameter is report id HID_REPORT_ID(n) (optional)
#define TUD_HID_REPORT_DESC_GENERIC_INOUT(report_size, ...) \
HID_USAGE_PAGE      ( HID_USAGE_PAGE_VENDOR ),\
HID_USAGE           ( HID_USAGE_PAGE_UNDEFINED ),\
HID_REPORT_SIZE     ( 8 ),\
HID_COLLECTION      ( HID_COLLECTION_LOGICAL ),\
  /* Report ID if any */\
  __VA_ARGS__ \
  /* Input */ \
  HID_REPORT_COUNT_N( report_size ,2 ),\
  HID_USAGE         ( 0x01 ),\
  HID_FEATURE       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),\
  /* Output */ \
  HID_REPORT_COUNT_N( report_size, 2 ),\
  /* Report ID if any */\
  __VA_ARGS__ \
  HID_USAGE         ( 0x01 ),\
  HID_FEATURE       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),\
HID_COLLECTION_END


// HID Generic Input & Output
// - 1st parameter is report size (mandatory)
// - 2nd parameter is report id HID_REPORT_ID(n) (optional)
#define TUD_HID_REPORT_DESC_GENERIC_INOUT_BACKUP(report_size, ...) \
HID_USAGE_PAGE_N    ( HID_USAGE_PAGE_VENDOR, 2               ),\
HID_USAGE         ( 0x01                                   ),\
HID_COLLECTION    ( HID_COLLECTION_APPLICATION             ),\
  HID_REPORT_COUNT_N( report_size ,2                         ),\
  /* Report ID if any */\
  __VA_ARGS__ \
  /* Input */ \
  HID_USAGE         ( 0x02                                   ),\
  HID_LOGICAL_MIN   ( 0x00                                   ),\
  HID_LOGICAL_MAX   ( 0xff                                   ),\
  HID_REPORT_SIZE   ( 8                                      ),\
  HID_FEATURE       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),\
  /* Output */ \
  HID_REPORT_COUNT_N( report_size, 2                         ),\
  /* Report ID if any */\
  __VA_ARGS__ \
  /* Input */ \
  HID_USAGE         ( 0x03                                   ),\
  HID_LOGICAL_MIN   ( 0x00                                   ),\
  HID_LOGICAL_MAX   ( 0xff                                   ),\
  HID_REPORT_SIZE   ( 8                                      ),\
  HID_FEATURE       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE ),\
HID_COLLECTION_END
