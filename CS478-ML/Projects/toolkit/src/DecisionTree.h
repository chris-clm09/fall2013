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

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#include "DecisionTreeNode.h"

using namespace std;

/************************************************************************
 * Return the max index of the max value.
 ************************************************************************/
unsigned int myIndexMax(double *options, int size)
{
   unsigned int max = 0;
   
   for (int i = 1; i < size; i++)
      if (options[max] < options[i])
         max = i;
   
   return max;
}

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
      entropy -= proportion * (proportion == 0 ? 0.0 : log2(proportion));
   }
   return entropy;
}

/************************************************************************
 * Accuracy
 ************************************************************************/
double accuracy(Matrix& set, Matrix& labels)
{
   double *percents = new double[labels.valueCount(0)];
   memset(percents, 0, labels.valueCount(0) * sizeof(double));
   
   for (int i  = 0; i < labels.rows();        i++) { percents[ (unsigned int)labels[i][0] ] += 1; }
   for (int i  = 0; i < labels.valueCount(0); i++) { percents[i] /= (double)labels.rows(); }
   
   int maxI = myIndexMax(percents, labels.valueCount(0));
   double currentBestGuessAccuracy = percents[maxI];
   
   delete [] percents;
   
   return currentBestGuessAccuracy;
}

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
   bool useAccuracyModel;
   
