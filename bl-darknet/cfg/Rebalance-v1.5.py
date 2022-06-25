# If problem with too big files, use: https://ipython-books.github.io/48-processing-large-numpy-arrays-with-memory-mapping
 
import keras
from keras.models import Sequential
from keras.layers import Dense, Activation, Conv2D, Flatten, MaxPooling2D, Dropout, LSTM
from keras.optimizers import SGD

from keras.layers.normalization import BatchNormalization

from keras.utils import plot_model

import keras.backend as K

from kerasify import export_model

import pickle
import os.path

import sys
sys.setrecursionlimit(15000) #1500) # for pickle

import numpy as np

import matplotlib.pyplot as plt

import math

from pdb import set_trace as bp

import time

#####
#mix_filename = "mix.dat"
#mask_filename = "mask.dat"
#model_pkl_filename = "model.pkl"
#score_train_filename = "score-train.txt"
#checkpoint_acc_filename = "checkpoint-acc.txt"
#checkpoint_loss_filename="checkpoint-loss.txt"
#score_test_filename="score-test.txt"
#model_filename="../resources/models/dnn-model-source0.model"

num = sys.argv[1]

mix_filename = "mix.dat"
mask_filename = "mask" + num + ".dat"
model_pkl_filename = "model" + num + ".pkl"
score_train_filename = "score-train" + num + ".txt"
checkpoint_acc_filename = "checkpoint-acc" + num + ".txt"
checkpoint_loss_filename="checkpoint-loss" + num + ".txt"
score_test_filename="score-test" + num + ".txt"
#model_filename="../resources/models/dnn-model-source" + num + ".model"
model_filename="dnn-model-source" + num + ".model"

#
num_samples_cut = 4 #2 # TEST #4 #1 #2 #8
frame_size = 256 # TEST 128
####

#import theano
#theano.config.cxx = '/usr/local/bin/clang-omp++'
#theano.config.cxx = '/usr/bin/llvm-g++'



#####

# read csv
#mix = np.loadtxt("mix.dat", delimiter=',')
#mask = np.loadtxt("mask.dat", delimiter=',')

# read binary
mix = np.fromfile(mix_filename, dtype='f4') # f8 for double
mask = np.fromfile(mask_filename, dtype='f4')

#mix = np.memmap(mix_filename, dtype='f4') # f8 for double
#mask = np.memmap(mask_filename, dtype='f4')

def split_samples(samples, size):
    samples = list()
    for i in range(len(samples)/sample_size):
        sample = list()
        sample.append(samples[i:i+size])
        samples.append(sample)
        
    samples = np.array(samples)
    return samples

# take 4x256 for mix, and 256 for mask
sample_size_x = num_samples_cut*frame_size
num_samples_x = len(mix)/sample_size_x

sample_size_y = frame_size
num_samples_y = len(mask)/sample_size_y

#samples_x = split_samples(mix, sample_size)
#samples_y = split_samples(mask, sample_size)
samples_x = np.array(mix)
samples_y = np.array(mask)

del mix
del mask

samples_x = np.reshape(samples_x, (num_samples_x, sample_size_x))
samples_y = np.reshape(samples_y, (num_samples_y, sample_size_y))

#def transpose_samples(samples):
#    for i in (0, num_samples-1):
#        lines = samples[i]
#        lines.reshape(num_samples_cut, 1024)
#        lines.transpose()
#        samples[i] = lines
#    return samples
        
#transpose, to have the same order as in the article
#samples_x = transpose_samples(samples_x)
#samples_y = transpose_samples(samples_y)

#def prepare_conv2d(samples):
#    for i in (0, num_samples-1):
#        lines = samples[i]
#        lines.reshape(1, num_samples_cut, 1024)
#        samples[i] = lines
#    return samples

#samples_x = prepare_conv2d(samples_x)
#samples_y = prepare_conv2d(samples_y)

#prepare for conv2d
#samples_x = np.reshape(samples_x, (num_samples, 1, 1024, num_samples_cut))
#samples_y = np.reshape(samples_y, (num_samples, 1, 1024, num_samples_cut))

train_size_x = int(num_samples_x * 0.8)
test_size_x =  num_samples_x - train_size_x

train_size_y = int(num_samples_y * 0.8)
test_size_y =  num_samples_y - train_size_y

