import numpy as np
from Neural_Network import *
from layers import *
import sys

#Load data
print 'Loading Data...'
LEARNING_RATE = float(sys.argv[2])
DROPOUT_PROB = float(sys.argv[1])
REG_STRENGTH = float(sys.argv[3])

raw_data = np.genfromtxt('train.csv', delimiter=',')

true_labels = raw_data[1:,0]
data = raw_data[1:,1:]
train_data = data[:33000,:]
train_true_labels = true_labels[:33000]
val_data = data[33000:, :]
val_true_labels = true_labels[33000:]

num_train_examples = train_data.shape[0]
num_val_examples = val_data.shape[0]

BATCH_SIZE = 30
FEATURE_DIM = data.shape[1]

OPTIMIZER = 'sgd'

NN = Neural_Network(BATCH_SIZE, FEATURE_DIM, REG_STRENGTH, LEARNING_RATE, OPTIMIZER)
NN.AddLayer('Affine', neuron_dim=250)
NN.AddLayer('Sigmoid')
NN.AddLayer('Dropout', dropout_prob = DROPOUT_PROB)
NN.AddLayer('Affine', neuron_dim=10)
NN.AddLayer('Softmax')

accuracy_history = []
for i in range(0,num_train_examples,BATCH_SIZE):
	batch = train_data[i:i+BATCH_SIZE,:]
	batch_labels = train_true_labels[i:i+BATCH_SIZE]
	accuracy_history.append(NN.train(batch, batch_labels))

print 'Starting validation'
accuracy_sum = 0.0
itr = 0
for i in range(0, num_val_examples,BATCH_SIZE):
	itr += 1
	batch = val_data[i:i+BATCH_SIZE,:]
	batch_labels = val_true_labels[i:i+BATCH_SIZE]
	predictions = NN.predict(batch)
	accuracy_sum += np.mean(batch_labels == predictions)

accuracy = accuracy_sum / itr
print 'validation accuracy of %f' % float(accuracy)
file = open('validation_accuracy.txt', 'w')
file.write(str(accuracy))
file.close()


