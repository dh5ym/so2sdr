---
layout: page
title: "Linux"
category: install
date: 2015-07-31 21:22:13
order: 1
---

You will need the following development libraries installed: Qt4, FFTW, Hamlib, and PortAudio. Other various development packages include g++, Git, and pkg-config. 


1. Clone the git repository to your local machine:
    
        git clone git://github.com/n4ogw/so2sdr.git

2. By default, so2sdr will be installed in /usr/local/bin, and associated
  data files will be placed in /usr/local/share/so2sdr. If you want to
  change the location of the program, edit SO2SDR_INSTALL_DIR in common.pri

3. In the directory so2sdr, 

    ```
    qmake
    make
    ```

    ``make -j 2``  will use 2 cores and go faster.
    Subdirectory Makefiles are created from the top level Makefile.

5. (as superuser) 
         
        make install
 
6. Test and contribute!

