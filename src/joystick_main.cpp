/*== JOYSTICK ==*/
#include <Arduino.h>
#include "hid.hpp"
#include <Adafruit_TinyUSB.h>
#include "HID_descriptors.hpp"
#include "HID_Report.hpp"
#include "input.hpp"
#include "Commands.hpp"
#include "CRC8.h"
#include "CRC.h"

/*== LED ==*/

#define NUM_PIXELS 1
#define WS_PIN 16
#include "NeoPixelPIO.h"

union _led {
	struct {
		byte R;
		byte G;
		byte B;
		/* data */
	};
	uint32_t raw:24;

} led;


uint8_t const desc_io_report[] =
{
	TUD_HID_REPORT_DESC_GENERIC_INOUT_BACKUP(254,HID_REPORT_ID(0xaa))
};

uint8_t const desc_hid_report[] = 
{
  TUD_JOYSTICK_REPORT_DESC(HID_REPORT_ID(1)),
  TUD_JOYSTICK_REPORT_DESC(HID_REPORT_ID(2)),

};


// Create an instance of NeoPixelPIO and initialize it
// to use GPIO pin 16 as the control pin, for a string
// of 1 neopixel. Name the instance pixelPIO
NeoPixelPIO PixelPIO(WS_PIN, NUM_PIXELS, false, pio0, 0);

Adafruit_USBD_HID usb_io(desc_io_report, sizeof(desc_io_report), HID_ITF_PROTOCOL_NONE,2,true);
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report),HID_ITF_PROTOCOL_NONE,2,true);

// Create an instance of the joystick report data struct
report _report1;
report _report2;

CRC8 crc;

int speed=1;
int i=1;

// Define usb buffer
uint8_t USBbuffer[255];
uint16_t USBbufOffset = 0;
uint16_t USBdataSize = 0;
uint16_t USBbufSize = 0;

// Flag to indicate whether new data is in the buffer
volatile bool isNewBuffer = false;


/*
	=============
		Callbacks  
	=============
*/
// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t get_report_callback (uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
	// not used in this example
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;
	switch (report_type)
	{
	case (hid_report_type_t)HID_REPORT_TYPE_INVALID:
		{
			Serial.print("get_report type: ");
			Serial.println((int)report_type);
		}
		break;
	case (hid_report_type_t)HID_REPORT_TYPE_INPUT:
		{
			Serial.print("get_report type: ");
			Serial.println((int)report_type);
		}
		break;
	case (hid_report_type_t)HID_REPORT_TYPE_OUTPUT:
		{
			Serial.print("get_report type: ");
			Serial.println((int)report_type);
		}
		break;
	case (hid_report_type_t)HID_REPORT_TYPE_FEATURE:	
		{
			Serial.print("get_report type: ");
			Serial.println((int)report_type);
		}
		break;
	}
  return 0;
}

// Invoked when received SET_REPORT control request or
void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  // This example doesn't use multiple report and report ID
  //(void) report_id;
  //(void) report_type;
  //(void) buffer;

			Serial.write(buffer,sizeof(buffer));

	switch (report_type)
	{
	case HID_REPORT_TYPE_INVALID:
		/* code */
		{
			Serial.print("set_report type: ");
			Serial.println((int)report_type);
		}
		break;
	
	case HID_REPORT_TYPE_INPUT:
		{
			Serial.print("set_report type: ");
			Serial.println((int)report_type);
		}
	break;

	case HID_REPORT_TYPE_OUTPUT:
		/* code */
		{
			Serial.print("set_report type: ");
			Serial.println((int)report_type);
		}
		break;

	case HID_REPORT_TYPE_FEATURE:
		{
			Serial.print("set_report type: ");
			Serial.println((int)report_type);
			// Disable interrupts
		  //__disable_irq();

		 	// Ensure the buffer won't overflow
		  if (USBbufOffset + bufsize > sizeof(USBbuffer)) {
		    // Handle overflow or error condition
		    // For simplicity, you can reset the buffer here
		    USBbufOffset = 0;
		  }
		  // Copy the received data to the USB buffer
		  memcpy(USBbuffer + USBbufOffset, buffer, bufsize);

		  // Calculate data length based on byte count
		  int dataLengthSize = USBbuffer[0];

		  // Only calculate the data size if the offset is 0
		  if (USBbufOffset == 0) {
		    // Reset data size
		    USBdataSize = 0;

	  	  // Combine the amount of bytes after index 0 based on index 0 value
	  	  for (int i = 0; i < dataLengthSize; i++) {
		      USBdataSize = (USBdataSize << 8) | USBbuffer[1 + i];
		    }
		  }

		  // Add incoming buffer size to offset
		  USBbufOffset += bufsize;

	  	// Check if enough bytes have been received to process a complete message
	  	if (USBbufOffset >= USBdataSize + dataLengthSize + 1) {
	  	  // Copy over the size
	  	  USBbufSize = USBbufOffset;
	  	  // Reset buffer offset for the next message
	  	  USBbufOffset = 0;
				if(calcCRC8((byte *)USBbuffer + dataLengthSize + 1, USBdataSize) == USBbuffer[USBbufSize-1]) {
	  	  	// Flag to indicate whether new data is in the buffer
	  	  	isNewBuffer = true;
				} else {
					memset(USBbuffer,0x00,sizeof(USBbuffer));
					USBbufSize = 0;
					USBdataSize = 0;
					USBbufOffset = 0;
				}
	  	  // Uncomment the line below if you want to send the report immediately
	  	  //usb_io.sendReport(0, &USBbuffer, USBbufSize);
	  	}

	  	// Re-enable interrupts
	  	//__enable_irq();
		}
		break;
	}
}

