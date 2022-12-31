#include <stdint.h>
#include <stdio.h>
#include <string.h> 
#include <stdbool.h>
//#include "aes.h"


#define AES_BLOCKLEN 16 //Block length in bytes AES is 128b block only
#define AES_KEYLEN 16   // Key length in bytes
#define AES_keyExpSize 176
// The number of columns comprising a state in AES. This is a constant in AES. Value=4
#define Nb 4
#define Nk 4        // The number of 32 bit words in a key.
#define Nr 9       // The number of rounds in AES Cipher.

struct Bit_81 {  bool c[4][2]; };

typedef uint8_t state_t[4][4];

void state_print(state_t* state) {
  printf("state\n");
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      printf("%d ",(*state)[i][j]);
    }
    printf("\n");
  }
  
}

struct AES_ctx
{
  uint8_t RoundKey[AES_keyExpSize];
  uint8_t Iv[AES_BLOCKLEN];
};

//sbox in 9*9
static const uint8_t sbox_9[9][9] = {
{53, 1, 47, 78, 42, 60, 70, 57, 54},
{23, 4, 38, 26, 5, 7, 20, 43, 10},
{48, 28, 30, 65, 45, 52, 0, 35, 63},
{82, 84, 74, 85, 56, 76, 2, 88, 66},
{71, 62, 36, 17, 13, 21, 75, 25, 67},
{27, 87, 37, 46, 77, 22, 12, 55, 64},
{11, 81, 6, 40, 61, 86, 8, 41, 51},
{31, 34, 83, 68, 80, 3, 15, 58, 32},
{18, 33, 44, 14, 73, 72, 50, 16, 24}
};

//reverse sbox 
static const uint8_t rsbox_9[9][9]= {
{26, 1, 36, 75, 11, 14, 62, 15, 66},
{18, 60, 56, 44, 83, 76, 87, 43, 80},
{16, 45, 55, 10, 88, 47, 13, 50, 21},
{22, 70, 78, 81, 71, 27, 42, 52, 12},
{63, 67, 4, 17, 82, 24, 53, 2, 20},
{86, 68, 25, 0, 8, 57, 34, 7, 77},
{5, 64, 41, 28, 58, 23, 38, 48, 73},
{6, 40, 85, 84, 32, 46, 35, 54, 3},
{74, 61, 30, 72, 31, 33, 65, 51, 37}
};

// The round constant word array, Rcon[i], contains the values given by 
// x to the power (i-1) being powers of x (x is denoted as {02}) in the field GF(3^4)
// Rcon[1]=x^0= 1, Rcon[5]=x* x^3=xtime(30)=leftshift(30) xor {88}, ...
/*
 actually used – up to rcon[10] for AES-128 (as 11 round keys are needed), 
 up to rcon[8] for AES-192, up to rcon[7] for AES-256. rcon[0] is not used in AES algorithm."
 */
static uint8_t Rcon[10] = {
  58, 1, 3, 10, 30, 88, 45, 5, 16, 50 };


// state - array holding the intermediate results during decryption.





struct Bit_81 xor_3_81(struct Bit_81 in_1, struct Bit_81 in_2) {
    struct Bit_81 out;

    out.c[0][0] = (!(in_1.c[0][1] || in_2.c[0][0])) && ((in_1.c[0][0] && in_2.c[0][1]) || !(in_1.c[0][0] || in_2.c[0][1]))&1;
    out.c[0][1] = (!in_1.c[0][1]) && ((in_1.c[0][0] && in_2.c[0][0] &&!in_2.c[0][1]) ||(!(in_1.c[0][0]||in_2.c[0][0])&&in_2.c[0][1]))&1;

    out.c[1][0] = (!(in_1.c[1][1] || in_2.c[1][0])) && ((in_1.c[1][0] && in_2.c[1][1]) || !(in_1.c[1][0] || in_2.c[1][1]))&1;
    out.c[1][1] = (!in_1.c[1][1]) && ((in_1.c[1][0] && in_2.c[1][0] &&!in_2.c[1][1]) ||(!(in_1.c[1][0]||in_2.c[1][0])&&in_2.c[1][1]))&1;

