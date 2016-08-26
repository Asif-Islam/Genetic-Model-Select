# Genetic-Model-Selection

This project aims to find the best set of hyperparameters for a given supervised learning python script. This is done in C++, using multithreaded genetic algorithm design to accomplish this. Below is a technical description of how this is accomplished. Using this program, I was able to get a two layer neural net made from my implementation here: https://github.com/AsifIslam11197/Basic-Neural-Net-API to achieve 90% validation accuracy perform digit recognition with Kaggle's training data!

## Tecnical Description

Most machine learning techniques for supervised learning often have hyperparameters. Hyperparameters are variables in the problem that do not change, but can decide the effectiveness of the learning. For example, a neural network has to have a predefined learning rate, number of neurons and regularization strength (For a basic NN). In most cases, people find the best combination by brute forcing combinations and see which one gets the best result.
