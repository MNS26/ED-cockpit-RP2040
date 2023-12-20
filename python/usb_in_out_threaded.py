import hid
import crc8
import time
import threading

USB_VID = (0xcafe, 0x239a, 0x2e8a, 0x303a)
crc = crc8.crc8()
print("VID list: " + ", ".join('%02x' % v for v in USB_VID))

number = 0


def transmitThread(dev: hid.Device) -> None:
    while True:
        str_var = input().encode('utf-8')
        str_len = len(str_var)
        str_len_count = len(str_len.to_bytes((str_len.bit_length() + 7) // 8, 'big'))
        crc.reset()
        crc.update(str_var)
        str_crc = crc.digest()
        str_out = str_len_count.to_bytes(1, 'big') + str_len.to_bytes(str_len_count, 'big') + str_var + str_crc
        try:
            print(f"sending LENGTH:{str_len} VAR:{str_var.decode('utf-8')} CRC:{str_crc}")
        except:
            print(f"sending LENGTH:{str_len} VAR:{str_var} CRC:{str_crc}")
        dev.write(str_out)

def receiveThread(dev: hid.Device) -> None:
    while True:
        try:
            data_in = dev.read(255, 100)
            if data_in:
                # Remove endoint from data
                #data_in = data_in[1:]

                # Read the length information
                len_count = data_in[0]

                # Read the length of the data
                len_data = 0
                for i in range(len_count):
                    len_data = len_data << 8 | data_in[i+1]
                # expand data_in untill its the size  of len_data or bigger (will keep it for now but idk if it will be used)
                while len_data > len(data_in):
                    if len_data > len(data_in):
                        data = dev.read(255,100)
                        data = data[1:]
                        for i in range(len(data)):
                            bytearray(data_in).append(i)
                received_data = bytearray()
                for i in range(len_data):
                    str(received_data.append(data_in[i+len_count+1]))
                crc_data = data_in[len_count + len_data + 1]
                try:
                    print(f'len count: {len_count} | len: {len_data} | data: {received_data.decode('utf-8')} | crc: {crc_data}')
                except:
                    print(f'len count: {len_count} | len: {len_data} | data: {received_data.decode('utf-8')} | crc: {crc_data}')
                print("Received from HID Device:", data_in, '\n')
            else:
                pass
                #print("Invalid data prefix. Discarding data.")
        except:
            pass


for vid in USB_VID:
    
    for device_info in hid.enumerate(vid):
        print(device_info)
        dev = hid.Device(device_info['vendor_id'], device_info['product_id'])
        if dev:
            transmit = threading.Thread(target=transmitThread, args=(dev,), daemon=True)
            transmit.start()

            receive = threading.Thread(target=receiveThread, args=(dev,), daemon=True)
            receive.start()
            try:
                while True:
                    threading.Event().wait(1)
            except KeyboardInterrupt:
                    print("Script terminated by user.")
                #str_var = input("input: ").encode('utf-8')
                #str_len = len(str_var)
                #str_len_count = len(str_len.to_bytes((str_len.bit_length() + 7) // 8, 'big'))
                #crc.reset()
                #crc.update(str_var)
                #str_crc = crc.digest()
                #
                #str_out = str_len_count.to_bytes(1, 'big') + str_len.to_bytes(str_len_count, 'big') + str_var + str_crc
                #try:
                #    print(f"sending LENGTH:{str_len} VAR:{str_var.decode('utf-8')} CRC:{str_crc}")
                #except:
                #    print(f"sending LENGTH:{str_len} VAR:{str_var} CRC:{str_crc}")
                #
                #dev.write(str_out)

                # Read the data prefix
                #data_in = dev.read(255, 100)
                #
                #if data_in[0] == 0x02:

                #    # Read the length information
                #    len_count = data_in[1]

                #    # Read the length of the data
                #    len_data = 0
                #    for i in range(len_count):
                #        len_data = len_data << 8 | data_in[i+2]
                #        
                #    # expand data_in untill its the size  of len_data or bigger (will keep it for now but idk if it will be used)
                #    while len_data > len(data_in):
                #        if len_data > len(data_in):
                #            data = dev.read(255,100)
                #            for i in range(len(data)):
                #                bytearray(data_in).append(i)
                #                        
                #    received_data = bytearray()
                #    for i in range(len_data):
                #        str(received_data.append(data_in[i+len_count+2]))

                #    crc_data = data_in[len_count + len_data + 2]
                #    try:
                #        print(f'len count: {len_count} | len: {len_data} | data: {received_data.decode('utf-8')} | crc: {crc_data}')
                #    except:
                #        print(f'len count: {len_count} | len: {len_data} | data: {received_data.decode('utf-8')} | crc: {crc_data}')

                #    print("Received from HID Device:", str_out, '\n')
                #else:
                #    pass
                #    #print("Invalid data prefix. Discarding data.")
