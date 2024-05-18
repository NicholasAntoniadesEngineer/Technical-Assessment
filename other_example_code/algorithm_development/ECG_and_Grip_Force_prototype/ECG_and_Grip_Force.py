"""
@file ecg_adc_spi.py
@brief ADS1293 ECG and ADS1241 ADC integration with NVIDIA Jetson

This file contains the implementation for configuring, setting, and reading 
data from the ADS1293 ECG sensor and ADS1241 ADC using SPI on an NVIDIA Jetson device.

@date April 2022
@version 1.0
@authour Nicholas Antoniades
"""

import spidev
import RPi.GPIO as GPIO
import time
import datetime
import csv

# Imports for filtering
from scipy.signal import butter, lfilter
import numpy as np
from scipy.signal import freqz


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

def ADS1293_init_1lead(device):
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
    print('ADS1293 1 lead mode programmed \n')

def read_ECG_data_CH1_CH2(device):
    '''Reads data from CH1 and CH2 of the ADS1293.'''
    # Set CS bit low for transmission
    GPIO.output(device, 0) 
    # Adding Read bit to the register address with an OR with 0x80
    Read_command = 0x50 | 0x80     
    # SPI read data from register 
    spi.xfer([Read_command])   
    # Write zeros to keep the clk ticking for the slave response
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
    data = spi.xfer([0x0, 0x0, 0x0])  
    # Set CS bit high for end of transmission
    GPIO.output(device, 1) 
    return data

def ADC_Programme():
    '''Initialises ADS1241.'''
    # Set CS bit low for transmission
    GPIO.output(ADC_PIN, 0) 
    # Programme Setup Register
    # Gain = 1
    spi.xfer([Setup_Reg, Blank, Blank])   
    time.sleep(0.1)
    # Set CS bit high for end of transmission
    GPIO.output(ADC_PIN, 1)

def Input_Select(Input_Sel):
    '''Selects the input for the ADS1241.'''
    # Set CS bit low for transmission
    GPIO.output(ADC_PIN, 0) 
    spi.xfer([MUX_Ctrl_Reg, Blank, Input_Sel, Blank])   
    # Set CS bit high for end of transmission
    GPIO.output(ADC_PIN, 1)

def Fetch_ADC_data():
    '''Fetches data from the ADS1241.'''
    # Set CS bit low for transmission
    GPIO.output(ADC_PIN, 0) 
    # Requesting new data
    spi.xfer([Read_Address])
    ADC_data = spi.xfer([Blank, Blank, Blank])
    # Set CS bit high for end of transmission
    GPIO.output(ADC_PIN, 1)
    return ADC_data

def init_spi():
    '''Initializes the RPi SPI interface and GPIO.'''
    global spi, ECG_PIN, ADC_PIN
    # Initialize to GPIO mode
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)
    # Set GPIO24 to be CS1 pin
    CS1 = 24       
    GPIO.setup(24, GPIO.OUT)
    # Set GPIO23 to be CS2 pin
    CS2 = 23       
    GPIO.setup(23, GPIO.OUT)
    # Set CS bits high for no transmission  
    GPIO.output(CS1, 1)   
    GPIO.output(CS2, 1)    
    ECG_PIN = CS1
    ADC_PIN = CS2

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
        print('Checking device on CS1')
        result = ADS1293_testConnection(ECG_PIN)

        if result == 1:
            print('Initialize Device 1 to 1 lead mode')
            ADS1293_init_1lead(ECG_PIN) 
            
            short_counter = 0
            P_Ain0_raw = 0 
            CH1data_raw = 0
            
            while True:
                # Request Data for P_Ain0
                Input_Select(P_Ain0) 
                P_Ain0_data = Fetch_ADC_data() 
                P_Ain0_raw = ((P_Ain0_data[0] & 0xFF) << 16) | ((P_Ain0_data[1] & 0xFF) << 8) | (P_Ain0_data[2] & 0xFF)
                
                while short_counter <= 7:
                    # Read in ECG data                                                                    
                    data = read_ECG_data_CH1_CH2(ECG_PIN)
                    CH1data_raw = ((data[0] & 0xFF) << 16) | ((data[1] & 0xFF) << 8) | (data[2] & 0xFF)
                    print(CH1data_raw, P_Ain0_raw)
                    short_counter += 1
                    time.sleep(0.005)         
                short_counter = 0

                print(CH1data_raw, P_Ain0_raw)
        else:
            print('Cannot detect device!')
            time.sleep(0.1)
