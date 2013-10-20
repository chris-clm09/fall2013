/************************************************************************
 * NN
 *
 *  Author: Christopher LaJon Morgan
 *    Date: Sep 17, 2013
 ************************************************************************/

#ifndef CLM_NN_H
#define CLM_NN_H

#include "learner.h"
#include "rand.h"
#include "error.h"
#include "math.h"

#include "NNNode.h"

#include <iostream>

#define RESET       "\033[0m"
#define BLACK       "\033[30m"      /* Black */
#define RED         "\033[31m"      /* Red */
#define GREEN       "\033[32m"      /* Green */
#define YELLOW      "\033[33m"      /* Yellow */
#define BLUE        "\033[34m"      /* Blue */
#define WHITE       "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

using namespace std;

const double STOP_ERROR_VALUE  = 5.0;

const int NUM_HIDDEN_LAYERS    = 2;
const int NUM_NODES_IN_LAYER[] = {6, 4};

//#define NUM_HIDDEN_LAYERS 1
//#define NUM_NODES_IN_LAYER {6}


/*======================================================================
 ======================================================================
 | Represents a Neural Net Learning Alorithm
 ======================================================================
 ======================================================================*/
class NN : public SupervisedLearner
{
private:
	Rand& m_rand;
   vector<vector<NNNode*>*> nnLayers;
   
public:
   /************************************************************************
    * Constructor
    * Init pseudo-random number generator.
    ************************************************************************/
	NN(Rand& r) : SupervisedLearner(), m_rand(r)
   {
   }
   
   /************************************************************************
    * Destructor
    ************************************************************************/
	virtual ~NN() { freeNeuralNet(); }

   /************************************************************************
    * Cleans up the NuralNet
    ************************************************************************/
   void freeNeuralNet()
   {
      return;
   }
   
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
		// Check assumptions
		if(features.rows() != labels.rows())
			ThrowError("Expected the features and labels to have the same number of rows");
      
      replaceUnknowValuesWithColumnMajority(features);
      
		// Shuffle the rows. (This won't really affect this learner, but with other
		// learners it is a good idea to shuffle the rows before doing any training.)
		features.shuffleRows(m_rand, &labels);
      
      //Setup Nural Network
      cout << "\n\t STARTING Up Complete\n";
      setUpNuralNet(features, labels);
      printNN();
      cout << "\n\t SET Up Complete\n";
      
