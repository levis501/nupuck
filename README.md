nupuck
======

Nupic Guided Shufflepuck: "nupuck"

This project was developed for the Numenta Hackathon, Spring 2014


nupuck initially based on TuxPuck at https://code.google.com/p/tuxpuck/

After hacking the port to build on MacOS Mavericks and homebrew, input for
nupic was generated as a field of view from the point of the human player.
(see docs/field\_of\_view\_two\_eyes.jpg).  When the puck appears to the left,
the human player's bat is moved to the left.  Similarly to the right when
the puck appears to the right.

Generated data is written to stdout, then filtered using command line tools
(awk, etc.) to build eye.csv.  Ultimately, this data was used to create two
models using nupic swarms.  One model to predict the future position of the
left eye, and one for the right.  Models were built using swarms predicting
40, 70, and 100 time steps out, and are located in this repository at 
models/l47x and models/r47x.

During gameplay, a local socket server is run from model\_pipe.py which
receives input from the a client in the tuxpuck executable, and returns the
predictions from each of the two models created above.

The predictions are ultimately used to move of the human player's bat
in the instances when the lower level behavior doesn't move the bat.  Other
combinations of predictive and primative behavior are possible.
