#! /usr/bin/env python

import math
import os
import string
import random

""" This program verifies the Law of Conservation of Generalization or """
""" Part ii of NFL Theorem 2 (i.e., E1(D|C)=E2(C|D)) for the space of  """
""" binary functions on NUM_INPUTS Boolean variables  """

""" Supporting functions  """

test_set_s = []
train_set_s = []

def int2bin(int_to_code, code_length):
    """ converts an integer 0 < n < 2^l-1 to a binary string of length l """
    bin_eq = ""
    while int_to_code <> 0:
        n_div_2 = divmod(int_to_code, 2)
        bin_eq = ',' + repr(n_div_2[1]) + bin_eq
        int_to_code = n_div_2[0]
    for i in range(len(bin_eq)/2, code_length):
        bin_eq = ',' + repr(0) + bin_eq
    return bin_eq[1:]

def makeBaseARFFHeaders():
    """ creates the headers of the base ARFF files """
    train_set = open(BASE_TRAIN_FILE_NAME, 'w')
    test_set = open(BASE_TEST_FILE_NAME, 'w')
    train_set.write('@relation BoolTrain')
    test_set.write('@relation BoolTest')
    for i in range(0, NUM_INPUTS):
        train_set.write('\n@attribute A' + repr(i) + ' {0,1}')
        test_set.write('\n@attribute A' + repr(i) + ' {0,1}')
    train_set.write('\n@attribute class {0,1}\n')
    test_set.write('\n@attribute class {0,1}\n')
    train_set.write('@data\n')
    test_set.write('@data\n')
    train_set.close()
    test_set.close()

def makeBaseARFFContents(task_index):
    """ creates the contents of the base ARFF files for task of index """
    """ task_index, with NUM_EXAMPLES total, NUM_TEST examples for    """
    """ testing (OTS) [NUM_EXAMPLES - NUM_TEST for training] and      """
    """ NUM_INPUTS                                                    """
    """ NOTE: Since we are in the Boolean case, we assume that        """
    """       the total number of tasks is pow(2, NUM_EXAMPLES)       """
    """       and hence task_index is coded on NUM_EXAMPLES bits      """
    train_set = open(BASE_TRAIN_FILE_NAME, 'a')
    test_set = open(BASE_TEST_FILE_NAME, 'a')
    vector_C = int2bin(task_index, NUM_EXAMPLES)
    for i in range(0, NUM_EXAMPLES - NUM_TEST):
	    train_set.write(int2bin(i, NUM_INPUTS) + ',' + vector_C[2*i] + '\n')
    for i in range(NUM_EXAMPLES - NUM_TEST, NUM_EXAMPLES):
	    test_set.write(int2bin(i, NUM_INPUTS) + ',' + vector_C[2*i] + '\n')
    train_set.close()
    test_set.close()

def makeBaseARFFContents_simple(task_index):
   """ creates the contents of the base ARFF files for task of index """
   """ task_index, with NUM_EXAMPLES total, NUM_TEST examples for    """
   """ testing (OTS) [NUM_EXAMPLES - NUM_TEST for training] and      """
   """ NUM_INPUTS                                                    """
   """ NOTE: Since we are in the Boolean case, we assume that        """
   """       the total number of tasks is pow(2, NUM_EXAMPLES)       """
   """       and hence task_index is coded on NUM_EXAMPLES bits      """
   global train_set_s
   global test_set_s

   train_set_s = []
   test_set_s  = []
   
   vector_C = int2bin(task_index, NUM_EXAMPLES)

   for i in range(0, NUM_EXAMPLES - NUM_TEST):
      train_set_s.append(vector_C[2*i])

   for i in range(NUM_EXAMPLES - NUM_TEST, NUM_EXAMPLES):
      test_set_s.append(vector_C[2*i])

def getMinority():
   val = [0,0]
   for c in train_set_s:
      c = int(c)
      val[c] = val[c] + 1
   
   #   print val, 0 if val[0] < val[1] else 1
   
   return 0 if val[0] < val[1] else 1

def getMajority():
   val = [0,0]
   for c in train_set_s:
      c = int(c)
      val[c] = val[c] + 1
   
#   print val, 0 if val[0] > val[1] else 1

   return 0 if val[0] > val[1] else 1

def getAccuracy():
   minority = getMinority() #getMajority()
   num_right = 0

   for v in test_set_s:
#      print v, minority, int(v) == minority
      if int(v) == minority:
         num_right = num_right + 1

   print minority, ":", test_set_s, (num_right / float(len(test_set_s))) * 100

   return (num_right / float(len(test_set_s))) * 100


""" Main program                                """

""" Get parameter values  """
NUM_INPUTS = 3#input("Number of binary inputs: ")
NUM_TEST = 5#input("Number of test instances: ")
classifier_name = input("Classifier (be sure to write it as \'class.name\', e.g., \'trees.J48\'): ")

""" Initialize global variables  """
NUM_EXAMPLES = pow(2, NUM_INPUTS)
NUM_TASKS = pow(2, NUM_EXAMPLES)

BASE_TRAIN_FILE_NAME = classifier_name + "Train.arff"
BASE_TEST_FILE_NAME = classifier_name + "Test.arff"
OUTPUT_FILE_NAME = classifier_name + "TaskGP.out"

""" Sets things up for the external call to Weka """
""" Result is stored in OUTPUT_FILE_NAME """
calling_stem = 'java -cp /Applications/weka-3-6-10/weka.jar weka.classifiers.'
base_options = ' -t ' + BASE_TRAIN_FILE_NAME + ' -T ' + BASE_TEST_FILE_NAME + ' -o > ' + OUTPUT_FILE_NAME

""" Run classifier on all tasks and print result """
accuracy_minus_50 = 0
accuracy = 0
for i in range(0, NUM_TASKS):
   if i % 10 == 0:
      print i, '/', NUM_TASKS

   makeBaseARFFHeaders()



   makeBaseARFFContents_simple(i)

   tmpAccuracy = getAccuracy()
   accuracy = accuracy + tmpAccuracy
   accuracy_minus_50 = accuracy_minus_50 + tmpAccuracy - 50


#   """ THIS IS WHERE YOUR CODE GOES TO RUN ON ALL TASKS """
#   """ TASKS ARE NUMBERED FROM 0 TO NUM_TASKS-1 """
#   """ TO CALL WEKA: USE os.system(calling_stem + classifier_name + base_options) """
#   makeBaseARFFContents(i)
#   os.system(calling_stem + classifier_name + base_options)
#
#   """ TO GET THE ACCURACY, OPEN OUTPUT_FILE_NAME, read its content into a string (infile.read() function) and """
#   infile = open(OUTPUT_FILE_NAME,'r')
#   file_str = infile.read()
#
#   """ USE ind1 = string.rfind(file_content, 'Correctly Classified Instances') """
#   ind1 = string.rfind(file_str, 'Correctly Classified Instances')
#
#   """          list1 = string.split(file_content[ind1+31:])    # take substring following '...Instances' and split into a list """
#   list1 = string.split(file_str[ind1+31:])
#
#   """          accuracy = float(list1[1])                     # accuracy is the 2nd list element """
#   accuracy = accuracy + float(list1[1])
#   accuracy_minus_50 = accuracy_minus_50 + float(list1[1]) - 50


""" COMPUTE OVERALL PERFORMANCE (SUM OF ACC-50) """
print "OVERALL PERFORMANCE: (a): ", accuracy, ", (a-50): ", accuracy_minus_50, ", avg: ", accuracy / NUM_TASKS, ", (a-50Avg): ", accuracy_minus_50 / NUM_TASKS

