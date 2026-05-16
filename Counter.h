#include <WebSocketsServer.h> //https://github.com/Links2004/arduinoWebSockets

WebSocketsServer stream=WebSocketsServer(81);

#define memoryDepthSec 300
#define memoryDepthMin 300

volatile uint8_t count=0;
uint8_t start=0;

void ISR() { count++; }

void streamEvent(uint8_t num,WStype_t type,uint8_t * payload,size_t length) {
  switch(type) {
    case WStype_CONNECTED: { Serial.print(stream.connectedClients()); Serial.println(" WebSocket clients connected."); 
      if (start==1) { start=2; } }
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
  static uint32_t counterMemorySec[memoryDepthSec],pointerMemorySec;
  static uint32_t counterMemoryMin[memoryDepthMin],pointerMemoryMin;
  if (start==2) { start=0;
    if (counterMemoryMin[pointerMemoryMin]>0) {
      for (int n=pointerMemoryMin;n<memoryDepthMin;n++) { stream.broadcastTXT(String(counterMemoryMin[n]).c_str()); } }
    for (int n=0;n<pointerMemoryMin;n++) { stream.broadcastTXT(String(counterMemoryMin[n]).c_str()); }
    if (counterMemorySec[pointerMemorySec]>0) {
      for (int n=pointerMemorySec;n<memoryDepthSec;n++) { stream.broadcastTXT(String(counterMemorySec[n]).c_str()); } }
    for (int n=0;n<pointerMemorySec;n++) { stream.broadcastTXT(String(counterMemorySec[n]).c_str()); } }

  static uint32_t timer=millis()+1000,counterSum=0,counter[60];
  static uint8_t pointer;
  if (count>0) { counter[pointer]+=count; counterSum+=count; count=0; }
  if (millis()>=timer) { timer=millis()+1000;
    Serial.print("CPM: "); Serial.print(counterSum);
    Serial.print(" µSv/h: "); Serial.println((float)counterSum/151);
    if (stream.connectedClients()>0) { stream.broadcastTXT(String(counterSum).c_str()); }
    counterMemorySec[pointerMemorySec]=counterSum; pointerMemorySec++; pointerMemorySec%=memoryDepthSec;
    if (pointer==59) { counterMemoryMin[pointerMemoryMin]=counterSum; pointerMemoryMin++; pointerMemoryMin%=memoryDepthMin; }
    pointer++; pointer%=60;
    counterSum-=counter[pointer];
    counter[pointer]=0; } }

void webSocketsWorker() { stream.loop(); }
