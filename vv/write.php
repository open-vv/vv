<?php
define('HIDDEN_DATABASE_FILE_NAME', './customers.csv');
$file=fopen(HIDDEN_DATABASE_FILE_NAME, 'a');
if($file===FALSE)exit(1);
define('TOKEN_CSV', '|');
define('ENTRY_SEPARATOR', "\n");
define('NAME_LENGTH', 30);
define('EMAIL_LENGTH', 40);
define('GROUP_LENGTH', 30);
define('OS_LENGTH', 20);
define('VV_VERSION_LENGTH', 30);
define('COUNTRY_LENGTH', 30); 
define('ARCHITECTURE_LENGTH',10);
define('COMPILATION_DATE_LENGTH', 30); 
function ensureString($var, $length){
  $var = str_replace("\n", '',$var);
  $var = str_replace(TOKEN_CSV, '', $var);
  return substr($var, 0, $length);
}
function writeLine($file, $data){
 fwrite($file, implode(TOKEN_CSV, $data).ENTRY_SEPARATOR);
}
$data['name']	  = ensureString($_GET['name'], NAME_LENGTH);
$data['lastname'] = ensureString($_GET['lastName'], NAME_LENGTH);
$data['email']	  = ensureString($_GET['email'], EMAIL_LENGTH);
$data['group']	  = ensureString($_GET['group'], GROUP_LENGTH);
$data['ip'] 	  = $_SERVER['REMOTE_ADDR'];
$data['os']	  = ensureString($_GET['os'], OS_LENGTH);
$data['vvVersion']= ensureString($_GET['vvVersion'], VV_VERSION_LENGTH);
$data['time']	  = date("F j, Y, g:i a");
$data['geoloc']	  = ensureString(file_get_contents('http://api.hostip.info/country.php?ip='.$_SERVER['REMOTE_ADDR']), COUNTRY_LENGTH);
$data['architecture'] = ensureString($_GET['architecture'], ARCHITECTURE_LENGTH);
$data['compilationDate'] = ensureString($_GET['compilationDate'], COMPILATION_DATE_LENGTH);
writeLine($file, $data);
fclose();
