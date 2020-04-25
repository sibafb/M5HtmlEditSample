#include <M5Stack.h>
#include <SPIFFS.h>

#include "HtmlEdit.h";

String ssid_string;
String ip_string;
String edit_string;

#define ButtonA 39

void setup() {
  /*LCDのフォントロードのためSPIFFSを起動*/
  if(!SPIFFS.begin()){
    return;
  }

  Serial.begin(115200);

  M5.begin();

  InitHtmlEdit(ssid_string, ip_string);
  /* LCDに文章表示 */
  M5LcdPrint( String(ssid_string)+"に接続し\n"+ ip_string +"に接続してください");
  edit_string = "editme!";
  delay(3000);
}

void loop() {
   EditStatus et_status;
   et_status = APEditing(edit_string);

   if(et_status == Changed){
      M5LcdPrint( String(ssid_string)+"に接続し\n"+ ip_string +"に接続してください\n"+"Edit String:\n"+edit_string);
   }
   /* 編集終了したら電源OFF */
   if(et_status == EditFinished){
       M5.Power.powerOFF();
   }
    if(digitalRead(ButtonA) == LOW){
       M5.Power.reset();
    }
}

void M5LcdPrint(String str)
{
      M5.Lcd.clear(BLACK);
      M5.Lcd.setCursor(0,0);
      M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK); 
      M5.Lcd.loadFont("melio28",false);
      M5.Lcd.println(str);
}
