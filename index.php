<?php
require __DIR__ . '/vendor/autoload.php';
require ('object.inc');

//use mbstring;

//phpinfo();

$creator = new bibParser();

echo print_r('doing something');
echo $creator -> letstest ("wow");
//echo mb_strtoupper ("omg");

use Cowsayphp\Farm;

header('Content-Type: text/plain');

$text = "Set a message by adding ?message=<message here> to the URL";
if(isset($_GET['message']) && $_GET['message'] != '') {
	$text = htmlspecialchars($_GET['message']);
}

$cow = Farm::create(\Cowsayphp\Farm\Cow::class);
echo $cow->say($text);
