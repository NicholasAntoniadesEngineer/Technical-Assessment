
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
CSPIN = CS1

# Enable SPI
spi = spidev.SpiDev()       
# Open a connection to the devices
spi.open(0, 0)         
# Set SPI speed
spi.max_speed_hz = 32000    
print('SPI initialised\n')

   
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
def ADS1293_init_3lead(device):


    #stop data conversion
    reg_write(device,0x0,0x0)


    #connect channel 1 pos to IN2 and neg to IN1
    reg_write(device, 0x01, 0x11)

    #connect channel 2 pos to IN3 and neg to IN1
    #enable this for 3 lead
    #reg_write(device, 0x02, 0x19)


    
    #enable common-mode detector on input pins IN1,IN2 and IN3
    #enable this for 3 lead
    #reg_write(device, 0x0A, 0x07)

    #enable common-mode detector on input pins IN1 and IN2
    reg_write(device, 0x0A, 0x03)

    #connect output of RLD amplifier to IN4
    #enable this for 3 lead?
    reg_write(device, 0x0C, 0x04)

    #shutdown right-leg drive amplifier
    #enable this for 1 lead? maybe not?
    #reg_write(device, 0x0C, 0x08)


    #enable clock to digital
    reg_write(device, 0x12, 0x04)



	#configure channels 1 and 2 for high frequency, high resolution
    #this programs the Sigma Delta Modulator (SDM) 
    reg_write(device, 0x13, 0x1B)



    #shut down onlyS channel 3s amplifier and ADC
    #enable this for 3 lead
    #reg_write(device, 0x14,0x24)

    #shut down channel 2 and 3s amplifier and ADC
    #enable this for 1 lead
    reg_write(device, 0x14,0x36)

    #all channels on
    #reg_write(device, 0x14, 0x0)



    #configure R2 decimation rate as 6 for all channels
    #reg_write(device, 0x21, 0x04)
    
    #configure R2 decimation rate as 8 for all channels
    reg_write(device, 0x21, 0x10)

    #configure R3 decimation rate as 16 for channels 1 and 2
    reg_write(device, 0x22, 0x10)
    reg_write(device, 0x23, 0x10)

    #configure R3 decimation rate as 16 for channels 3
    # reg_write(device, 0x24, 0x10) 

    #configure R3 decimation rate as 6 for channels 1 and 2
    # reg_write(device, 0x22, 0x2)
    # reg_write(device, 0x23, 0x2)

    #configure R1 decimation rate as single for all channels
    reg_write(device, 0x25, 0x00)



    #configure DRDYB source to channel 1 ECG
    reg_write(device, 0x27, 0x08)



    #enable STATUS, channel 1 ECG and channel 2 ECG for loop read-back mode
    # reg_write(device, 0x2F, 0x70)
    reg_write(device, 0x2F, 0x30)
    #start data conversion
    reg_write(device, 0x00, 0x01)
    print('ADS1293 3 lead mode programmed \n')



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


'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
@brief Bandpass filter implementation

'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''

# # Initialise data to be filtered
CH1data_unfiltered = []
CH1data_unfiltered = [0 for i in range(30)] 
CH1data_filtered = []
CH1data_filtered = [0 for i in range(30)] 

# Sample rate and desired cutoff frequencies (in Hz).
fs = 200.0
lowcut = 0.5
highcut = 40



while True:
    # def main(args):
    print('Checking device on CS1')
    result = ADS1293_testConnection(CSPIN)

    # print('Checking device on CS2')
    # ADS1293_testConnection(CS2)

    #result = 1


    if (result == 1):
        
        print('Initialize Device 1 to 3 lead mode')
        ADS1293_init_3lead(CSPIN) 

        print('Reading in data packets')
        # open the file in the write mode
        f = open('test.csv', 'w')
        # create the csv writer
        writer = csv.writer(f)
        while True:
                                                                                        
                data = read_ECG_data_CH1_CH2(CSPIN)
                # First put together rae ECG ADC data
                CH1data_raw = ((data[0] & 0xFF) << 16) | ((data[1] & 0xFF) << 8) | (data[2] & 0xFF)
                CH2data_raw = ((data[3] & 0xFF) << 16) | ((data[4] & 0xFF) << 8) | (data[5] & 0xFF)
            
                # Second go from raw ADC values to an ECG value
                ADC_MAX = 0xf30000
                V_REF = 2.4

                # CH1 ECG processing
                CH1_TEMP = CH1data_raw/ADC_MAX
                CH1_TEMP = CH1_TEMP - 0.5
                CH1_TEMP = CH1_TEMP*V_REF*2
                CH1_TEMP = CH1_TEMP/3.5
                CH1_ECG = CH1_TEMP
                
                # CH2 ECG processing
                CH2_TEMP = CH2data_raw/ADC_MAX
                CH2_TEMP = CH2_TEMP - 0.5
                CH2_TEMP = CH2_TEMP*V_REF*2
                CH2_TEMP = CH2_TEMP/3.5
                CH2_ECG = CH2_TEMP

                # Apply a bandpass filter here       
                
                #Test comment
                #CH1data_filtered = butter_bandpass_filter(CH1data_unfiltered, lowcut, highcut, fs, order=1)
                
                
                # Shifting data
                # Does the length of this array effect filtering?
                # i = len(CH1data_unfiltered) - 1
                # while i > 0:
                #     CH1data_unfiltered[i] = CH1data_unfiltered[i-1]
                #     i = i - 1
                # CH1data_unfiltered[0] = CH1data_raw

                
                #print(CH1data_filtered)
                #print((CH1data_filtered[0],CH2data_raw))
                timeStamp = datetime.datetime.fromtimestamp(time.time()).strftime('%H:%M:%S:%f')
                print(CH1data_raw)
                
                # write a row to the csv file
                data=[timeStamp,CH1data_raw]
                writer.writerow(data)
                
                
                time.sleep(0.005)
                
                
            
        # close the file
        f.close()
        #return 0
    else:
        print('Cannot detect device!')
        time.sleep(0.1)
    # if __name__ == '__main__':
    #     main(0)
    #     import sys
    



            
    