# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
sys.path.insert(0, os.path.abspath('../../../ReplicatedFocusingBeliefPropagation/'))


# -- Project information -----------------------------------------------------

project = 'Replicated Focusing Belief Propagation algorithm'
copyright = "2020, Nico Curti, Daniele Dall'Olio, Enrico Giampieri"
author = "Nico Curti, Daniele Dall'Olio, Enrico Giampieri"

# The full version, including alpha/beta/rc tags
release = '1.0.1'

master_doc = 'index'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = ['sphinx.ext.napoleon',
						  'sphinx.ext.viewcode',
						  'breathe'
]

# Add any paths that contain templates here, relative to this directory.
templates_path = []

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = []


breathe_projects = {
  'FocusingProtocol' : './doxydoc/',
  'AtanhErf' : './doxydoc/',
  'Cavity_Message' : './doxydoc/',
  'magnetization' : './doxydoc/',
  'MagP64' : './doxydoc/',
  'MagT64' : './doxydoc/',
  'Params' : './doxydoc/',
  'Patterns' : './doxydoc/',
  'ReplicatedFocusingBeliefPropagation' : './doxydoc/',
  }
