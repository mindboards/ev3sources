#! /bin/bash

cp ~/projects/lms2012/lms2012/doc/Doxyfile Doxyfile
doxygen Doxyfile
cp ~/projects/lms2012/lms2012/doc/graphics/LEGO.jpg ~/projects/lms2012/lms2012/doc/html/LEGO.jpg
cp ~/projects/lms2012/lms2012/doc/graphics/Lms2012.jpg ~/projects/lms2012/lms2012/doc/html/Lms2012.jpg
cp ~/projects/lms2012/lms2012/doc/graphics/SystemBlock.jpg ~/projects/lms2012/lms2012/doc/html/SystemBlock.jpg

cp ~/projects/lms2012/lms2012/doc/flowcharts/generalnavigation.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/flowcharts/program.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/flowcharts/view.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/flowcharts/datalog.png ~/projects/lms2012/lms2012/doc/html/

cp ~/projects/lms2012/lms2012/doc/flowcharts/motorcontrol.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/flowcharts/ircontrol.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/flowcharts/filenavigation.png ~/projects/lms2012/lms2012/doc/html/
cp ~/projects/lms2012/lms2012/doc/flowcharts/buttonlight.png ~/projects/lms2012/lms2012/doc/html/

rm Doxyfile

cd ~/projects/lms2012/lms2012/doc/html
firefox index.html

