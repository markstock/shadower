# shadower
Add soft shadows to a png image

### Using shadower

    make
    ./shadower -h
    ./shadower in.png -sc=0.6 -sw=2.0 -ss=0.06 -nb=20 -o=out.png

The last command turns the first image into the second image.

<img src="media/in.png" width="320" height="320" /> <img src="media/out.png" width="320" height="320" />

The arguments are:

* "shade coefficient" (-sc) controls the darkness, with 1.0 being black and 0.0 having no effect
* "shadow width" (-sw) is how spread or diffuse the shadow becomes from top to bottom, where 0.0 gives a sharp shadow, 1.0 gives a reasonably diffuse one, and 2.0 gives a very diffuse one
* "shadow shift" (-ss) controls the "depth" of the image, or the angle at which the light comes from, with 0.0 giving no shift (the light is directly behind the viewer), 0.1 means the light is angled such that the longest shadow is about 10% of the height of the image
* "invert" (-i) will change which end of the brightness scale is "on top"; by default, light colors will appear to be on top, but with "-i" on the command line, darker colors will be on top.

### Thanks

Many thanks to Lode Vandevenne for the [LodePNG](http://lodev.org/lodepng/) code!
