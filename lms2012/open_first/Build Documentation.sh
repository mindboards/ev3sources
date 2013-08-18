#! /bin/bash

cd ../lms2012/doc
doxygen Doxyfile
cp ~/projects/lms2012/lms2012/doc/graphics/LEGO.jpg ~/projects/lms2012/lms2012/doc/html/LEGO.jpg

firefox html/index.html

