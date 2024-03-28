#include <grrlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <time.h>   // For nanosleep, clock

#include "BMfont4_png.h"

// RGBA Colors
#define GRRLIB_BLACK   0x000000FF
#define GRRLIB_MAROON  0x800000FF
#define GRRLIB_GREEN   0x008000FF
#define GRRLIB_OLIVE   0x808000FF
#define GRRLIB_NAVY    0x000080FF
#define GRRLIB_PURPLE  0x800080FF
#define GRRLIB_TEAL    0x008080FF
#define GRRLIB_GRAY    0x808080FF
#define GRRLIB_SILVER  0xC0C0C0FF
#define GRRLIB_RED     0xFF0000FF
#define GRRLIB_LIME    0x00FF00FF
#define GRRLIB_YELLOW  0xFFFF00FF
#define GRRLIB_BLUE    0x0000FFFF
#define GRRLIB_FUCHSIA 0xFF00FFFF
#define GRRLIB_AQUA    0x00FFFFFF
#define GRRLIB_WHITE   0xFFFFFFFF

void Initialise() {
  
	// Initialise the Graphics & Video subsystem
    GRRLIB_Init();
	
	rmode = &TVNtsc240Ds;
	
	VIDEO_Configure(rmode);

    // Initialise the Wiimotes
	WPAD_Init();
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
}

