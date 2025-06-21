
// ESP32 Jammer Verici Kutusu - Potansiyometre Deliği Eklendi

$fn=100;

module verici_case_pot() {
    wall = 2;
    length = 90;
    width = 60;
    height = 30;

    difference() {
        cube([length, width, height]);
        translate([wall, wall, wall])
            cube([length - 2*wall, width - 2*wall, height]);

        // USB-C açıklığı (yan panel)
        translate([0, width/2 - 5, 10])
            cube([wall+0.1, 10, 10]);

        // SMA anten açıklığı (üst)
        translate([length/2 - 5, width, height - 10])
            rotate([90,0,0])
            cylinder(r=4, h=wall+0.1);

        // Buton deliği (üst yüzey)
        translate([length/2, width/2, height])
            cylinder(r=3, h=wall+0.5);

        // DHT sensör havalandırması
        for (i = [0:2]) {
            translate([5 + i*5, width - wall, height - 5])
                rotate([90,0,0])
                cylinder(r=1, h=wall+0.2);
        }

        // Montaj delikleri
        for (x = [5, length-5], y = [5, width-5]) {
            translate([x, y, 0])
                cylinder(r=1.2, h=height + 1);
        }

        // Potansiyometre deliği (üst yüzey)
        translate([20, 20, height])
            cylinder(r=3.25, h=wall+0.5);
    }
}

verici_case_pot();
