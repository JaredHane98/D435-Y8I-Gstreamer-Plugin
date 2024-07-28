# D435-Y8I-Gstreamer-Plugin


The following repository provides support for Y8I format for GStreamer. The library does not attempt to provide official support for Y8I. The plugin overrides the format prior to PLAYING and deinterlaces Y8I into GRAY8.


## How to build

- clone the repository
- cd into the plugin directory
- run cmake ./ -DLIBRARY_DEST:PATH= **WHERE GSTREAMER LIBRARIES ARE LOCATED**
- run sudo make install

## How to use

- With a D435 camera plugged in find the correct file handle
- EG: v4l2-ctl -d /dev/video2 --list-formats-ext
      Index       : 0
      Type        : Video Capture
      Pixel Format: 'GREY'
      Name        : 8-bit Greyscale

- Then create a pipeline using the provided parameters in gst-launch-1.0

- gst-launch-1.0 v4l2src device=/dev/video2 ! video/x-raw, format=GRAY8, width=1280, height=720, interlace-mode=progressive, framerate=30/1 ! rsdeinterlace ! videoconvert ! video/x-raw, format=I420 ! autovideosink

Future plans
- [ ] Add support for disabling IR


Tested on Ubuntu 18.04 with GStreamer 1.14.5 and Nvidia Jetson with GStreamer 1.14.5




