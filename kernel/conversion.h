/*****************************************
* conversion.h
* Converts numbers and strings
*
* Written by Cael Rasmussen 
* Copyright (c) 2020 Cael Rasmussen
*
* TO DO LIST
* Work on this
******************************************/

#include <stdint.h>
#include <stdbool.h>

const char* int_to_char(int a);
int number_of_digits(int num);
int get_digit(int value, int position);

namespace convert{
const char int_to_char(int a)
{
    /* Convert an integer to a string */
    const char* result[number_of_digits(a)];
    const char* num_count;
    for(int column;column < number_of_digits(a);column++){
        switch(get_digit(a,column)){
            case 0:
                num_count = "0";
                break;
            case 1:
                num_count = "1";
                break;
            case 2:
                num_count = "2";
                break;
            case 3:
                num_count = "3";
                break;
            case 4:
                num_count = "4";
                break;
            case 5:
                num_count = "5";
                break;
            case 6:
                num_count = "6";
                break;
            case 7:
                num_count = "7";
                break;
            case 8:
                num_count = "8";
                break;
            case 9:
                num_count = "9";
                break;
        }
        result[column] = num_count;
    }
    return (const char)result;

}

int number_of_digits(int num) {
   int count = 0;
   while (num != 0) {   
      count++;  
      num /= 10;
   } 
   return count;
}
int get_digit(int value, int position)
{
    int posFromRight = 1;
    {
        int v = value;
        while (v /= 10)
            ++posFromRight;
    }
    posFromRight -= position - 1;
    while (--posFromRight)
        value /= 10;
    value %= 10;
    return value > 0 ? value : -value;
}
};