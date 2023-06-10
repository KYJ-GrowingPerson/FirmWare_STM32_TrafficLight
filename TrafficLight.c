#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_
#endif
#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_
#include "def.h"
#include "stm32f7xx_hal.h"
static void SystemClock_Config(void);
#endif
void hwInit(void);
void MyApp();

static void SystemClock_Config(void)
{
   RCC_ClkInitTypeDef RCC_ClkInitStruct;
   RCC_OscInitTypeDef RCC_OscInitStruct;

   /* Enable HSE Oscillator and activate PLL with HSE as source */
   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
   RCC_OscInitStruct.HSEState = RCC_HSE_ON;
   RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
   RCC_OscInitStruct.PLL.PLLM = 25;
   RCC_OscInitStruct.PLL.PLLN = 432;
   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
   RCC_OscInitStruct.PLL.PLLQ = 9;
   if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
   {
      //Error_Handler();
   }

   /* activate the OverDrive to reach the 216 Mhz Frequency */
   if (HAL_PWREx_EnableOverDrive() != HAL_OK)
   {
      //Error_Handler();
   }

   /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
      clocks dividers */
   RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
   if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
   {
      //Error_Handler();
   }
}

int main(void)
{
   hwInit();
   MyApp();
}

void hwInit(void)
{
   SCB_EnableICache();
   SCB_EnableDCache();
   HAL_Init();
   SystemClock_Config();
}

void SevenSegment(unsigned int No) {
   unsigned int Number[8] = {
         0b01100001, // 1
         0b11011011, // 2
         0b11110011, // 3
         0b01100111, // 4
         0b10110111, // 5
         0b00111111, // 6
         0b11100101, // 7
         0b00000000  // 전부 끄기
   };

   int i;
   for (i = 0; i < 8; i++) {
      if (Number[No - 1] >> i & 1) {
         //DS PIN
         *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 9);
         *(volatile unsigned int*)(0x40020418U) |= (0x01 << 25);
      }
      else {
         //DS PIN
         *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 25);
         *(volatile unsigned int*)(0x40020418U) |= (0x01 << 9);
      }
      // OneClockON
      *(volatile unsigned int*)(0x40020418U) |= 0x01 << 15;
      *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 15);
      *(volatile unsigned int*)(0x40020418U) |= 0x01 << 31;
      *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 31);
   }
   // shift 완료 ... LATCH ON
   *(volatile unsigned int*)(0x40020418U) |= 0x01 << 14;
   *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 14);
   *(volatile unsigned int*)(0x40020418U) |= 0x01 << 30;
   *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 30);
}

void MyDelay(unsigned int n, unsigned char* count, unsigned int pedestrian)
{
   volatile unsigned int delay;
   int flag = 0;
   int ExternalButtonflag = 0;
   int j = 0;
   if (pedestrian == 0) {
      SevenSegment(8);
   }
   else if (pedestrian == 1) {
      *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 8);
      *(volatile unsigned int*)(0x40020418U) |= (0x01 << 24);
   }
   else if (pedestrian == 2) {
      *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 7);
      *(volatile unsigned int*)(0x40020418U) |= (0x01 << 23);
   }
   for (int seg = n; seg > 0; seg--) {
      if (pedestrian > 0) {
         SevenSegment(seg);
      }
      for (j = 0; j < 20; j++) {
         if (pedestrian == 1 && seg <= 2) {
            if (j % 2 == 0) {
               *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 24);
               *(volatile unsigned int*)(0x40020418U) |= (0x01 << 8);
            }
            else {
               *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 8);
               *(volatile unsigned int*)(0x40020418U) |= (0x01 << 24);
            }
         }
         if (pedestrian == 2 && seg <= 2) {
            if (j % 2 == 0) {
               *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 23);
               *(volatile unsigned int*)(0x40020418U) |= (0x01 << 7);
            }
            else {
               *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 7);
               *(volatile unsigned int*)(0x40020418U) |= (0x01 << 23);
            }
         }
         for (delay = 0; delay <= 490000; delay++) {
            if (*(volatile unsigned int*)(0x40020810U) >> 12 & 0x01) {
               flag = 1;
            }
            else if (flag == 1) {
               *count = 1;
               *(volatile unsigned int*)(0x40021818U) &= ~(0x01 << 28);   // 내부 LED 전부 끄기
               *(volatile unsigned int*)(0x40021018U) &= ~(0x01 << 21);
               *(volatile unsigned int*)(0x40021018U) &= ~(0x01 << 21);
               *(volatile unsigned int*)(0x40021818U) &= ~(0x01 << 26);

               *(volatile unsigned int*)(0x40021818U) |= 0x01 << 12;   // 내부 LED 전부 끄기
               *(volatile unsigned int*)(0x40021018U) |= 0x01 << 5;
               *(volatile unsigned int*)(0x40021018U) |= 0x01 << 4;
               *(volatile unsigned int*)(0x40021818U) |= 0x01 << 10;

               flag = 0;
            }
            if (!(*(volatile unsigned int*)(0x40021810U) >> 6 & 0x01)) {   // 스위치를 누르면 트리거
               ExternalButtonflag = 1;
            }
            else if (ExternalButtonflag == 1) {
               *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 23);
               *(volatile unsigned int*)(0x40020418U) |= (0x01 << 7);
               *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 24);
               *(volatile unsigned int*)(0x40020418U) |= (0x01 << 8);
               ExternalButtonflag = 0;
               return;
            }
         }
      }
   }
   *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 23);
   *(volatile unsigned int*)(0x40020418U) |= (0x01 << 7);
   *(volatile unsigned int*)(0x40020418U) &= ~(0x01 << 24);
   *(volatile unsigned int*)(0x40020418U) |= (0x01 << 8);
}

