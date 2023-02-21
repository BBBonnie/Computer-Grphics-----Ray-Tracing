COMPILATION

To compile the code, run "scons" from the commandline. If you do not already
have this program, you will need to install it:https://scons.org/pages/download.html. 
You may also need to install other libraries (such as the dev files for libpng: http://www.libpng.org/pub/png/libpng.html).


TEST CASES

The directory "test level 1", "test level 2", etc. contains the test cases.
The main.cpp file contains detailed instructions on how to run the program on test cases.


PIXEL TRACES

Some test cases also come with a pixel trace.  These are debugging aids.  They
provide a detailed trace through the computations and intermediates that are
encountered while computing the color of one particular pixel.  It is highly
recommended that you add print statements to your code to print out information
equivalent to what is in the debug trace and compare the results.  (The
Pixel_Print function in misc.h should help you do this.)  They should match.

These files are named as ??-t.txt.  The first line of these files will be
something like "debug pixel: -x 350 -y 240".  In this case, to compare with the
trace you should add "-x 350 -y 240" to your commandline when you run your
program.  The 350,240 is the pixel that is being traced.  The pixel is colored
green in the output png so you can see visually what pixel is being traced.

These traces serve a number of useful roles.  (1) It helps track down the source
of many bugs.  For example, it can tell you whether your intersection routine is
wrong or whether you were correctly intersecting against the wrong ray.  (2)
They let you verify that various routines are working correctly.  For example,
it can tell you that you are correctly computing your rays or that your
intersection routines are correct.  (3) If you are having difficulties passing a
test case that has a pixel trace, matching the pixel trace (though tedious) will
often be the easiest path to a fix.  In fact, it might be easier to find a later
test case that does have a pixel trace.  
