
#ifndef LCD_H_
#define LCD_H_

//----------------------------------------
void LCD_ini(void);
void setpos(unsigned char x, unsigned y);
void str_lcd (char str1[]);
void clearlcd(void);
void sendcharlcd(unsigned char c);
//----------------------------------------
#define e1   I2C_SendByteByADDR(portlcd|=0x04,0b01001110)
#define e0   I2C_SendByteByADDR(portlcd&=~0x04,0b01001110)
#define rs1    I2C_SendByteByADDR(portlcd|=0x01,0b01001110)
#define rs0   I2C_SendByteByADDR(portlcd&=~0x01,0b01001110)
#define setled()    I2C_SendByteByADDR(portlcd|=0x08,0b01001110)
#define setwrite()   I2C_SendByteByADDR(portlcd&=~0x02,0b01001110)
//----------------------------------------

#endif /* LCD_H_ */