      //Run NN Learning Algorithm
      learn(features, labels);
      
	}
   
   /************************************************************************
    * This function will set up the nural network.
    ************************************************************************/
   void setUpNuralNet(Matrix& features, Matrix& labels)
   {
      //Setup input nodes
      vector<NNNode*>* inNodes = new vector<NNNode*>;
      
      for (int i  = 0; i < features.cols(); i++)
         inNodes->push_back(new NNNode(m_rand, 0));
      
      nnLayers.push_back(inNodes);
      
      //Setup hidden nodes
      for (int l = 0; l < NUM_HIDDEN_LAYERS; l++)
      {
         vector<NNNode*>* hiddenLayer = new vector<NNNode*>;

         for (int n = 0; n < NUM_NODES_IN_LAYER[l]; n++)
            hiddenLayer->push_back(new NNNode(m_rand, nnLayers[l]->size()));
         
         nnLayers.push_back(hiddenLayer);
      }
      
      //Setup output nodes
      vector<NNNode*>* outputNodes = new vector<NNNode*>;
      
      for (int i = 0; i < labels.valueCount(0); i++)
         outputNodes->push_back(new NNNode(m_rand, NUM_NODES_IN_LAYER[NUM_HIDDEN_LAYERS-1]));
      
      nnLayers.push_back(outputNodes);
      
      return;
   }
   
   /************************************************************************
    * This function will print out the neural net.
    ************************************************************************/
   void printNN()
   {
      for (int i  = 0; i < nnLayers.size(); i++)
      {
         cout << "Layer(" << i << "): ";
         for (int n = 0; n < nnLayers[i]->size(); n++)
         {
            NNNode* node = (*(nnLayers[i]))[n];
            cout << "N:(V:" << node->value << ",NW:" << node->incomeingWeights.size() << ") ; ";
         }
         cout << endl;
      }
      return;
   }
   
   /************************************************************************
    * This function will simulate a NN Learning Algorithm.
    ************************************************************************/
   void learn(Matrix &features, Matrix &labels)
   {
      double error = 1000.0;
      while (error > STOP_ERROR_VALUE)
      {
         error = 0;
         for (int i = 0; i < features.rows(); i++)
         {
            runInstanceTrough(features[i], labels[i][0]);
            error = backErrorPropigate(labels[i][0]);
         }
         error /= (double)features.rows();
      }
      return;
   }
   
   /************************************************************************
    ************************************************************************/
   void runInstanceTrough(vector<double>& row, double ansClass)
   {
      //SET Input Node Values
      for (int i = 0; i < row.size(); i++)
      {
        (*(nnLayers[0]))[i]->value = row[i];
      }
      
      //FOREACH non-Input Layer
      for (int i = 1; i < nnLayers.size(); i++)
      {
         for (int n = 0; n < (*(nnLayers[i])).size(); n++)
         {
            //Calculate value of node
            (*(nnLayers[i]))[n]->calculateValue( (*(nnLayers[i-1])) );
         }
      }
      return;
   }
   
   /************************************************************************
    * Calculates the error at each node, the change in weights required
    * given the error, and then applies the change of weights to each
    * node link weight.
    ************************************************************************/
   double backErrorPropigate(double answer)
   {
      double errorFound = calcBackErrorPropigation(answer);
      applyBackErrorCalculations();
      return errorFound;
   }
   
   /************************************************************************
    * Calculates and set the error for the output nodes and then for
    * each hidden node.  Also updates the change in weights requried for
    * the given amount of error found.
    ************************************************************************/
   double calcBackErrorPropigation(double answer)
   {
      double errorFound = 0;
      
      //Calc error of output nodes
      errorFound += calcErrorOfOutputNodes(answer);
      
      //Calc error of the hidden nodes
      for (int l = nnLayers.size() - 2; l > 0; l--)
      {
         vector<NNNode*>* layer = nnLayers[l];
         for (int n = 0; n < layer->size(); n++)
         {
            (*layer)[n]->calculateErrorValueAndSetChangeInWeights(*(nnLayers[l+1]),
                                                                  n,
                                                                  *(nnLayers[l-1])
                                                                  );
         }
      }
      
      return errorFound;
   }
   
   /************************************************************************
    * Calculates the error and sets the weight for the max out put node and
    * the answer output node.  If they are the same no error occured.
    ************************************************************************/
   double calcErrorOfOutputNodes(double answer)
   {
      //Get the max output value
      int max = getIndexOfTheMaxOutput();
      
      vector<NNNode*> *outputLayer = nnLayers[nnLayers.size() - 1];
      vector<NNNode*> &backNodes   = *(nnLayers[nnLayers.size() - 2]);
      
      double error = 0;
      //FOREACH output node calculate error
      for (int n = 0; n < outputLayer->size(); n++)
      {
         (*outputLayer)[n]->calculateErrorValueAndSetChangeInWeights(
                                                                     (n == (int)answer ? 1 : 0),
                                                                     backNodes
                                                                     );
         error += abs((*outputLayer)[n]->errorValue);
      }

      return error;
   }
   
   /************************************************************************
    * Returns the index of the output with the max value.
    ************************************************************************/
   int getIndexOfTheMaxOutput()
   {
      int max = 0;
      
      vector<NNNode*> *outputLayer = nnLayers[nnLayers.size() - 1];
      
      for (int n = 1; n < outputLayer->size(); n++)
         if ((*outputLayer)[max] < (*outputLayer)[n])
            max = n;

      return max;
   }
   
   /************************************************************************
    ************************************************************************/
   void applyBackErrorCalculations()
   {
      //FOREACH non-Input Layer
      for (int i = 1; i < nnLayers.size(); i++)
         for (int n = 0; n < (*(nnLayers[i])).size(); n++)
            //Calculate value of node
            (*(nnLayers[i]))[n]->applyChangeInIncomeingWeightsToIncomeingWeights();

      return;
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
      
      
      //      cerr << "\nAnswer: ";
      //      for (int i = 0; i < labels.size(); i++) {
      //         cerr << labels[i] <<  " ";
      //      }
      //      cerr << endl;
	}
};


#endif
