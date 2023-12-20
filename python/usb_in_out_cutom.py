import hid
import threading

class DeviceCommunication():
    def __init__(self) -> None:
        self.data_from_rx = True

    # default is TinyUSB (0xcafe), Adafruit (0x239a), RaspberryPi (0x2e8a), Espressif (0x303a) VID
    USB_VID = (0xcafe, 0x239a, 0x2e8a, 0x303a)

    # Specify the endpoint you want to communicate with
    ENDPOINT_NUMBER = 0x02  # Endpoint we use

    def read_from_device(self, device):
        while True:
            # Read from the specified endpoint
            str_in = device.read(64)
            
            # Check if the first byte corresponds to the expected endpoint
            if str_in and str_in[0] == self.ENDPOINT_NUMBER:
                try:
                    # Shift all data bytes left by one position
                    shifted_data = bytes([byte for byte in str_in[1:]] + [0])

                    # Decode with error handling, replacing invalid bytes with the Unicode replacement character
                    decoded_data = shifted_data.decode('utf-8', errors='replace')

                    print("Received from RP2040:", decoded_data)
                    self.data_from_rx = True
                except UnicodeDecodeError as e:
                    print("Error decoding data: ", e)
                    print("Raw bytes:", str_in)
                finally:
                    pass
            else:
                # print("Received unexpected data:", str_in, '\n')
                pass

    def transmit_to_device(self, device):
        number = 0
        while True:
            # Get input from console and encode to UTF-8 for an array of chars.
            # HID generic input is a single report; therefore, by HIDAPI requirement,
            # it must be preceded with 0x00 as a dummy reportID
            if self.data_from_rx:
                str_var = str(number).encode('utf-8')
                str_crc = 0xff
                str_out = b'\x00' + len(str_var).to_bytes() + str_var + str_crc.to_bytes() # type: ignore
                if str_out != b'\x00':
                    device.write(str_out)
                    print("Sent to RP2040:", str(number))
                    self.data_from_rx = False
                # Add a small delay to avoid busy-waiting and excessive CPU usage
                # You may need to adjust this delay based on your specific requirements
                number += 1
                threading.Event().wait(0.1)
                print("Transmission thread: Data transmitted")

    def communicate_with_device(self):
        # Print the VID list
        print("VID list: " + ", ".join('%02x' % v for v in self.USB_VID))

        # Iterate through VID values
        for vid in self.USB_VID:
            # Enumerate devices with the specified VID
            for device_info in hid.enumerate(vid):
                print(device_info)
                # Create a device instance
                dev = hid.Device(device_info['vendor_id'], device_info['product_id'])
                
                if dev:
                    # Start a separate thread for reading from the device
                    reader_thread = threading.Thread(target=self.read_from_device, args=(dev,), daemon=True)
                    reader_thread.start()

                    # Start a separate thread for transmitting to the device
                    transmitter_thread = threading.Thread(target=self.transmit_to_device, args=(dev,), daemon=True)
                    transmitter_thread.start()

                    threading.Event().wait(1)  # Optional delay to allow threads to start before moving on
                    print("Communication initiated")

# Example usage
device_communication = DeviceCommunication()
device_communication.communicate_with_device()

# Keep the script running until manually interrupted
try:
    while True:
        threading.Event().wait(1)
except KeyboardInterrupt:
    print("Script terminated by user.")


#VID list: cafe, 239a, 2e8a, 303a
#{'path':'/dev/hidraw5','vendor_id':11914,'product_id':10,'serial_number':'E66138935F286C28','release_number':256,'manufacturer_string':'Raspberry Pi','product_string':'Pico','usage_page':1,'usage':4,'interface_number':2}
#{'path':'/dev/hidraw5','vendor_id':11914,'product_id':10,'serial_number':'E66138935F286C28','release_number':256,'manufacturer_string':'Raspberry Pi','product_string':'Pico','usage_page':65280,'usage':1,'interface_number':2}

#{'path':'/dev/hidraw9','vendor_id':11914,'product_id':10,'serial_number':'E66138935F286C28','release_number':256,'manufacturer_string':'Raspberry Pi','product_string':'Pico','usage_page':1,'usage':4,'interface_number':3}
#{'path':'/dev/hidraw9','vendor_id':11914,'product_id':10,'serial_number':'E66138935F286C28','release_number':256,'manufacturer_string':'Raspberry Pi','product_string':'Pico','usage_page':65280,'usage':1,'interface_number':3}