int main() {

	struct timespec ts;
    const int TARGET_FPS = 60;
    const double TARGET_SECONDS_PER_FRAME = 1.0 / TARGET_FPS;

	int pong1y = 72;
    int pong1speed = 4; //your speed
    int pong1score = 0; //your score
    
    int pong2y = 72;
	int pong2speed = 4;
    bool enemy_rising = false; //is enemy going up?
    int pong2score = 0; //enemy score
	
    int ballx = 312;
    int bally = 116;
    bool ballfaceleft = false;
    s16 deltx = 0; //delta x and y
    s16 delty = 0;
	
	//LOL SO RANDOM
    srand(0x54671243);
    u32 pongRand = rand();
 
	Initialise();
	
	ir_t ir1;
 
	GRRLIB_texImg *tex_BMfont4 = GRRLIB_LoadTexture(BMfont4_png);
    GRRLIB_InitTileSet(tex_BMfont4, 16, 16, 32);
	
	u16 buttonsDown = WPAD_ButtonsDown(0);
	u16 buttonsHeld = WPAD_ButtonsHeld(0);
	
	while(!buttonsDown) {
	
		GRRLIB_Printf(248, 112, tex_BMfont4, GRRLIB_WHITE, 1, "PONGCHAMP");
		GRRLIB_Printf(264, 144, tex_BMfont4, GRRLIB_WHITE, 1, "PRESS +");
		
		WPAD_ScanPads();
		buttonsDown = WPAD_ButtonsDown(0);
		buttonsHeld = WPAD_ButtonsHeld(0);
		GRRLIB_Render();  // Render the frame buffer to the TV
	}
	
	
	
	while(1) {
	
		// Start the timer
        clock_t start = clock();

        // Update the game state
	
		WPAD_SetVRes(0, 640, 480);
		WPAD_ScanPads();
		
		buttonsDown = WPAD_ButtonsDown(0);
		buttonsHeld = WPAD_ButtonsHeld(0);
		
		WPAD_IR(WPAD_CHAN_0, &ir1);
		
		int ir1ypos = (int)ir1.y >> 1;
		
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)  break;
		
		if ( buttonsHeld & WPAD_BUTTON_UP && pong1y > 24) {
			pong1y -= pong1speed;
		} else if ( buttonsHeld & WPAD_BUTTON_DOWN && pong1y < 152) {
			pong1y += pong1speed;
		}
		
		if (ir1ypos < 152 && ir1ypos > 24) {
			pong1y = ir1ypos;
			//GRRLIB_Printf(248, 112, tex_BMfont4, GRRLIB_WHITE, 1, "%d", ir1ypos);
		}
		
		if (buttonsHeld & WPAD_BUTTON_B) {
			pong1speed = 8;
		} else if (WPAD_ButtonsUp(0) & WPAD_BUTTON_B) {
			pong1speed = 4;
		}
		
		if(enemy_rising){
          
			if(pong2y > 24 && bally < (pong2y+24)){
				pong2y -= pong2speed;  //move enemy paddle according to ball
			}else{
				enemy_rising = false;
			}
        
		}else{
			if(pong2y < 152 && bally > (pong2y+24)){
				pong2y += pong2speed;  //move enemy paddle accoding to ball
			}else{
				enemy_rising = true;
			}
		}
		
		if ( buttonsDown & WPAD_BUTTON_PLUS && deltx == 0 && delty == 0) {
			
			while(deltx == 0 || delty == 0){
				//randomise deltas
				pongRand = rand();
				deltx = pongRand>>27;
				pongRand = rand();
				delty = pongRand>>27;
			}
			
			if(pong1score % 10 == 0){
              pong1score = 0;
              pong2score = 0;
            }
		}
		
		ballx += deltx;
		bally += delty;
		
		if(ballx < 96){
          
          if(abs((bally) - (pong1y+32)) <= 48){
            deltx *= -1;
            delty = (bally - (pong1y))>>3;
            ballfaceleft = false;
			
            
          }else{
			
            pong2score++;
            ballx = 312;
            bally = 116;
            //randomise deltas
			pongRand = rand();
			deltx = pongRand>>27;
			pongRand = rand();
			delty = pongRand>>27;
            if(deltx == 0){
              deltx = 4;  //zero x speed check
            }
            if(pong2score % 10 == 0){
              deltx = 0;
              delty = 0;
            }
          }
          
      }else if(ballx > 528){
          if(abs((bally) - (pong2y+32)) <= 48){
            deltx *= -1;
            delty = (bally - pong2y)>>3;
            ballfaceleft = true;
          }else{
            pong1score++;
            ballx = 312;
            bally = 116;
            //randomise deltas
			pongRand = rand();
			deltx = pongRand>>27;
			pongRand = rand();
			delty = pongRand>>27;
            if(deltx == 0){
              deltx = -4;  //zero x speed check
            }
            if(pong1score % 10 == 0){
              deltx = 0;
              delty = 0;
            }
          }
      }
	  
	  if(bally < 24){
          delty *= -1;  //ball bounces off of ceiling
        }else if(bally > 208){
          delty *= -1;  //ball bounces off of floor
      }
	  
	  GRRLIB_Rectangle(64, pong1y, 32, 64, GRRLIB_WHITE, 1);
	  
	  GRRLIB_Rectangle(544, pong2y, 32, 64, GRRLIB_WHITE, 1);
	  
	  GRRLIB_Rectangle(ballx, bally, 16, 8, GRRLIB_WHITE, 1);
	  
		// Use snprintf to determine the length of the string representation of the integer
		int nslength = snprintf(NULL, 0, "%d", pong1score);

		// Allocate memory for the string, considering the null-terminating character
		char *str_number = (char *)malloc(nslength + 1);

		// Use snprintf again to convert the integer to a string
		snprintf(str_number, nslength + 1, "%d", pong1score);

		// Now, str_number contains the string representation of the integer
		GRRLIB_Printf(64, 40, tex_BMfont4, GRRLIB_WHITE, 1, "%s", str_number);
		
		// Use snprintf to determine the length of the string representation of the integer
		nslength = snprintf(NULL, 0, "%d", pong2score);

		// Allocate memory for the string, considering the null-terminating character
		str_number = (char *)malloc(nslength + 1);

		// Use snprintf again to convert the integer to a string
		snprintf(str_number, nslength + 1, "%d", pong2score);

		// Now, str_number contains the string representation of the integer
		GRRLIB_Printf(544, 40, tex_BMfont4, GRRLIB_WHITE, 1, "%s", str_number);


		// Free the allocated memory
		free(str_number);

	  //GRRLIB_Printf(0, 0, tex_BMfont4, GRRLIB_WHITE, 1, "The string representation of the integer is: %s", );
	  
	  GRRLIB_Render();  // Render the frame buffer to the TV
	
		// Calculate the time elapsed since the last frame
        clock_t end = clock();
        double elapsedSeconds = (double)(end - start) / CLOCKS_PER_SEC;
        // Sleep for the remaining time to maintain a 30fps frame rate
        if (elapsedSeconds < TARGET_SECONDS_PER_FRAME) {
            double sleepTime = TARGET_SECONDS_PER_FRAME - elapsedSeconds;
            ts.tv_sec = sleepTime; // Extract the integer part
            ts.tv_nsec = (sleepTime - ts.tv_sec) * 1e9; // Convert the remainder to nanoseconds
            
            nanosleep(&ts, NULL); // Sleep to maintain frame rate
        }
	
	}
 
	GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);  // Use exit() to exit a program, do not use 'return' from main()
}