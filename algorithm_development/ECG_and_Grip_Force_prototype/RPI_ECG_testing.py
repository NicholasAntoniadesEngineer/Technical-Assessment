"""
@file RPI_ECG_testing.py
@brief ADS1293 ECG integration with Raspberry Pi

This file contains the implementation for configuring, setting, and reading 
data from the ADS1293 ECG sensor using SPI on a Raspberry Pi.

@date April 2022
@version 1.0
@authour Nicholas Antoniades
"""

import spidev
import RPi.GPIO as GPIO
import time
import datetime
import csv
import numpy as np
from scipy.signal import butter, lfilter, freqz

def init_spi():
    '''Initializes the Raspberry Pi SPI interface and GPIO.'''
    global spi, CSPIN
    # Initialize to GPIO mode
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)
    # Set GPIO pins for CS
    CS1 = 24  # CS1 pin
    CS2 = 23  # CS2 pin
    GPIO.setup(CS1, GPIO.OUT)
    GPIO.setup(CS2, GPIO.OUT)
    # Set CS bits high for no transmission
    GPIO.output(CS1, 1)
    GPIO.output(CS2, 1)
    CSPIN = CS1

    # Enable SPI
    spi = spidev.SpiDev()
    # Open a connection to the devices
    spi.open(0, 0)
    # Set SPI speed
    spi.max_speed_hz = 32000
    print('SPI initialized\n')

def reg_read(device, reg_address): 
    '''Reads a single byte from a single address on the SPI device.'''
    # Set CS bit low for transmission
    GPIO.output(device, 0)
    # Adding Read bit to the register address
    Read_command = reg_address | 0x80     
    # SPI read data from register 
    spi.xfer([Read_command])
    # Write zeros to keep the clk ticking for the slave response
    data = spi.xfer([0x0])  
    # Set CS bit high for end of transmission
    GPIO.output(device, 1)
    return data

def reg_write(device, reg_address, data):
    '''Writes a single byte to a single address on the SPI device.'''
    # Set CS bit low for transmission
    GPIO.output(device, 0)
    # SPI write data to register
    spi.xfer([reg_address, data])
    # Set CS bit high for end of transmission
    GPIO.output(device, 1)

def ADS1293_testConnection(device):
    '''Tests connection to ADS1293 by requesting known register.'''
    # Stop data conversion
    reg_write(device, 0x0, 0x0)
    # Read REVID register
    data = reg_read(device, 0x40)
            
    # If result equals expected value of the REVID register
    if str(data[0]) == '1':          
        print('Success: ' + str(data[0]) + '\n')
        return 1
    else:
        print('Failure, returned value: ' + str(data[0]) + '\n')
        return 0          

def ADS1293_init_3lead(device):
    '''Programs on-board ADS1293 device with pre-set values for 3-lead ECG operation.'''
    # Stop data conversion
    reg_write(device, 0x0, 0x0)

    # Connect channel 1 pos to IN2 and neg to IN1
    reg_write(device, 0x01, 0x11)

    # Enable common-mode detector on input pins IN1 and IN2
    reg_write(device, 0x0A, 0x03)

    # Connect output of RLD amplifier to IN4
    reg_write(device, 0x0C, 0x04)

    # Enable clock to digital
    reg_write(device, 0x12, 0x04)

    # Configure channels 1 and 2 for high frequency, high resolution
    reg_write(device, 0x13, 0x1B)

    # Shut down channel 2 and 3 amplifiers and ADC
    reg_write(device, 0x14, 0x36)

    # Configure R2 decimation rate as 8 for all channels
    reg_write(device, 0x21, 0x10)

    # Configure R3 decimation rate as 16 for channels 1 and 2
    reg_write(device, 0x22, 0x10)
    reg_write(device, 0x23, 0x10)

    # Configure R1 decimation rate as single for all channels
    reg_write(device, 0x25, 0x00)

    # Configure DRDYB source to channel 1 ECG
    reg_write(device, 0x27, 0x08)

    # Enable STATUS, channel 1 ECG and channel 2 ECG for loop read-back mode
    reg_write(device, 0x2F, 0x30)

    # Start data conversion
    reg_write(device, 0x00, 0x01)
    print('ADS1293 3 lead mode programmed \n')

