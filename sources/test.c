#include <stdio.h>
#include "dec.h"


void main()
{
	unsigned int buf[8];
    char choice=' ';
    char buffer[17];
    unsigned char key[9], plaintext[9],ciphertext[9];
    
    int ii;

    while(1)
    {
        do
        {
            printf("Please make a choice:\n    1.encryption\n    2.decryption\n    3.exit\n");
            scanf("\n%c",&choice);
        }while(choice!='1'&&choice!='2'&&choice!='3');

        switch(choice)
        {
        case '1':
            printf("Please input key(length=16):\n****************\n");
            scanf("%s",buffer);
            sscanf(buffer,"%2x%2x%2x%2x%2x%2x%2x%2x",&buf[0],&buf[1],&buf[2],&buf[3],&buf[4],&buf[5],&buf[6],&buf[7]);
            for(ii=0;ii<8;ii++)
                key[ii]=buf[ii];
            printf("Please input plaintext(length=16):\n");
            scanf("%s",buffer);
            sscanf(buffer,"%2x%2x%2x%2x%2x%2x%2x%2x",&buf[0],&buf[1],&buf[2],&buf[3],&buf[4],&buf[5],&buf[6],&buf[7]);
            for(ii=0;ii<8;ii++)
                plaintext[ii]=buf[ii];
            setkey(key);
            enctransform(plaintext,ciphertext);
            for(ii=0;ii<8;ii++)
                buf[ii]=ciphertext[ii];
            printf("ciphertext:\n%02x%02x%02x%02x%02x%02x%02x%02x\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
            break;
        case '2':
            printf("Please input key(length=16):\n****************\n");
            scanf("%s",buffer);
            sscanf(buffer,"%2x%2x%2x%2x%2x%2x%2x%2x",&buf[0],&buf[1],&buf[2],&buf[3],&buf[4],&buf[5],&buf[6],&buf[7]);
            for(ii=0;ii<8;ii++)
                key[ii]=buf[ii];
            printf("Please input ciphertext(length=16):\n");
            scanf("%s",buffer);
            sscanf(buffer,"%2x%2x%2x%2x%2x%2x%2x%2x",&buf[0],&buf[1],&buf[2],&buf[3],&buf[4],&buf[5],&buf[6],&buf[7]);
            for(ii=0;ii<8;ii++)
                ciphertext[ii]=buf[ii];
            setkey(key);
            dectransform(ciphertext,plaintext);
            for(ii=0;ii<8;ii++)
                buf[ii]=plaintext[ii];
            printf("plaintext:\n%02x%02x%02x%02x%02x%02x%02x%02x\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
            break;
        case '3':
            return 0;
            break;
        }
    }//end of while(true)
}