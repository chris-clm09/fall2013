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


/*======================================================================
 ======================================================================
 |
 |
 ======================================================================
 ======================================================================*/
class NN : public SupervisedLearner
{
private:
	Rand& m_rand; // pseudo-random number generator (not actually used by the baseline learner)
   
public:
   /************************************************************************
    * Constructor
    * Init pseudo-random number generator.
    ************************************************************************/
	NN(Rand& r) : SupervisedLearner(), m_rand(r)
   { }
   
   /************************************************************************
    * Destructor
    ************************************************************************/
	virtual ~NN() { }
   
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
