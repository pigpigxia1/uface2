#include <stdio.h>
#include "dec.h"


unsigned char enckey[16][48];//存放加密子密钥
unsigned char deckey[16][48];//存放解密子密钥

const unsigned char IP_Table[64] = {
    58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17,  9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
};
// final permutation IP^-1
const unsigned char IPR_Table[64] = {
    40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41,  9, 49, 17, 57, 25
};
// expansion operation matrix
const unsigned char E_Table[48] = {
    32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
     8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
};
// 32-bit permutation function P used on the output of the S-boxes
const unsigned char P_Table[32] = {
    16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
    2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25
};
// permuted choice table (key)
const unsigned char PC1_Table[56] = {
    57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
    10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
    14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
};
// permuted choice key (table)
const unsigned char PC2_Table[48] = {
    14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};
// number left rotations of pc1
const unsigned char LOOP_Table[16] = {
    1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};
// The S-boxes
const unsigned char S_Box[8][4][16] = {
    // S1
    14,     4,    13,     1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
     0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
     4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
    15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
    // S2
    15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
     3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
     0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
    13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
    // S3
    10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
    13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
    13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
     1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
    // S4
     7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
    13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
    10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
     3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
    // S5
     2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
    14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
     4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
    11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
    // S6
    12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
    10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
     9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
     4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
    // S7
     4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
    13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
     1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
     6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
    // S8
    13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
     1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
     7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
     2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};



void setkey( unsigned char const *userkey/*用户输入的64位密钥首地址*/ )
{
	printf("%s:%d\n",__FUNCTION__,__LINE__);
	int ii,i,j;
	int k,p,r,kk;
    //密钥扩展算法，即由主密钥生成子密钥，参见教材44-45页
    unsigned char userkey2[8];
    for(ii=0;ii<8;ii++)
        userkey2[ii]=userkey[ii];//复制主密钥
        //化比特为数组
    unsigned char key[64];
    for(i=7;i>=0;i--)
	{
        for( j=7;j>=0;j--)
		{
			key[i*8+j]=userkey2[i]&0x01;
			userkey2[i]=userkey2[i]>>1;
			printf("key = %x\t",key[i*8+j]);
		}
		printf("\n");
	}
    //输出测试
   // for(int jj=0;jj<64;jj++)
     //   printf("%d",key[jj]);
        //去除校验位，变为56
    unsigned char key2[56];
    for( k=1,kk=0;k<=64;k++)
    {
        if(k%8!=0)
        {
            key2[kk]=key[k-1];
            kk++;
        }
    }
    //输出测试 去除校验位后的
    //printf("\n");
    //for(int j=0;j<56;j++)
    //    printf("%d",key2[j]);
    //    printf("\n");
    unsigned char c0[28];
    unsigned char c1[28];
    //拆分为两个
    for( p=0;p<28;p++)
    {
        c0[p]=key2[PC1_Table[p]-1];
        c1[p]=key2[PC1_Table[p+28]-1];//c1就是d
    }
    //输出测试两个拆分
    //for(int p=0;p<28;p++)
    //{
    //    printf("%d",c0[p]);
    //}
    //printf("\n");
    //    for(int p=0;p<28;p++)
    //{
    //    printf("%d",c1[p]);
    //}
    //    printf("\n");

    int tempint1;
    int tempint2;
    int tempint11;
    int tempint22;
    //16个子密钥的生成
    for( i=0;i<16;i++)
    {
        if(LOOP_Table[i]==1)
        {
        tempint1=c0[0];
        tempint11=c1[0];
        for(j=1;j<28;j++)
        {
            c0[j-1]=c0[j];
            c1[j-1]=c1[j];
        }
        c0[27]=tempint1;
        c1[27]=tempint11;
        }
        else
        {
            tempint1=c0[0];
            tempint2=c0[1];
            tempint11=c1[0];
            tempint22=c1[1];
            for(j=2;j<28;j++)
            {
                c0[j-2]=c0[j];
                c1[j-2]=c1[j];
            }
            c0[26]=tempint1;
            c0[27]=tempint2;
            c1[26]=tempint11;
            c1[27]=tempint22;
        }
        for(r=0;r<56;r++)
        {
            if(r<28)
                key2[r]=c0[r];
            else
                key2[r]=c1[r-28];
        }
        for(r=0;r<48;r++)
        {
            enckey[i][r]=key2[PC2_Table[r]-1];
        }

   // for(int k=0;k<48;k++)
   //     printf("%d",enckey[i][k]);

   // printf("\n");
    }
//printf("\n");
printf("%s:%d\n",__FUNCTION__,__LINE__);
}

