# Mastik_CacheNoise

## About

The cache is a vital shared resource between programs. Many frameworks simulate cache designs, such as Gem5(https://www.gem5.org/) and CacheFX(https://cs.adelaide.edu.au/~yval/pdfs/GenkinKLTUY22.pdf).

However, these frameworks only simulate cache changes caused by specific programs. This simulation is not realistic enough because many processes running concurrently with these specific programs result in extra memory accesses. We call these memory accesses cache noise.

We exploited Mastik, a micro-architectural side-channel toolkit, to collect cache noise on Linux machines in this project. 

We add two programs, demo/L3-CollectCacheNoise.c and L3-ComputeNoiseTime.c. And we run these programs without any other programs running except for system programs.

#### L3-CollectCacheNoise.c
This file collects cache noise from Linux systems. It probes every 64 sets continuously and repeatedly (with gaps of 64 sets to avoid data prefetching), and uses set i to represent the actual set i\*64. 
It probes each set for SAMPLES\*10(10010\*10) times and prints the results - (set, miss number of this set) into text and CSV files. CSV files are used for more visual observation, and text files are used for easier use by further exploitation.
 
#### L3-ComputeNoiseTime.c
Adding an array to record each probe's time (cycles) would take up large memory space, and accessing these memory spaces would cause a lot of extra noise. We found that the time between each probe was easy to measure and calculate. This program is used to calculate the noise time between each probe situation.

Noise data is stored in /NoiseData.

The following are Mastik's readme.
*********************************

# Mastik: A Micro-Architectural Side-Channel Toolkit

## About

Micro-architectural side-channel attacks exploit contention on internal components
of the processor to leak information between processes. 
While in theory such attacks are straightforward, 
practical implementations tend to be finicky and 
require significant understanding of poorly documented processor features 
and other domain-specific arcane knowledge. 
Consequently, there is a barrier to entry into work on 
micro-architectural side-channel attacks, 
which hinders the development of the area and the analysis of the 
resilience of existing software against such attacks.

This repository contains Mastik, a toolkit for experimenting with micro-architectural side-channel attacks. Mastik aims to provide implementations of published attack and analysis techniques. 
Currently, Mastik supports six side-channel attack techniques on the Intel x86-64 architecture:

- [Prime+Probe](https://www.cs.tau.ac.il/~tromer/papers/cache-joc-official.pdf) on several caches, including
    - [L1 data cache](https://www.cs.tau.ac.il/~tromer/papers/cache-joc-official.pdf)
    - [L1 instruction](https://www.iacr.org/archive/ches2010/62250105/62250105.pdf)
    - L2 (only with huge pages)
    - [Last Level Cache+Probe](http://palms.ee.princeton.edu/system/files/SP_vfinal.pdf)
- [Prime+Abort](https://www.usenix.org/system/files/conference/usenixsecurity17/sec17-disselkoen.pdf)
- [Flush+Reload](https://www.usenix.org/system/files/conference/usenixsecurity14/sec14-paper-yarom.pdf)
- [Flush+Flush](https://arxiv.org/pdf/1511.04594.pdf)
- Two variants of [CacheBleed](https://trustworthy.systems/publications/nicta_full_text/9535.pdf)
- [Performance-degradation attack](https://trustworthy.systems/publications/nicta_full_text/9427.pdf)



## Installation

Mastik follows the de-facto standard Linux installation process.
If the system has all of the required software and 
you want the default configuration, use:  
`$ ./configure && make && sudo make install`  

See below for more information.

#### Required packages
For the core operation, Mastik does not require any packages.
However, the ability to resolve symbols in binaries is extremely useful,
and requires the build packages `binutils` and `libdwarf` on Linux.  
On Mac OS X there is no support for debugging symbols yet.

The Mastik GUI interface `doubloon`
requires Python 3 with the following packages:
`wx`, `numpy`, `paramiko`, `matplotlib`, and `tqdm`.

Installation depends on the flavour of the Operating System.

**Fedora**:
`$ sudo dnf install binutils-devel libdwarf-devel`  

**Ubuntu**:
`$ sudo apt-get install binutils-dev libdwarf-dev libelf-dev`

**CentOS**:  
Enable the PowerTools repository:  
`$ sudo dnf config-manager --set-enabled PowerTools`  
Then install the packages:  
`$ sudo dnf install libdwarf binutils-devel elfutils-libelf-devel libdwarf-devel python3`


#### Configuring the build directory

To set up the build environment, use the `configure` script.
To use the default options, use:

`$ ./configure`  

The script accepts several flags to modify its behaviour.
Some of these are listed below.
Use `./configure --help` for a complete list.

|Flag|Description|
|----|-----------|
| `--help`          | Help message                         |
| `--prefix=PREFIX` | Install files in PREFIX [/usr/local] |
| `--disable-symbols` | Disables handling of symbol tables in binaries.  Also, removes requirement for `binutils` and `libdwarf` |
| `--disable-debug-symbols` | Disable handling of debug symbols in binaries.  Removes requirement for `libdwarf`                |
| `--disable-doubloon`      | Do not install `doubloon`. Removes requirement for Python3 |


#### Building Mastik

After configuring the build environment, build Mastik using:

`$ make`

#### Installing Mastik
To install Mastik, use:

`$ make install`

To change the install directory use:

`$ make prefix=DIR install`

## Usage

For example of usage look at the demo folder.



## 


## Thanks

Mastik is supported by a generous gift from Intel.

