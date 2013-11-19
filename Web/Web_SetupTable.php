<?php
$fp = fopen("~/value.txt","r");
while(!feof($fp)) { 
  $str = fgets($fp,10000); 
  $arr[] = $str; 
} 
fclose($fp); 
fclose($fp1); 
$fp = fopen("~/sete.txt","w");
if ($arr[1]<500&&$arr[1]>=400){
	fwrite($fp, $_POST[Sauce1] + ' ');
	fwrite($fp, $_POST[updown1]);
}
if ($arr[1]<400&&$arr[1]>=300){
	fwrite($fp, $_POST[Sauce2] + ' ');
	fwrite($fp, $_POST[updown2]);
}
if ($arr[1]<300&&$arr[1]>=200){
	fwrite($fp, $_POST[Sauce3] + ' ');
	fwrite($fp, $_POST[updown3]);
}
if ($arr[1]<200&&$arr[1]>=100){
	fwrite($fp, $_POST[Sauce4] + ' ');
	fwrite($fp, $_POST[updown4]);
}
if ($arr[1]<100){
	fwrite($fp, 0);
	fwrite($fp, '\r\n');
	fwrite($fp, 0);
}
echo '적용 완료';
?>