void enctransform( const unsigned char *In /*待加密的64位数据首地址*/,unsigned char *Out )
{
	printf("%s:%d\n",__FUNCTION__,__LINE__);
	int ii,i,j;
	int i1,p,q;
	int k,f1,jj;
    //加密变换算法，参见教材42、43、46页
     unsigned char In2[8];
    for(ii=0;ii<8;ii++)
        In2[ii]=In[ii];//复制64位明文

	printf("%s:%d\n",__FUNCTION__,__LINE__);
        //化比特为数组
    unsigned char In3[64];
    for(i=7;i>=0;i--)
        for(j=7;j>=0;j--)
    {
        In3[i*8+j]=In2[i]&0x01;
        In2[i]=In2[i]>>1;
    }
	printf("%s:%d\n",__FUNCTION__,__LINE__);
    //观察是否拆分成功                //到这里是对的
    //for(int i=0;i<64;i++)
    //    printf("%d",In3[i]);
    //printf("\n");

    //printf("%d\n",In3[63]);
    //初始置换IP                       //到这里还是对的
    unsigned char In4[64];
    for( i=0;i<64; i++)
        In4[i]=In3[IP_Table[i]-1];
	printf("%s:%d\n",__FUNCTION__,__LINE__);
    //观察是否置换成功
    //for(int i=0;i<64;i++)
    //printf("%d",In4[i]);
    //printf("\n");
    //16次迭代
    unsigned char L[32],R[32],tempL[32],tempR[32],F[32],FP[32],ER[48];
    unsigned char B1[6],B2[6],B3[6],B4[6],B5[6],B6[6],B7[6],B8[6];
    for(i=0;i<32;i++)
    {
        L[i]=In4[i];
        R[i]=In4[i+32];
    }
	printf("%s:%d\n",__FUNCTION__,__LINE__);
    for(i=0;i<16;i++)
    {
		for(j=0;j<32;j++)
		{
		tempL[j]=L[j];
		tempR[j]=R[j];
		}
		
		for( j=0;j<32;j++)
			L[j]=tempR[j];
		
     //f函数定义

    // for(int i3=0;i3<48;i3++)
     //   printf("%d-",tempR[E_Table[i3]-1]);
    // printf("\n");
		for(i1=0;i1<48;i1++)
		{
			ER[i1]=tempR[E_Table[i1]-1]^enckey[i][i1];
			printf("enckey[i][%d]=%d\t",i1,enckey[i][i1]);
			if((i1+1)%6 == 0)
				printf("\n");
		}

		
     //    for(int i2=0;i2<48;i2++)
     //       printf("%d ",ER[i2]);
     //    printf("\n");
		for( p=0;p<48;p++)
		{
			int temp=p/6;
			switch(temp)
			{
				case 0:  B1[p]=ER[p];break;
				case 1:  B2[p-6]=ER[p];break;
				case 2:  B3[p-12]=ER[p];break;
				case 3:  B4[p-18]=ER[p];break;
				case 4:  B5[p-24]=ER[p];break;
				case 5:  B6[p-30]=ER[p];break;
				case 6:  B7[p-36]=ER[p];break;
				case 7:  B8[p-42]=ER[p];break;
				default: printf("error in switch!\n");
			}
		}
		
	
        unsigned char f;
        f=S_Box[0][B1[0]*2+B1[5]][B1[1]*8+B1[2]*4+B1[3]*2+B1[4]];
        for(f1=3;f1>=0;f1--)
        {
			F[f1]=f&0x01;
			f=f>>1;
		   // printf("%d,%d:%d\n",f,f1,F[f1]);
        }
		
        f=S_Box[1][B2[0]*2+B2[5]][B2[1]*8+B2[2]*4+B2[3]*2+B2[4]];
        for(f1=3;f1>=0;f1--)
        {
			F[f1+4]=f&0x01;
			f=f>>1;
        }
		
        f=S_Box[2][B3[0]*2+B3[5]][B3[1]*8+B3[2]*4+B3[3]*2+B3[4]];
        for(f1=3;f1>=0;f1--)
        {
			F[f1+8]=f&0x01;
			f=f>>1;
        }
		
        f=S_Box[3][B4[0]*2+B4[5]][B4[1]*8+B4[2]*4+B4[3]*2+B4[4]];
        for(f1=3;f1>=0;f1--)
        {
			F[f1+12]=f&0x01;
			f=f>>1;
        }
		
        f=S_Box[4][B5[0]*2+B5[5]][B5[1]*8+B5[2]*4+B5[3]*2+B5[4]];
        for(f1=3;f1>=0;f1--)
        {
			F[f1+16]=f&0x01;
			f=f>>1;
        }
		
        f=S_Box[5][B6[0]*2+B6[5]][B6[1]*8+B6[2]*4+B6[3]*2+B6[4]];
        for(f1=3;f1>=0;f1--)
        {
			F[f1+20]=f&0x01;
			f=f>>1;
        }
		printf("%s:%d\n",__FUNCTION__,__LINE__);
		printf("B7[0]:%d B7[1]:%d B7[2]:%d B7[3]:%d B7[4]:%d B7[5]:%d\n",B7[0],B7[1],B7[2],B7[3],B7[4],B7[5]);
        f=S_Box[6][B7[0]*2+B7[5]][B7[1]*8+B7[2]*4+B7[3]*2+B7[4]];
        for( f1=3;f1>=0;f1--)
        {
			F[f1+24]=f&0x01;
			f=f>>1;
        }
		printf("%s:%d\n",__FUNCTION__,__LINE__);
        f=S_Box[7][B8[0]*2+B8[5]][B8[1]*8+B8[2]*4+B8[3]*2+B8[4]];
        for(f1=3;f1>=0;f1--)
        {
			F[f1+28]=f&0x01;
			f=f>>1;
        }
		
		for( q=0;q<32;q++)
		{
			FP[q]=F[P_Table[q]-1];
		}
	

		 for(k=0;k<32;k++)
		 {
			 R[k]=tempL[k]^FP[k];
		 }

    }
	
    //合并
        for(i=0;i<32;i++)
    {
        In4[i]=R[i];
        In4[i+32]=L[i];
    }
	
    //逆置换
    for(i=0;i<64;i++)
     {
        In3[i]=In4[IPR_Table[i]-1];
     }

	 
     //ce shi
    // for(int i=0;i<64;i++)
     //   printf("%d",In3[i]);
    //转出  1 2 4 8 16 32 64 128
    for(i=0;i<8;i++)
    {
        Out[i]=0;
        for(j=0;j<8;j++)
    {
         p=128,jj=j;
        while(jj!=0)
        {
            p=p/2;
            jj--;
        }
        Out[i]+=In3[i*8+j]*p;
    }
    }
	printf("%s:%d\n",__FUNCTION__,__LINE__);
}

