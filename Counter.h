#include <WebSocketsServer.h> //https://github.com/Links2004/arduinoWebSockets

WebSocketsServer stream=WebSocketsServer(81);

volatile uint8_t count=0;

void ISR() { count++; }

void streamEvent(uint8_t num,WStype_t type,uint8_t * payload,size_t length) {
  switch(type) {
    case WStype_CONNECTED: { Serial.print(stream.connectedClients()); Serial.println(" WebSocket clients connected."); }
    case WStype_DISCONNECTED: { Serial.print(stream.connectedClients()); Serial.println(" WebSocket clients connected."); }
    case WStype_TEXT: { }
    case WStype_BIN: { }
    case WStype_ERROR: { }
    default: { } } }

void initCounter() {
  pinMode(16,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(16),ISR,RISING); }

void initWebSockets() {
  stream.begin();
  stream.onEvent(streamEvent); }

void counterWorker() {
  static uint32_t timer=millis()+1000,counterSum=0,counter[60];
  static uint8_t pointer;
  if (count>0) { counter[pointer]+=count; counterSum+=count; count=0; }
  if (millis()>=timer) { timer=millis()+1000;
    Serial.print("CPM: "); Serial.print(counterSum);
    Serial.print(" µSv/h: "); Serial.println((float)counterSum/151);
    if (stream.connectedClients()>0) { stream.broadcastTXT(String(counterSum).c_str()); }
    pointer++; pointer%=60;
    counterSum-=counter[pointer];
    counter[pointer]=0; } }

void webSocketsWorker() { stream.loop(); }
