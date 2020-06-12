# ECC608-MassProvisioning

This configures ATECC608 secure chip from ESP32 and get a public key corresponding to slot 0 private key.  
Prepare a host PC and automatically create and write a certificate with a Python script through serial communication.  
Due to chip's secure function, once if it has configured, you can't change config contents.  

# Requirements

  Platformio with VS Code environment.   
  install "Espressif 32" platform definition on Platformio   

# Environment reference
  
  Espressif ESP32-DevkitC  
  this project initializes both of I2C 0,1 port, and the device on I2C port 0 is absent.  
  pin assined as below:  


      I2C 0 SDA GPIO_NUM_18
      I2C 0 SCL GPIO_NUM_19

      I2C 1 SDA GPIO_NUM_21
      I2C 1 SCL GPIO_NUM_22
          
  Microchip ATECC608(on I2C port 1)   (not supported TRUST&GO and TRUSTFLEX)

# Usage

"git clone --recursive <this pages URL>" on your target directory.  
you need to change a serial port number which actually connected to ESP32 in platformio.ini.

# Run this project

First, on the host PC, run the python script in the scripts folder to prepare   
the root certificate and signer certificate.  
Then create a device certificate template (cert_chain.c) to allow one firmware to   create individual device certificates for multiple devices.  

On ESP32 side, add "cert_chain.c" above and execute "Upload" on platformio.   
This completes a binary that can be used as a master.  

When "deviceprovision.py" is executed on the host PC side, a series of operations  
to  extract the public key from the ESP32 side, sign it and write it back   
will be executed.  

For the second and subsequent ESP32s, simply copy the binary created above and   
execute "deviceprovision.py" from the host PC again, and provisioning will proceed  
automatically.

# License

This software is released under the MIT License, see LICENSE.
