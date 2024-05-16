
#!/usr/bin/env python3
import spidev
import RPi.GPIO as GPIO
import time
import datetime
import csv

#Imports for filtering
from scipy.signal import butter, lfilter
import numpy as np
from scipy.signal import freqz

   
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
@brief Reads a single byte to a single address on the SPI device
@return value of requested register
@param[in] device       SPI device to be written to
@param[in] reg_address  address of register on target device
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''     
def reg_read(device, reg_address): 
    # Set CS bit low for transmission
    GPIO.output(device,0) 
    # Adding Read bit to the register address
    Read_command = reg_address|0x80     
    # SPI read data from register 
    spi.xfer([Read_command])   
    # Write zeros to keep the clk ticking for the slave response
    data = spi.xfer([0x0])  
    # Set CS bit high for end of transmission
    GPIO.output(device,1) 
    return data
    
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
@brief Writes a single byte to a single address on the SPI device
@param[in] device       SPI device to be written to
@param[in] data         address of register on the target device
@param[in] reg_address  value to be written to target register
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''    
def reg_write(device, reg_address, data):
    # Set CS bit low for transmission
    GPIO.output(device,0) 
    # SPI write data to register
    spi.xfer([reg_address,data])
    # Set CS bit high for end of transmission
    GPIO.output(device,1) 

'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
@brief Tests connection to ADS1293 by requesting known register
@return value of requested register
@param[in] reg_address  address of register on target device
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''     
def ADS1293_testConnection(device):  
    # Stop data conversion
    reg_write(device,0x0,0x0)
    # Read REVID register
    data = reg_read(device,0x40)
            
    # If result equals expected value of the REVID register
    if str(data[0]) == '1':          
        print('Success: ' + str(data[0]) +'\n')
        return 1
    else:
        print('Failure, returned value: ' + str(data[0]) +'\n')
        return 0          


'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
@brief Programs on-board ADS1293 device with pre-set values for 3-lead ECG operation
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
def ADS1293_init_1lead(device):

    #stop data conversion
    reg_write(device,0x0,0x0)

    #connect channel 1 pos to IN2 and neg to IN1
    reg_write(device, 0x01, 0x11)

    #enable common-mode detector on input pins IN1 and IN2
    reg_write(device, 0x0A, 0x03)

    #connect output of RLD amplifier to IN4
    reg_write(device, 0x0C, 0x04)

    #enable clock to digital
    reg_write(device, 0x12, 0x04)

	#configure channels 1 and 2 for high frequency, high resolution
    reg_write(device, 0x13, 0x1B)

    #shut down channel 2 and 3s amplifier and ADC
    reg_write(device, 0x14,0x36)

    #configure R2 decimation rate as 8 for all channels
    reg_write(device, 0x21, 0x10)

    #configure R3 decimation rate as 16 for channels 1 and 2
    reg_write(device, 0x22, 0x10)
    reg_write(device, 0x23, 0x10)

    #configure R1 decimation rate as single for all channels
    reg_write(device, 0x25, 0x00)

    #configure DRDYB source to channel 1 ECG
    reg_write(device, 0x27, 0x08)

    #enable STATUS, channel 1 ECG and channel 2 ECG for loop read-back mode
    reg_write(device, 0x2F, 0x30)
    
    #start data conversion
    reg_write(device, 0x00, 0x01)
    print('ADS1293 1 lead mode programmed \n')



'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
@brief reads data from CH1 and CH2 of the ADS1293
@return value of CH1 and CH2 data
@param[in] device being requested for data, this ensure the correct CS line is pulled low 
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

def read_ECG_data_CH1_CH2(device):
    # Read operation from the DATA_LOOP register
    # Extend the CSB assertion beyond 16 clocks
    # Streaming mode supported by DATA_STATUS, DATA_...CH1_PACE,CH2_PACE,CH3_PACE,CH1_ECG,CH2_ECG,CH3_ECG
    # ECG data is 3 bytes long?
    
    # Do I need to check the DATA_STATUS register to see if new ECG data is ready?
    
    # Set CS bit low for transmission
    GPIO.output(device,0) 
    # Adding Read bit to the register address with an OR with 0x80
    Read_command = 0x50|0x80     
    # SPI read data from register 
    spi.xfer([Read_command])   
    # Write zeros to keep the clk ticking for the slave response
    # 3 Bytes for CH1, 3 Bytes for CH2
    data = spi.xfer([0x0,0x0,0x0,0x0,0x0,0x0])  
    # Set CS bit high for end of transmission
    GPIO.output(device,1) 
    return data


'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
@brief reads data from CH1 of the ADS1293
@return value of CH1 data
@param[in] device being requested for data, this ensure the correct CS line is pulled low 
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

