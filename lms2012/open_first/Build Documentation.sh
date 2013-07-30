#! /bin/bash

cp ~/projects/lms2012/lms2012/doc/Doxyfile Doxyfile
doxygen Doxyfile
cp ~/projects/lms2012/lms2012/doc/graphics/LEGO.jpg ~/projects/lms2012/lms2012/doc/html/LEGO.jpg
cp ~/projects/lms2012/lms2012/doc/graphics/Lms2012.jpg ~/projects/lms2012/lms2012/doc/html/Lms2012.jpg
cp ~/projects/lms2012/lms2012/doc/graphics/SystemBlock.jpg ~/projects/lms2012/lms2012/doc/html/SystemBlock.jpg

cp ~/projects/lms2012/lms2012/doc/flowcharts/generalnavigation.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/flowcharts/connections.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/flowcharts/program.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/flowcharts/view.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/flowcharts/datalog.png ~/projects/lms2012/lms2012/doc/html/

cp ~/projects/lms2012/lms2012/doc/flowcharts/motorcontrol.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/flowcharts/ircontrol.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/flowcharts/filenavigation.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/flowcharts/buttonlight.png ~/projects/lms2012/lms2012/doc/html/

cp ~/projects/lms2012/lms2012/doc/schematics/ep2_p2.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/schematics/ep2_p3.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/schematics/ep2_p4.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/schematics/ep2_p5.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/schematics/ep2_p6.png ~/projects/lms2012/lms2012/doc/html/

cp ~/projects/lms2012/lms2012/doc/schematics/ev3_color_sensor.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/schematics/ev3_gyro_sensor.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/schematics/ev3_ir_sensor.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/schematics/ev3_touch_sensor.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/schematics/ev3_ultrasonic_sensor.png ~/projects/lms2012/lms2012/doc/html/

rm Doxyfile

cd ~/projects/lms2012/lms2012/doc/html
firefox index.html

