"""
@file adc_spi_rpi.py
@brief ADS1241 ADC integration with Raspberry Pi

This file contains the implementation for configuring, setting, and reading 
data from the ADS1241 ADC using SPI on a Raspberry Pi.

@date April 2022
@version 1.0
@authour Nicholas Antoniades
"""

import spidev
import RPi.GPIO as GPIO
import time
import datetime
import csv

# Initialize constants for ADS1241
P_Ain0 = 0b00000111
P_Ain1 = 0b00010111
Blank = 0b00000000
Read_Address = 0b00000001
Setup_Reg = 0b01010000
MUX_Ctrl_Reg = 0b01010001
Alg_Ctrl_Reg = 0b01010010

def ADC_Programme():
    '''Initializes ADS1241 with default settings.'''
    # Set CS bit low for transmission
    GPIO.output(CSPIN, 0)
    # Program Setup Register with Gain = 1
    spi.xfer([Setup_Reg, Blank, Blank])
    time.sleep(0.1)
    # Set CS bit high for end of transmission
    GPIO.output(CSPIN, 1)

def Input_Select(Input_Sel):
    '''Selects the input channel for ADS1241.'''
    # Set CS bit low for transmission
    GPIO.output(CSPIN, 0)
    spi.xfer([MUX_Ctrl_Reg, Blank, Input_Sel, Blank])
    time.sleep(0.06)
    # Set CS bit high for end of transmission
    GPIO.output(CSPIN, 1)

def Fetch_ADC_data():
    '''Fetches ADC data from ADS1241.'''
    # Set CS bit low for transmission
    GPIO.output(CSPIN, 0)
    # Request new data
    spi.xfer([Read_Address])
    ADC_data = spi.xfer([Blank, Blank, Blank])
    # Set CS bit high for end of transmission
    GPIO.output(CSPIN, 1)
    return ADC_data

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
    CSPIN = CS2

    # Enable SPI
    spi = spidev.SpiDev()
    # Open a connection to the devices
    spi.open(0, 0)
    # Set SPI speed
    spi.max_speed_hz = 32000
    print('SPI initialized\n')

if __name__ == "__main__":
    init_spi()
    ADC_Programme()

    while True:
        try:
            with open('Strain_gauge_data.csv', 'a') as f:
                # Create the csv writer
                writer = csv.writer(f)
                
                while True:
                    # Request Data for P_Ain0
                    Input_Select(P_Ain0)
                    P_Ain0_data = Fetch_ADC_data()
                    P_Ain0_raw = ((P_Ain0_data[0] & 0xFF) << 16) | ((P_Ain0_data[1] & 0xFF) << 8) | (P_Ain0_data[2] & 0xFF)

                    # Optional: Request Data for P_Ain1
                    # Input_Select(P_Ain1)
                    # P_Ain1_data = Fetch_ADC_data()
                    # P_Ain1_raw = ((P_Ain1_data[0] & 0xFF) << 16) | ((P_Ain1_data[1] & 0xFF) << 8) | (P_Ain1_data[2] & 0xFF)

                    # Print and log data with timestamp
                    timeStamp = datetime.datetime.fromtimestamp(time.time()).strftime('%H:%M:%S:%f')
                    print(P_Ain0_raw, 0)
                    data = [timeStamp, P_Ain0_raw]
                    writer.writerow(data)

                    time.sleep(0.05)

        except KeyboardInterrupt:
            print("Process interrupted")
            break
        finally:
            f.close()
            GPIO.cleanup()
