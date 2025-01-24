use <../../ktne_3d_models/bomb_base.scad>;

$fn = $preview ? 50 : 100;

tolerance = 0.4;
detail_wall_thickness = 1.5;
detail_height = 1.4;
detail_tolerance = 0.15;
detail_corner_radius = 1;
detail_skirt_size = 1;
detail_skirt_height = 1;

wall_thickness = 2;

speaker_hole_radius = 1.8 / 2;
speaker_hole_space = 3;
speaker_hole_sides = 6;

button_radius = 3.8;
button_height = 7;
button_height_above_top = 2;

module main_module() {
    difference() {
        bomb_module_top(height_above_pcb = 10.5 - detail_height);
        translate([45, 71.5]) cube([25 + 2 * detail_wall_thickness + tolerance, 21 + 2 * detail_wall_thickness + tolerance, 10], center = true);
        translate([45, 42.5]) cube([75.2 + 2 * detail_wall_thickness + tolerance, 25 + 2 * detail_wall_thickness + tolerance, 10], center = true);
        translate([90 - 18.5, 90 - 18.5]) cylinder(10, 9, 9, center = true);
        translate([26.2, 16]) cylinder(10, 4, 4, center = true);
        translate([90 - 26.2, 16]) cylinder(10, 4, 4, center = true);
    }
}

module strike_border_detail() {
    difference() {
        union() {
            minkowski() {
                translate([0, 0, (detail_height + wall_thickness - detail_corner_radius) / 2])
                cube([25 + 2 * detail_wall_thickness - 2 * detail_corner_radius - detail_tolerance, 21 + 2 * detail_wall_thickness - 2 * detail_corner_radius - detail_tolerance, detail_height + wall_thickness - detail_corner_radius], center = true);
                difference() {
                    sphere(detail_corner_radius);
                    translate([0, 0, -detail_corner_radius]) cube([2 * detail_corner_radius, 2 * detail_corner_radius, 2 * detail_corner_radius], center = true);
                }
            }
            translate([0, 0, - detail_skirt_height / 2]) cube([25 + 2 * detail_wall_thickness - detail_tolerance + 2 * detail_skirt_size, 21 + 2 * detail_wall_thickness - detail_tolerance + 2 * detail_skirt_size, detail_skirt_height], center = true);
        }
        cube([25 + tolerance, 21 + tolerance, 20], center = true);
    }
}

module timer_border_detail() {
    difference() {
        union() {
            minkowski() {
                translate([0, 0, (detail_height + wall_thickness - detail_corner_radius) / 2])
                cube([75.2 + 2 * detail_wall_thickness - 2 * detail_corner_radius - detail_tolerance, 25 + 2 * detail_wall_thickness - 2 * detail_corner_radius - detail_tolerance, detail_height + wall_thickness - detail_corner_radius], center = true);
                difference() {
                    sphere(detail_corner_radius);
                    translate([0, 0, -detail_corner_radius]) cube([2 * detail_corner_radius, 2 * detail_corner_radius, 2 * detail_corner_radius], center = true);
                }
            }
            translate([0, 0, - detail_skirt_height / 2]) cube([75.2 + 2 * detail_wall_thickness - detail_tolerance + 2 * detail_skirt_size, 25 + 2 * detail_wall_thickness - detail_tolerance + 2 * detail_skirt_size, detail_skirt_height], center = true);
        }
        cube([75.2 + tolerance, 25 + tolerance, 20], center = true);
    }
}

module speaker_detail() {
    difference() {
        translate([0, 0, detail_skirt_height]) union() {
            minkowski() {
                cylinder(wall_thickness + detail_height - detail_corner_radius, 9 - detail_corner_radius - detail_tolerance / 2, 9 - detail_corner_radius - detail_tolerance / 2);
                difference() {
                    sphere(detail_corner_radius);
                    translate([0, 0, -detail_corner_radius]) cube([2 * detail_corner_radius, 2 * detail_corner_radius, 2 * detail_corner_radius], center = true);
                }
            }
            translate([0, 0, - detail_skirt_height]) cylinder(detail_skirt_height, 9 + detail_skirt_size / 2, 9 + detail_skirt_size / 2);
        }
        cylinder(detail_skirt_height + wall_thickness, 8, 8);
        for (r = [0:speaker_hole_sides]) {
            rotate([0, 0, (360 / speaker_hole_sides) * r]) {
                for (i = [0:2]) {
                    translate([i * speaker_hole_space, 0, 0]) cylinder(20, speaker_hole_radius, speaker_hole_radius);
                }
                rotate([0, 0, 360 / speaker_hole_sides / 2]) translate([2 * speaker_hole_space, 0, 0]) cylinder(20, speaker_hole_radius, speaker_hole_radius);
            }
        }
    }
}

module button() {
    union() {
        minkowski() {
            cylinder(7 - detail_corner_radius, button_radius - detail_corner_radius, button_radius - detail_corner_radius);
            difference() {
                sphere(detail_corner_radius);
                translate([0, 0, -detail_corner_radius]) cube([2 * detail_corner_radius, 2 * detail_corner_radius, 2 * detail_corner_radius], center = true);
            }
        }
        cylinder(button_height - button_height_above_top - wall_thickness, button_radius + wall_thickness, button_radius + wall_thickness);
    }
}

color("darkgray") main_module();
color("black") translate([45, 71.5, -wall_thickness]) strike_border_detail();
color("black") translate([45, 42.5, -wall_thickness]) timer_border_detail();
color("black") translate([90 - 18.5, 90 - 18.5, -wall_thickness]) speaker_detail();
color("black") translate([26.2, 16, - button_height + button_height_above_top]) button();
color("black") translate([90 - 26.2, 16, - button_height + button_height_above_top]) button();