/*
	=========
	  Setup
	=========
 */

void setup()
{
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as
  // - mbed rp2040
  TinyUSB_Device_Init(0);
#endif

	pinMode(9,INPUT_PULLDOWN);
	pinMode(10,OUTPUT);
	digitalWrite(10,HIGH);
	digitalWrite(9,LOW);
	//pinMode(11,OUTPUT_2MA);
	//pinMode(12,INPUT_PULLDOWN);
	//pinMode(13,OUTPUT_2MA);

  TinyUSBDevice.setID(VID,PID);
  TinyUSBDevice.setManufacturerDescriptor("Raspberry Pi");
  TinyUSBDevice.setProductDescriptor("Pico");

  usb_io.setReportCallback(get_report_callback, set_report_callback);
	usb_io.begin();
	//usb_hid.begin();

	SerialTinyUSB.begin(115200);

	// start the USB device
 	USBDevice.attach();

	// wait until device mounted
	//while (!TinyUSBDevice.mounted()){	delay(1); }

}

/*
	========
		Loop
	========
 */
int lastTime = micros();
void loop()
{
	
if(TinyUSBDevice.ready()) {
		led.R = 0;
		led.G = 1;
		led.B = 0;
	} else if (TinyUSBDevice.mounted())
	{
		led.R = 1;
		led.G = 1;
		led.B = 0;
	} else {
		led.R = 1;
		led.G = 0;
		led.B = 0;
	}	
	// Remote wakeup
	if (BOOTSEL && TinyUSBDevice.suspended() )
	{
		// Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
	}
	if(!TinyUSBDevice.ready()) { return; }


	if(isNewBuffer)
	{
		Serial.print("DATA lenth: ");    
		Serial.print(USBdataSize);

		Serial.print(" | DATA: ");
		Serial.write((char *)USBbuffer+USBbufSize-USBdataSize-1,(size_t)USBdataSize);

		Serial.print(" CRC: ");    
		Serial.print(USBbuffer[USBbufSize-1]);

		Serial.print(" | Buffer size: ");
		Serial.print(USBdataSize+USBbuffer[0]+2);

		Serial.print("\r\n");

		isNewBuffer = false;

   	// Send the buffer back for now
		Serial.print("1st: ");
		Serial.println(usb_io.sendReport(0, &USBbuffer,USBbufSize));

		Serial.print("2nd: ");
    Serial.println(usb_io.sendReport(0, &USBbuffer,USBbufSize));

	}



	if(micros()-lastTime > 1000)
	{
		lastTime = micros(); 
		if(i < 1 || i > 2047)
			speed=-speed;
		i+=speed; 
		_report1.X = i;
		_report1.Y = i;
		_report1.Z = i;
		_report1.rX = i;
		_report1.rY = i;
		_report1.rZ = i;
		_report1.Slider = i;
		_report1.Dial = i;
		_report1.buttons = random(0,UINT32_MAX);
		//buttons(&_report1,random(0,UINT32_MAX));
		//_report1.Wheel = i;
		//_report1.Dpad1 = random(0,8);
		//_report1.Dpad2 = random(0,8);
		//_report1.buttons = i;

		//_report1.buttons=random(0 , UINT32_MAX);
//	setAxisValue(&hidReport, MouseX, ii);
//	setAxisValue(&hidReport, MouseY, ii);
//	for (int i = -1; i<9;i++)
//	{
//		setHatValue(&hidReport,i);
//		usb_hid1.sendReport(0, &hidReport, sizeof(hidReport));
//		delay(50);
//	}
		//sendHID(1,&usb_hid1);
		//if(digitalRead(9))
		//	press(&_report1, 9);
		//else
		//	release(&_report1, 9);
		
	}
		delayMicroseconds(500);
		//usb_hid.sendReport(1, &_report1, sizeof(_report1));
		delayMicroseconds(500);
		//usb_hid.sendReport(2, &_report1, sizeof(_report1));
}


void setup1()
{

}

void loop1()
{
	for (int i = 0; i<255; i+=(int)(255/20))
	{
		PixelPIO.neoPixelSetValue(0, led.R*i, led.G*i, led.B*i, true);
		delay(100);
	}
	for (int i = 255; i>0; i-=(int)(255/20))
	{
		PixelPIO.neoPixelSetValue(0, led.R*i, led.G*i, led.B*i, true);
		delay(100);
	}
	//PixelPIO.neoPixelClear(true);
}