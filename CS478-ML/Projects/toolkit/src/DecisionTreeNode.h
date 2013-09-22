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
   :indexOfChosenClass(-1), indexOfPropertyChosen(-1), indexOfPropertyChosen2(-1), currentEntropy(0), currentAccuracy(0),
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
   :indexOfChosenClass(aIndexOfChosenClass), indexOfPropertyChosen(-1), indexOfPropertyChosen2(-1), currentEntropy(0), currentAccuracy(0)
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
   double predictAccuracyDoubleAtter(const std::vector<double>& features)
   {
      if (indexOfChosenClass != -1)
      {
         return indexOfChosenClass;
      }
      else
      {
         if (indexOfPropertyChosen2 != -1) // Double Property Node
         {
            int sizeOfAttributeTwo = (this->features.valueCount(indexOfPropertyChosen2) == 0 ? 2
                                      : this->features.valueCount(indexOfPropertyChosen2));
            
            int n1;
            int n2;
            
            if (this->features.valueCount(indexOfPropertyChosen)  == 0) //IS Continuious
            {
               double splitPoint = this->features.columnMean(indexOfPropertyChosen);
               
               if (features[indexOfPropertyChosen] < splitPoint)
                  n1 = 0;
               else
                  n1 = 1;
            }
            else
            {
               if (features[indexOfPropertyChosen] == UNKNOWN_VALUE)
               {
                  n1 = this->features.mostCommonValue(indexOfPropertyChosen);
               }
               else
                  n1 = features[indexOfPropertyChosen];
            }
            
            if (this->features.valueCount(indexOfPropertyChosen2) == 0) //IS Continious
            {
               double splitPoint = this->features.columnMean(indexOfPropertyChosen2);
               
               if (features[indexOfPropertyChosen2] < splitPoint)
                  n2 = 0;
               else
                  n2 = 1;

            }
            else
            {
               if (features[indexOfPropertyChosen2] == UNKNOWN_VALUE)
                  n2 = this->features.mostCommonValue(indexOfPropertyChosen2);
               else
                  n2 = features[indexOfPropertyChosen2];
            }
            
            int i = (n1 * sizeOfAttributeTwo) + n2;
            
//            cout << "Index : " << i << " n1: " << n1 << " n2: " << n2 << " Size of N2: " << sizeOfAttributeTwo << endl;
            
            if (children[i] != NULL)
               return children[i]->predictAccuracyDoubleAtter(features);
            else
               return -1;
         }
                         
         unsigned int numAttributeChoices = this->features.valueCount(indexOfPropertyChosen);
  
         if (numAttributeChoices == 0)// 0 == is Continuious
         {
            double splitPoint = this->features.columnMean(indexOfPropertyChosen);
            
            if (features[indexOfPropertyChosen] < splitPoint)
               return children[0]->predictAccuracyDoubleAtter(features);
            else
               return children[1]->predictAccuracyDoubleAtter(features);
         }
         else if (children[features[indexOfPropertyChosen]] != NULL)
            return children[features[indexOfPropertyChosen]]->predictAccuracyDoubleAtter(features);
         else
            return -1;
      }
   }

   
   /************************************************************************
    ************************************************************************/
   void print()
   {
      cout << "Me: \n";
      cout << "Property: " << indexOfPropertyChosen <<  " : "  << indexOfPropertyChosen2 << endl;
      
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
         
         if (indexOfPropertyChosen2 != -1)
         {
            int n1 = (features.valueCount(indexOfPropertyChosen) == 0 ? 2  : features.valueCount(indexOfPropertyChosen));
            int n2 = (features.valueCount(indexOfPropertyChosen2) == 0 ? 2 : features.valueCount(indexOfPropertyChosen2));
            cout << "CombPropertiy: [" << i / n2 << "," << i%n2 << "]" << endl;
         }
         
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
      if (indexOfPropertyChosen  != -1) cerr << "Property: " << RED << features.attrName(indexOfPropertyChosen)  << RESET;
      if (indexOfPropertyChosen2 != -1) cerr << RED << " & "        << features.attrName(indexOfPropertyChosen2) << RESET;
      if (indexOfChosenClass     != -1) cerr << " Class: " << BLUE  << indexOfChosenClass << RESET;
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
   int indexOfPropertyChosen2; // If not -1 then Accuracy is using to attribute selection.
   
   double currentEntropy;  //Entropy at just the current node
   double currentAccuracy;
   
   vector<DecisionTreeNode*> children;
   
};
