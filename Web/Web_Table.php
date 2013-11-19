<?php 
$fp1 = fopen("~/watt.txt","r");
while(!feof($fp1)) { 
  $str1 = fgets($fp1,10000); 
} 
if ($str1<=100){
	$str2 = $str1*59.10+400;
}
else if ($str1>=101){
	$str2 = $str1*122.60+890;
}
else if ($str1>=201){
	$str2 = $str1*183+1560;
}
else if ($str1>=301){
	$str2 = $str1*273.20+7110;
}
else if ($str1>=401){
	$str2 = $str1*406.70+7110;
}
else if ($str1>=501){
	$str2 = $str1*690.80+12600;
}

$fp = fopen("~/value.txt","r");
while(!feof($fp)) { 
  $str = fgets($fp,10000); 
  $arr[] = $str; 
} 


echo "<BODY background='background.png'>
<table id='table1' width='210' height='180' border='1'>";
echo "
  <tr id='row1'>
	<td align='center'>사용량</td>
	<td align='center'>$str1 kW</td>
  </tr>
  <tr id='row1'>
	<td align='center'>요금</td>
	<td align='center'>$str2 원</td>
  </tr>
  <tr id='row2'>
	<td align='center'>현재부하</td>
	<td align='center'>$arr[0] 만 kW</td>
  </tr>
  <tr id='row3'>
	<td align='center'>운영예비력</td>
	<td align='center'>$arr[1] 만 kW</td>
  </tr>
  <tr id='row4'>
	<td align='center'>운영예비율</td>
	<td align='center'>$arr[2] %</td>
  </tr>";
echo "</table>";
fclose($fp); 
fclose($fp1); 
?>
