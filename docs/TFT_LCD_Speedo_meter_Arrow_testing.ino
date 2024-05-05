#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP = 6, XM = A2, YP = A1, YM = 7; //ID=0x9341

const int TS_LEFT = 910, TS_RT = 169, TS_TOP = 900, TS_BOT = 180;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button on_btn;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        Serial.println("P.X:"+String(p.x));
        Serial.println("P.Y:"+String(p.y));
        pixel_x = map(p.x, TS_RT, TS_LEFT,  0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
        Serial.println("X:"+String(pixel_x));
        Serial.println("Y:"+String(pixel_y));
        Serial.println("");
    }
    return pressed;
}

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
float i=0;
int randomvalue=-90;
int returnx=0;
float angle,x,y;
bool down;
unsigned long old =0;
unsigned long current =0;
long interval=0;
void setup(void)
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    Serial.println("Calibrate for your Touch Panel");
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(1);            //PORTRAIT
    tft.setTextSize(3);
    tft.fillScreen(BLACK);
    on_btn.initButton(&tft,  40, 70, 80, 50, WHITE, MAGENTA, WHITE, "RUN", 2);
    
    on_btn.drawButton(false);

    
    tft.setTextColor(WHITE, BLACK);
    tft.setCursor(15, 240);
    tft.drawCircle(160,240,155,WHITE);
     for (i=-90; i<=-30; i=i+0.1)
    {
    float angle = (i  / 57.2958)  - 1.57; 
    float x= 160 + cos(angle) * 150;
    float y = 239+sin(angle) * 150;
    tft.drawLine(160, 239,x,y, GREEN);
    
    }
      for (i=-30; i<=30; i=i+0.1)
    {
    float angle = (i  / 57.2958)  - 1.57; 
    float x= 160 + cos(angle) * 150;
    float y = 239+sin(angle) * 150;
    tft.drawLine(160, 239,x,y, YELLOW);
    
    }
        for (i=30; i<=90; i=i+0.1)
    {
    float angle = (i  / 57.2958)  - 1.57; 
    float x= 160 + cos(angle) * 150;
    float y = 239+sin(angle) * 150;
    tft.drawLine(160, 239,x,y, RED);
    
    }
   // tft.fillCircle(160,240,150,YELLOW);
    tft.fillCircle(160,240,130,BLACK);
    tft.drawCircle(160,240,127,WHITE);
    
    i=randomvalue;
    for (i=-90; i<=90; i=i+30)
    {
    float angle = (i  / 57.2958)  - 1.57; 
    float x= 160 + cos(angle) * 126;
    float y = 239+sin(angle) * 126;
    tft.drawLine(160, 239,x,y, WHITE);
    
    }
    tft.fillCircle(160,240,115,BLACK);
     tft.setTextSize(2);
     tft.setTextColor(GREEN, BLACK);
     tft.setCursor(50,225);
     tft.print("0");
      tft.setCursor(65,180);
     tft.print("20");
      tft.setCursor(95,143);
     tft.print("40");
     tft.setTextColor(YELLOW, BLACK);
     tft.setCursor(150,127);
     tft.print("60");
     tft.setCursor(203,143);
     tft.print("80");
     tft.setTextColor(RED, BLACK);
     tft.setCursor(233,183);
     tft.print("100");
     tft.setCursor(240,225);
     tft.print("120");
    tft.fillCircle(160,240,20,MAGENTA);
    tft.setTextColor(WHITE, BLACK);
    tft.setTextSize(3);
    tft.setCursor(110,45);
    tft.print("          ");
    tft.setCursor(110,45);
    tft.print(randomvalue+90);
    tft.print(" Km/Hr");
     tft.fillCircle(160,240,20,MAGENTA);
               angle = (-90  / 57.2958)  - 1.57; 
              x= 160 + cos(angle) * 80;
              y = 239+sin(angle) * 80;
              float angle1 =angle  - 1.57; 
               float angle2 =angle  + 1.57; 
               float angle3 =angle  + 3.14; 
              x= 160 + cos(angle) * 80;
              y = 239+sin(angle) * 80;
              float x1= 160 + cos(angle1) * 5;
              float y1 = 239+sin(angle1) * 5;
              float x2= 160 + cos(angle2) *5;
              float y2 = 239+sin(angle2) * 5;
              float x3= 160 + cos(angle3) *5;
              float y3 = 239+sin(angle3) * 5;
           // tft.drawLine(160, 239,x,y, WHITE);
              tft.fillCircle(160,239,81,BLACK);
              tft.fillCircle(160,240,20,MAGENTA);
              tft.fillTriangle(x,y,x1,y1,x2,y2, WHITE);
              tft.fillTriangle(x1,y1,x2,y2,x3,y3, WHITE);
   tft.setTextSize(2);
   tft.fillRect(0,0,320,25,GREEN);
   tft.setTextColor(BLACK);
   tft.setCursor(0,5);
   tft.print("     Teach Me Something   ");
  //tft.setTextColor(YELLOW,BLACK);
}

