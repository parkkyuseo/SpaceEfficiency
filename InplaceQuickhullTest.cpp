
// Flags that indicate which convex hull algorithms are tested and compared. A flag equal to 0 means that an algorithm
// is not tested; otherwise, it is tested. At the same time, each number unequal to 0 is interpreted as an
// "algorithm number" that uniquely identifies the algorithm and can be used in other contexts.
#define CONVEX_HULL_QUICK_HULL              1     //  1 if tested, 0 if not tested
#define CONVEX_HULL_IN_PLACE_QUICK_HULL     2     //  2 if tested, 0 if not tested 
#define CONVEX_HULL_IN_PLACE_QUICK_HULL_2   3     //  3 if tested, 0 if not tested 
#define MAX_NUMBER_OF_CH_ALGORITHMS         3


// Depending on the flags above, the corresponding include files are loaded.
#if CONVEX_HULL_QUICK_HULL
#include "ConvexHullQuickHull.h"
#endif
#if CONVEX_HULL_IN_PLACE_QUICK_HULL
#include "ConvexHullQuickHull.h"
#endif
#if CONVEX_HULL_IN_PLACE_QUICK_HULL_2
#include "ConvexHullQuickHull.h"
#endif

#define CHT 1

#if CHT
#include <iostream>
#include <ostream>
#include <fstream>
#include <iomanip>
#endif
#include <string>
#include <vector>
#include "PointHandler.h"
#include "TimeMeasurement.h"

//***************************************
// Constants and constant data structures
//***************************************

// Constant for the number of times each convex hull algorithm is executed in order to average the execution times.
const size_t NUMBER_OF_RUNS = 20; // 20 runs intended

// Vector that keeps the numbers of points that will be consecutively generated and for which the performance of the
// convex hull algorithms will be tested. Note that it is assumed that the numbers are listed in increasing order in
// the vector.
const std::vector<size_t> numberOfPointsList {10000, 100000, 1000000, 10000000 , 100000000};

//************************************
// Printing in place quickhull result
//************************************
void printInplaceQuickhull(PointSequence::iterator it, PointSequence& resultOfInplaceQuickhull);

// Store the generated points to plot the zone configuration.
void storeGeneratedPointsToFiles(const PointSequence& pointSeq);

//*************
// Main program
//*************

