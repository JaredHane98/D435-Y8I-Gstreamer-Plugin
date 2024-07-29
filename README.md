# D435-Y8I-Gstreamer-Plugin


The following repository provides support for Y8I format for GStreamer. The library does not attempt to provide official support for Y8I. The plugin overrides gstv4l2src format.

## How to build

- cmake ./ -DLIBRARY_DEST:PATH=**GSTREAMER LIBRARY DESTINATION**
- sudo make install

## How to use

- launch the plugin with the correct device
- gst-launch-1.0 v4l2src device=/dev/video2 ! video/x-raw, format=GRAY8, width=1280, height=720, interlace-mode=progressive, framerate=30/1 ! rsdeinterlace ! videoconvert ! video/x-raw, format=I420 ! autovideosink

Future plans
- [ ] Add support for disabling IR


Tested on Ubuntu 18.04 with GStreamer 1.14.5 and Nvidia Jetson with GStreamer 1.14.5




