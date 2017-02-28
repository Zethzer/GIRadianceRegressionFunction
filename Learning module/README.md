# Learning module

This module will train a Neural Network with provided config file and training data.

__use: ./LearningModule \<training data directory\>__

* __training data directory__ is a directory that must contain a config.xml file __(see the config.xml example)__ and __*.data__ files.
* __*.data__ files must be binary files, extracted from the Extraction module.

The program will create a Network, iterate through every __.data__ file, extract a DataSet and train the Network with it.
At each iteration, a backup of the current Neural Network will be saved in _neuralnetworksave1.xml_ and _neuralnetworksave2.xml (depending on the current iteration)_,
and the name last opened __.data__ file will be written on a log file, preventing from looping over already-treated files when interrupted.

If you want to restart from beginning after an interruption, you must delete the log file and neuralnetwork saves.
