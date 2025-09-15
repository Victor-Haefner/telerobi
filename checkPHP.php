<?php
$folder = 'botData';

if (!is_writable($folder)) {
    echo "Warning: PHP cannot write to $folder";
} else {
    echo "PHP permissions ok";
}
?>