def read_ECG_data_CH1(device):
    # Read operation from the DATA_LOOP register
    # Extend the CSB assertion beyond 16 clocks
    # Streaming mode supported by DATA_STATUS, DATA_...CH1_PACE,CH2_PACE,CH3_PACE,CH1_ECG,CH2_ECG,CH3_ECG
    # ECG data is 3 bytes long?
    
    # Do I need to check the DATA_STATUS register to see if new ECG data is ready?
    
    # Set CS bit low for transmission
    GPIO.output(device,0) 
    # Adding Read bit to the register address with an OR with 0x80
    Read_command = 0x50|0x80     
    # SPI read data from register 
    spi.xfer([Read_command])   
    # Write zeros to keep the clk ticking for the slave response
    # 3 Bytes for CH1
    data = spi.xfer([0x0,0x0,0x0])  
    # Set CS bit high for end of transmission
    GPIO.output(device,1) 
    return data


'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
@brief Initialises ADS1241
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

P_Ain0        = 0b00000111
P_Ain1        = 0b00010111
Blank         = 0b00000000
Read_Address  = 0b00000001
Setup_Reg     = 0b01010000
MUX_Ctrl_Reg  = 0b01010001
Alg_Ctrl_Reg  = 0b01010010


def ADC_Programme():
       # Set CS bit low for transmission
       GPIO.output(ADC_PIN,0) 
       #WREG = 0b0101
       # Programme Setup Register
       # Gain = 1
       spi.xfer([Setup_Reg,Blank,Blank])   
       time.sleep(0.1)
       # Set CS bit high for end of transmission
       GPIO.output(ADC_PIN,1)

def Input_Select(Input_Sel):
       # Set CS bit low for transmission
       GPIO.output(ADC_PIN,0) 
       spi.xfer([MUX_Ctrl_Reg,Blank,Input_Sel,Blank])   
       # Set CS bit high for end of transmission
       GPIO.output(ADC_PIN,1)

def Fetch_ADC_data():
       # Set CS bit low for transmission
       GPIO.output(ADC_PIN,0) 
       #  spi.xfer([0b00011110,0b11101010])  
       # Requesting new data
       spi.xfer([Read_Address])
       ADC_data = spi.xfer([Blank,Blank,Blank])
       #data1 = spi.xfer([0x0,0x0, Byte1,Byte2])
       # Set CS bit high for end of transmission
       GPIO.output(ADC_PIN,1)
       return ADC_data

'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
@brief Initialises Rpi SPI interface
@note  CS pin for spidev library has issues and oscillates at the end of transmission
       as a solution a GPIO is set as a manual cs pin for each device
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

# Initialise to GPIO mode
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
# Set GPIO24 to be CS1 pin
CS1 = 24       
GPIO.setup(23, GPIO.OUT)
# Set GPIO23 to be CS2 pin
CS2 = 23       
GPIO.setup(24, GPIO.OUT)
# Set CS bits high for no transmission  
GPIO.output(CS1,1)   
GPIO.output(CS2,1)    
ECG_PIN = CS1
ADC_PIN = CS2

# Enable SPI
spi = spidev.SpiDev()       
# Open a connection to the devices
spi.open(0, 0)         
# Set SPI speed
spi.max_speed_hz = 32000    
print('SPI initialised\n')

# Programme ADC
ADC_Programme() 


while True:
    print('Checking device on CS1')
    result = ADS1293_testConnection(ECG_PIN)


    if (result == 1):
        
        print('Initialize Device 1 to 1 lead mode')
        ADS1293_init_1lead(ECG_PIN) 
        
        short_counter = 0
        P_Ain0_raw    = 0 
        P_Ain1_raw    = 0 
        CH1data_raw   = 0
        while True:

            # Request Data for P_Ain0
            Input_Select(P_Ain0) 
            P_Ain0_data = Fetch_ADC_data() 
            P_Ain0_raw = ((P_Ain0_data[0] & 0xFF) << 16) | ((P_Ain0_data[1] & 0xFF) << 8) | (P_Ain0_data[2] & 0xFF)
            

            while short_counter<=7:
                # Read in ECG data                                                                    
                data = read_ECG_data_CH1_CH2(ECG_PIN)
                CH1data_raw = ((data[0] & 0xFF) << 16) | ((data[1] & 0xFF) << 8) | (data[2] & 0xFF)
                print(CH1data_raw, P_Ain0_raw)
                short_counter = short_counter + 1
                time.sleep(0.005)         
            short_counter = 0
            

            # Request Data for P_Ain1
            # Input_Select(P_Ain1) 
            # P_Ain1_data = Fetch_ADC_data() 
            # P_Ain1_raw = ((P_Ain1_data[0] & 0xFF) << 16) | ((P_Ain1_data[1] & 0xFF) << 8) | (P_Ain1_data[2] & 0xFF)
                        
                                
            # while short_counter<=7:
            #     # Read in ECG data                                                                    
            #     data = read_ECG_data_CH1_CH2(ECG_PIN)
            #     CH1data_raw = ((data[0] & 0xFF) << 16) | ((data[1] & 0xFF) << 8) | (data[2] & 0xFF)
            #     print(CH1data_raw, P_Ain1_raw)
            #     short_counter = short_counter + 1
            #     time.sleep(0.005)
            # short_counter = 0

            print(CH1data_raw, P_Ain0_raw)
            
        
        #return 0
    else:
        print('Cannot detect device!')
        time.sleep(0.1)
    # if __name__ == '__main__':
    #     main(0)
    #     import sys
    



            
    