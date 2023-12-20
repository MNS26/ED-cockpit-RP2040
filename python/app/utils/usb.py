''' app/utils/usb.py '''
import hid
import struct
from enum import Enum
from .config import AppConfig
'''

virpil protocol
| stat | board type | led | color |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | end |  
   02        64        00    00    00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   f0

'''
'''
| Mode   |  Operation  |  ID      |  data         |  data       |  data
  Read   |  Led        |  Led     |  Led red      |  Led blue   |  Led green
  Write  |  Button     |  Button  |  Button state |             |  
         |  Axis       |  Axis    |  Axis value   |  Axis value |  

'''


class Cmd_Mode(Enum):

  CMD_READ   = 0B00000000,
  CMD_WRITE  = 0B00000001,

class Cmd_Type(Enum):

  CMD_LED     = 0B00000001,
  CMD_BTN     = 0B00000010,
  CMD_AXIS    = 0B00000011,
  CMD_DISPLAY = 0b00000000,



class HidData:
  LED_OPTION = 1
  PIP_OPTION = 2
  BUTTON_OPTION = 3
  AXIS_OPTION = 4

  def __init__(self,device):
    self.deviceName = AppConfig.usb_device
    self.enable = AppConfig.enable_usb
    
    self.deviceVID = device['vendor_id']
    self.devicePID = device['product_id']
    self.devicePATH = device['path']
    print(self.devicePATH)
    pass


  def packet(self,option,*args):
    self.option = option
    self.args = args

  def pack_data(self):
    if self.option == self.LED_OPTION:
      format_string = 'BBBBBB49x'
    elif self.option == self.PIP_OPTION:
      format_string = 'BBBBBBB57x'
    elif self.option == self.BUTTON_OPTION:
      format_string = 'BBB?B60s'  # Assuming 'enabled' is a boolean
    elif self.option == self.AXIS_OPTION:
      format_string = 'BB2H2H60s'  # Assuming 'range min' and 'range max' are 2-byte shorts
    else:
      raise ValueError("Invalid option")

    data_struct = struct.Struct(format_string)
    return data_struct.pack(*self.args)

# Example usage for LED option:
option = HidData.LED_OPTION
index = 10  # LED number
red = 255   # 0xFF as an integer
green = 255 # 0xFF as an integer
blue = 255  # 0xFF as an integer
additional_data = b'This is additional data'

hid_packet = HidData({})
hid_packet.packet(option, index, red, green, blue, additional_data)
packed_data = hid_packet.pack_data()

# Example usage for BUTTON option:
option = HidData.BUTTON_OPTION
button_index = 5
button_enabled = True
button_state = 1
button_additional_data = b'Button data'
hid_packet = HidData({})
hid_packet.packet(option, button_index, button_enabled, button_state, button_additional_data)
packed_data = hid_packet.pack_data()

# Example usage for AXIS option:
option = HidData.AXIS_OPTION
axis_index = 2
range_min = -32768
range_max = 32767
axis_additional_data = b'Axis data'

hid_packet = HidData({})
hid_packet.packet(option, axis_index, range_min, range_max, axis_additional_data)
packed_data = hid_packet.pack_data()


'''
    x: Padding byte (no value)
    c: Character (1 byte)
    b: Signed byte (1 byte)
    B: Unsigned byte (1 byte)
    h: Short integer (2 bytes)
    H: Unsigned short integer (2 bytes)
    i: Integer (4 bytes)
    I: Unsigned integer (4 bytes)
    l: Long integer (4 bytes on most platforms)
    L: Unsigned long integer (4 bytes on most platforms)
    q: Long long integer (8 bytes)
    Q: Unsigned long long integer (8 bytes)
    f: Single-precision float (4 bytes)
    d: Double-precision float (8 bytes)
    s: String (must be followed by a number indicating the string length)
    p: Packed string (null-terminated; must be followed by a number indicating the maximum string length)

You can also use various format characters in combination to create more complex structures.
For example, 2i represents two 4-byte integers, 3f represents three 4-byte single-precision floats, and so on.
'''