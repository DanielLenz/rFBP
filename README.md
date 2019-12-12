| **Authors**  | **Project** | **Documentation** | **Build Status** | **Code Quality** | **Coverage** |
|:------------:|:-----------:|:-----------------:|:----------------:|:----------------:|:------------:|
| [**N. Curti**](https://github.com/Nico-Curti) <br/> [**D. Dall'Olio**](https://github.com/DanieleDallOlio)   |  **rFBP**  | [![docs](https://img.shields.io/badge/doc-latest-blue.svg?style=plastic)](https://nico-curti.github.io/rFBP/) | **Linux/MacOS** : [![Travis](https://travis-ci.com/Nico-Curti/rFBP.svg?token=7QqsqaQiuDHSyGDT3xek&branch=master)](https://travis-ci.com/Nico-Curti/rFBP) <br/> **Windows** : [![appveyor](https://ci.appveyor.com/api/projects/status/obuq56lhyd90pmup?svg=true)](https://ci.appveyor.com/project/Nico-Curti/rfbp) | **Codacy** : [![Codacy](https://api.codacy.com/project/badge/Grade/a6fdac990b6f4141a5bd9e8171ddaf53)](https://www.codacy.com/manual/Nico-Curti/rFBP?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Nico-Curti/rFBP&amp;utm_campaign=Badge_Grade) <br/> **Codebeat** : [![Codebeat](https://codebeat.co/badges/cc761a7c-79fa-4a66-984f-bef6fd145d34)](https://codebeat.co/projects/github-com-nico-curti-rfbp-master) | [![codecov](https://codecov.io/gh/Nico-Curti/rFBP/branch/master/graph/badge.svg)](https://codecov.io/gh/Nico-Curti/rFBP) |

[![GitHub pull-requests](https://img.shields.io/github/issues-pr/Nico-Curti/rFBP.svg?style=plastic)](https://github.com/Nico-Curti/rFBP/pulls)
[![GitHub issues](https://img.shields.io/github/issues/Nico-Curti/rFBP.svg?style=plastic)](https://github.com/Nico-Curti/rFBP/issues)

[![GitHub stars](https://img.shields.io/github/stars/Nico-Curti/rFBP.svg?label=Stars&style=social)](https://github.com/Nico-Curti/rFBP/stargazers)
[![GitHub watchers](https://img.shields.io/github/watchers/Nico-Curti/rFBP.svg?label=Watch&style=social)](https://github.com/Nico-Curti/rFBP/watchers)

<a href="https://github.com/UniboDIFABiophysics">
<div class="image">
<img src="https://cdn.rawgit.com/physycom/templates/697b327d/logo_unibo.png" width="90" height="90">
</div>
</a>

# Replicated Focusing Belief Propagation algorithm

Optimization and extension of the [**Replicated Focusing Belief Propagation**](https://github.com/carlobaldassi/BinaryCommitteeMachineFBP.jl) Julia package.

* [Overview](#overview)
* [Theory](#theory)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
* [Efficiency](#efficiency)
* [Usage](#usage)
* [Contribution](#contribution)
* [References](#references)
* [Authors](#authors)
* [License](#license)
* [Acknowledgments](#acknowledgments)
* [Citation](#citation)

## Overview

The learning problem could be faced through statistical mechanic models joined with the so-called Large Deviation Theory.
In general, the learning problem can be split in two sub-parts: the classification problem and the generalization one.
The first aims to completely store a pattern sample, i.e a prior known ensemble of input-output associations (*perfect learning*).
The second one corresponds to compute a discriminant function based on a set of features of the input which guarantees a unique association of a pattern.

From a statistical point-of-view many Neural Network models have been proposed and the most promising seems to be spin-glass models based.
Starting from a balanced distribution of the system, generally based on Boltzmann distribution, and under proper conditions, we can prove that the classification problem becomes a NP-complete computational problem.
A wide range of heuristic solutions to that type of problems were proposed.

In this project we show one of these algorithms developed by Zecchina et al. [[BaldassiE7655](https://www.pnas.org/content/113/48/E7655)] and called *Replicated Focusing Belief Propagation* (`rFBP`).
The `rFBP` algorithm is a learning algorithm developed to justify the learning process of a binary neural network framework.
The model is based on a spin-glass distribution of neurons put on a fully connected neural network architecture.
In this way each neuron is identified by a spin and so only binary weights (-1 and 1) can be assumed by each entry.
The learning rule which controls the weight updates is given by the Belief Propagation method.

A first implementation of the algorithm was proposed in the original paper [[BaldassiE7655](https://www.pnas.org/content/113/48/E7655)] jointly with an open-source Github repository.
The original version of the code was written in `Julia` language and despite it is a quite efficient implementation the `Julia` programming language stays on difficult and far from many users.
To broaden the scope and use of the method, a `C++` implementation was developed with a joint `Cython` wrap for `Python` users.
The `C++` language guarantees better computational performances against the `Julia` implementation and the `Python` version enhance its usability.
This implementation is optimized for parallel computing and is endowed with a custom `C++` library called [`Scorer`](https://github.com/Nico-Curti/scorer) for further details), which is able to compute a large number of statistical measurements based on a hierarchical graph scheme.
With this optimized implementation we try to encourage researchers to approach these alternative algorithms and to use them more frequently on real context.

As the `Julia` implementation also the `C++` one provides the entire `rFBP` framework in a single library callable via a command line interface.
The library widely uses template syntaxes to perform dynamic specialization of the methods between two magnetization versions of the algorithm.
The main object categories needed by the algorithm are wrapped in handy `C++` objects easy to use also from the `Python` interface.
A further optimization is given by the reduction of the number of available functions: in the original implementation a large amount of small functions are used to perform a single complex computation step, enlarging the amount of call stack; in the `C++` implementation the main functions are re-written with the minimizing the call stack to ease the vectorization of the code.

## Theory

**TODO**

## Prerequisites

The `rFBP` project is written in `C++` using a large amount of c++17 features.
To enlarge the usability of our package we provide also a retro-compatibility of all the c++17 modules reaching an usability (tested) of our code from gcc 4.8.5+.
The package installation can be performed via [`CMake`](https://github.com/Nico-Curti/rFBP/blob/master/CMakeLists.txt) or [`Makefile`](https://github.com/Nico-Curti/rFBP/blob/master/Makefile).
If you are using the `CMake` (recommended) installer the maximum version of C++ standard is automatic detected.

You can use also the `rFBP` package in `Python` using the `Cython` wrap provided inside this project.
The `Python` wrap guarantees also a good integration with the other common Machine Learning tools provided by `scikit-learn` `Python` package; in this way we can use the `rFBP` algorithm as equivalent alternative also in other pipelines.
Like other Machine Learning algorithm also the `rFBP` one depends on many parameters, i.e its hyper-parameters, which has to be tuned according to the given problem.
The `Python` wrap of the library was written according to `scikit-optimize` `Python` package to allow an easy hyper-parameters optimization using the already implemented classical methods.


## Installation

Follow the instruction about your needs.

A complete list of instructions "for beginners" is also provided for both [cpp](https://github.com/Nico-Curti/rFBP/blob/master/docs/cpp_install.md) and [python](https://github.com/Nico-Curti/rFBP/blob/master/docs/python_install.md) versions.

### CMake C++ installation

We recommend the use of `CMake` for the installation since it is the most automated way to reach your needs.
First of all make sure you have a sufficient version of `CMake` installed (3.9 minimum version required).
If you are working on a machine without root privileges and you need to upgrade your `CMake` version a valid solution to overcome your problems is provided [here](https://github.com/Nico-Curti/Shut).

With a valid `CMake` version installed first of all clone the project as:

```bash
git clone https://github.com/Nico-Curti/rFBP
cd rFBP
```

The you can build the `rFBP` package with

```bash
mkdir -p build
cd build && cmake .. && cmake --build . --target install
```

or more easily

```bash
./build.sh
```

if you are working on a Windows machine the right script to call is the [`build.ps1`](https://Nico-Curti/rFBP/blob/master/build.ps1).

**NOTE 1:** if you want enable the OpenMP support (*4.5 version is required*) compile the library with `-DOMP=ON`.

**NOTE 2:** if you use MagT configuration, please download the `atanherf coefficients` file before running any executable. You can find a downloader script inside the [scripts](https://github.com/Nico-Curti/rFBP/tree/master/scripts) folder. Enter in that folder and just run `python dowload_atanherf.py`.

### Make C++ installation

The `Make` installation requires more attention!
First of all the `Make` installation assumes that you compiler is able to support the c++17 standard: if it is not your case you have to change the `STD` variable into the `Makefile` script.

Then if you call just:

```bash
make
```

you can view the complete list of available examples.
With

```bash
make main
```

you can compile the main example and the `C++` library.

### Python installation

Python version supported : ![Python version](https://img.shields.io/badge/python-3.5|3.6|3.7-blue.svg)

The `Python` installation can be performed with or without the `C++` installation.
The `Python` installation is always executed using [`setup.py`](https://github.com/Nico-Curti/blob/master/setup.py) script.

If you have already build the `rFBP` `C++` library the installation is performed faster and the `Cython` wrap directly links to the last library installed.
Otherwise the full list of dependencies is build.

In both cases the installation steps are

```bash
python -m pip install -r ./requirements.txt
```

to install the prerequisites and then

```bash
python setup.py install
```

or for installing in development mode:

```bash
python setup.py develop --user
```

## Efficiency

![Comparison of time performances between the original `Julia` implementation and our `Cython` one of the `rFBP` algorithm varying the input dimension sizes (number of samples, `M`, and number of variables, `N`). For each input configuration 100 runs of both algorithm were performed and the results were normalized by the `Julia` implementation. In these cases we fixed the magnetization to **MagP64**.](./img/rfbp_magp_timing.svg)

![Comparison of time performances between the original `Julia` implementation and our `Cython` one of the `rFBP` algorithm varying the input dimension sizes (number of samples, `M`, and number of variables, `N`). For each input configuration 100 runs of both algorithm were performed and the results were normalized by the `Julia` implementation. In these cases we fixed the magnetization to **MagT64**.](./img/rfbp_magt_timing.svg)

We firstly test the computational efficiency of our implementation against the original `Julia` one.
The tests were performed comparing our `Cython` version of the code (and thus with a slight overhead given by the `Python` interpreter) and the `Julia` implementation as reference.
Varying the dimension sizes (number of samples, `M`, and number of variables, `N`) we tested the time efficiency over 100 runs of both the algorithms.
We divided our simulation according to the two possible type of magnetizations (`MagP64` and `MagT64` as described by the original implementation available [here](https://github.com/carlobaldassi/BinaryCommitteeMachineFBP.jl)) and the obtained results are shown in Fig. [[1](./img/rgbp_magp_timing.svg), [2](./img/rgbp_magt_timing.svg)], respectively.

As can be seen by the two simulations our implementation always overcome the time performances of the original one, taken as reference in the plot.
However, we can not guarantee a perfect parallel execution of our version: also with multi-threading support the scalability of our implementation does not follow a linear trend with the number of available cores.
In our simulation, in fact, we used 32 cores against the single thread execution of the `Julia` implementation but we gained only a 4x and 2x of speedup for `MagT64` and `MagP64`, respectively.
The network training is a sequential process by definition and thus it is hard to obtain a relevant speedup using a parallel implementation.
In this case it is probably jointed to a not perfect parallelization strategy chosen which bring to a not efficient scalability of our algorithm version.
However, the improvements performed to the code allow us to use this algorithm with bigger dataset sizes.

## Usage

### C++ Version

**TODO**

### Python Version

The `rfbp` object is totally equivalent to a `scikit-learn` classifier and thus it provides the member functions `fit` (to train your model) and `predict` (to test a trained model on new samples).

First of all you need to import the `rFBP` modules.

```python
from ReplicatedFocusingBeliefPropagation import Mag
from ReplicatedFocusingBeliefPropagation import Pattern
from ReplicatedFocusingBeliefPropagation import ReplicatedFocusingBeliefPropagation as rFBP
```

If your want to run your script with multiple cores you can simply import also

```python
from ReplicatedFocusingBeliefPropagation import NTH
```

which is set to the maximum number of core in your computer.

We encourage to use the `Scorer` package for the evaluation of performances but all the simulation can be performed also without it

```python
try:

  from scorer import Scorer

  USE_SCORER = True

except ImportError:

  USE_SCORER = False
```

You can start to try the package functionality using a random pattern using the `Pattern` object provided by the module

```python
N, M = (20, 101) # M must be odd
pattern = Pattern(N, M)
```

In the [example](https://github.com/Nico-Curti/rFBP/blob/master/ReplicatedFocusingBeliefPropagation/example/) folder you can find a training/test example using a pattern imported from file (a more realistic example).

The next step is the creation of the `Replicated Focusing Belief Propagation` model.

```python
rfbp = rFBP(mag=Mag.magT,
            hidden=3,
            max_iter=1000,
            seed=135,
            damping=0.5,
            accuracy=['accurate','exact'],
            randfact=0.1,
            epsil=0.5,
            protocol='pseudo_reinforcement',
            size=101,
            nth=NTH)
```

Now you can fit your model and predict:

```python
rfbp.fit(pattern)
predicted_labels = rfbp.predict(pattern)
```

which is clearly an overfitting! But it works as example :blush: and show you that also the `predict` function requires a `Pattern` object as input.


## Contribution

Any contribution is more than welcome :heart:. Just fill an [issue](https://github.com/Nico-Curti/rFBP/blob/master/ISSUE_TEMPLATE.md) or a [pull request](https://github.com/Nico-Curti/rFBP/blob/master/PULL_REQUEST_TEMPLATE.md) and we will check ASAP!

See [here](https://github.com/Nico-Curti/rFBP/blob/master/CONTRIBUTING.md) for further informations about how to contribute with this project.

## References

<blockquote>1- D. Dall'Olio, N. Curti, G. Castellani, A. Bazzani, D. Remondini. "Classification of Genome Wide Association data by Belief Propagation Neural network", CCS Italy, 2019. </blockquote>

<blockquote>2- C. Baldassi, C. Borgs, J. T. Chayes, A. Ingrosso, C. Lucibello, L. Saglietti, and R. Zecchina. "Unreasonable effectiveness of learning neural networks: From accessible states and robust ensembles to basic algorithmic schemes", Proceedings of the National Academy of Sciences, 113(48):E7655-E7662, 2016. </blockquote>

<blockquote>3- C. Baldassi, A. Braunstein, N. Brunel, R. Zecchina. "Efficient supervised learning in networks with binary synapses", Proceedings of the National Academy of Sciences, 104(26):11079-11084, 2007. </blockquote>

<blockquote>4- A., Braunstein, R. Zecchina. "Learning by message passing in networks of discrete synapses". Physical Review Letters 96(3), 2006. </blockquote>

<blockquote>5- C. Baldassi, F. Gerace, C. Lucibello, L. Saglietti, R. Zecchina. "Learning may need only a few bits of synaptic precision", Physical Review E, 93, 2016 </blockquote>

<blockquote>6- A. Blum, R. L. Rivest. "Training a 3-node neural network is NP-complete", Neural Networks, 1992 </blockquote>

<blockquote>7- W. Krauth, M. Mezard. "Storage capacity of memory networks with binary coupling", Journal of Physics (France), 1989 </blockquote>

<blockquote>8- H. Huang, Y. Kabashima. "Origin of the computational hardness for learning with binary synapses", Physical Review E - Statistical, Nonlinear, and Soft Matter Physics, 2014 </blockquote>

<blockquote>9- C. Baldassi, A. Ingrosso, C. Lucibello, L. Saglietti, R. Zecchina. "Local entropy as a measure for sampling solutions in constraint satisfaction problems", Journal of Statistical Mechanics: Theory and Experiment, 2016 </blockquote>

<blockquote>10- R. Monasson, R. Zecchina. "Learning and Generalization Theories of Large Committee Machines", Modern Physics Letters B, 1995 </blockquote>

<blockquote>11- R. Monasson, R. Zecchina. "Weight space structure and internal representations: A direct approach to learning and generalization in multilayer neural networks", Physical Review Letters, 1995 </blockquote>

<blockquote>12- C. Baldassi, A. Braunstein. "A Max-Sum algorithm for training discrete neural networks", Journal of Statistical Mechanics: Theory and Experiment, 2015 </blockquote>

<blockquote>13- G. Parisi. "Mean field theory of spin glasses: statics and dynamics", arXiv, 2007 </blockquote>


## Authors

* **Nico Curti** [git](https://github.com/Nico-Curti), [unibo](https://www.unibo.it/sitoweb/nico.curti2)
* **Daniele Dall'Olio** [git](https://github.com/DanieleDallOlio)
* **Daniel Remondini** [git](https://github.com/dremondini), [unibo](https://www.unibo.it/sitoweb/daniel.remondini)
* **Gastone Castellani** [unibo](https://www.unibo.it/sitoweb/gastone.castellani)
* **Enrico Giampieri** [git](https://github.com/EnricoGiampieri), [unibo](https://www.unibo.it/sitoweb/enrico.giampieri)

See also the list of [contributors](https://github.com/Nico-Curti/rFBP/contributors) [![GitHub contributors](https://img.shields.io/github/contributors/Nico-Curti/rFBP.svg?style=plastic)](https://github.com/Nico-Curti/rFBP/graphs/contributors/) who participated in this project.

## License

The `rFBP` package is licensed under the MIT "Expat" License. [![License](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/Nico-Curti/rFBP/blob/master/LICENSE.md)

## Acknowledgments

Thanks goes to all contributors of this project.

## Citation

If you have found `rFBP` helpful in your research, please consider citing the paper

```tex
@misc{DallOlioCCS19,
  author = {Dall'Olio, Daniele and Curti, Nico and Castellani, Gastone and Bazzani, Armando and Remondini, Daniel},
  title = {Classification of Genome Wide Association data by Belief Propagation Neural network},
  year = {2019},
  conference = {Conference of Complex System}
}
```

or just this project repository

```tex
@misc{ReplicatedFocusingBeliefPropagation,
  author = {Curti, Nico and Dall'Olio, Daniele},
  title = {Replicated Focusing Belief Propagation},
  year = {2019},
  publisher = {GitHub},
  howpublished = {\url{https://github.com/Nico-Curti/rFBP}},
}
```
