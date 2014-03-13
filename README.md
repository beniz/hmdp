## hmdp
hmdp is a library and tools for solving Markov Decision Processes (MDPs) with hybrid discrete and/or continuous state-spaces. It includes various algorithms for computing optimal solutions. It is open-source software distributed under the [Apache License v2.0](http://www.apache.org/licenses/LICENSE-2.0).

### Authors
hmdp is designed and implemented by Emmanuel Benazera.

### Build
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
