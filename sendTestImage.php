<?php

include 'robot.php';

$uid = $_GET['botID'];
$robots = parseRobots();
$bot = $robots[$uid];
$port = $bot->udpPort;


// get test image
$filename = "testImage.jpg"; 
$handle = fopen($filename, "rb"); 
$fsize = filesize($filename); 
$contents = fread($handle, $fsize); 


function sendUDPPacket($sock, $port, $header, $contents, $begin, $chunkSize, $dataSize) {
  if ($chunkSize == 0) return 0;
  
  $csBytes = unpack("C*", pack("L", $chunkSize));
  $dsBytes = unpack("C*", pack("L", $dataSize));

  $packetBuffer = str_repeat(chr(0), 1024);
  $headerSize = min(255,strlen($header));
  $headerOffset = 2 + sizeof($csBytes) + sizeof($dsBytes);
  $dataOffset = $headerOffset+$headerSize;
  $packetBuffer[0] = chr($headerOffset);
  $packetBuffer[1] = chr($dataOffset);

  $chunkSize = min($chunkSize, 1024 - $dataOffset);
  for ($i=0; $i<sizeof($dsBytes); $i++) $packetBuffer[2+$i] = chr($dsBytes[1+$i]);
  for ($i=0; $i<sizeof($csBytes); $i++) $packetBuffer[2+sizeof($dsBytes)+$i] = chr($csBytes[1+$i]);
  for ($i=0; $i<strlen($header); $i++) $packetBuffer[$headerOffset+$i] = $header[$i];
  for ($i=0; $i<$chunkSize; $i++) $packetBuffer[$dataOffset+$i] = $contents[$begin+$i];

  $packetSize = $dataOffset + $chunkSize;
  socket_sendto($sock, $packetBuffer, $packetSize, 0, '127.0.0.1', $port);
  //echo "<br> send $packetSize bytes on port $port";
  return $chunkSize;
}

$sock = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);

for ($n = 0; $n<$fsize; ) {
    $header = "A";
    if ($n == 0) $header = "N";
    
    $written = sendUDPPacket($sock, $port, $header, $contents, $n, $fsize-$n, $fsize);
    $n += $written;
    if ($written == 0) break;
}

socket_close($sock);

?>
