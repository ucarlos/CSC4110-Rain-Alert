# CSC 4110 Project &#x2013; Rain Detector using a Raspberry Pi &#x2013; Post Project Branch

This is the Post Project Branch of the Rain Alert Application. This repository contains all the files needed to use a Raspberry Pi as a Rain Detector, which reports changes in local rain levels to a email server, as well as other options. This branch is to complete all the specified functionality that I could not implement in time. If you are looking for the project files that I used for my project, [Click here.](https://github.com/ucarlos/CSC4110_Project/tree/master)


## Future Additions

-   Functional Text-based UI
-   Make Sensor tracking work in the background to enable other components of the application to be used.
-   Improved Function Options
-   Debug Mode


## Necessary Hardware

This project required the following hardware in order to work:

-   Rain Sensor
-   Float Sensor
-   Rain Gauge (You can use a Graduated Cylinder in lieu of one)
-   Raspberry Pi Zero W


## Necessary Software


### tldr

If on Debian/Ubuntu/Raspbian, execute this command:

```bash
sudo apt-get install git postgresql libpq-dev libpqxx-dev cmake 
sudo apt-get install libcurl4-openssl-dev ncurses dialog 
sudo apt-get install libssl-dev libboost-dev
```


### Long Explanation

Since this is a CLion Project, you can try installing it on your system and opening it, making sure to have the following components installed on your system. CLion comes with a version of CMake, but you need to install the following regardless:

1.  Git

2.  PostgreSQL with libpq and libpqxx >= 6

    You can install all three on an Debian/Ubuntu/Raspbian using the following command:
    
    ```bash
    sudo apt-get install postgresql libpq-dev libpqxx-dev
    ```
    
    Please be aware that libpqxx-dev included with your package manager may be an older version, so you may have to [compile from source](https://github.com/jtv/libpqxx) to get the latest version.

3.  CMake >= 3.15 (If CLion cannot be installed on your system)

    If on Debian/Ubuntu/Raspbian, use the following command:
    
    ```bash
    sudo apt-get install cmake
    ```
    
    On other Linux or UNIX Systems, you may need to use your package manager or [compile from source.](https://cmake.org/download/) You may also have to compile from source if your CMake version is less than 3.15.

4.  libcurl-dev >= 7.50.0 with SSL.

    To install libcurl-dev on Debian/Ubuntu/Raspbian, do
    
    ```bash
    sudo apt-get install libcurl4-openssl-dev libssl-dev
    ```
    
    Or use whatever package manager you have to install it.
    
    In order to know if SSL is enabled within curl, do the following command:
    
    ```bash
    curl-config --feature
    ```
    
    You should see a list that should contain "SSL". If not, install a more recent version of libcurl through your package manager or [compile from source.](https://curl.haxx.se/download.html)

5.  NCurses and Dialog Libraries

    In order to display the text-based UI, NCurses and dialog must be installed on your system. To do so, do
    
    ```bash
    sudo apt-get install ncurses dialog
    ```

6.  Boost Library

    XML Configuration uses XML parsing and property tree libraries from Boost. You can install boost by doing
    
    ```bash
    sudo apt-get install libboost-dev
    ```


## Explanation

A Video explaining the project can be found [here.](https://youtu.be/HR-591p1Yrk)


## How to compile

First, clone the repository.

```bash
git clone https://github.com/ucarlos/CSC4110_Project.git
```

If you can get CLion running on your machine, simply execute the program by pressing the run button on the top right (SHIFT-F10). If you can't get CLion (Or decide not to), run the following command in the project directory:

```bash
cmake .
make
```


## Example Usage

To run the program, do

```sh
./CSC4110_Project
```

This executable may be found in the project directory, or in one of the cmake builds.


## References used for the project

-   [Pthread Tutorial](https://computing.llnl.gov/tutorials/pthreads/)
-   [C++ Reference](https://en.cppreference.com)
-   [C++ Chrono Library](https://en.cppreference.com/w/cpp/chrono)
-   [C++ std::thread](https://en.cppreference.com/w/cpp/thread/thread)
-   [POSIX Thread on C++](https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html)
-   [libcurl Documentation](https://curl.haxx.se/libcurl/c/)
-   [libpqxx Documentation](https://libpqxx.readthedocs.io/en/latest/)
-   [Boost XML Parser and Property Trees](https://www.boost.org/doc/libs/1_44_0/doc/html/boost_propertytree/parsers.html)
-   <https://github.com/leemunroe/responsive-html-email-template>