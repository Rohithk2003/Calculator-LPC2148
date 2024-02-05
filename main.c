#include <lpc21xx.h>
#define LCD (0xff << 8)
#define RS (1 << 16)
#define RW (1 << 17)
#define EN (1 << 18)

#define col1 (1 << 16)
#define col2 (1 << 17)
#define col3 (1 << 18)
#define col4 (1 << 19)
#define row1 (1 << 20)
#define row2 (1 << 21)
#define row3 (1 << 22)
#define row4 (1 << 23)

void delay(unsigned int time);
void lcd_ini(void);
void lcd_print(char *str);
void lcd_cmd(unsigned char command);
void lcd_dat(unsigned int data);
unsigned char keypad(void);
void keypad_delay(void);

void addDigits(char a, char b, char *result)
{
    int intA = a - '0';
    int intB = b - '0';

    int sum = intA + intB;

    if (sum > 9)
    {
        result[0] = '1';
        result[1] = (sum % 10) + '0';
        result[2] = '\0';
    }
    else
    {
        result[0] = sum + '0';
        result[1] = '\0';
    }
}

void subtractDigits(char a, char b, char *result)
{
    int intA = a - '0';
    int intB = b - '0';

    int diff = intA - intB;

    if (diff < 0)
    {
        result[0] = '-';
        result[1] = (-diff) + '0';
        result[2] = '\0';
    }
    else
    {
        result[0] = diff + '0';
        result[1] = '\0';
    }
}

void multiplyDigits(char a, char b, char *result)
{
    int intA = a - '0';
    int intB = b - '0';

    int product = intA * intB;

    result[0] = (product / 10) + '0';
    result[1] = (product % 10) + '0';
    result[2] = '\0';
}

void divideDigits(char a, char b, char *result)
{
    int intA = a - '0';
    int intB = b - '0';

    if (intB == 0)
    {
        result[0] = 'E';
        result[1] = '\0';
    }
    else
    {
        int quotient = intA / intB;
        int remainder = intA % intB;
        if (remainder != 0)
        {
            result[0] = quotient + '0';
            result[1] = remainder + '0';
            result[2] = '\0';
        }
        {
            result[0] = quotient + '0';
            result[1] = '\0';
        }
    }
}

