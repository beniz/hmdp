## hmdp
hmdp is a C++ library and tools for solving Markov Decision Processes (MDPs) with hybrid discrete and/or continuous state-spaces. It includes various algorithms for computing optimal solutions. It is open-source software distributed under the [Apache License v2.0](http://www.apache.org/licenses/LICENSE-2.0).

Main functionalities:
* library for building higher-level solutions that require the solving of MDPs, possibly with multi-dimensional continuous state-space;
* embeds a solver built with the library, and with many options in the command line;
* includes an optional customized PPDDL model library that allows to describe complex worlds in a simple manner, and solve them as MDPs with hybrid discrete / continuous state-space.
* includes the optional ability to handle piecewise-linear functions with lpsolve library.
* easy to enhance with new algorithms.

Dependencies:
* [gflags](https://code.google.com/p/gflags/) for command line parsing;
* [lpsolve](http://sourceforge.net/projects/lpsolve/) for handling piecewise-linear functions (optional).

### Authors
hmdp is designed and implemented by Emmanuel Benazera.

### Build
Beware of dependencies, typically on Debian/Ubuntu Linux, do:
```
sudo apt-get install libgoogle-glog-dev libgflags-dev libeigen3-dev liblpsolve55-dev autotools-dev autoconf
```

For compiling with basic options enabled:
```
./autogen.sh
./configure
make
```

More options are available with 
```
./configure --help
```

### Run examples

The simplest example is the Mars Rover problem from [J. Bresina, R. Dearden, N. Meuleau, S. Ramakrishnan, D. Smith and
R. Washington, 2002, "Planning under continuous time and resource
uncertainty: A challenge for AI", UAI-2002](http://arxiv.org/abs/1301.0559) and [Z. Feng, R. Dearden, N. Meuleau, R. Washington, 2004, "Dynamic Programming for Structured Continuous Markov Decision Problems", UAI-2004](http://arxiv.org/abs/1207.4115).

Run it:
```
cd src/apps
./hmdp --ppddl_file ../hmdpsim/examples/uai.ppddl --point_based_output_step 20,0.5 --prec 1e-3 --vf_output_formats=box,dat,mat --truncate_negative_ct_outcomes
```

Ouput should look like:
```
[Info]: ppddl_parser::domain: actions successfully instantiated
Total time         #discs    #vf_backups         lbtime         mbtime        ntiles
0.02900             10             11        0.02700        0.00000       524
backup time: 0.02000
total number of discrete states (dfs): 10
written file ../hmdpsim/examples/uai.ppddl_state_0.box
written file ../hmdpsim/examples/uai.ppddl_state_0.dat
written file ../hmdpsim/examples/uai.ppddl_state_0.mat

```

To plot the resulting value function:

* with octave
```
data=load(../hmdpsim/examples/uai.ppddl_state_0.mat)
mesh(data)
```
![alt text](/src/hmdpsim/examples/uai_mat.png "title")

* with gnuplot
```
splot '../hmdpsim/examples/uai.ppddl_state_0.dat' w lp
```
![alt text](/src/hmdpsim/examples/uai_dat.png "title")

or to see the 2D partitioning:
```
plot '../hmdpsim/examples/uai.ppddl_state_0.box' w boxxyerror
```
![alt text](/src/hmdpsim/examples/uai_box.png "title")
