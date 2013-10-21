/************************************************************************
 * NNNode
 *
 *  Author: Christopher LaJon Morgan
 *    Date: Sep 17, 2013
 ************************************************************************/

#ifndef CLM_NNNode_H
#define CLM_NNNode_H

#include "rand.h"
#include "error.h"
#include "math.h"
#include <vector>

#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>

#define RESET       "\033[0m"
#define BLACK       "\033[30m"         /* Black */
#define RED         "\033[31m"         /* Red */
#define GREEN       "\033[32m"         /* Green */
#define YELLOW      "\033[33m"         /* Yellow */
#define BLUE        "\033[34m"         /* Blue */
#define WHITE       "\033[37m"         /* White */
#define BOLDBLACK   "\033[1m\033[30m"  /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"  /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"  /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"  /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"  /* Bold Blue */
#define BOLDWHITE   "\033[1m\033[37m"  /* Bold White */

using namespace std;


/*======================================================================
 ======================================================================
 | Represents a Neural Net Learning Node
 ======================================================================
 ======================================================================*/
class NNNode
{
public:
   /************************************************************************
    * Constructor
    ************************************************************************/
   NNNode(Rand& r, int numIncomingConnections)
   :value(0),
    errorValue(0),
    incomeingWeights(numIncomingConnections,1),
    changeInIncomeingWeights(numIncomingConnections,0),
   LEARNING_CONSTANT(0.3)
   {
      randomizeIncomeingWeights(r);
      return;
   }
   
   /************************************************************************
    * Randomizes all of the incomeing weights, then moves the mean of the
    * weights to zero.
    ************************************************************************/
   double randomizeIncomeingWeights(Rand& r)
   {
      double total = 0;
      for (int i  = 0; i<incomeingWeights.size(); i++)
      {
         incomeingWeights[i] = r.uniform();
         total += incomeingWeights[i];
      }
      
      double average = total / (double)incomeingWeights.size();
      double moveAvergeToZeroCalc = average / (double)incomeingWeights.size();
      
      total = 0;
      for (int i  = 0; i<incomeingWeights.size(); i++)
      {
         incomeingWeights[i] = incomeingWeights[i] - average;
         total += incomeingWeights[i];
      }
      
      //Should be Zero
      return total;
   }
   
   /************************************************************************
    * Sets the nodes values according to the value and weight of the
    * incomeing weights.
    ************************************************************************/
   double calculateValue(vector<NNNode*>& backNodes)
   {
      value = 1 / (1.0 + pow(M_E, -calcNet(backNodes)));
      return value;
   }

   /************************************************************************
    * Calcualtes the net value st net = sum(inputs) wi*xi
    ************************************************************************/
   double calcNet(vector<NNNode*>& backNodes)
   {
      double total = 0;
      
      for (int i = 0; i < incomeingWeights.size(); i++)
         total += incomeingWeights[i] * backNodes[i]->value;
      
      return total;
   }
   
   /************************************************************************
    * OUTPUT NODE
    * Calculates the error of an output node and calculates the incomeing
    * weights accordingly.
    ************************************************************************/
   void calculateErrorValueAndSetChangeInWeights(double expected, vector<NNNode*>& backNodes)
   {
      errorValue = (expected - value) * (value * (1 - value));
      
      for (int i = 0; i < backNodes.size(); i++)
         changeInIncomeingWeights[i] = LEARNING_CONSTANT * backNodes[i]->value * errorValue;
      
      return;
   }
   
   /************************************************************************
    * HIDDEN NODE
    * Calcualtes the error of a hidden node and sets its incomeing weights
    * accordingly.
    ************************************************************************/
   void calculateErrorValueAndSetChangeInWeights(vector<NNNode*>& frontNodes,
                                                 int frontLinkIndex,
                                                 vector<NNNode*>& backNodes)
   {
      errorValue = 0;
      for (int i  = 0; i < frontNodes.size(); i++)
      {
         errorValue += frontNodes[i]->errorValue *
                       frontNodes[i]->incomeingWeights[frontLinkIndex] *
                       (value * (1 - value));
      }
      
      for (int i = 0; i < backNodes.size(); i++)
         changeInIncomeingWeights[i] = LEARNING_CONSTANT * backNodes[i]->value * errorValue;
      
      return;
   }
   
   /************************************************************************
    * Apply the change in weights to incomeingWeights.
    ************************************************************************/
   void applyChangeInIncomeingWeightsToIncomeingWeights()
   {
      for (int i  = 0; i < incomeingWeights.size(); i++)
         incomeingWeights[i] += changeInIncomeingWeights[i];
      
      return;
   }
   
   //----------------------Data Variables----------------------//
   double value;
   double errorValue;
   vector<double> incomeingWeights;
   vector<double> changeInIncomeingWeights;
   const double LEARNING_CONSTANT;
};

#endif
