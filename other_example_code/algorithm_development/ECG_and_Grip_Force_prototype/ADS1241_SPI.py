#!/usr/bin/env python3
import spidev
import RPi.GPIO as GPIO
import time
import datetime
import csv


'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
@brief Initialises ADS1241 -
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
       GPIO.output(CSPIN,0) 
       #WREG = 0b0101
       # Programme Setup Register
       # Gain = 1
       spi.xfer([Setup_Reg,Blank,Blank])   
       time.sleep(0.1)
       # Set CS bit high for end of transmission
       GPIO.output(CSPIN,1)

def Input_Select(Input_Sel):
       # Set CS bit low for transmission
       GPIO.output(CSPIN,0) 
       spi.xfer([MUX_Ctrl_Reg,Blank,Input_Sel,Blank])   
       time.sleep(0.06)
       # Set CS bit high for end of transmission
       GPIO.output(CSPIN,1)

def Fetch_ADC_data():
       # Set CS bit low for transmission
       GPIO.output(CSPIN,0) 
       #  spi.xfer([0b00011110,0b11101010])  
       # Requesting new data
       spi.xfer([Read_Address])
       ADC_data = spi.xfer([Blank,Blank,Blank])
       #data1 = spi.xfer([0x0,0x0, Byte1,Byte2])
       # Set CS bit high for end of transmission
       GPIO.output(CSPIN,1)
       return ADC_data

'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
@brief Initialises Rpi SPI interface
@note  CS pin for spidev library has issues and oscillates at the end of transmission
       as a solution a GPIO is set as a manual cs pin for each device
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

# Initialise to GPIO mode
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
# Set GPIO23 to be CS1 pin
CS2 = 23 
# Set GPIO24 to be CS2 pin
CS1 = 24          
GPIO.setup(CS1, GPIO.OUT)
GPIO.setup(CS2, GPIO.OUT)
# Set CS bits high for no transmission  
GPIO.output(CS1,1)   
GPIO.output(CS2,1)    
CSPIN = int(CS2)

# Enable SPI
spi = spidev.SpiDev()       
# Open a connection to the devices
spi.open(0, 0)         
# Set SPI speed
spi.max_speed_hz = 32000    

# Programme ADC
ADC_Programme() 



while True:


       # open the file in the write mode

       while True:
              f = open('Strain_gauage_data.csv', 'a')
              # create the csv writer
              writer = csv.writer(f)
              # Request Data for P_Ain0
              Input_Select(P_Ain0) 
              P_Ain0_data = Fetch_ADC_data() 
              P_Ain0_raw = ((P_Ain0_data[0] & 0xFF) << 16) | ((P_Ain0_data[1] & 0xFF) << 8) | (P_Ain0_data[2] & 0xFF)

              
              # # Request Data for P_Ain1
              # Input_Select(P_Ain1) 
              # P_Ain1_data = Fetch_ADC_data() 
              # P_Ain1_raw = ((P_Ain1_data[0] & 0xFF) << 16) | ((P_Ain1_data[1] & 0xFF) << 8) | (P_Ain1_data[2] & 0xFF)
              
              # time.sleep(0.5)
              
              #print(P_Ain0_raw, P_Ain1_raw)
              timeStamp = datetime.datetime.fromtimestamp(time.time()).strftime('%H:%M:%S:%f')
              print(P_Ain0_raw, 0)

              # write a row to the csv file
              data=[timeStamp,P_Ain0_raw]
              writer.writerow(data)
              
              time.sleep(0.05)

       # close the file
       f.close()

