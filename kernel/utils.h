#pragma once

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
	{
		len++;
	}
	return len;
}

size_t strlen(char* str) 
{
	size_t len = 0;
	while (str[len])
	{
		len++;
	}
	return len;
}

int digits_of_int(int num) {
   int count = 0;
   while (num != 0) {   
      count++;  
      num /= 10;
   } 
   return count;
}