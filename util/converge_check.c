/*program to find the words 'cycle converged' from the end of a file (currently the final 255 bytes), for use with the SPRKKR program - Chris Davis 17:39 24/01/2016*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  
  if(argc != 2)
  {
      return 2;
  }
  
  FILE *fp = fopen(argv[1], "r");
  
  if(fp == 0)
  {
      return 3;
  }
  
  char str[256];
  
    /*finding the correct location in the file*/
    fseek(fp, -255, SEEK_END);
  
    /*reading the file section to an array*/
    fread(str, 1, 255, fp);
    
    /*adding an end clause*/
    str[255] = '\0';
    
    fclose(fp);
    
    /*debugging print statements*/
    
    /*if(strstr(str, "cycle converged") == 0)
    {
      printf("cycle did not converge\n");
    }
    else
    {
      printf("cycle converged\n");
    }*/
    
    /*returns success if the cycle converges, failure otherwise*/
    
  return ! strstr(str, "cycle converged");
  
}