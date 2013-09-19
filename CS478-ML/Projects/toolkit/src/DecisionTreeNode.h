/*======================================================================
 ======================================================================
 || Represents a Choice made by a decision tree learning algorithm.
 ======================================================================
 ======================================================================*/
class DecisionTreeNode
{
public:
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   
   /************************************************************************
    ************************************************************************/
   DecisionTreeNode(Matrix& aFeatures, Matrix& aLabels)
   :indexOfChosenClass(-1), indexOfPropertyChosen(-1), currentEntropy(0), currentAccuracy(0),
   labels(aLabels), features(aFeatures)
   {
      for (int r = 0; r < aFeatures.rows(); r++)
      {
         features.copyRow(aFeatures[r]);
         labels.copyRow(aLabels[r]);
      }
   }
   
   /************************************************************************
    ************************************************************************/
   DecisionTreeNode(int aIndexOfChosenClass)
   :indexOfChosenClass(aIndexOfChosenClass), indexOfPropertyChosen(-1), currentEntropy(0), currentAccuracy(0)
   {
   }
   
   /************************************************************************
    ************************************************************************/
   ~DecisionTreeNode() { for (int i = 0; i<children.size(); i++) { free(children[i]); } }
   
   /************************************************************************
    ************************************************************************/
   double predict(const std::vector<double>& features)
   {
      if (indexOfChosenClass != -1)
      {
         return indexOfChosenClass;
      }
      else
      {
         unsigned int numAttributeChoices = this->features.valueCount(indexOfPropertyChosen);
         
         if (numAttributeChoices == 0)// 0 == is Continuious
         {
            double splitPoint = this->features.columnMean(indexOfPropertyChosen);
            
            if (features[indexOfPropertyChosen] < splitPoint)
               return children[0]->predict(features);
            else
               return children[1]->predict(features);
         }
         else if (children[features[indexOfPropertyChosen]] != NULL)
            return children[features[indexOfPropertyChosen]]->predict(features);
         else
            return -1;
      }
   }
   
   /************************************************************************
    ************************************************************************/
   void print()
   {
      cout << "Me: \n";
      cout << "Property: " << indexOfPropertyChosen << endl;
      
      cout << "Set: " << features.rows() << endl;
      features.printMatrix();
      
      cout << "Labels: " << labels.rows() << endl;
      labels.printMatrix();
      
      printChildren();
      
      cout << "-------------------------\n";
      
   }
   
   /************************************************************************
    ************************************************************************/
   void printChildren()
   {
      for (int i = 0; i < children.size(); i++)
      {
         cout << "Child : " << i << endl;
         
         children[i]->features.printMatrix();
         children[i]->labels.printMatrix();
         
         cout << endl;
      }
   }
   
   /************************************************************************
    ************************************************************************/
   void px(int i){while (i--) {cout << "       ";}}
   void treePrint(int i)
   {
      px(i);
      
      cerr << "Node: ";
      if (indexOfPropertyChosen != -1) cerr << "Property: " << RED << indexOfPropertyChosen << RESET;
      if (indexOfChosenClass    != -1) cerr << " Class: " << BLUE << indexOfChosenClass << RESET;
      cerr << endl;
      
      for (int c = 0; c < children.size(); c++)
         if (children[c] != NULL)
            children[c]->treePrint(i + 1);
         else
         {
            px(i + 1);
            cerr << "NULL" << endl;
         }
   }
   
   // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Data Members ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   Matrix features;
   Matrix labels;
   
   int indexOfChosenClass; //Index of class chosen or -1 for not chosen
   int indexOfPropertyChosen;
   
   double currentEntropy;  //Entropy at just the current node
   double currentAccuracy;
   
   vector<DecisionTreeNode*> children;
   
};