def read_ECG_data_CH1_CH2(device):
    '''Reads data from CH1 and CH2 of the ADS1293.'''
    # Set CS bit low for transmission
    GPIO.output(device, 0)
    # Adding Read bit to the register address with an OR with 0x80
    Read_command = 0x50 | 0x80
    # SPI read data from register
    spi.xfer([Read_command])
    # Write zeros to keep the clk ticking for the slave response
    # 3 Bytes for CH1, 3 Bytes for CH2
    data = spi.xfer([0x0, 0x0, 0x0, 0x0, 0x0, 0x0])
    # Set CS bit high for end of transmission
    GPIO.output(device, 1)
    return data

def read_ECG_data_CH1(device):
    '''Reads data from CH1 of the ADS1293.'''
    # Set CS bit low for transmission
    GPIO.output(device, 0)
    # Adding Read bit to the register address with an OR with 0x80
    Read_command = 0x50 | 0x80
    # SPI read data from register
    spi.xfer([Read_command])
    # Write zeros to keep the clk ticking for the slave response
    # 3 Bytes for CH1
    data = spi.xfer([0x0, 0x0, 0x0])
    # Set CS bit high for end of transmission
    GPIO.output(device, 1)
    return data

def butter_bandpass(lowcut, highcut, fs, order=5):
    '''Creates a Butterworth bandpass filter.'''
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return b, a

def butter_bandpass_filter(data, lowcut, highcut, fs, order=5):
    '''Applies a Butterworth bandpass filter to the data.'''
    b, a = butter_bandpass(lowcut, highcut, fs, order=order)
    y = lfilter(b, a, data)
    return y

if __name__ == "__main__":
    init_spi()

    while True:
        print('Checking device on CS1')
        result = ADS1293_testConnection(CSPIN)

        if result == 1:
            print('Initialize Device 1 to 3 lead mode')
            ADS1293_init_3lead(CSPIN)

            print('Reading in data packets')
            with open('test.csv', 'w') as f:
                # Create the csv writer
                writer = csv.writer(f)
                while True:
                    data = read_ECG_data_CH1_CH2(CSPIN)
                    # First put together raw ECG ADC data
                    CH1data_raw = ((data[0] & 0xFF) << 16) | ((data[1] & 0xFF) << 8) | (data[2] & 0xFF)
                    CH2data_raw = ((data[3] & 0xFF) << 16) | ((data[4] & 0xFF) << 8) | (data[5] & 0xFF)

                    # Second go from raw ADC values to an ECG value
                    ADC_MAX = 0xf30000
                    V_REF = 2.4

                    # CH1 ECG processing
                    CH1_TEMP = CH1data_raw / ADC_MAX
                    CH1_TEMP = CH1_TEMP - 0.5
                    CH1_TEMP = CH1_TEMP * V_REF * 2
                    CH1_TEMP = CH1_TEMP / 3.5
                    CH1_ECG = CH1_TEMP

                    # CH2 ECG processing
                    CH2_TEMP = CH2data_raw / ADC_MAX
                    CH2_TEMP = CH2_TEMP - 0.5
                    CH2_TEMP = CH2_TEMP * V_REF * 2
                    CH2_TEMP = CH2_TEMP / 3.5
                    CH2_ECG = CH2_TEMP

                    # Apply a bandpass filter here       
                    # CH1data_filtered = butter_bandpass_filter(CH1data_unfiltered, lowcut, highcut, fs, order=1)
                    # Shifting data
                    # Does the length of this array affect filtering?
                    # i = len(CH1data_unfiltered) - 1
                    # while i > 0:
                    #     CH1data_unfiltered[i] = CH1data_unfiltered[i - 1]
                    #     i = i - 1
                    # CH1data_unfiltered[0] = CH1data_raw

                    # Print and log data with timestamp
                    timeStamp = datetime.datetime.fromtimestamp(time.time()).strftime('%H:%M:%S:%f')
                    print(CH1data_raw)
                    data = [timeStamp, CH1data_raw]
                    writer.writerow(data)

                    time.sleep(0.005)
        else:
            print('Cannot detect device!')
            time.sleep(0.1)
