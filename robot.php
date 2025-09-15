<?php

ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

class Robot {
	public $uID;
	public $name;
	public $state;
	public $udpPort;
	public $tcpPort;
	public $pID;
	public $lastRegistred;

	function fromString($line) {
		$parts = explode("|", $line);
		$this->uID = $parts[0];
		$this->name = $parts[1];
		$this->state = $parts[2];
		$this->udpPort = $parts[3];
		$this->tcpPort = $parts[4];
		$this->pID = $parts[5];
		$this->lastRegistred = $parts[6];
	}

	function asString() {
		$uID = $this->uID;
		$name = $this->name;
		$state = $this->state;
		$udpPort = $this->udpPort;
		$tcpPort = $this->tcpPort;
		$pID = $this->pID;
		$lReg = $this->lastRegistred; 
		return "$uID|$name|$state|$udpPort|$tcpPort|$pID|$lReg";
	}
}

function parseRobots() {
	if (!file_exists("botData/robots.txt")) return [];

	$robotsData = file_get_contents("botData/robots.txt");
	$robots = [];
	$rows = explode("\n", $robotsData); // all robots lines

	for ($i = 0; $i < count($rows); $i++) {
		if ($rows[$i] == "") continue;
		$bot = new Robot();
		$bot->fromString($rows[$i]);
		$robots[$bot->uID] = $bot;
	}
	return $robots;
}

?>