public:
   /************************************************************************
    * Constructor
    * Init pseudo-random number generator.
    ************************************************************************/
	DecisionTree(Rand& r) : SupervisedLearner(), m_rand(r), dTree(NULL), useAccuracyModel(false) { }

   /************************************************************************
    * Constructor
    * Init pseudo-random number generator.
    * Init if useing accuracy model.
    ************************************************************************/
	DecisionTree(Rand& r, bool useAccuracyModel)
   : SupervisedLearner(), m_rand(r), dTree(NULL), useAccuracyModel(useAccuracyModel) { }
   
   /************************************************************************
    * Destructor
    ************************************************************************/
	virtual ~DecisionTree() { freeDTree(); }

   /************************************************************************
	 * Free up the decision tree.
    ************************************************************************/
   void freeDTree() {if (dTree != NULL) delete dTree; dTree = NULL;}
   
   /************************************************************************
    ************************************************************************/
   void replaceUnknowValuesWithColumnMajority(Matrix &features)
   {
      for (int r = 0; r < features.rows(); r++)
         for (int c = 0; c < features[r].size(); c++)
            if (features[r][c] == UNKNOWN_VALUE)
               features[r][c] = features.mostCommonValue(c);
      return;
   }
   
   /************************************************************************
	 * Train the model to predict the labels
    ************************************************************************/
	virtual void train(Matrix& features, Matrix& labels)
	{
      cout << "Using accuracy : " << useAccuracyModel << endl;
      
		// Check assumptions
		if(features.rows() != labels.rows())
			ThrowError("Expected the features and labels to have the same number of rows");

      replaceUnknowValuesWithColumnMajority(features);
      
		// Shuffle the rows. (This won't really affect this learner, but with other
		// learners it is a good idea to shuffle the rows before doing any training.)
		features.shuffleRows(m_rand, &labels);
      
      freeDTree();
      
//      cout << "Train me: " << endl;
//      features.printMatrix();
//      labels.printMatrix();
//      cout << endl;
      
		// Train it
      vector<unsigned int> availableAttributes;
      for (int i = 0; i < features[0].size(); i++)
         availableAttributes.push_back(i);
      
      dTree = induceTree(availableAttributes, features, labels);
      
//      cout << "DONE =============== \n";
//      dTree->treePrint(0);
      
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
      
//      cout << "Generated Split for Attribute: " << attributeIndex << endl;
//      newNode->print();
//      
//      cout << "Info Gain : " << infoGain(newNode) << endl;
//      
//      int a;
//      cin >> a;
      
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
      
//      cout << "Parent E: " << attributeSelectedNode->currentEntropy << endl;
      
      for (int childIndex = 0; childIndex < attributeSelectedNode->children.size(); childIndex++)
      {
         (attributeSelectedNode->children[childIndex])->currentEntropy = entropy(
                        (attributeSelectedNode->children[childIndex])->features,
                        (attributeSelectedNode->children[childIndex])->labels);
//         cout << "Child E: " << (attributeSelectedNode->children[childIndex])->currentEntropy << endl;
         
      }
      
      //Calc Weigted Entropy of Children
      double weightedChildrensEntropy = 0;
      for (int childIndex = 0; childIndex < attributeSelectedNode->children.size(); childIndex++)
      {
         double weight = (attributeSelectedNode->children[childIndex])->features.rows() /
                          (double) attributeSelectedNode->features.rows();
         
         weightedChildrensEntropy += weight *
                                     (attributeSelectedNode->children[childIndex])->currentEntropy;
      }
      
      //return gain
      return attributeSelectedNode->currentEntropy - weightedChildrensEntropy;
   }
   
   /************************************************************************
    * Return the node with the best entropy split.
    ************************************************************************/
   DecisionTreeNode* returnBestEntropySplitNode(const vector<unsigned int> indexsOfAttributesAvailable,
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
      
      ///
//      cout << "E Choices: ";
//      for (int i = 0; i < possibilities.size(); i++)
//         cout << infoGainOfPossibilities[i] << ' ';
//      cout << endl;
      ///
      
      //Select Max Knowldege Gain
      int iMax = myIndexMax(infoGainOfPossibilities, possibilities.size());
//      cout << "I Max Chosen : " << iMax << endl;
      
      
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
    * Calc Accuracy Gained
    ************************************************************************/
   double accuracyGain(DecisionTreeNode* attributeSelectedNode)
   {
      //Calc Accuracy
      attributeSelectedNode->currentAccuracy = accuracy(attributeSelectedNode->features,
                                                        attributeSelectedNode->labels);
      for (int childIndex = 0; childIndex < attributeSelectedNode->children.size(); childIndex++)
      {
         (attributeSelectedNode->children[childIndex])->currentAccuracy =
         
         accuracy((attributeSelectedNode->children[childIndex])->features,
                  (attributeSelectedNode->children[childIndex])->labels);
      }
      
      //Calc Weigted Entropy of Children
      double weightedChildrensAccuracy = 0;
      for (int childIndex = 0; childIndex < attributeSelectedNode->children.size(); childIndex++)
      {
         double weight = (attributeSelectedNode->children[childIndex])->features.rows() /
                         (double) attributeSelectedNode->features.rows();
         
         weightedChildrensAccuracy += weight *
         (attributeSelectedNode->children[childIndex])->currentAccuracy;
      }
      
      //return Accuracy
      return weightedChildrensAccuracy - attributeSelectedNode->currentAccuracy;
   }

   
   /************************************************************************
    * Return the node with the best accuracy split.
    ************************************************************************/
   DecisionTreeNode* returnBestAccuracySplitNode(const vector<unsigned int> indexsOfAttributesAvailable,
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
      double *accuracyGainOfPossibilities = new double[possibilities.size()];
      
      for (int i = 0; i < possibilities.size(); i++)
         accuracyGainOfPossibilities[i] = accuracyGain(possibilities[i]);
      
      //Select Max Knowldege Gain
      int iMax = myIndexMax(accuracyGainOfPossibilities, possibilities.size());
      
      //Clean up possibilites
      delete [] accuracyGainOfPossibilities;
      
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
      //log
//      cout << "Atts: ";
//      
//      for(int i = 0; i < indexsOfAttributesAvailable.size(); i++)
//         cout << indexsOfAttributesAvailable[i] << " ";
//      
//      cout << "\nSet: " << exampleSet.rows() << ":\n";
//      exampleSet.printMatrix();
//      cout << "Labels: "<< labels.rows() << "\n";
//      labels.printMatrix();
      //
      
      
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
         DecisionTreeNode *node = (useAccuracyModel ?
                                   returnBestAccuracySplitNode(indexsOfAttributesAvailable,
                                                               exampleSet,
                                                               labels)
                                                    :
                                   returnBestEntropySplitNode(indexsOfAttributesAvailable,
                                                             exampleSet,
                                                             labels));

         if (node == NULL) { cout << "Holly Molly it was NULL\n"; }
         
         //log
//         cout << "Chose Attribute: " << node->indexOfPropertyChosen << endl;
         
         //Remove selected attribute from list
         vector<unsigned int> newIndexsOfAttributesAvailable;
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
         
         returnNode = node;
      }
      
      return returnNode;
   }
   
   /************************************************************************
	 * Evaluate the features and predict the labels
    ************************************************************************/
	virtual void predict(const std::vector<double>& features, std::vector<double>& labels)
	{
//      cerr << "\nPredict : ";
//      for (int i = 0; i < features.size(); i++) {
//         cerr << features[i] << " ";
//      }
      
      labels[0] = dTree->predict(features);
      
//      cerr << "\nAnswer: ";
//      for (int i = 0; i < labels.size(); i++) {
//         cerr << labels[i] <<  " ";
//      }
//      cerr << endl;
	}
};


#endif // BASELINE_H
