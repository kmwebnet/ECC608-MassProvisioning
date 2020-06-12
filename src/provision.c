#include "provision.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "crypto/atca_crypto_sw_sha1.h"

extern uint8_t provisioning_signer_cert[500];
extern uint8_t provisioning_device_cert[500];
extern uint8_t provisioning_root_public_key[100];
extern char serialstr[20];

uint8_t device_der_qa[sizeof(provisioning_device_cert) ] = {};
uint8_t signer_der_qa[sizeof(provisioning_signer_cert) ] = {};


/* Writes the certificates into a device*/
int atca_provision(ATCAIfaceCfg *cfg)
{
	int ret = 1;
	size_t tmp_size;
	ATCA_STATUS status;
	int i;
	bool diff = false;

	/* Start a session */
	if (ATCA_SUCCESS != (status = atcab_init(cfg)))
	{
		printf("Failed to init: %d\r\n", status);
		goto exit;
	}

    if (g_cert_def_1_signer.ca_cert_def)
    {
        /* Write the ca public key (signer authority) - normally static data */
        printf("Writing Root Public Key\r\n");

        if (ATCA_SUCCESS != (status = atcab_write_pubkey(g_cert_def_1_signer.ca_cert_def->public_key_dev_loc.slot,
            &provisioning_root_public_key[27])))
        {
            printf("Failed to write root ca public key\r\n");
            goto exit;
        }
    }

// write device serial and verify

    status = atcab_write_bytes_zone(ATCA_ZONE_DATA, 8 , 52 , (uint8_t *)serialstr , 20 );

    if (status != ATCA_SUCCESS) {
	printf("write device sn to slot8 failed with ret=0x%08d/r/n", status);
    }

    char verifyresult2 [20] = {};
    status = atcab_read_bytes_zone(ATCA_ZONE_DATA, 8 , 52 , (uint8_t *)verifyresult2 , 20 );

    if (status != ATCA_SUCCESS) {
	printf("read device sn to slot8 failed with ret=0x%08d/r/n", status);
    }

    if (strcmp(serialstr , verifyresult2) == 0)
    {
    printf("device sn provisioning complete.\n");
    }
    else
    {
        printf("error comparing device sn\n");
    }


vTaskDelay(10 / portTICK_PERIOD_MS);


	/* Write the signer certificate */
	printf("Writing Signer Certificate\r\n");
	if (ATCA_SUCCESS != (status = atcacert_write_cert(&g_cert_def_1_signer, provisioning_signer_cert, g_cert_def_1_signer.cert_template_size)))
	{
		printf("Failed to write signer certificate: %d\r\n", status);
		goto exit;
	}





	atcab_release();
	if (ATCA_SUCCESS != (status = atcab_init(cfg)))
	{
		printf("Failed to init: %d\r\n", status);
		goto exit;
	}

	/* Write the device certificate */
	printf("Writing Device Certificate\r\n");
	if (ATCA_SUCCESS != (status = atcacert_write_cert(&g_cert_def_2_device, provisioning_device_cert, g_cert_def_2_device.cert_template_size)))
	{
		printf("Failed to write device certificate: %d\r\n", status);
		goto exit;
	}


	/* Read back the signer certificate */
	tmp_size = sizeof(signer_der_qa) ; // signer_size + 10
	printf("Reading Signer Certificate\r\n");
	if (ATCA_SUCCESS != (status = atcacert_read_cert(&g_cert_def_1_signer, &provisioning_root_public_key[27], signer_der_qa, &tmp_size)))
	{
		printf("Failed to read signer certificate: %d\r\n", status);
		goto exit;
	}

	/* Compare the signer certificate */
	printf("Comparing Signer Certificate\r\n");
	if (memcmp(provisioning_signer_cert, signer_der_qa, tmp_size))
	{
		printf("Signer certificate missmatch\r\n");
		diff = false;
		for (i = 0; i < tmp_size; i++)
		{
			if (provisioning_signer_cert[i] != signer_der_qa[i])
			{
				diff = true;
			}

			if (0 == (i % 16))
			{
				printf("%s\r\n%04X: ", diff?"*":"", i);
				diff = false;
			}
			printf("%02X|%02X ", provisioning_signer_cert[i], signer_der_qa[i]);
		}
	}

	/* Read back the device certificate */
	tmp_size = sizeof(device_der_qa); //device_size + 10;
	printf("Reading Device Certificate\r\n");
	if (ATCA_SUCCESS != (status = atcacert_read_cert(&g_cert_def_2_device,
		&signer_der_qa[g_cert_def_1_signer.std_cert_elements[0].offset],
		device_der_qa, &tmp_size)))
	{
		printf("Failed to read device certificate: %d\r\n", status);
	}

	/* Compare the device certificate */
	printf("Comparing Device Certificate\r\n");
	if (memcmp(provisioning_device_cert, device_der_qa, tmp_size))
	{
		printf("Device certificate missmatch\r\n");

		diff = false;
		for (i = 0; i < tmp_size; i++)
		{
			if (provisioning_device_cert[i] != device_der_qa[i])
			{
				diff = true;
			}

			if (0 == (i % 16))
			{
				printf("%s\r\n%04X: ", diff ? "*" : "", i);
				diff = false;
			}
			printf("%02X|%02X ", provisioning_device_cert[i], device_der_qa[i]);
		}
	}

	printf("\r\nDevice Provisioning Successful!\r\n");

	/* End the session */

exit:
	return ret;
}

#if defined(_WIN32) || defined(__linux__) || defined(__unix__)

int main(int argc, char *argv[])
{
    return atca_provision();
}

#endif
