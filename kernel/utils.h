#pragma once

int digits_of_int(int num) {
   int count = 0;
   while (num != 0) {   
      count++;  
      num /= 10;
   } 
   return count;
}
