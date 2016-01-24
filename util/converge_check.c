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
  
    fseek(fp, -255, SEEK_END);
  
    fread(str, 1, 255, fp);
    
    str[255] = '\0';
    
    fclose(fp);
    
    /*if(strstr(str, "cycle converged") == 0)
    {
      printf("cycle did not converge\n");
    }
    else
    {
      printf("cycle converged\n");
    }*/
    
  return ! strstr(str, "cycle converged");
  
}