void dectransform( const unsigned char *In /*待解密的64位数据首地址*/, unsigned char *Out)
{
	printf("%s:%d\n",__FUNCTION__,__LINE__);
	int ii,i,j;
	int i1,p,q;
	int k,f1,jj;
//解密变换算法，参见教材48页
     unsigned char In2[8];
    for(ii=0;ii<8;ii++)
        In2[ii]=In[ii];//复制64位明文

        //化比特为数组
    unsigned char In3[64];
    for(i=7;i>=0;i--)
        for(j=7;j>=0;j--)
    {
        In3[i*8+j]=In2[i]&0x01;
        In2[i]=In2[i]>>1;
    }
    //观察是否拆分成功                //到这里是对的
    //for(int i=0;i<64;i++)
    //    printf("%d",In3[i]);
    //printf("\n");

    //printf("%d\n",In3[63]);
    //初始置换IP                       //到这里还是对的
    unsigned char In4[64];
    for( i=0;i<64; i++)
        In4[i]=In3[IP_Table[i]-1];
    //观察是否置换成功
    //for(int i=0;i<64;i++)
    //printf("%d",In4[i]);
    //printf("\n");
    //16次迭代
    unsigned char L[32],R[32],tempL[32],tempR[32],F[32],FP[32],ER[48];
    unsigned char B1[6],B2[6],B3[6],B4[6],B5[6],B6[6],B7[6],B8[6];
    for( i=0;i<32;i++)
    {
        L[i]=In4[i];
        R[i]=In4[i+32];
    }
    for( i=0;i<16;i++)
    {
     for( j=0;j<32;j++)
     {
        tempL[j]=L[j];
        tempR[j]=R[j];
     }
     for( j=0;j<32;j++)
        L[j]=tempR[j];
     //f函数定义

    // for(int i3=0;i3<48;i3++)
     //   printf("%d-",tempR[E_Table[i3]-1]);
    // printf("\n");
         for(i1=0;i1<48;i1++)
         ER[i1]=tempR[E_Table[i1]-1]^enckey[15-i][i1];

     //    for(int i2=0;i2<48;i2++)
     //       printf("%d ",ER[i2]);
     //    printf("\n");
         for( p=0;p<48;p++)
         {
             int temp=p/6;
             switch(temp)
             {
                 case 0:  B1[p]=ER[p];break;
                 case 1:  B2[p-6]=ER[p];break;
                 case 2:  B3[p-12]=ER[p];break;
                 case 3:  B4[p-18]=ER[p];break;
                 case 4:  B5[p-24]=ER[p];break;
                 case 5:  B6[p-30]=ER[p];break;
                 case 6:  B7[p-36]=ER[p];break;
                 case 7:  B8[p-42]=ER[p];break;
                default: printf("error in switch!\n");
             }
         }

        unsigned char f;
        f=S_Box[0][B1[0]*2+B1[5]][B1[1]*8+B1[2]*4+B1[3]*2+B1[4]];
        for(f1=3;f1>=0;f1--)
        {
        F[f1]=f&0x01;
        f=f>>1;
       // printf("%d,%d:%d\n",f,f1,F[f1]);
        }
                f=S_Box[1][B2[0]*2+B2[5]][B2[1]*8+B2[2]*4+B2[3]*2+B2[4]];
        for( f1=3;f1>=0;f1--)
        {
        F[f1+4]=f&0x01;
        f=f>>1;
        }
                f=S_Box[2][B3[0]*2+B3[5]][B3[1]*8+B3[2]*4+B3[3]*2+B3[4]];
        for(f1=3;f1>=0;f1--)
        {
        F[f1+8]=f&0x01;
        f=f>>1;
        }
                f=S_Box[3][B4[0]*2+B4[5]][B4[1]*8+B4[2]*4+B4[3]*2+B4[4]];
        for( f1=3;f1>=0;f1--)
        {
        F[f1+12]=f&0x01;
        f=f>>1;
        }
                f=S_Box[4][B5[0]*2+B5[5]][B5[1]*8+B5[2]*4+B5[3]*2+B5[4]];
        for( f1=3;f1>=0;f1--)
        {
        F[f1+16]=f&0x01;
        f=f>>1;
        }
                f=S_Box[5][B6[0]*2+B6[5]][B6[1]*8+B6[2]*4+B6[3]*2+B6[4]];
        for( f1=3;f1>=0;f1--)
        {
        F[f1+20]=f&0x01;
        f=f>>1;
        }
                f=S_Box[6][B7[0]*2+B7[5]][B7[1]*8+B7[2]*4+B7[3]*2+B7[4]];
        for( f1=3;f1>=0;f1--)
        {
        F[f1+24]=f&0x01;
        f=f>>1;
        }
                f=S_Box[7][B8[0]*2+B8[5]][B8[1]*8+B8[2]*4+B8[3]*2+B8[4]];
        for( f1=3;f1>=0;f1--)
        {
        F[f1+28]=f&0x01;
        f=f>>1;
        }
    for( q=0;q<32;q++)
    {
        FP[q]=F[P_Table[q]-1];
    }

     for( k=0;k<32;k++)
     {
         R[k]=tempL[k]^FP[k];
     }

    }
    //合并
        for(i=0;i<32;i++)
    {
        In4[i]=R[i];
        In4[i+32]=L[i];
    }
    //逆置换
    for(i=0;i<64;i++)
     {
        In3[i]=In4[IPR_Table[i]-1];
     }

     //ce shi
    // for(int i=0;i<64;i++)
     //   printf("%d",In3[i]);
    //转出  1 2 4 8 16 32 64 128
    for(i=0;i<8;i++)
    {
        Out[i]=0;
        for( j=0;j<8;j++)
    {
        p=128,jj=j;
        while(jj!=0)
        {
            p=p/2;
            jj--;
        }
        Out[i]+=In3[i*8+j]*p;
    }
    }
	printf("%s:%d\n",__FUNCTION__,__LINE__);
}
