#include <wiringPi.h>
int main (void)
{
  wiringPiSetup () ;
  pinMode (5, OUTPUT) ;
  for (;;)
  {
    digitalWrite (5, HIGH) ;
    digitalWrite (5,  LOW) ; 
    delay (500) ;
  }
  return 0 ;
}
