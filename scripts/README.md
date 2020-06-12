
These tools are used to set up an example chain of trust ecosystem for use with   
Microchip ATECC608A (not including TRUST&GO and TRUSTFLEX) .   
Included are utilities to create the ecosystem keys and certificates.  

## Dependencies

Python scripts will require python 3 to be installed.   
Once python is installed
install the requirements (from the path of this file):

```
> pip install -r requirements.txt
```

## Set up a Certificate Ecosystem

The first step is to set up a certificate chain that mirrors how a secure iot
ecosystem would be configured.  
For this we'll create a dummy root certificate
authority (normally this would be handled by a third party, or an internal
PKI system) and an intermediate (Signing) certificate authority.

### Create the Root CA
This command interactively enters the organization name and common name, and creates the related certificate and private key.
```
> create_root.py 
organization name:<your own name>
common name:<your own common name>

```

### Create the Signing CA
This command interactively enters the organization name and common name, and creates the related certificate and private key.  
make sure not to set the same name as accepted by "create_root.py".
```
> create_signer.py
organization name:<your own name>
common name:<your own common name>
```

### Create the dummy Device Certificate

This command interactively enters the organization name and creates an associated dummy device certificate. The common name is 0123xxxxxxxxxxxxee in 18-character format, and the unique value is extracted from the device and set.

```
> create_device.py  
organization name:testcorp  

```

This performs the certificate creation and signing process but does not load the
certificates into the device (provisioning).

### Create certificate definition file
```
> cert2certdef.py  

```
This will create a "cert_chain.c" file. This file will be used as a common   
certificate definition later in the process, to save the generated certificate   
to the secure element and to restore the certificate from the secure element.  



### Run the provisioning

Connect the ESP32 with the written firmware to the host PC with a serial cable  
and execute the following on the host side.  

```
> deviceprovision.py  


```

it asks which port you use.

```
Connected COM ports: ['COM6', 'COM4']  
Enter the number of the port to use :2  
Use COM port: COM4  

```

When the processing progresses as follows and "Device Provisioning Successful!"   
is displayed at the end, provisioning is completed.  

```
Recv:
Send: b'r'
Recv: Ready.

communication ready.
Send: b's'
Recv: 0123xxxxxxxxxxxxee
got serial number.

Load Device Public Key
    Loading from 0123xxxxxxxxxxxxee-device-pubkey.pem

Load Signer
    Loading key from signer-ca.key
    Loading certificate from signer-ca.crt

Create Device Certificate
F:\VScodes\azuredpstest\ECC608-MassProvisioning\scripts\create_device_be.py:114: CryptographyDeprecationWarning: Extension objects are deprecated as arguments to from_issuer_subject_key_identifier and support will be removed soon. Please migrate to passing a SubjectKeyIdentifier directly.
  x509.AuthorityKeyIdentifier.from_issuer_subject_key_identifier(issuer_ski),
    Save Device Certificate to 0123xxxxxxxxxxxxee-device.crt
    Save Root Public Key to root-pub.pem
Send: b'c'
Recv:
Send: b'-----BEGIN CERTIFICATE-----\n'
Recv:
Send: b'MIIByj….AyMRUw\n'
Recv:
Send: b'EwYD…Jvb3Qw\n'
Recv:
Send: b'HhcN….AxrbXdl\n'
Recv:
Send: b'Ym5l…ByqGSM49\n'
Recv:
Send: b'AgEGC….h/+GHngh\n'
Recv:
Send: b'qnkIq4P….EwEB/wQI\n'
Recv:
Send: b'MAYBA…JgeU7\n'
Recv:
Send: b'xW….hkjO\n'
Recv:
Send: b'PQQDA….sfoCIDkS\n'
Recv:
Send: b'Iiuo4I…6Oej5t7\n'
Recv:
Send: b'-----END CERTIFICATE-----\n'
Recv:
Send: b'\n'
Recv:
Send: b'v'
Recv:
Send: b'-----BEGIN CERTIFICATE-----\n'
Recv:
Send: b'MIIBpzC…Uw\n'
Recv:
Send: b'Ew….NpZ25l\n'
Recv:
Send: b'cjAgF…CgwI\n'
Recv:
Send: b'dGVz..qGSM49\n'
Recv:
Send: b'AgE…38S2ie\n'
Recv:
Send: b'gJ8A….BBQE\n'
Recv:
Send: b'sAQ…..UC\n'
Recv:
Send: b'gXCV…..dU0\n'
Recv:
Send: b'kPL…….rA5gw7rg==\n'
Recv:
Send: b'-----END CERTIFICATE-----\n'
Recv:
Send: b'\n'
Recv:
Send: b'b'
Recv:
Send: b'-----BEGIN PUBLIC KEY-----\n'
Recv:
Send: b'MFk…………T8D5p\n'
Recv:
Send: b'z…=\n'
Recv:
Send: b'-----END PUBLIC KEY-----\n'
Recv:
Send: b'\n'
Recv:
Send: b'q'
Recv:
provisioning start
Writing Root Public Key

Recv: device sn provisioning complete.
Writing Signer Certificate
Writing Device Certificate
Reading Signer Certificate
Comparing Signer Certificate
Reading Device Certificate
Comparing Device Certificate

Device Provisioning Successful!

```


