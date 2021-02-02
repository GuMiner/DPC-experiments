MAX = 20;
union(){
for(i=[1:MAX])
{
	for (j=[1:MAX])
	{
 		translate([i, j, 1])
		cube([1, 1, 1.1+sin((i-MAX/2)*360/MAX)*cos((j-MAX)*360/MAX)]);
	}
}
}