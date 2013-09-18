/************************************************************************
 * Decision Tree
 * 
 *  Author: Christopher LaJon Morgan
 *    Date: Sep 17, 2013
 ************************************************************************/

#ifndef CLM_DT_H
#define CLM_DT_H

#include "learner.h"
#include "rand.h"
#include "error.h"
#include "math.h"

#include <iostream>

using namespace std;

/************************************************************************
 * Returns the number of insances that are of a particular class.
 * classIndex is a enum that represents a particular class.
 ************************************************************************/
double numberInClassI(Matrix& labels, int classIndex)
{
   double cnt = 0;
   
   for (int i = 0; i < labels.rows(); i++)
      if (labels[i][0] == classIndex)
         cnt++;
   
   return cnt;
}

/************************************************************************
 * Entropy
 ************************************************************************/
double entropy(Matrix& set, Matrix& labels)
{
   double entropy = 0;
   
   for (int i = 0; i < labels.valueCount(0); i++)
   {
      double proportion = numberInClassI(labels, i) / (double) labels.rows();
      entropy -= proportion * log2(proportion);
   }
   return entropy;
}


/*======================================================================
  ======================================================================
  || Represents a Choice made by a decision tree learning algorithm.
  ======================================================================
  ======================================================================*/
class DecisionTreeNode
{
public:
   DecisionTreeNode(Matrix& aFeatures, Matrix& aLabels)
   :indexOfChosenClass(-1), indexOfPropertyChosen(-1), valueOfPropertyChosen(0), currentEntropy(0),
   labels(aLabels), features(aFeatures)
   {
      for (int r = 0; r < aFeatures.rows(); r++)
      {
         features.copyRow(aFeatures[r]);
         aLabels.copyRow(aLabels[r]);
      }
   }
   DecisionTreeNode(int aIndexOfChosenClass) { indexOfChosenClass = aIndexOfChosenClass; }
   ~DecisionTreeNode() { for (int i = 0; i<children.size(); i++) { free(children[i]); } }

   Matrix features;
   Matrix labels;
   
   int indexOfChosenClass; //Index of class chosen or -1 for not chosen
   int indexOfPropertyChosen;
   double valueOfPropertyChosen;
   double currentEntropy;  //Entropy at just the current node
   vector<DecisionTreeNode*> children;
};


/*======================================================================
  ======================================================================
  || This is a very simple supervised learning algorithm that we provide as an example
  || to get you started. It finds the most common class in the training set and
  || always predicts that class. (Or, if you are doing regression, it always predicts
  || the centroid label.)
  ======================================================================
  ======================================================================*/
class DecisionTree : public SupervisedLearner
{
private:
	Rand& m_rand; // pseudo-random number generator (not actually used by the baseline learner)
	vector<double> m_labelVec; // The label vector that this learner will always predict
   DecisionTreeNode* dTree;
   
public:
   /************************************************************************
    *
    ************************************************************************/
	DecisionTree(Rand& r) : SupervisedLearner(), m_rand(r)
	{
	}

   /************************************************************************
    *
    ************************************************************************/
	virtual ~DecisionTree()
	{
	}

   /************************************************************************
	 * Train the model to predict the labels
    ************************************************************************/
	virtual void train(Matrix& features, Matrix& labels)
	{
      cout << "Labels Count : " << labels.valueCount(0) << endl;
		// Check assumptions
		if(features.rows() != labels.rows())
			ThrowError("Expected the features and labels to have the same number of rows");

		// Shuffle the rows. (This won't really affect this learner, but with other
		// learners it is a good idea to shuffle the rows before doing any training.)
		features.shuffleRows(m_rand, &labels);

		// Throw away any previous training
		m_labelVec.clear();

      
      
		// Train it
		for(size_t i = 0; i < labels.cols(); i++)
		{
			size_t values = labels.valueCount(i);
			if(values == 0) // if the label is continuous...
				m_labelVec.push_back(labels.columnMean(i));
			else
				m_labelVec.push_back(labels.mostCommonValue(i));
		}
	}

   /************************************************************************
    * Returns -1 if labels doesn't indicate that each element is of the
    * same class.  Otherwise it return the enum of the unanimous class.
    ************************************************************************/
   int sameClass(Matrix& labels)
   {
      int theSameClass = (labels.rows() > 0 ? labels[0][0] : -1);
      
      for (int r = 1; r < labels.rows(); r++)
      {
         if (theSameClass != labels[r][0])
            return -1;
      }
      
      return theSameClass;
   }

   /************************************************************************
    * Creates a DecisionTreeNode with the given attributeIndex as the 
    * selection.  Splits the exampleSet accordingly.
    ************************************************************************/
   DecisionTreeNode* decisiontTreeNodeFromAttributeIndex(unsigned int attributeIndex,
                                                         Matrix& exampleSet,
                                                         Matrix& labels)
   {
      //Check if attribute is nominal
      if (exampleSet.valueCount(attributeIndex))
      {// 0 == is Continuious
         
      }
      else
      {// 2, 3, 4, ... is Nominal
         
      }
   }
   
   /************************************************************************
    * Return the node with the best entropy split.
    ************************************************************************/
   DecisionTreeNode* returnBestEntropySplitNode(vector<unsigned int> indexsOfAttributesAvailable,
                                               Matrix& exampleSet,
                                               Matrix& labels)
   {
      vector<DecisionTreeNode*> possibilities;
      
      //Generate all Split Possibilities
      for (int a = 0; a < indexsOfAttributesAvailable.size(); a++)
      {
         possibilities.push_back(decisiontTreeNodeFromAttributeIndex(indexsOfAttributesAvailable[a],
                                                                     exampleSet,
                                                                     labels));
      }
      
      //Calc Knowledge Gain
      
      //Select Max Knowldege Gain
      
      //Clean up possibilites
      
      //Return bestSplit
      
   }
   
   /************************************************************************
    * If all elements in Example-set are in the same class
    *      return a leaf node labeled with that class
    * Else if Properties is empty
    *      return a leaf node labeled with the majority class in Example-set
    * Else
    *      Select P from Properties  (*)
    *      Remove P from Properties
    *      Make P the root of the current tree
    *      For each value V of P
    *         Create a branch of the current tree labeled by V
    *         Partition_V <- Elements of Example-set with value V for P
    *         Induce-Tree(Partition_V, Properties)
    *         Attach result to branch V
    ************************************************************************/
   DecisionTreeNode* induceTree(vector<unsigned int> indexsOfAttributesAvailable,
                               Matrix& exampleSet,
                               Matrix& labels)
   {
      DecisionTreeNode * returnNode = NULL;
      
      int sameClassEnum = sameClass(labels);
      
      if (sameClassEnum != -1)
      {
         returnNode = new DecisionTreeNode(sameClassEnum);
      }
      else if (indexsOfAttributesAvailable.size() < 1)
      {
         returnNode = new DecisionTreeNode(labels.mostCommonValue(0));
      }
      else if (exampleSet.rows() > 0)
      {
         
      }
         
      return returnNode;
   }

   /************************************************************************
    * 
    ************************************************************************/
   double gain(Matrix& set, Matrix& labels, unsigned int propertySelectedIndex)
   {
      return 0;
   }
   
   /************************************************************************
	 * Evaluate the features and predict the labels
    ************************************************************************/
	virtual void predict(const std::vector<double>& features, std::vector<double>& labels)
	{      
		if(labels.size() != m_labelVec.size())
			ThrowError("Unexpected number of label dims");
		for(size_t i = 0; i < labels.size(); i++)
			labels[i] = m_labelVec[i];
	}
};


#endif // BASELINE_H
