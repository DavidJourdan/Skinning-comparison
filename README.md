# Skinning Comparison

A simple, multi-view application to compare 3 skeletal skinning methods in a single screen. The methods used are Linear Blend Skinning, Dual Quaternion Skinning and a more recent one called Skinning with Optimized Centers of Rotation.

## Installation

### Prerequisites

This project was built with Qt 5 which is available in all platforms. The easiest way to install it is probably to download [Qt Creator](https://www.qt.io/download), but if you don't need all of its features, you can install qt5 and qmake separately in most distributions.

In Ubuntu/Debian:
```
apt-get install qt5-default qt5-qmake
```
In Fedora:
```
dnf install qt-devel
```
In OSX, something like `brew install qt` would probably work.

### Other dependencies

You will also need OpenGL >= 4.1 and a modern, C++14 compatible compiler.

To install that on Ubuntu/Debian:
```
apt-get install build-essential libgl1-mesa-dev
```
On Fedora:
```
dnf install make gcc-c++
```

### Compiling

In Unix-based systems, `qmake ; make` should work, on Windows use Qt Creator.

## Usage

```
./skinning -i /path/to/mesh.obj -s /path/to/file.skeleton -w /path/to/file.weights
```
For example, if you want to load the dino file on the root project folder
```
./skinning -i model/dino/cubic_dino.obj -s model/dino/skel1.skeleton -w model/dino/skel1.weights
```
Note that computing the centers of rotation takes some time (a few minutes on a decent machine), but they are saved in a .cor file after computation so you don't need to wait every time.

## Authors

This was a group project we did in our final year of engineering school, the 3 of us contributed equally:

* **Ronan Desplanques**

* **Timothée Tallon**

* **David Jourdan**

## License

This project is licensed under the GPL License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

Thanks to our advisor [Jean-Marc Thiery](https://perso.telecom-paristech.fr/jthiery/) for his help and advice