int main()
{
  PointSequence copiedPointSeq;
  std::string fileName, header;
  std::vector<std::string> ConvexHullAlgorithmNames(MAX_NUMBER_OF_CH_ALGORITHMS + 1, "");
  AlgorithmRuntimeManager runtimeManager(MAX_NUMBER_OF_CH_ALGORITHMS, numberOfPointsList);
  BaseTimeUnit timeUnit;
  CCWPointSequence ccwPointSeq;
  #if CHT
  std::cout << "Allocating a point sequence for maximally " << numberOfPointsList[numberOfPointsList.size()-1]
            << " points begins ..." << std::endl;
  #endif
  PointSequence pointSeq(numberOfPointsList[numberOfPointsList.size() - 1]); // Maximally needed size
  #if CHT
  std::cout << "... and is completed now." << std::endl;
  #endif
  Timer timer;
  TimeDuration duration;
  std::vector<Point>::iterator it;

  for(size_t i = 0; i < numberOfPointsList.size(); i++)
  {
    for(size_t j = 0; j < NUMBER_OF_RUNS; j++)
    {
      #if CHT
      std::cout << "Point sequence size: " << numberOfPointsList[i] << ", run number: " << j << std::endl;
      std::cout << "Generating random point sequence without duplicates ..." << std::endl;
      #endif

      // Create a sequence with randomly generated points.
      generateRandomPoints(pointSeq, numberOfPointsList[i]);

      // Create a sequence with randomly generated points. Points are placed in a shape of a Circle. 
      /* generateCircledPointSequence(pointSeq, numberOfPointsList[i]); */

      #if CHT
      std::cout << "... completed with " << pointSeq.size() << " points." << std::endl;
      printPointSequence("The input point sequence has", "points. Its points are:", pointSeq);
      std::cout << std::endl;
      #endif

      #if CONVEX_HULL_QUICK_HULL
      ConvexHullAlgorithmNames[CONVEX_HULL_QUICK_HULL].assign("Quickhull algorithm"); 
      #if CHT
      std::cout << "Quickhull algorithm begins ... " << std::endl;
      #endif
      copiedPointSeq.clear();
      copiedPointSeq = pointSeq;
      timer.setStartTime();
      ccwPointSeq = ConvexHullQuickHull(copiedPointSeq);
      timer.setStopTime();
      duration = timer.getElapsedTime();
      runtimeManager.addDuration(CONVEX_HULL_QUICK_HULL, numberOfPointsList[i], duration);
      #if CHT
      std::cout << "... and is completed now in " << duration.convertToString(BaseTimeUnit::MILLISECONDS)
                << " milliseconds." << std::endl;
      /* printPointSequence("The convex hull contains", "points. The counterclockwise point sequence is:",
       *                    ccwPointSeq); */
      #endif
      #endif

      #if CONVEX_HULL_IN_PLACE_QUICK_HULL
      ConvexHullAlgorithmNames[CONVEX_HULL_IN_PLACE_QUICK_HULL].assign("In place Quickhull algorithm"); 
      #if CHT
      std::cout << "In place Quickhull algorithm begins ... " << std::endl;
      #endif
      copiedPointSeq.clear();
      copiedPointSeq = pointSeq;
      timer.setStartTime();
      it = ConvexHullInPlaceQuickHull(copiedPointSeq);
      timer.setStopTime();
      duration = timer.getElapsedTime();
      runtimeManager.addDuration(CONVEX_HULL_IN_PLACE_QUICK_HULL, numberOfPointsList[i], duration);
      #if CHT
      std::cout << "... and is completed now in " << duration.convertToString(BaseTimeUnit::MILLISECONDS)
                << " milliseconds." << std::endl;
      /* printInplaceQuickhull(it, copiedPointSeq); */
      /* printPointSequence("The convex hull contains", "points. The counterclockwise point sequence is:",
       *                    ccwPointSeq); */
      #endif
      #endif

      #if CONVEX_HULL_IN_PLACE_QUICK_HULL_2
      ConvexHullAlgorithmNames[CONVEX_HULL_IN_PLACE_QUICK_HULL_2].assign("In place Quickhull-2 algorithm"); 
      #if CHT
      std::cout << "In place Quickhull-2 algorithm begins ... " << std::endl;
      #endif
      copiedPointSeq.clear();
      copiedPointSeq = pointSeq;
      timer.setStartTime();
      it = TheirConvexHullInPlaceQuickHull(copiedPointSeq);
      timer.setStopTime();
      duration = timer.getElapsedTime();
      runtimeManager.addDuration(CONVEX_HULL_IN_PLACE_QUICK_HULL_2, numberOfPointsList[i], duration);
      #if CHT
      std::cout << "... and is completed now in " << duration.convertToString(BaseTimeUnit::MILLISECONDS)
                << " milliseconds." << std::endl;
      /* printInplaceQuickhull(it, copiedPointSeq); */
      /* printPointSequence("The convex hull contains", "points. The counterclockwise point sequence is:",
       *                    ccwPointSeq); */
      #endif
      #endif
      /* storeGeneratedPointsToFiles(pointSeq); */

    }
  }

  // Write the collected runtime information into a CSV file.
  fileName.assign("ConvexHullAlgorithmsTest.csv");
  header.assign("Performance Test of Selected Convex Hull Algorithms\n"
                "(Runtimes are provided in milliseconds)\n");
  timeUnit = BaseTimeUnit::MILLISECONDS;
  runtimeManager.writeToCSVFile(fileName, header, timeUnit, ConvexHullAlgorithmNames);
  
  return 0;
}
void printInplaceQuickhull(PointSequence::iterator it, PointSequence& resultOfInplaceQuickhull)
{
  for (auto i = resultOfInplaceQuickhull.begin(); i != it; ++i)
    std::cout << *i << ", ";

  std::cout << "\n\n";
}

// Store the generated points to plot the zone configuration.
void storeGeneratedPointsToFiles(const PointSequence& pointSeq)
{
  std::string xPoints = "generatedXPoints.txt";
  std::string yPoints = "generatedYPoints.txt";
  std::ofstream pointsX(xPoints);
  std::ofstream pointsY(yPoints);
  pointsX << std::fixed << std::setprecision(2) << std::endl;
  pointsY << std::fixed << std::setprecision(2) << std::endl;
  for(size_t i = 0 ; i < pointSeq.size(); i++)
  {
    pointsX << pointSeq[i].x <<  std::endl;
    pointsY << pointSeq[i].y <<  std::endl;
  }
  pointsX.close();
  pointsY.close();

}
