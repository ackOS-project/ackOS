#include "interrupts.h"

const char* readStr()
{
    char buff;
    const char* buffstr;
    int i = 0;
    int reading = 1;
    while(reading)
    {
        if(inportb(0x64) & 0x1)                 
        {
            switch(inportb(0x60))
            { 
      /*case 1:
                printch("(char)27);           Escape button
                buffstr = (char)27;
                i++;
                break;*/
        case 2:
                printch("1");
                buffstr = "1";
                i++;
                break;
        case 3:
                printch("2");
                buffstr = "2";
                i++;
                break;
        case 4:
                printch("3");
                buffstr = "3";
                i++;
                break;
        case 5:
                printch("4");
                buffstr = "4";
                i++;
                break;
        case 6:
                printch("5");
                buffstr = "5";
                i++;
                break;
        case 7:
                printch("6");
                buffstr = "6";
                i++;
                break;
        case 8:
                printch("7");
                buffstr = "7";
                i++;
                break;
        case 9:
                printch("8");
                buffstr = "8";
                i++;
                break;
        case 10:
                printch("9");
                buffstr = "9";
                i++;
                break;
        case 11:
                printch("0");
                buffstr = "0";
                i++;
                break;
        case 12:
                printch("-");
                buffstr = "-";
                i++;
                break;
        case 13:
                printch("=");
                buffstr = "=";
                i++;
                break;
        case 14:
                printch("\b");
                i--;
                buffstr = 0;
                break;
       /* case 15:
                printch("\t");          Tab button
                buffstr = "\t";
                i++;
                break;*/
        case 16:
                printch("q");
                buffstr = "q";
                i++;
                break;
        case 17:
                printch("w");
                buffstr = "w";
                i++;
                break;
        case 18:
                printch("e");
                buffstr = "e";
                i++;
                break;
        case 19:
                printch("r");
                buffstr = "r";
                i++;
                break;
        case 20:
                printch("t");
                buffstr = "t";
                i++;
                break;
        case 21:
                printch("y");
                buffstr = "y";
                i++;
                break;
        case 22:
                printch("u");
                buffstr = "u";
                i++;
                break;
        case 23:
                printch("i");
                buffstr = "i";
                i++;
                break;
        case 24:
                printch("o");
                buffstr = "o";
                i++;
                break;
        case 25:
                printch("p");
                buffstr = "p";
                i++;
                break;
        case 26:
                printch("[");
                buffstr = "[";
                i++;
                break;
        case 27:
                printch("]");
                buffstr = "]";
                i++;
                break;
        case 28:
               // printch("\n");
               // buffstr = "\n";
                  i++;
               reading = 0;
                break;
      /*  case 29:
                printch("q");           Left Control
                buffstr = "q";
                i++;
                break;*/
        case 30:
                printch("a");
                buffstr = "a";
                i++;
                break;
        case 31:
                printch("s");
                buffstr = "s";
                i++;
                break;
        case 32:
                printch("d");
                buffstr = "d";
                i++;
                break;
        case 33:
                printch("f");
                buffstr = "f";
                i++;
                break;
        case 34:
                printch("g");
                buffstr = "g";
                i++;
                break;
        case 35:
                printch("h");
                buffstr = "h";
                i++;
                break;
        case 36:
                printch("j");
                buffstr = "j";
                i++;
                break;
        case 37:
                printch("k");
                buffstr = "k";
                i++;
                break;
        case 38:
                printch("l");
                buffstr = "l";
                i++;
                break;
        case 39:
                printch(";");
                buffstr = ";";
                i++;
                break;
        case 40:
                printch((char)44);               //   Single quote (")
                buffstr = (char)44;
                i++;
                break;
        case 41:
                printch((char)44);               // Back tick (`)
                buffstr = (char)44;
                i++;
                break;
     /* case 42:                                 Left shift 
                printch("q");
                buffstr = "q";
                i++;
                break;
        case 43:                                 \ (< for somekeyboards)   
                printch((char)92);
                buffstr = "q";
                i++;
                break;*/
        case 44:
                printch("z");
                buffstr = "z";
                i++;
                break;
        case 45:
                printch("x");
                buffstr = "x";
                i++;
                break;
        case 46:
                printch("c");
                buffstr = "c";
                i++;
                break;
        case 47:
                printch("v");
                buffstr = "v";
                i++;
                break;                
        case 48:
                printch("b");
                buffstr = "b";
                i++;
                break;               
        case 49:
                printch("n");
                buffstr = "n";
                i++;
                break;                
        case 50:
                printch("m");
                buffstr = "m";
                i++;
                break;               
        case 51:
                printch(",");
                buffstr = ",";
                i++;
                break;                
        case 52:
                printch(".");
                buffstr = ".";
                i++;
                break;            
        case 53:
                printch("/");
                buffstr = "/";
                i++;
                break;            
        case 54:
                printch(".");
                buffstr = ".";
                i++;
                break;            
        case 55:
                printch("/");
                buffstr = "/";
                i++;
                break;            
      /*case 56:
                printch(" ");           Right shift
                buffstr = " ";
                i++;
                break;*/           
        case 57:
                printch(" ");
                buffstr = " ";
                i++;
                break;
            }
        }
    }
    //buffstr = 0;                
    return buff;
}
