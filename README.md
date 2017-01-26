# lgmath
lgmath is a C++ library for handling geometry in state estimation problems in robotics. It can be
used to manipulate the three-dimensional rotations and transformations of points and coordinate
frames. This library uses concepts and mathematics found in Tim Barfoot's upcoming
book [State Estimation for Robotics](asrl.utias.utoronto.ca/~tdb/bib/barfoot_ser15.pdf). It is used
in his robotics research at the Autonomous Space Robotics Lab; most notably in the STEAM Engine, a
library for Simultaneous Trajectory Estimation and Mapping.

# Getting Started
You will need:
- A compiler with C++11 support
- Eigen, at least 3.2.5
- Either the CMake or catkin build systems

Check out the lgmath repository. ```bash git checkout --recursive
https://github.com/utiasASRL/lgmath.git ```

## Dependencies

### Eigen
In a folder for 3rd party dependencies, ```bash wget
http://bitbucket.org/eigen/eigen/get/3.2.5.tar.gz tar zxvf 3.2.5.tar.gz cd
eigen-eigen-bdd17ee3b1b3/ mkdir build && cd build cmake .. sudo make install ```

### Build
In your development folder ```bash mkdir lgmath-ws && cd $_ git clone
https://github.com/utiasASRL/lgmath.git cd lgmath && git submodule update --init --remote ```

Using [catkin](https://github.com/ros/catkin)
and [catkin tools](https://github.com/catkin/catkin_tools) (recommended) ```bash cd deps/catkin &&
catkin build cd ../.. && catkin build ```

Using CMake (manual) ```bash cd .. && mkdir -p build/catkin_optional && cd $_ cmake
../../lgmath/deps/catkin/catkin_optional && make cd ../.. && mkdir -p build/catch && cd $_ cmake
../../lgmath/deps/catkin/catch && make cd ../.. && mkdir -p build/lgmath && cd $_ cmake ../../lgmath
&& make -j4 ```

## Install (optional)

Since the catkin build produces a catkin workspace you can overlay, and the CMake build exports
packageConfig.cmake files, it is unnecessary to install lgmath except in production environments. If
you are really sure you need to install, you can use the following procedure.

Using catkin tools (recommended) ```bash cd lgmath catkin profile add --copy-active install catkin
profile set install catkin config --install catkin build ```

Using CMake (manual) ```bash cd build/lgmath sudo make install ```

## Uninstall (Optional)

If you have installed, and would like to uninstall,

Using catkin tools (recommended) ```bash cd lgmath && catkin clean -i ```

Using CMake (manual) ```bash cd build/lgmath && sudo make uninstall ```