void Trafficlight(unsigned char data_out)
{
   int i;
   for (i = 0; i < 8; i++)
   {
      if ((data_out >> i) & (0x01)) {
         *(volatile unsigned int*)(0x40021818U) &= ~(0x01 << 7);
         *(volatile unsigned int*)(0x40021818U) |= 0x01 << 23;   // data pin 에 reset value 1 부여
      }
      else {
         *(volatile unsigned int*)(0x40021818U) &= ~(0x01 << 23);
         *(volatile unsigned int*)(0x40021818U) |= 0x01 << 7; // data pin 에 set value 1부여
      }

      *(volatile unsigned int*)(0x40021418U) |= 0x01 << 10;
      *(volatile unsigned int*)(0x40021418U) &= ~(0x01 << 10);
      *(volatile unsigned int*)(0x40021418U) |= 0x01 << 26;
      *(volatile unsigned int*)(0x40021418U) &= ~(0x01 << 26);

   }
   *(volatile unsigned int*)(0x40021418U) |= 0x01 << 9;
   *(volatile unsigned int*)(0x40021418U) &= ~(0x01 << 9);
   *(volatile unsigned int*)(0x40021418U) |= 0x01 << 25;
   *(volatile unsigned int*)(0x40021418U) &= ~(0x01 << 25);
   // Data finally submitted 8번 shift 완료했을 때 latch를 올려줌 으로써 출력
}

void InnerLEDCOUNT(unsigned char* count) {
   *(volatile unsigned int*)(0x40021818U) |= (*count >> 0 & 0x01) << 28;
   *(volatile unsigned int*)(0x40021818U) |= !(*count >> 0 & 0x01) << 12;

   *(volatile unsigned int*)(0x40021018U) |= (*count >> 1 & 0x01) << 21;  // 내부 LED 2
   *(volatile unsigned int*)(0x40021018U) |= !(*count >> 1 & 0x01) << 5;  // 내부 LED 2

   *(volatile unsigned int*)(0x40021018U) |= (*count >> 2 & 0x01) << 20;   // 내부 LED 3
   *(volatile unsigned int*)(0x40021018U) |= !(*count >> 2 & 0x01) << 4;  // 내부 LED 2

   *(volatile unsigned int*)(0x40021818U) |= (*count >> 3 & 0x01) << 26;   // 내부 LED 4
   *(volatile unsigned int*)(0x40021818U) |= !(*count >> 3 & 0x01) << 10;   // 내부 LED 4
   *count += 1;
   if (*count == 16) *count = 0;
}

