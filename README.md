Turmite
=======

A simple turmite implementation. In computer science, a turmite is a Turing machine which has an orientation as well as a current state and a "tape" that consists of an infinite two-dimensional grid of cells. The terms ant and vant are also used. Langton's ant is a well-known type of turmite defined on the cells of a square grid.

I've worked under Linux using OpenGL and two library for PNG saving and a library and to read an external XML for the turmite configuration.
I've used Freeglut and Glew to quickly setup the window.

Once run, you can quit pressing 'q', or save the current world in a PNG with 's', or add 1,10,100 or 1000 steps with keys '1','2','3','4'.
I wrote a couple of comments in the XML files to configure the turmite and the world size and the boundary condition (fixed, toroidal, infinite). The system support any kind of movement: L (90° left), R (90° right), N (no turn) and U (180° U-turn).

The system support multiple states and colours. The syntax to run the application is:

./Turmite &lt;config xml file&gt; &lt;number of steps&gt;

The configuration file LangtonsAnt.xml is exactly Langton's Ant turmite, and Turmite.xml is another 2-state 2-colour configuration.
