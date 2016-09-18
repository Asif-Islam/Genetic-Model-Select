# Genetic-Model-Selection

This project aims to find the best set of hyperparameters for a given supervised learning python script. This is done in C++, using multithreaded genetic algorithm design to accomplish this. Below is a technical description of how this is accomplished. Using this program, I was able to get a two layer neural net made from my implementation here: https://github.com/AsifIslam11197/Basic-Neural-Net-API to achieve 90% validation accuracy perform digit recognition with Kaggle's training data!

## Tecnical Description

Most machine learning techniques for supervised learning often have hyperparameters. Hyperparameters are constant values that are determined before training that define how the supervised learning method will behave. For example, a neural network will have hyperparameters like 'Learning Rate', 'Regularization Strength' and 'Dropout Probability'. Often times, when we have various hyperparameters, we are taught at least to brute force various combinations and stick with the hyperparameters that give the best validation accuracy.

Well, that's not really fun.

I've implemented my own neural network from scratch as the platform to testing the genetic algorithm. In this case, the network has four hyperparameters:

1. Learning Rate - The scaling factor to which we aim to step towards an optimized minimum cost during gradient descent
2. Regularization Strength - A scalar value added to our cost to punish more radically large weights (to avoid overfitting to data)
3. Dropout Probability - The probability that some neurons are set to 0 (to avoid overfitting to data)
4. Neuron Dimensions - How many neurons we have in our network's layer.

The genetic algorithm works by encapsulating these hyperparameters into a "Chromosome". These chromosomes contain numerical values for these hyperparameters, stored as binary strings. In this case, each hyperparameter is split into two bit strings - One for a numerical value 1 <= v < 10, and a power p. It holds the number (v * 10 ^ p)l

The algorithm works by first generating ~24 chromosomes with random hyperparameters. We then create 2 threads (going over 2 is doable, but not for the given hardware capabilities of my computer). Each thread takes half of the chromosome population, and iteratively calls on the python supervised learning script with the given hyperparameters. The python script saves the accuracy in a textfile which is retrieved by our code and stored as the "fitness" of the hyperparameter combination.

After all chromosomes are done and the threads join, we then do a short-sighted sort. That is, we take the top quarter of the best performing chromosomes and keep them for the next generation. As for the other three quarters, we undergo the magic of the genetic algorithm. In this case, we use a weighted-roulette based approach to randomly choose two chromosomes. A chromsome is more likely to be chosen if it has a higher accuracy. We then, across every pair of bitstrings between the two chromosomes, perform crossover. Crossover essentially takes a random pivot point in the bit string, and copies the left half of the bitstring from the first chromosome and combines it with the right half from the second chromomsome, and vice versa to create a second new bitstring . This process occurs for all bit strings, creating two new offspring!

The last process with our offspring is to loop through every bit for all bitstrings and challenge a "mutation". Though a very low probability, ~0.01%, we allow the possibility for each bit to flip. This is so we can attempt to avoid hooking onto local optima.

After enough crossovers, we create a new second "generation", consisting of the most fit (most accurate) chromosomes from the previous generation plus new offspring! Now we simply loop this process - and surpisingly we see that our accuracy slowly plateaus upwards! In my case, I plateaud at 90% accuracy for digit recognition with Kaggle data.

---

One can argue that the convergence for the genetic algorithm can definitely be slower than the brute force method. But similarly, the brute force method requires the programmer to hard code and choose some possible candidates for "Reasonable hyperparameters" Suppose we choose 5 possible values for each of my 4 hyperparameters. That's 5*5*5*5 = 625 different training cycles. With 24 chromosomes, that's about 26 generations - A fairly reasonable amount of generations to attempt convergence. The benefit of this method is that rather than relying on hardcoded possible values, our genetic algorithm though crossover can choose much more unique combinations that may be off the wall for our brute force quadruple for-loop, and could open up the opportunity for better accuracy with slightly more pain in running time for the optimization.