void init() {
   *(volatile unsigned int*)(0x40023830U) |= 0x00000074U; // PF, PG, PC, PE 포트 clock시작
   *(volatile unsigned int*)(0x40023830U) |= 0x01 << 1;    // PB 포트 clock 시작


   *(volatile unsigned int*)(0x40021800U) |= 0x00004000U; // PG 7번 세팅
   *(volatile unsigned int*)(0x40021808U) |= 0x00008000U;
   *(volatile unsigned int*)(0x4002180CU) |= 0x00004000U;

   // PF 10 : clk
   // PF 9 : latch
   *(volatile unsigned int*)(0x40021400U) |= 0x00140000U;  //PF 10, 9세팅 output
   *(volatile unsigned int*)(0x40021408U) |= 0x003C0000U;  //PF clock speed
   *(volatile unsigned int*)(0x4002140CU) |= 0x00140000U;

   *(volatile unsigned int*)(0x40021418U) |= 0x06000000U; //초기 PF 10, 9 Reset

   // 1번, 4번LED내부 led set    // Board LED PG 12, 10

   *(volatile unsigned int*)(0x40021800U) |= 0x01100000U;
   *(volatile unsigned int*)(0x40021808U) |= 0x03300000U;
   *(volatile unsigned int*)(0x4002180CU) |= 0x01100000U;

   // 2번, 3번 LED 내부 led set    // Board LED PE 4,5
   *(volatile unsigned int*)(0x40021000U) |= 0x00000500U;
   *(volatile unsigned int*)(0x40021008U) |= 0x00000F00U;
   *(volatile unsigned int*)(0x4002100CU) |= 0x00000500U;

   *(volatile unsigned int*)(0x40021818U) |= 0x00001400U;
   *(volatile unsigned int*)(0x40021018U) |= 0x00000030U;

   // PC 12 내부 버튼 1
   *(volatile unsigned int*)(0x40020800U) &= ~0x03000000U;
   *(volatile unsigned int*)(0x40020808U) &= ~0x03000000U;  // Switch clock Speed low
   *(volatile unsigned int*)(0x4002080CU) &= ~0x03000000U;

   // PG6 외부 스위치
   *(volatile unsigned int*)(0x40021800U) &= ~0x00003000U;  // PG6 Input Mode
   *(volatile unsigned int*)(0x40021808U) |= 0x00003000U;   // very high speed
   *(volatile unsigned int*)(0x4002180CU) |= 0x00001000U;   // PULL DOWN

   // Segment를 위한 shift Regiter
   // PB 9(ARD_D10) - DS // PB 14(ARD_D12) - Latch // PB 15(ARD_D11) - Clk

   *(volatile unsigned int*)(0x40020400U) |= 0x01 << 18; // PB9   OUTPUT
   *(volatile unsigned int*)(0x40020400U) |= 0x01 << 28; // PB14 OUTPUT
   *(volatile unsigned int*)(0x40020400U) |= 0x01 << 30; // PB15 OUTPUT

   *(volatile unsigned int*)(0x40020408U) |= 0x03 << 18; // speed veryhigh
   *(volatile unsigned int*)(0x40020408U) |= 0x03 << 28;
   *(volatile unsigned int*)(0x40020408U) |= 0x03 << 30;

   *(volatile unsigned int*)(0x4002040CU) |= 0x01 << 18; // Pull-up
   *(volatile unsigned int*)(0x4002040CU) |= 0x01 << 28;
   *(volatile unsigned int*)(0x4002040CU) |= 0x01 << 30;

   *(volatile unsigned int*)(0x40020418U) |= 0x01 << 25;
   *(volatile unsigned int*)(0x40020418U) |= 0x01 << 30;
   *(volatile unsigned int*)(0x40020418U) |= 0x01 << 31;


   // PB7 ARD_14(GREEN) 보행자 신호등
   *(volatile unsigned int*)(0x40020400U) |= 0x01 << 14; // PB7 OUTPUT
   *(volatile unsigned int*)(0x40020408U) |= 0x03 << 14; // PB7 VERY HIGH SPEED
   *(volatile unsigned int*)(0x4002040CU) |= 0x01 << 14; // PB7 PULL-up
   *(volatile unsigned int*)(0x40020418U) |= 0x01 << 7;

   // PB8 ARD_15(좌측 보행자 GREEN)
   *(volatile unsigned int*)(0x40020400U) |= 0x01 << 16; // PB8 OUTPUT
   *(volatile unsigned int*)(0x40020408U) |= 0x03 << 16; // PB8 VERY HIGH SPEED
   *(volatile unsigned int*)(0x4002040CU) |= 0x01 << 16; // PB8 PULL-up
   *(volatile unsigned int*)(0x40020418U) |= 0x01 << 8;
}

void MyApp()
{
   unsigned char count = 0;
   init();
   while (1) {
      Trafficlight(0b00011000);
      InnerLEDCOUNT(&count);
      MyDelay(7, &count, 1);

      Trafficlight(0b00101000);
      InnerLEDCOUNT(&count);
      MyDelay(2, &count, 0);

      Trafficlight(0b11001000);
      InnerLEDCOUNT(&count);
      MyDelay(7, &count, 0);

      Trafficlight(0b10101000);
      InnerLEDCOUNT(&count);
      MyDelay(2, &count, 0);

      Trafficlight(0b10000001);
      InnerLEDCOUNT(&count);
      MyDelay(7, &count, 2);

      Trafficlight(0b10000010);
      InnerLEDCOUNT(&count);
      MyDelay(2, &count, 0);

      Trafficlight(0b10001100);
      InnerLEDCOUNT(&count);
      MyDelay(7, &count, 0);

      Trafficlight(0b10001010);
      InnerLEDCOUNT(&count);
      MyDelay(2, &count, 0);
   }
}