    out.c[2][0] = (!(in_1.c[2][1] || in_2.c[2][0])) && ((in_1.c[2][0] && in_2.c[2][1]) || !(in_1.c[2][0] || in_2.c[2][1]))&1;
    out.c[2][1] = (!in_1.c[2][1]) && ((in_1.c[2][0] && in_2.c[2][0] &&!in_2.c[2][1]) ||(!(in_1.c[2][0]||in_2.c[2][0])&&in_2.c[2][1]))&1;

    out.c[3][0] = (!(in_1.c[3][1] || in_2.c[3][0])) && ((in_1.c[3][0] && in_2.c[3][1]) || !(in_1.c[3][0] || in_2.c[3][1]))&1;
    out.c[3][1] = (!in_1.c[3][1]) && ((in_1.c[3][0] && in_2.c[3][0] &&!in_2.c[3][1]) ||(!(in_1.c[3][0]||in_2.c[3][0])&&in_2.c[3][1]))&1;

    return out;
}

//uint8_t Bit_81_to_num(struct Bit_81 bit_81);

//PLUS arithmetic in GF(3^4)

struct Bit_81 PLUS_3_81(struct Bit_81 in_1, struct Bit_81 in_2) {
    struct Bit_81 out;

    // !c && ((a && !(b || d)) || (!a && b && d))
    out.c[0][0] = !in_2.c[0][0] && ((in_1.c[0][0] && !(in_1.c[0][1] || in_2.c[0][1])) || (!in_1.c[0][0] && in_1.c[0][1] && in_2.c[0][1]));

    out.c[0][1]=(!in_1.c[0][1])&&((in_1.c[0][0]&&in_2.c[0][0]&&(!in_2.c[0][1]))||((!(in_1.c[0][0]||in_2.c[0][0]))&&in_2.c[0][1]));

    out.c[1][0] = !in_2.c[1][0] && ((in_1.c[1][0] && !(in_1.c[1][1] || in_2.c[1][1])) || (!in_1.c[1][0] && in_1.c[1][1] && in_2.c[1][1]));

    out.c[1][1]=(!in_1.c[1][1])&&((in_1.c[1][0]&&in_2.c[1][0]&&(!in_2.c[1][1]))||((!(in_1.c[1][0]||in_2.c[1][0]))&&in_2.c[1][1]));

    out.c[2][0] = !in_2.c[2][0] && ((in_1.c[2][0] && !(in_1.c[2][1] || in_2.c[2][1])) || (!in_1.c[2][0] && in_1.c[2][1] && in_2.c[2][1]));

    out.c[2][1]=(!in_1.c[2][1])&&((in_1.c[2][0]&&in_2.c[2][0]&&(!in_2.c[2][1]))||((!(in_1.c[2][0]||in_2.c[2][0]))&&in_2.c[2][1]));

    out.c[3][0] = !in_2.c[3][0] && ((in_1.c[3][0] && !(in_1.c[3][1] || in_2.c[3][1])) || (!in_1.c[3][0] && in_1.c[3][1] && in_2.c[3][1]));

    out.c[3][1]=(!in_1.c[3][1])&&((in_1.c[3][0]&&in_2.c[3][0]&&(!in_2.c[3][1]))||((!(in_1.c[3][0]||in_2.c[3][0]))&&in_2.c[3][1]));
    //printf("%d\n",Bit_81_to_num(out));
    return out;
}


//number to Bit81 structure

struct Bit_81 num_to_Bit_81(uint8_t num) {
  if(num>88){
    printf("%d ", num);
  }
  uint8_t size = sizeof(num) / sizeof(uint8_t);
  struct Bit_81 bit81;
  uint8_t temp1, temp2;
  
    temp1 = num / 30;
    temp2 = num % 30;
    bit81.c[0][0] = (temp1 >> 1)&1;
    bit81.c[0][1]=temp1 & 1;

    temp1 = temp2 / 10;
    temp2 = temp2 % 10;
    bit81.c[1][0]=(temp1 >> 1)&1;
    bit81.c[1][1]=temp1 & 1;

