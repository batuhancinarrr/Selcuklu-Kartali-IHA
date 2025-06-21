
$fn=100;
length = 200; width = 80; height = 40; wall = 3;

difference() {
    cube([length, width, height]);
    translate([wall, wall, wall])
        cube([length - 2*wall, width - 2*wall, height]);
    translate([20, 20, height - 10])
        cylinder(h=10, r=2.5);
    translate([20, width - 20, height - 10])
        cylinder(h=10, r=2.5);
    translate([length - 10, width/2 - 5, 0])
        cube([5, 10, 15]);
}
