#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#include "HtmlEdit.h";

const char *ssid = "M5EDIT";
const char *password = "editme2020";
//const IPAddress ip(192,168,30,3);
//const IPAddress subnet(255,255,255,0);

WiFiServer server(80);

bool InitHtmlEdit(String& ssid_in ,String& ip_string){
    // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);

  ssid_in = String(ssid);
  IPAddress myIP = WiFi.softAPIP();
  ip_string = myIP.toString();
  
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  delay(100);
  server.begin();

  Serial.println("Server started");
}

EditStatus APEditing(String& edit_string){
  WiFiClient client = server.available();  

  String buff = "";

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    String forrequest = "";
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print(HtmlForEdit("Edit_String",edit_string));

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
          forrequest += c;
        }
        if (forrequest.indexOf("edit_string=") >= 0) {
          /* "edit_string="以下の値を buffに書き込み　*/
          buff="";
          GetHtmlRequest( "edit_string=", buff, forrequest);
          
          /* buffの内容が空白でなく、edit_stringと違っていたら書き換え＆再表示 */
          if(buff.compareTo(edit_string) != 0 && buff.compareTo("") != 0){
            /* 書き換え処理 */
            edit_string = buff;
            buff="";//再利用できるようにするため、buffを元に戻す
            M5LcdPrint( "SSID:" + String(ssid_string) + "に接続し\n"+ ip_string +"に接続してください\n"+"Edit String:\n"+edit_string);  //ここでreturn返すとclientとの接続が切れるので直接書き換え
          } 
        }
        if (forrequest.indexOf("edit_end=") >= 0) {
          /* "edit_end="以下の値を buffに書き込み　*/
          buff="";
          GetHtmlRequest( "edit_end=", buff, forrequest);
          
          if(buff.compareTo("end") == 0){
            return EditFinished;
          }
        }

      }

      
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

/**
 * @brief HTML からgetリクエストの文字列を抽出し、buf（content）に書き込み
 * @param[in] request_name htmlリクエストのタグ名（input_name=以下の値）
 * @param[in] html_request htmlリクエスト全文
 * @param[out] request_content 入力用バッファ
 * @return none
 */
void GetHtmlRequest(String request_name,String &request_content,String html_request){
  int i;
  request_content = "";
  i = html_request.indexOf(request_name);
  i = i + request_name.length();
  /* Accept-Encoding対策 */
  int accept_encoding = html_request.indexOf("Accept-Encoding");
  while(html_request[i] != '\n'&& html_request[i] != '&'&& html_request[i] != ' '&& html_request[i] != '\0'&& html_request[i] != '\r' && accept_encoding > i){
     request_content += html_request[i];
     i++;
  }
}

/**
 * @brief 引数を代入したhtmlデータの生成
 * @param[in] edit_string_name
 * @param[in] hedit_string
 * @return html
 */
String HtmlForEdit(String edit_string_name, String edit_string){
    String edit_html_for_return = 
    "<!DOCTYPE html><html lang='ja'><head><meta charset='UTF-8'>\
     <meta http-equiv='Pragma' content='no-cache'>\
     <meta http-equiv='Cache-Control' content='no-cache'>\
     <meta http-equiv='Expires' content='0'>\
     <style>input {margin:8px;width:120px;}\
      div {font-size:200%;color:black;text-align:left;width:1000px;border:black 1px black;}</style>\
      <div>\
      <title>" + edit_string_name + "</title></head>\
      <form method='get'>\
      <span style='font-size:160%;'>" + edit_string_name  + "</span><br>\
      <textarea id='t_message' style='font-size:180%;' name='edit_string' cols='30' rows='5'  placeholder=" + edit_string + "></textarea><br>\
      <input type='submit' name='edit_end' style='font-size:180%;height:100px;width:300px;' value='send' />\
    　<input type='submit' name='edit_end' style='font-size:180%;height:100px;width:300px;' value='end' /><br>\
      </form>\
      </div></body></html>\
    ";
    return edit_html_for_return;
}