    temp1 = temp2 / 3;
    temp2 = temp2 % 3;
    bit81.c[2][0]=temp1 >> 1;
    bit81.c[2][1]=temp1 & 1;
    bit81.c[3][0]=temp2 >> 1;
    bit81.c[3][1]=temp2 & 1;

  return bit81;

}

void num_to_Bit_81_arr(uint8_t* numarr, struct Bit_81 arr81[], uint8_t size) {
  //uint8_t size = sizeof(numarr) / sizeof(uint8_t);
  
  uint8_t temp1, temp2;
  for(int i = 0; i < size; i++) {
    temp1 = numarr[i] / 30;
    temp2 = numarr[i] % 30;
    arr81[i].c[0][0] = temp1 >> 1;
    arr81[i].c[0][1]=temp1 & 1;

    temp1 = temp2 / 10;
    temp2 = temp2 % 10;
    arr81[i].c[1][0]=temp1 >> 1;
    arr81[i].c[1][1]=temp1 & 1;

    temp1 = temp2 / 3;
    temp2 = temp2 % 3;
    arr81[i].c[2][0]=temp1 >> 1;
    arr81[i].c[2][1]=temp1 & 1;
    arr81[i].c[3][0]=temp2 >> 1;
    arr81[i].c[3][1]=temp2 & 1;

  }


}


//Bit81 structure to number

uint8_t Bit_81_to_num(struct Bit_81 bit_81) {
  //uint8_t size = sizeof(bit_81) / sizeof(struct Bit_81);
	uint8_t num;
  
    num =(bit_81.c[0][0]&1) * 2 * 30 + (bit_81.c[0][1]&1) * 30
    + (bit_81.c[1][0]&1) * 2 * 10 + (bit_81.c[1][1]&1) * 10
    + (bit_81.c[2][0]&1) * 2 * 3 + (bit_81.c[2][1]&1) * 3
    + (bit_81.c[3][0]&1) * 2 + (bit_81.c[3][1]&1);
	return num;
  
}

void Bit_81_to_num_arr(struct Bit_81* bit_81, uint8_t numarr[], uint8_t size) {

  //uint8_t size = sizeof(bit_81) / sizeof(struct Bit_81);
  

  for(int i = 0; i < size; i++) {
    for(int j=0;j< 4;j++) {
    if(bit_81[i].c[j][0] && bit_81[i].c[j][1]){
      bit_81[i].c[j][0]=0;
      bit_81[i].c[j][1]=0;
    }
    }
    numarr[i] = bit_81[i].c[0][0] * 2 * 30 + bit_81[i].c[0][1] * 30
    + bit_81[i].c[1][0] * 2 * 10 + bit_81[i].c[1][1] * 10
    + bit_81[i].c[2][0] * 2 * 3 + bit_81[i].c[2][1] * 3
    + bit_81[i].c[3][0] * 2 + bit_81[i].c[3][1];

  }

  
}

static struct Bit_81 kiyak;
static struct Bit_81 zero;

static struct Bit_81 Rcon_bit[10];

void pmf() {      //This initializes kiyak and zero struct.
  kiyak.c[0][0]=1;
  kiyak.c[0][1]=0;
  kiyak.c[1][0]=1;
  kiyak.c[1][1]=0;
  kiyak.c[2][0]=1;
  kiyak.c[2][1]=0;
  kiyak.c[3][0]=1;
  kiyak.c[3][1]=0;
  zero.c[0][0]=0;
  zero.c[0][1]=0;
  zero.c[1][0]=0;
  zero.c[1][1]=0;
  zero.c[2][0]=0;
  zero.c[2][1]=0;
  zero.c[3][0]=0;
  zero.c[3][1]=0;

  num_to_Bit_81_arr(Rcon, Rcon_bit, 10);
}





#pragma inline
void *myMemcpy(void * s1, const void * s2, size_t n )
{       /* copy char s2[n] to s1[n] in any order */
    char *su1 = (char *)s1;
    const char *su2 = (const char *)s2;

    for (; 0 < n; ++su1, ++su2, --n)
      *su1 = *su2;
    return (s1);
}


//getsbox

