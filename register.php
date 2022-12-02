<?php
// 1) get the registration ID from URI/GET parameters
// 2) read the robots file, append new robot if not present, write file
// 3) check 

// port ranges, UDP: 4500-4599, TCP: 4600-4699

include 'robot.php';

// send via get request: 
//    https:\\...\register.php?name=bob&uid=13.13.23.13.45.45
$name = $_GET['name'];
$uid = $_GET['uid'];

// parse robots data
$robots = parseRobots();

// add robot if necessary
if (array_key_exists($uid, $robots) === false) {
	$bot = new Robot();
	$bot->uID = $uid;
	$bot->name = $name;
	$bot->state = "connected";
	$bot->udpPort = 0;
	$bot->tcpPort = 0;
	$bot->pID = 0;
	$robots[$bot->uID] = $bot;
}


// make changes as necessary
$bot = $robots[$uid];
$bot->name = $name;
$bot->state = "connected";
$bot->lastRegistred = time();

function isPortFree($robots, $port) {
	foreach($robots as $uid => $bot) {
		if ($bot->udpPort == $port) return false;
		if ($bot->tcpPort == $port) return false;
	}
	return true;
}

if ($bot->udpPort == 0) {
	$tmpPort = 4500;
	while(isPortFree($robots, $tmpPort) === false) {
		$tmpPort += 1;
	}
	$bot->udpPort = $tmpPort;
}

if ($bot->tcpPort == 0) {
	$tmpPort = 4600;
	while(isPortFree($robots, $tmpPort) === false) {
		$tmpPort += 1;
	}
	$bot->tcpPort = $tmpPort;
}


function startDeamon($bot) {
	//echo "start deamon for $bot->uID!\n";
	$cmd = "python telerobiDeamon.py $bot->uID $bot->udpPort $bot->tcpPort > /dev/null &";
	exec($cmd);
	usleep(1000000); // wait for python to write its pID
	$pIDfile = "tmp/$bot->uID.txt";
	if (file_exists($pIDfile)) {
		$bot->pID = file_get_contents($pIDfile);
		unlink($pIDfile);
		//echo " started with pID $bot->pID\n";
	} else {
		$bot->pID = 0;
		//echo " could not start deamon!\n";
	}
}

function stopDeamon($bot) {
	$out = [];
	$cmd = "kill $bot->pID";
	exec($cmd, $out);
}

function deamonRunning($bot) {
	$out = [];
	exec("ps -p $bot->pID", $out);
	for ($i = 0; $i < count($out); $i++) {
		if (strpos($out[$i], $bot->pID) !== false) {
		    	return true;
		}
	}
	return false;
}

if ($bot->pID == 0) {
	startDeamon($bot);
} else {
	stopDeamon($bot);
	usleep(1000000); // wait for deamon to be killed!
	startDeamon($bot);
	/*if (deamonRunning($bot) === false) {
		startDeamon($bot);
	}*/
}


// serialize and write back
$rows = [];
foreach($robots as $uid => $b) {
	$rows[] = $b->asString();
}
$robotsData = join("\n", $rows);
file_put_contents("tmp/robots.txt", $robotsData);

echo "ports: $bot->udpPort $bot->tcpPort";
?>
