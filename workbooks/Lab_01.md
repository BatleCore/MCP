## LED CL-Resistor

Vs = 5 # 5V from Arduino
Ic = 0.02 # 20mA
Vf = 2 # LED forward voltage

CLResistor = ( 5 - Vf ) / 0.02 = 150 R 

## Written Pin Specifiers

For ports with 8 pins, pins can be refferenced in group with an 8-bit int.
pin numbers are 0 -> 7
eg:
PORTA = 0b00000000 # each pin is assigned 0 value
PORTA = 0 # 0 in base 10 == 00000000 in base 2, therefore functions the same as above



" | "   : Logic operator for "OR"
" & "   : Logic operator for "AND"

### eg:

  A = 0101
  B = 0011

  A2 = ~A # NOT A
  A2 == 1010

  B2 = ~B NOT B
  B2 == 1100

  C = A | B # A OR B
  C == 0111

  D = A & B # A AND B
  D == 0001

  C2 = A |~ B # A OR NOT B
  C2 == 1101

  D2 = A &~ B # A AND NOT B
  D2 == 0100

### Removing Logic operators

  The OR and NAND operations ensure only the specified bit is edited.
  Without these operations, all pins are edited. All other pins are set to the opposing value of the specified bit.

  Assume PORTA == 0b00000000 # all pins low

  #### OR

    PORTA |= (1<<PA3);

    (1<<PA3):
      the int "1" == 00000001
      PA3 is value "3"
      therefore: (1<<PA3) == 00000100 # third bit HIGH

    PORTA = 0b00000000
    (1<<PA3) = 0b00000100

    PORTA |= (1<<PA3)
    PORTA = PORTA OR (1<<PA3)
    PORTA = 0b00000000 OR 0b00000100
    PORTA = 0b00000100

    without " | " operator:

    PORTA = (1<<PA3) = 0b00000100 # any other pins that may have been HIGH are now set LOW

  #### AND (NAND)
    PORTA &=~ (1<<PA3);

    (1<<PA3):
      the int "1" == 00000001
      PA3 is value "3"
      therefore: (1<<PA3) == 00000100 # third bit HIGH
      " ~ " inverts value
      therefore: ~(1<<PA3) == 11111011 # third bit LOW

    PORTA = 0b00000000
    (1<<PA3) = 0b11111011

    PORTA &=~ (1<<PA3)
    PORTA = PORTA NAND (1<<PA3)
    PORTA = 0b00000000 AND 0b11111011
    PORTA = 0b00000000

    without " & " operator:

    PORTA =~ (1<<PA3) = 0b11111011 # any other pins that may have been LOW are now set HIGH