uint8_t getSBoxValue(uint8_t num) {
  return (sbox_9[(num)/10][(num)%10]);
}
uint8_t getSBoxInvert(uint8_t num){
  return (rsbox_9[(num)/10][(num)%10]);
}

// This function produces Nb(Nr+1) round keys. The round keys are used in each round to decrypt the states. 
static void KeyExpansion(uint8_t* RoundKey, uint8_t* Key)
{
  unsigned i, j, k;
  
  uint8_t tempa[4]; // Used for the column/row operations
  
  // The first round key is the key itself.
  for (i = 0; i < Nk; ++i)
  {
    RoundKey[(i * 4) + 0] = Key[(i * 4) + 0];
    RoundKey[(i * 4) + 1] = Key[(i * 4) + 1];
    RoundKey[(i * 4) + 2] = Key[(i * 4) + 2];
    RoundKey[(i * 4) + 3] = Key[(i * 4) + 3];
  }


  // All other round keys are found from the previous round keys.
  for (i = Nk; i < Nb * (Nr + 1); ++i)
  {
    {
      k = (i - 1) * 4;
      tempa[0]=RoundKey[k + 0];
      if(tempa[0]>88) {
        printf("flag\n");
      }
      tempa[1]=RoundKey[k + 1];
      tempa[2]=RoundKey[k + 2];
      tempa[3]=RoundKey[k + 3];

    }

    if (i % Nk == 0)
    {
      // This function shifts the 4 bytes in a word to the left once.
      // [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

      
      {
        uint8_t bittmp = tempa[0];
        tempa[0] = tempa[1];
        tempa[1] = tempa[2];
        tempa[2] = tempa[3];
        tempa[3] = bittmp;
      }


      
      
      tempa[0]=Bit_81_to_num(xor_3_81(num_to_Bit_81(tempa[0]), Rcon_bit[i/Nk]));
      
    }

    j = i * 4; 
    k=(i - Nk) * 4;
    
	  struct Bit_81 c=num_to_Bit_81(RoundKey[k]);
	  
    RoundKey[j + 0]=Bit_81_to_num(xor_3_81(num_to_Bit_81 (RoundKey[k + 0]), num_to_Bit_81(tempa[0])));
    //printf("%d ff\n", tempa[0]);
    //printf("rk%d\n", RoundKey[j]);
    
    RoundKey[j + 1]=Bit_81_to_num(xor_3_81(num_to_Bit_81 (RoundKey[k + 1]), num_to_Bit_81(tempa[1])));
    RoundKey[j + 2]=Bit_81_to_num(xor_3_81(num_to_Bit_81 (RoundKey[k + 2]), num_to_Bit_81(tempa[2])));
    RoundKey[j + 3]=Bit_81_to_num(xor_3_81(num_to_Bit_81 (RoundKey[k + 3]), num_to_Bit_81(tempa[3])));
    
  }
}

//iv initialization
void AES_init_ctx_iv(struct AES_ctx* ctx, uint8_t* key,uint8_t* iv)
{
  KeyExpansion(ctx->RoundKey, key);
  myMemcpy (ctx->Iv, (uint8_t*)iv, AES_BLOCKLEN);
  
}

// This function adds the round key to state.
// The round key is added to the state by an new XOR function. Which is called as OSF axioms.
static void AddRoundKey(uint8_t round,state_t* state,uint8_t* RoundKey)
{
  uint8_t i,j;
  
  for (i = 0; i < 4; ++i)
  {
    for (j = 0; j < 4; ++j)
    {
      (*state)[i][j]=Bit_81_to_num(xor_3_81(num_to_Bit_81((*state)[i][j]) ,num_to_Bit_81(RoundKey[(round * Nb * 4) + (i * Nb) + j])));
    }
  }
}



// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
static void SubBytes(state_t* state)
{
  uint8_t i, j;
  

  for (i = 0; i < 4; ++i)
  { 
    
    
    for (j = 0; j < 4; ++j)
    {
      (*state)[i][j]=getSBoxValue((*state)[i][j]);  
    }
    
  }
}

// The ShiftRows() function shifts the rows in the state to the left.

