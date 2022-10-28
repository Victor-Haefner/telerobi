<?php

include 'robot.php';

$uid = $_GET['botID'];
$robots = parseRobots();
$bot = $robots[$uid];
$port = $bot->tcpPort;


$sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
socket_connect($sock, '127.0.0.1', $port);
$msg = "getStatus";
socket_write($sock, $msg, strlen($msg));
echo socket_read($sock, 2048);
socket_close($sock);

?>
