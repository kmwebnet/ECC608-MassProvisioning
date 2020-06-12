import serial
import serial.tools.list_ports
import time
import binascii
import os
import sys
from create_device_be import create_device

# definition of read and write
def uart_write_read(w_data, r_size):
    # Write
    ser.write(w_data)
    print('Send: '+ str(w_data))

    ser.flush()
    time.sleep(0.5)
    # Read
    r_data = ser.read_all().decode('utf-8', "backslashreplace")
    print('Recv: ' + str(r_data))

    return r_data

def uart_read(r_size):
    # Read
    r_data = ser.read_all().decode('utf-8', "backslashreplace")
    print('Recv: ' + str(r_data))

    return r_data


# auto detect available serial port
def search_com_port():
    coms = serial.tools.list_ports.comports()
    comlist = []
    for com in coms:
        comlist.append(com.device)
    print('Connected COM ports: ' + str(comlist))
    
    pport = input("Enter the number of the port to use :")
    pport = int(pport) - 1

# choice which port    
    use_port = comlist[pport]
    print('Use COM port: ' + use_port)

    return use_port




# main routine
if __name__ == '__main__':
# Search COM Ports
    use_port = search_com_port()

# Init Serial Port Setting
    ser = serial.Serial(port = use_port ,
    bytesize = serial.EIGHTBITS ,
    parity = serial.PARITY_NONE,
    stopbits = serial.STOPBITS_ONE ,
    baudrate = 115200 ,
    timeout = 1
    )

# establishing handshake
    r_size = 5000
    r_data = uart_read( r_size)

    for i in range (5):
        time.sleep(1)
        w_data = b'r'    
        r_data = uart_write_read(w_data, r_size)
        
        if 'Ready.' in r_data :
            print('communication ready.')
            break

# get device serial number
            
    for i in range (5):
        time.sleep(1)
        w_data = b's'    
        r_data = uart_write_read(w_data, r_size)

        if len(r_data) > 0 :
            with open('{}-serial.txt'.format(r_data), 'w' ) as f:
                f.write(r_data)
                commonname = "%s-serial.txt" % r_data
            print('got serial number.')
            break
        
    filename = "%s-device-pubkey.pem" % r_data
    target_filename = "%s-device.crt" % r_data

# get device public key if it isnt exist
    if not os.path.exists('{}'.format(filename)):
        for i in range (5):
            time.sleep(1)
            w_data = b'k'    
            r_data = uart_write_read(w_data, r_size)
            time.sleep(5)
            
            if len(r_data) > 0 :
                with open('{}'.format(filename), 'w', encoding="utf-8" ,newline="\n") as f:
                    f.write(r_data.replace('\r', ''))
                print('got device public key.')
                break

# make device certificate
            
    create_device('{}'.format(target_filename), '{}'.format(filename), 'signer-ca.crt', 'signer-ca.key', 'root-ca.crt', 'root-pub.pem', 'ou.txt', '{}'.format(commonname)) 
    
# signer certificate transmission start    
    
    time.sleep(1)
    w_data = b'c'    
    r_data = uart_write_read(w_data, r_size)   

    f = open('signer-ca.crt')
    columns = f.read().replace('\r', '').split("\n")
    f.close()
    
    for column in columns:  
        w_data = column.encode() + b'\n'
        r_data = uart_write_read(w_data, r_size)
    
# device certificate transmission start
    columns = ""

    ser.flush()    
    
    time.sleep(1)
    w_data = b'v'    
    r_data = uart_write_read(w_data, r_size)   

    f = open('{}'.format(target_filename))
    columns = f.read().replace('\r', '').split("\n")
    f.close()
    
    for column in columns:  
        w_data = column.encode() + b'\n'
        r_data = uart_write_read(w_data, r_size)

# root public key transmission start
    columns = ""

    ser.flush()

    time.sleep(1)
    w_data = b'b'    
    r_data = uart_write_read(w_data, r_size)   

    f = open('root-pub.pem')
    columns = f.read().replace('\r', '').split("\n")
    f.close()
    
    for column in columns:  
        w_data = column.encode() + b'\n'
        r_data = uart_write_read(w_data, r_size)

           
        
#Verify        
        
#    w_data = b'p'    
#    r_data = uart_write_read(w_data, r_size)
        
#    ser.flush()
        
#    w_data = b'o'    
#    r_data = uart_write_read(w_data, r_size)
    
#quit interactive mode and certificate provisioning start    

    w_data = b'q'    
    r_data = uart_write_read(w_data, r_size)
    
    time.sleep(3)
    
    r_data = uart_read(r_size)
    
    ser.flush()
    
    time.sleep(5)

    r_data = uart_read(r_size)
    
    ser.flush()

    time.sleep(5)

    r_data = uart_read(r_size)
    
    ser.flush()

    time.sleep(5)

    r_data = uart_read(r_size)

    time.sleep(5)    
    ser.flush()

    w_data = b' '
    r_data = uart_write_read(w_data, r_size)

    time.sleep(5)    
    ser.flush()
    
    r_data = uart_read(r_size)
    
    time.sleep(5)    
    ser.flush()

    r_data = uart_read(r_size)
    
    ser.close()