int main(void)
{
    unsigned char key_value;
    int first_operand = -1, first_operator = -1, equal_ = -1, operation_completed = 0, second_operand = 0;
    char result[3]; // Buffer to store the result as a string

    PINSEL0 = 0x00000000;
    IODIR0 = 0xffffffff;
    PINSEL1 = 0x00000000;
    IODIR1 = 0X00f00000;
    lcd_ini();
    lcd_print("Press a key");
    lcd_cmd(0xc0);
    while (1)
    {

        key_value = keypad();
        if (key_value && operation_completed != 0)
        {
            operation_completed = 0;
            lcd_ini();
        }
        else if (key_value == 'r')
        {
            first_operand = -1, first_operator = -1, equal_ = -1, operation_completed = 0, second_operand = 0;
            lcd_ini();
        }
        else
        {
            lcd_dat(key_value);
        }

        keypad_delay();
        if (key_value != 'r')
        {
            if (first_operand == -1)
            {
                first_operand = key_value;
            }
            else if (first_operator == -1)
            {
                first_operator = key_value;
            }
            else if (second_operand == 0)
            {
                second_operand = key_value;
            }
            else if (first_operand != -1 && first_operator != -1 && second_operand != -1)
            {
                equal_ = key_value;
                if ((char)first_operator == '+')
                {
                    addDigits(first_operand, second_operand, result);
                    first_operand = -1;
                    first_operator = -1;
                    second_operand = 0;
                    equal_ = -1;
                    operation_completed = 1;
                    lcd_print(result);
                    keypad_delay();
                }
                else if ((char)first_operator == '-')
                {
                    subtractDigits(first_operand, second_operand, result);
                    first_operand = -1;
                    first_operator = -1;
                    equal_ = -1;
                    second_operand = 0;
                    operation_completed = 1;
                    lcd_print(result);
                    keypad_delay();
                }
                else if ((char)first_operator == '*')
                {
                    multiplyDigits(first_operand, second_operand, result);
                    first_operand = -1;
                    first_operator = -1;
                    equal_ = -1;
                    second_operand = 0;
                    operation_completed = 1;
                    lcd_print(result);
                    keypad_delay();
                }
                else if ((char)first_operator == '/')
                {
                    divideDigits(first_operand, second_operand, result);
                    first_operand = -1;
                    first_operator = -1;
                    equal_ = -1;
                    second_operand = 0;
                    operation_completed = 1;
                    lcd_print(result);
                    keypad_delay();
                }
            }
        }
    }
    return 0;
}
void keypad_delay(void)
{
    unsigned int t1, t2;
    for (t1 = 0; t1 < 500; t1++)
        for (t2 = 0; t2 < 1275; t2++)
            ;
}
unsigned char keypad(void)
{
    unsigned char key;
    IOCLR1 |= (row1 | row2 | row3 | row4 | col1 | col2 | col3 | col4);
    while (1)
    {
        IOCLR1 |= row1;
        IOSET1 = (row2 | row3 | row4);

        if ((IOPIN1 & col1) == 0)
        {
            key = '7';
            keypad_delay();
            return key;
        }
        else if ((IOPIN1 & col2) == 0)
        {
            key = '8';
            keypad_delay();
            return key;
        }
        else if ((IOPIN1 & col3) == 0)
        {
            key = '9';
            keypad_delay();
            return key;
        }
        else if ((IOPIN1 & col4) == 0)
        {
            key = '/';
            keypad_delay();
            return key;
        }
        IOCLR1 |= row2;
        IOSET1 = (row1 | row3 | row4);
        if ((IOPIN1 & col1) == 0)
        {
            key = '4';
            keypad_delay();
            return key;
        }
        else if ((IOPIN1 & col2) == 0)
        {
            key = '5';
            keypad_delay();
            return key;
        }
        else if ((IOPIN1 & col3) == 0)
        {
            key = '6';
            keypad_delay();
            return key;
        }
        else if ((IOPIN1 & col4) == 0)
        {
            key = '*';
            keypad_delay();
            return key;
        }
        IOCLR1 |= row3;
        IOSET1 = (row1 | row2 | row4);
        if ((IOPIN1 & col1) == 0)
        {
            key = '1';
            keypad_delay();
            return key;
        }
        else if ((IOPIN1 & col2) == 0)
        {
            key = '2';
            keypad_delay();
            return key;
        }
        else if ((IOPIN1 & col3) == 0)
        {
            key = '3';
            keypad_delay();
            return key;
        }
        else if ((IOPIN1 & col4) == 0)
        {
            key = '-';
            keypad_delay();
            return key;
        }
        IOCLR1 |= row4;
        IOSET1 = (row1 | row2 | row3);
        if ((IOPIN1 & col1) == 0)
        {
            lcd_cmd(0x01);
            keypad_delay();
            return 'r';
        }
        else if ((IOPIN1 & col2) == 0)
        {
            key = '0';
            keypad_delay();
            return key;
        }
        else if ((IOPIN1 & col3) == 0)
        {
            key = '=';
            keypad_delay();
            return key;
        }
        else if ((IOPIN1 & col4) == 0)
        {
            key = '+';
            keypad_delay();
            return key;
        }
    }
}

void lcd_cmd(unsigned char command)
{
    IO0CLR |= (RS | RW | EN | LCD);
    IO0SET |= (command << 8);
    IO0CLR |= RS;
    IO0CLR |= RW;
    IO0SET |= EN;
    delay(2);
    IO0CLR |= EN;
    delay(3);
}
void lcd_dat(unsigned int data)
{
    IO0CLR |= (RS | RW | EN | LCD);
    IO0SET |= (data << 8);
    IO0SET |= RS;
    IO0CLR |= RW;
    IO0SET |= EN;
    delay(2);
    IO0CLR |= EN;
    delay(3);
}
void lcd_print(char *str)
{
    while (*str != '\0')
    {
        lcd_dat(*str);
        str++;
    }
}

void delay(unsigned int time)
{
    unsigned int t1, t2;
    for (t1 = 0; t1 < time; t1++)
        for (t2 = 0; t2 < 1275; t2++)
            ;
}
void lcd_ini(void)
{
    delay(5);
    lcd_cmd(0X38);
    lcd_cmd(0X0f);
    lcd_cmd(0X06);
    delay(5);
    lcd_cmd(0X80);
}