static void ShiftRows(state_t* state)
{
  uint8_t temp;

  // Rotate first row 1 columns to left  
  temp           = (*state)[0][1];
  (*state)[0][1] = (*state)[1][1];
  (*state)[1][1] = (*state)[2][1];
  (*state)[2][1] = (*state)[3][1];
  (*state)[3][1] = temp;

  // Rotate second row 2 columns to left  
  temp           = (*state)[0][2];
  (*state)[0][2] = (*state)[2][2];
  (*state)[2][2] = temp;

  temp           = (*state)[1][2];
  (*state)[1][2] = (*state)[3][2];
  (*state)[3][2] = temp;

  // Rotate third row 3 columns to left
  temp           = (*state)[0][3];
  (*state)[0][3] = (*state)[3][3];
  (*state)[3][3] = (*state)[2][3];
  (*state)[2][3] = (*state)[1][3];
  (*state)[1][3] = temp;
}


// Multiply is used to multiply numbers in the field GF(3^8)
static struct Bit_81 xtime_3(struct Bit_81 x)
{
  struct Bit_81 y;
  if((x.c[0][0] || x.c[0][1]) & 1) {
    y.c[0][0]=x.c[1][0];
    y.c[0][1]=x.c[1][1];
    y.c[1][0]=x.c[2][0];
    y.c[1][1]=x.c[2][1];
    y.c[2][0]=x.c[3][0];
    y.c[2][1]=x.c[3][1];
    y.c[3][0]=0;
    y.c[3][1]=0;
    y=PLUS_3_81(y,kiyak);
    //printf("%d\n",Bit_81_to_num(y));
    return PLUS_3_81(y,kiyak);
    }
  else {
    y.c[0][0]=x.c[1][0];
    y.c[0][1]=x.c[1][1];
    y.c[1][0]=x.c[2][0];
    y.c[1][1]=x.c[2][1];
    y.c[2][0]=x.c[3][0];
    y.c[2][1]=x.c[3][1];
    y.c[3][0]=0;
    y.c[3][1]=0;
    return y;
  }
}

// MixColumns function mixes the columns of the state matrix
// (3,4,1,1)
// (1,3,4,1)
// (1,1,3,4)
// (4,1,1,3)
static void MixColumns(state_t* state)
{
  uint8_t i;
  //printf("%d\n", (*state)[0][0]);
  
  struct Bit_81 tmp[4];
  struct Bit_81 tmp3[4];

  for (i = 0; i < 4; ++i)
  {  
    tmp[0]=num_to_Bit_81((*state)[i][0]);
    tmp[1]=num_to_Bit_81((*state)[i][1]);
    tmp[2]=num_to_Bit_81((*state)[i][2]);
    tmp[3]=num_to_Bit_81((*state)[i][3]);
    
    
    for(int j = 0; j<4; j++) { 
      //printf("%d ",Bit_81_to_num(tmp[j]));
      
      //printf("%d\n",Bit_81_to_num(tmp3[j]));
      tmp3[j] = xtime_3(tmp[j]);
      

    }
    //state_print(state);
    (*state)[i][0]=Bit_81_to_num(PLUS_3_81(PLUS_3_81(tmp3[0],PLUS_3_81(tmp3[1],tmp[1])), PLUS_3_81(tmp[2], tmp[3])));
    (*state)[i][1]=Bit_81_to_num(PLUS_3_81(PLUS_3_81(tmp[0], tmp3[1]), PLUS_3_81(PLUS_3_81(tmp3[2],tmp[2]), tmp[3])));
    (*state)[i][2]=Bit_81_to_num(PLUS_3_81(PLUS_3_81(tmp[0],tmp[1]), PLUS_3_81(tmp3[2],PLUS_3_81(tmp3[3],tmp3[3]))));
    (*state)[i][3]=Bit_81_to_num(PLUS_3_81(PLUS_3_81(PLUS_3_81(tmp3[0],tmp[0]),tmp[1]), PLUS_3_81(tmp[2], tmp3[3])));
    //state_print(state);
      }
  //state_print(state);
}








