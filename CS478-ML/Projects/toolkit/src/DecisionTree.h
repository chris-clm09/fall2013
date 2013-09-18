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
	DecisionTreeNode* dTree;
   
public:
   /************************************************************************
    * Constructor
    * Init pseudo-random number generator.
    ************************************************************************/
	DecisionTree(Rand& r) : SupervisedLearner(), m_rand(r), dTree(NULL) { }

   /************************************************************************
    * Destructor
    ************************************************************************/
	virtual ~DecisionTree() { freeDTree(); }

   /************************************************************************
	 * Free up the decision tree.
    ************************************************************************/
   void freeDTree() {if (dTree != NULL) delete dTree; dTree = NULL;}
   
   /************************************************************************
	 * Train the model to predict the labels
    ************************************************************************/
	virtual void train(Matrix& features, Matrix& labels)
	{
		// Check assumptions
		if(features.rows() != labels.rows())
			ThrowError("Expected the features and labels to have the same number of rows");

		// Shuffle the rows. (This won't really affect this learner, but with other
		// learners it is a good idea to shuffle the rows before doing any training.)
		features.shuffleRows(m_rand, &labels);
      
      freeDTree();
      
		// Train it
      vector<unsigned int> availableAttributes;
      for (int i = 0; i < features[0].size(); i++)
         availableAttributes.push_back(i);
      
      dTree = induceTree(availableAttributes, features, labels);
      
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
      DecisionTreeNode *newNode     = new DecisionTreeNode(exampleSet, labels);
      newNode->indexOfPropertyChosen = attributeIndex;
      
      //Check if attribute is nominal
      unsigned int numAttributeChoices = exampleSet.valueCount(attributeIndex);
      if (numAttributeChoices == 0)
      {// 0 == is Continuious
         
         double splitPoint = exampleSet.columnMean(attributeIndex);
         
         Matrix newExampleSet(exampleSet);
         Matrix newLabels(labels);

         Matrix newExampleSet2(exampleSet);
         Matrix newLabels2(labels);
         
         for (int r = 0; r < exampleSet.rows(); r++)
         {
            if (exampleSet[r][attributeIndex] < splitPoint)
            {
               newExampleSet.copyRow(exampleSet[r]);
               newLabels.copyRow(labels[r]);
            }
            else
            {
               newExampleSet2.copyRow(exampleSet[r]);
               newLabels2.copyRow(labels[r]);
            }
         }
         
         newNode->children.push_back(new DecisionTreeNode(newExampleSet, newLabels));
         newNode->children.push_back(new DecisionTreeNode(newExampleSet2, newLabels2));
      }
      else
      {// 2, 3, 4, ... is Nominal
         for (int c = 0; c < numAttributeChoices; c++)
         {
            Matrix newExampleSet(exampleSet);
            Matrix newLabels(labels);
            
            for (int r = 0; r < exampleSet.rows(); r++)
            {
               if (exampleSet[r][attributeIndex] == c)
               {
                  newExampleSet.copyRow(exampleSet[r]);
                  newLabels.copyRow(labels[r]);
               }
            }
            
            newNode->children.push_back(new DecisionTreeNode(newExampleSet, newLabels));
         }
      }
      
      return newNode;
   }
   
   /************************************************************************
    * Calc Knowledge Gained
    ************************************************************************/
   double infoGain(DecisionTreeNode* attributeSelectedNode)
   {
      //Calc Entropy
      attributeSelectedNode->currentEntropy = entropy(attributeSelectedNode->features,
                                                      attributeSelectedNode->labels);
      for (int childIndex = 0; childIndex < attributeSelectedNode->children.size(); childIndex++)
      {
         (attributeSelectedNode->children[childIndex])->currentEntropy = entropy(
                        (attributeSelectedNode->children[childIndex])->features,
                        (attributeSelectedNode->children[childIndex])->labels);
         
      }
      
      //Calc Weigted Entropy of Children
      double weightedChildrensEntropy = 0;
      for (int childIndex = 0; childIndex < attributeSelectedNode->children.size(); childIndex++)
      {
         double weight = (attributeSelectedNode->children[childIndex])->features.rows() /
                          attributeSelectedNode->features.rows();
         
         weightedChildrensEntropy += weight *
                                     (attributeSelectedNode->children[childIndex])->currentEntropy;
      }
      
      //return gain
      return attributeSelectedNode->currentEntropy - weightedChildrensEntropy;
   }

   unsigned int max(double *options, int size)
   {
      unsigned int max = 0;

      for (int i = 1; i < size; i++)
         if (options[max] > options[i])
            max = i;
   
      return max;
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
      double *infoGainOfPossibilities = new double[possibilities.size()];

      for (int i = 0; i < possibilities.size(); i++)
         infoGainOfPossibilities[i] = infoGain(possibilities[i]);
      
      //Select Max Knowldege Gain
      int iMax = max(infoGainOfPossibilities, possibilities.size());
      
      //Clean up possibilites
      delete [] infoGainOfPossibilities;
      
      for (int i = 0; i < possibilities.size(); i++)
      {
         if (i != iMax)
            delete possibilities[i];
      }
      
      //Return bestSplit
      return possibilities[iMax];
      
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
         DecisionTreeNode *node = returnBestEntropySplitNode(indexsOfAttributesAvailable,
                                                             exampleSet,
                                                             labels);
         if (node == NULL)
         {
            cout << "Holly Molly it was NULL\n";
         }
         
         //Remove selected attribute from list
         vector<unsigned int> newIndexsOfAttributesAvailable(indexsOfAttributesAvailable.size() - 1);

         for (int i = 0; i < indexsOfAttributesAvailable.size(); i++)
            if (indexsOfAttributesAvailable[i] != node->indexOfPropertyChosen)
               newIndexsOfAttributesAvailable.push_back(indexsOfAttributesAvailable[i]);
         
         //Fill in Remainning Attribute Choices
         for (int childIndex = 0; childIndex < node->children.size(); childIndex++)
         {
            DecisionTreeNode * child = induceTree(newIndexsOfAttributesAvailable,
                                                  node->children[childIndex]->features,
                                                  node->children[childIndex]->labels);
            delete node->children[childIndex];
            
            node->children[childIndex] = child;
         }
      }
      
      return returnNode;
   }
   
   /************************************************************************
	 * Evaluate the features and predict the labels
    ************************************************************************/
	virtual void predict(const std::vector<double>& features, std::vector<double>& labels)
	{
      //TODO
	}
};


#endif // BASELINE_H
