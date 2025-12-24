#include <WebServer.h>
#include <ESPmDNS.h>

WebServer webServer(80);

const char *loginIndex=R"(
<!DOCTYPE html>
<html lang="de"><head>
<meta charset="UTF-8">
<title>gammaDetect</title>
<style>
  body { font-family:Arial,sans-serif; margin:30px; }
  button { padding:0.5em; float:right; }
</style>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<script src="https://cdn.jsdelivr.net/npm/hammerjs"></script>
<script src="https://cdn.jsdelivr.net/npm/chartjs-plugin-zoom"></script>
<script>
function initMain() { openStream(); window.setInterval("checkStream();",5000);
  labels=[]; data1=[]; data2=[]; oldx=0; data=""; config=""; getChart();
  myChart=new Chart(document.getElementById('myChart'),config); }

function openStream() { stream=new WebSocket("ws://"+window.location.hostname+":81");
  stream.onmessage=streamMessage; stream.heartbeat=0; }
function checkStream() { if (stream.heartbeat==0) { stream.close(); openStream(); } else { stream.heartbeat=0; } }
function streamMessage(event) { stream.heartbeat++; x1=event.data*1; x2=x1/151;
  if (labels.length==0) { oldx=x2 } x3=(x2+oldx)/2; oldx=x3;
  id("text1").innerHTML=x1; id("text2").innerHTML=x2.toFixed(2);
  labels.push(labels.length); data1.push(x1); data2.push(x3); myChart.update("none"); }

function getChart() {
  data={labels:labels,datasets:[{
    label:'cpm',
    backgroundColor:'rgb(170,170,170)',borderColor:'rgb(170,170,170)',borderWidth:2,
    yAxisID:'y1',xAxisID:'x',data:data1,hidden:true,
    cubicInterpolationMode:'monotone',tension:0.4},{
    label:'µSv/h',
    backgroundColor:'rgb(200,80,80)',borderColor:'rgb(200,80,80)',borderWidth:2,
    yAxisID:'y2',xAxisID:'x',data:data2,
    cubicInterpolationMode:'monotone',tension:0.4}]};

  config={type:'line',data,options:{
    interaction:{mode:'nearest',intersect:false},
    resizeDelay:50,radius:0,scales:{
      x:{min:function(){if(labels.length>120){return labels.length-120;}else{return 0;}}},
      y1:{position:"left",min:0,grid:{drawOnChartArea:true},display:'auto'},
      y2:{position:"right",min:0,grid:{drawOnChartArea:true},display:'auto'}},
    plugins:{zoom:{
      pan:{enabled:true,mode:'x'},
      zoom:{mode:'x',
        wheel:{enabled:true},
        pinch:{enabled:true}}}}}}; }

function resetZoom() { myChart.resetZoom();
  myChart.config.options.scales.x.min=function(){if(labels.length>120){return labels.length-120;}else{return 0;}}; }

function id(id) { return document.getElementById(id); }
</script></head>
<body onload="initMain();">
  <span><span id="text1">0</span> cpm &nbsp; &nbsp; &nbsp; <span id="text2">0</span> µSv/h
  <button type="button" onclick="resetZoom();">Reset Zoom</button></span>
  <canvas id="myChart"></canvas>
</body></html>)";

void initHTTP() {
  MDNS.begin("gammadetect");
  webServer.on("/", HTTP_GET, []() {
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/html", loginIndex);});
  webServer.begin(); }

void httpWorker() { webServer.handleClient(); }