// Cipher is the main function that encrypts the PlainText.
static void Cipher(state_t* state, uint8_t* RoundKey)
{
  uint8_t round = 0;

  // Add the First round key to the state before starting the rounds.
  
  AddRoundKey(0, state, RoundKey); 
  
  
  // There will be Nr rounds.

  for (round = 1; round < Nr; ++round)
  {
    
    SubBytes(state);
    
    
    ShiftRows(state);
    
    
    MixColumns(state);
    
    AddRoundKey(round, state, RoundKey);
    //state_print(state);
  }
  
  // The last round is in below.
  // The MixColumns function is not here in the last round.
  SubBytes(state);
  ShiftRows(state);
  AddRoundKey(Nr, state, RoundKey);
  //state_print(state);
}




void AES_CTR_crypt_buffer(struct AES_ctx* ctx, struct Bit_81* buf, size_t length)
{
  uint8_t buffer[AES_BLOCKLEN];
  uint8_t tmp;
  struct Bit_81 buffer_bit[16];
  size_t i,k;
  int bi;
  //printf("%d\n", Bit_81_to_num(buf[0]));
  

  for (i = 0, bi = AES_BLOCKLEN; i < length; i++, bi++)
  {
    if (bi == AES_BLOCKLEN) 
    {
      
      memcpy(buffer, ctx->Iv, AES_BLOCKLEN);
      k=i/16;
      for(;k>0;k--){
        if(ctx->Iv[bi]%10==8){
          ctx->Iv[bi]+=1;
        }
        ctx->Iv[bi]+=1;
      }
      Cipher((state_t*)buffer,ctx->RoundKey);
      bi=0;
    }
    
    //printf("1 %d\n", buffer[i]);
    num_to_Bit_81_arr(buffer, buffer_bit, 16);
    
    buf[i] = xor_3_81(buf[i], buffer_bit[bi]);
    //printf("2 %d\n", Bit_81_to_num(buf[i]));
    
  }
}


static int test_crypt_CTR(void)
{
    
    // Encryption key Public key fixed value
    uint8_t key[16] = { 
      21, 55, 55, 55, 21, 55, 55, 21, 21, 21, 55, 21, 55, 21, 21, 21
       };
    
    // The initialization vector value must continue to change to random form.
    uint8_t iv[16]  = { 
      0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 14, 15, 16 
      };

    // Input data Value of encrypted state
    uint8_t in[64]  = { 
    26, 41, 67, 15, 12, 78, 77, 35,
    1, 14, 28, 18, 12, 65, 36, 14,
    32, 77, 35, 43, 35, 75, 0, 16,
    53, 18, 23, 73, 67, 71, 47, 47,
    2, 42, 65, 63, 50, 10, 11, 63,
    70, 14, 5, 70, 12, 45, 62, 33,
    37, 51, 75, 41, 34, 68, 25, 77,
    70, 77, 18, 43, 18, 45, 0, 23
     };

                      
    struct AES_ctx ctx;

    struct Bit_81 key_bit[16];
    
    num_to_Bit_81_arr(key, key_bit,16);
    //printf("%d\n", key_bit[1].c[0][1]);
    struct Bit_81 iv_bit[16];
    num_to_Bit_81_arr(iv, iv_bit,16);
    struct Bit_81 in_bit[64];
    num_to_Bit_81_arr(in, in_bit,16);
    uint8_t out[64];

    // Generate RoundKey with initialization vector value and public key
    AES_init_ctx_iv(&ctx, key, iv);
    
    
    //encryption
    AES_CTR_crypt_buffer(&ctx, in_bit, 64);
    Bit_81_to_num_arr(in_bit, out, 64);
    
    printf("CTR crypt: \n");
    
    
    for(uint8_t i = 0; i < 64; i++) {
      printf("%d ", out[i]);
      if (i % 8 == 7) printf("\n");
    }
    
    
}



int enctest(void)
{
    int exit;
    
    
    exit = test_crypt_CTR();
    
}

int main(void) {
    pmf();
    //printf("start %d\n",Bit_81_to_num(PLUS_3_81(kiyak,zero)));
    enctest();
   
    return 0;
}