#samples_x_train = samples_x[0:train_size_x]
#samples_x_test = samples_x[train_size_x:num_samples_x]

#del samples_x

#samples_y_train = samples_y[0:train_size_y]
#samples_y_test = samples_y[train_size_y:num_samples_y]

#del samples_y

samples_x_train, samples_x_test = np.split(samples_x, [train_size_x,])
del samples_x

samples_y_train, samples_y_test = np.split(samples_y, [train_size_y,])
del samples_y


#plt.plot(train[0:1023])
#plt.legend(['---']);
#plt.show()

def create_model():
    model = Sequential()
    model.add(Dense(sample_size))
    # converges well with this additional dense layer
    #model.add(Dense(sample_size)) # add another Dense ?
    model.compile(loss='mse', optimizer='nadam', metrics=['accuracy'])     
    #sgd
    # converges well with nadam instead of sgd

    #model = Sequential()
    #model.add(Conv2D(64, kernel_size=(3, 3),
    #                 activation='relu')) #,
    #input_shape=(1, 1024, num_samples_cut)))
    #model.add(Conv2D(64, (4, 4), strides = (1, 1), padding = 'same'))
    #model.add(Dense(sample_size))
    #model.compile(loss='mse', optimizer='sgd', metrics=['accuracy'])

    return model


def create_model2():
    model = Sequential()
    model.add(LSTM(sample_size,
                   input_shape=(1, sample_size),
                   return_sequences=True))
    model.add(Dense(sample_size))
    
    model.compile(loss='mse', optimizer='nadam', metrics=['accuracy'])

    #model.compile(loss='mean_squared_error', optimizer='adam', metrics =["accuracy"])

    # reshape for LSTM
    global samples_x_train
    global samples_y_train
    global samples_x_test
    global samples_y_test
    
    samples_x_train = np.reshape(samples_x_train,
                                 (len(samples_x_train), 1, sample_size))
    samples_y_train = np.reshape(samples_y_train,
                                 (len(samples_y_train), 1, sample_size))
    samples_x_test = np.reshape(samples_x_test,
                                 (len(samples_x_test), 1, sample_size))
    samples_y_test = np.reshape(samples_y_test,
                                 (len(samples_y_test), 1, sample_size))

    return model

def create_model3():
    model = Sequential()
    model.add(Dense(sample_size_y))
    model.compile(loss='mse', optimizer='nadam', metrics=['accuracy'])     
    return model

if os.path.isfile(model_pkl_filename):
    input = open(model_pkl_filename, 'rb')
    model = pickle.load(input)
    input.close();
else:
    # model

    #trainx = trainx.reshape(trainx.shape[0], split_size, 1)

    #model = create_model()
    model = create_model3()

    # for the following, num_samples_cut must be set to 1
    #model = create_model2()
    
class AccHistory(keras.callbacks.Callback):
        def on_batch_end(self, batch, logs={}):
            self.acc = logs.get('acc')
        
        def on_train_end(self, batch, logs={}):
            score_train_file = open(score_train_filename, 'a')
            score_train_file.write(str(self.acc))
            score_train_file.write(" ")
            score_train_file.close()

        def on_epoch_end(self, epoch, logs={}):
            score_acc_file = open(checkpoint_acc_filename, 'a')
            score_acc_file.write(str(logs.get('acc')))
            score_acc_file.write(" ")
            score_acc_file.close()

            score_loss_file = open(checkpoint_loss_filename, 'a')
            score_loss_file.write(str(logs.get('loss')))
            score_loss_file.write(" ")
            score_loss_file.close()
            
history = AccHistory()
            
while True:
#if True:
    model.fit(samples_x_train, samples_y_train,
              epochs=20, #0, #40, #10, #100, #10, #100,
              batch_size=64, #16, #256, #64, #16, #64, #16, #64, # origin: 16 
              callbacks=[history])
    score = model.evaluate(samples_x_test, samples_y_test, batch_size=64)

    print ("score", score)

    score_test_file = open(score_test_filename, 'a')
    score_test_file.write(str(score[1]))
    score_test_file.write(" ")
    score_test_file.close();
    
    export_model(model, model_filename)

    # long in time !
    output = open(model_pkl_filename, 'wb')
    pickle.dump(model, output)
    output.close()

    #time.sleep(30)
    time.sleep(300) # sleep 5mn to cool down the processor