void loop(void)
{
   current =millis();
   if( (current-old)/100>2)
   returnx=0;
    
   down = Touch_getXY();
    if (down)
    {
            on_btn.press(down && on_btn.contains(pixel_x, pixel_y));
        
            if (on_btn.justPressed()) 
            {
              //  on_btn.drawButton(true);
               Serial.println("Button Pressed");
               on_btn.press(false);
               //on_btn.drawButton(false);
            }
           
              i=randomvalue; 
              angle = (i/ 57.2958)  - 1.57; 
              x= 160 + cos(angle) * 80;
              y = 239+sin(angle) * 80;
              tft.drawLine(160, 239,x,y, BLACK);
         
              randomvalue=randomvalue+2; 
              i=randomvalue;
              if(i>90)
                { 
                  i=90;
                  randomvalue=90;
                }
            
              // tft.fillCircle(160, 239,90, BLACK);
              tft.fillCircle(160,240,20,MAGENTA);
               angle = (i  / 57.2958)  - 1.57; 
               float angle1 =angle  - 1.57; 
               float angle2 =angle  + 1.57; 
               float angle3 =angle  + 3.14; 
              x= 160 + cos(angle) * 80;
              y = 239+sin(angle) * 80;
              float x1= 160 + cos(angle1) * 5;
              float y1 = 239+sin(angle1) * 5;
              float x2= 160 + cos(angle2) *5;
              float y2 = 239+sin(angle2) * 5;
              float x3= 160 + cos(angle3) *5;
              float y3 = 239+sin(angle3) * 5;
           // tft.drawLine(160, 239,x,y, WHITE);
              tft.fillCircle(160,239,81,BLACK);
              tft.fillCircle(160,240,20,MAGENTA);
              tft.fillTriangle(x,y,x1,y1,x2,y2, WHITE);
              tft.fillTriangle(x1,y1,x2,y2,x3,y3, WHITE);
              
              tft.setTextSize(3);
              tft.setCursor(110,45);
              tft.setTextColor(WHITE, BLACK);
              int jj = map(randomvalue+90,0,180,0,120);
              tft.print(jj);
              tft.print(" Km/Hr");
              returnx=1;
              delay(30);
              current =millis();
              old=current;
  
  }
  
  
  else if (!down && returnx ==0)
  {
          down = Touch_getXY();
          if(randomvalue<=-90 || down==1)
          return;
          i=randomvalue; 
          angle = (i/ 57.2958)  - 1.57; 
          x= 160 + cos(angle) * 80;
          y = 239+sin(angle) * 80;
          tft.drawLine(160, 239,x,y, BLACK);
          
       
          randomvalue=randomvalue-4; 
          i=randomvalue;
          if(i<-90)
            { 
              i=-90;
              randomvalue=-90;
            }
          
         // tft.fillCircle(160, 239,90, BLACK);
          tft.fillCircle(160,240,20,MAGENTA);
          angle = (i  / 57.2958)  - 1.57; 
          x= 160 + cos(angle) * 80;
          y = 239+sin(angle) * 80;
          tft.setTextColor(WHITE, BLACK);
         
           float angle1 =angle  - 1.57; 
               float angle2 =angle  + 1.57; 
               float angle3 =angle  + 3.14; 
              x= 160 + cos(angle) * 80;
              y = 239+sin(angle) * 80;
              float x1= 160 + cos(angle1) * 5;
              float y1 = 239+sin(angle1) * 5;
              float x2= 160 + cos(angle2) * 5;
              float y2 = 239+sin(angle2) * 5;
              float x3= 160 + cos(angle3) *5;
              float y3 = 239+sin(angle3) * 5;
              tft.fillCircle(160,239,81,BLACK);
              tft.fillCircle(160,240,20,MAGENTA);
              tft.fillTriangle(x,y,x1,y1,x2,y2, WHITE);
              tft.fillTriangle(x1,y1,x2,y2,x3,y3, WHITE);
         // tft.drawLine(160, 239,x,y, WHITE);
          tft.setTextSize(3);
       
          tft.setCursor(110,45);
          int jj = map(randomvalue+90,0,180,0,120);
          tft.print(jj);
          tft.print(" Km/Hr  ");
          delay(30);
        }
 
}
