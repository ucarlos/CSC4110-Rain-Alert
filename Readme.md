# CSC 4110 Project &#x2013; Rain Detector using a Raspberry Pi

This repository contains all the files needed to use a Raspberry Pi as a Rain Detector, which reports changes in local rain levels to a email server, as well as other options. This project was led by Ulysses Carlos (myself) and Jared Goldman.


## Project Timeline


## Necessary Hardware

This project required the following hardware in order to work:

-   Rain Sensor
-   Float Sensor
-   Graduated Cylinder


## Necessary Software

Since this is a CLion Project, you can try installing it on your system and opening it, making sure to have the following components installed on your system. CLion comes with a version of CMake, but you may need to install the following:

1.  PostgreSQL with libpq and libpqxx >= 6

    You can install all three on an Debian/Ubuntu/Raspbian using the following command:
    
    ```bash
    sudo apt-get install postgresql libpq-dev libpqxx-dev
    ```
    
    If on macOS, you can install the packages using homebrew:
    
    ```bash
    brew install postgresql
    brew install libpq
    brew install libpqxx
    ```

2.  Cmake (If CLion cannot be installed on your system)

    If on Debian/Ubuntu/Raspbian, use the following command:
    
    ```bash
    sudo apt-get install cmake
    ```
    
    On other Linux or UNIX Systems, you may need to use your package manager or [compile from source.](https://cmake.org/download/)


## Explanation


## How to compile

If you can get CLion running on your machine, simply execute the program by pressing the run button on the top right (SHIFT-F10). If you can't get CLion (Or decide not to), run the following command in the project directory:

```sh
cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" .
make
./CSC4110_Project
```


## Example Usage

```sh

```


## References
