#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse

from ReplicatedFocusingBeliefPropagation import MagP64
from ReplicatedFocusingBeliefPropagation import MagT64
from ReplicatedFocusingBeliefPropagation import Pattern
from ReplicatedFocusingBeliefPropagation import NTH
from ReplicatedFocusingBeliefPropagation import ReplicatedFocusingBeliefPropagation as rFBP

__author__  = ["Nico Curti", "Daniele Dall'Olio"]
__email__   = ['nico.curti2@unibo.it', 'daniele.dallolio@studio.unibo.it']


def parse_args ():

  description = "rFBP train example"

  parser = argparse.ArgumentParser(description=description)
  parser.add_argument('--patterns',
                      dest='patterns',
                      required=True,
                      type=str,
                      action='store',
                      help='Pattern Filename' )
  parser.add_argument('--bin',
                      dest='bin',
                      required=False,
                      type=bool,
                      action='store',
                      help='File format: (0) Textfile(default), (1) Binary',
                      default=False,
                      choices=[0, 1])
  parser.add_argument('--delimiter',
                      dest='delimiter',
                      required=False,
                      type=str,
                      action='store',
                      help='Delimiter for text files (default: \"\\t\")',
                      default='\t')
  parser.add_argument('--hidden',
                      dest='hidden',
                      required=False,
                      type=int,
                      action='store',
                      help='Number of Hidden Layers (default: 3)',
                      default=3)
  parser.add_argument('--iteration',
                      dest='iteration',
                      required=False,
                      type=int,
                      action='store',
                      help='Max Number of iterations (default: 1000)',
                      default=1000)
  parser.add_argument('--seed',
                      dest='seed',
                      required=False,
                      type=int,
                      action='store',
                      help='Seed random generator (default: 135)',
                      default=135)
  parser.add_argument('--randfact',
                      dest='randfact',
                      required=False,
                      type=float,
                      action='store',
                      help='Seed random generator of Cavity Messages (default: 0.1)',
                      default=1e-1)
  parser.add_argument('--damping',
                      dest='damping',
                      required=False,
                      type=float,
                      action='store',
                      help='Damping parameter (default: 0.5)',
                      default=.5)
  parser.add_argument('--accuracy',
                      dest='accuracy',
                      required=False,
                      type=str,
                      nargs='+',
                      action='store',
                      help='Accuracy of the messages computation \
                              at the hidden units level (choose between "\
                              "\'exact\'(default), \'accurate\', \'approx\', \'none\')',
                      default=['accurate', 'exact'],
                      choices=['accurate', 'exact', 'approx', 'none'])
  parser.add_argument('--protocol',
                      dest='protocol',
                      required=False,
                      type=str,
                      action='store',
                      help='Specify protocol :\
                              (scoping)                Scooping,\
                              (pseudo_reinforcement)   PseudoReinforcement(default),\
                              (free_scoping)           FreeScoping,\
                              (standard_reinforcement) StandardReinforcement',
                      default='pseudo_reinforcement',
                      choices=['scoping', 'pseudo_reinforcement', 'free_scoping', 'standard_reinforcement'])
  parser.add_argument('--epsilon',
                      dest='epsilon',
                      required=False,
                      type=float,
                      action='store',
                      help='Threshold for convergence (default: 0.1)',
                      default=1e-1)
  parser.add_argument('--steps',
                      dest='steps',
                      required=False,
                      type=int,
                      action='store',
                      help='Max Number of Steps for chosen protocol(default: 101)',
                      default=101)
  parser.add_argument('--mag',
                      dest='mag',
                      required=False,
                      type=int,
                      action='store',
                      help='Specify Magnetization: \
                              (0) MagnetizationP (MagP64),\
                              (1) MagnetizationT (MagT64)',
                      default=1,
                      choices=[MagP64, MagT64])
  parser.add_argument('--outweights',
                      dest='outweights',
                      required=False,
                      type=str,
                      action='store',
                      help='Output Messages file',
                      default='rFBP_weights.dat')
  parser.add_argument('--delweights',
                      dest='delweights',
                      required=False,
                      type=str,
                      action='store',
                      help='Delimiter for Messages file (default: \"\\t\")',
                      default='\t')
  parser.add_argument('--binweights',
                      dest='binweights',
                      required=False,
                      type=bool,
                      action='store',
                      help='Messages files fmt: (0) Textfile(default) (1) Binary',
                      default=False,
                      choices=[0, 1]),
  parser.add_argument('--nth',
                      dest='nth',
                      required=False,
                      type=int,
                      action='store',
                      help='Number of thread to use',
                      default=NTH)

  args = parser.parse_args()

  if len(args.accuracy) > 2:
    raise ValueError('')

  elif len(args.accuracy) == 1:
    args.accuracy.append('exact')

  return args


def train ():
  args = parse_args()

  pattern = Pattern().load(filename=args.patterns, binary=args.bin, delimiter=args.delimiter)

  args.mag = MagP64 if args.mag == 0 else MagT64

  rfbp = rFBP(mag=args.mag,
              hidden=args.hidden,
              max_iter=args.iteration,
              seed=args.seed,
              damping=args.damping,
              accuracy=args.accuracy,
              randfact=args.randfact,
              epsil=args.epsilon,
              protocol=args.protocol,
              size=args.steps,
              nth=args.nth)

  rfbp.fit(pattern)
  rfbp.save_weights(args.outweights, args.delweights, args.binweights)

if __name__ == '__main__':

  train()
