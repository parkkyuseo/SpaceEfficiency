
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream> // for testing purposes
#include <cmath>
#include <fstream>
#include <list>
#include <numeric>  // std::accumulate
#include <sstream>
#include <string>
#include <vector>
#include "TimeMeasurement.h"

//++++++++++++++++
// Data structures
//++++++++++++++++

// Data structure for keeping Timer objects
struct Timer::TimerImplementation
{
  std::chrono::steady_clock::time_point startTime;
  std::chrono::steady_clock::time_point stopTime;
  bool running;
};

struct TimeDuration::TimeDurationImplementation
{
  std::chrono::steady_clock::duration elapsedTime;
};

struct TimeDurationSeries::TimeDurationSeriesImplementation
{
  std::vector<std::chrono::steady_clock::duration> series;
  bool meanDurationIsUpToDate = false;
  std::chrono::steady_clock::duration meanDuration = std::chrono::steady_clock::duration::zero();
  bool varianceDurationIsUpToDate = false;
  std::chrono::steady_clock::duration varianceDuration = std::chrono::steady_clock::duration::zero();
  bool standardDeviationIsUpToDate = false;
  std::chrono::steady_clock::duration standardDeviationDuration = std::chrono::steady_clock::duration::zero();
};

struct AlgorithmRuntimesForTheSameNumberOfElements
{
  size_t numberOfElements;
  std::vector<TimeDurationSeries> series;
};

struct AlgorithmRuntimeManager::AlgorithmRuntimeManagerImplementation
{
  std::list<AlgorithmRuntimesForTheSameNumberOfElements> runTimes;
};


//+++++++++++++++++++
// Class TimeDuration
//+++++++++++++++++++

// Default constructor that creates and initializes a TimeDuration object.
TimeDuration::TimeDuration()
{
  timeDurationHandle = new TimeDurationImplementation;
  timeDurationHandle->elapsedTime = std::chrono::steady_clock::duration::zero();
}

//Copy constructor that copies the TimeDuration object "rhs" to the current TimeDuration object.
TimeDuration::TimeDuration(const TimeDuration& rhs)
{
  timeDurationHandle = new TimeDurationImplementation;
  *timeDurationHandle = *rhs.timeDurationHandle;
}

// Move constructor that moves the TimeDuration object "rhs" to the current TimeDuration object.
TimeDuration::TimeDuration(TimeDuration&& rhs)
{
  // Move the value of "rhs" to "this".
  timeDurationHandle = rhs.timeDurationHandle;
  // Initialize "rhs" with nullptr.
  rhs.timeDurationHandle = nullptr;
}

// The destructor frees the needed memory resources of the current TimeDuration object.
TimeDuration::~TimeDuration()
{
  delete timeDurationHandle;
}

// Copy assignment operator that copies the TimeDuration object "rhs" to the current TimeDuration object.
TimeDuration& TimeDuration::operator = (const TimeDuration& rhs)
{
  if (this != &rhs) *timeDurationHandle = *rhs.timeDurationHandle;
  return *this;
}

// Move assignment operator that moves the TimeDuration object "rhs" to the current TimeDuration object.
TimeDuration& TimeDuration::operator = (TimeDuration&& rhs)
{
  if (this != &rhs)
  {
    // Free the current TimeDuration object "*this".
    delete timeDurationHandle;
    // Move the value of "rhs" to "this".
    timeDurationHandle = rhs.timeDurationHandle;
    // Initialize "rhs" with nullptr.
    rhs.timeDurationHandle = nullptr;
  }

  return *this;
}

// Method that converts a TimeDurationObject into a double with respect to a given base time unit
double TimeDuration::convertTo(const BaseTimeUnit& unit)
{
  double duration;

  duration = std::chrono::duration_cast<std::chrono::nanoseconds>(timeDurationHandle->elapsedTime).count();

  switch(unit)
  {
    case BaseTimeUnit::NANOSECONDS  : return duration;              break;
    case BaseTimeUnit::MICROSECONDS : return duration/1000.0;       break;
    case BaseTimeUnit::MILLISECONDS : return duration/1000000.0;    break;
    case BaseTimeUnit::SECONDS      : return duration/1000000000.0; break;
  }

  return duration;
}

// Method that converts a TimeDurationObject into a string with respect to a given base time unit
std::string TimeDuration::convertToString(const BaseTimeUnit& unit)
{
  double duration;
  std::ostringstream convert;

  duration = std::chrono::duration_cast<std::chrono::nanoseconds>(timeDurationHandle->elapsedTime).count();

  switch(unit)
  {
    case BaseTimeUnit::NANOSECONDS  :                           break;
    case BaseTimeUnit::MICROSECONDS : duration /= 1000.0;       break;
    case BaseTimeUnit::MILLISECONDS : duration /= 1000000.0;    break;
    case BaseTimeUnit::SECONDS      : duration /= 1000000000.0; break;
  }

  convert << std::fixed << duration;
  return convert.str();
}

// Method that converts a TimeDurationObject into a string with respect to a given extended time unit
std::string TimeDuration::convertToString(const ExtendedTimeUnit& unit)
{
  double duration, seconds;
  int64_t minutes;
  std::ostringstream convert;

  duration = std::chrono::duration_cast<std::chrono::nanoseconds>(timeDurationHandle->elapsedTime).count();

  switch(unit)
  {
    case ExtendedTimeUnit::MIN_SEC :
      minutes = int64_t(duration / 60000000000.0);
      seconds = (duration - minutes * 60000000000.0) / 1000000000;
      convert << minutes << "m " << seconds << "s";
      break;
  }

  return convert.str();
}


//++++++++++++
// Class Timer
//++++++++++++

// Default constructor that creates and initializes a Timer object.
Timer::Timer()
{
  timerHandle = new TimerImplementation;
  timerHandle->startTime = timerHandle->stopTime = std::chrono::steady_clock::now();
  timerHandle->running = false;
}

//Copy constructor that copies the Timer object "rhs" to the current Timer object.
Timer::Timer(const Timer& rhs)
{
  timerHandle = new TimerImplementation;
  *timerHandle = *rhs.timerHandle;
}

// Move constructor that moves the Timer object "rhs" to the current Timer object.
Timer::Timer(Timer&& rhs)
{
  // Move the value of "rhs" to "this".
  timerHandle = rhs.timerHandle;
  // Initialize "rhs" with nullptr.
  rhs.timerHandle = nullptr;
}

// The destructor frees the needed memory resources of the current Timer object.
Timer::~Timer()
{
  delete timerHandle;
}

// Copy assignment operator that copies the Timer object "rhs" to the current Timer object.
Timer& Timer::operator = (const Timer& rhs)
{
  if (this != &rhs) *timerHandle = *rhs.timerHandle;
  return *this;
}

// Move assignment operator that moves the Timer object "rhs" to the current Timer object.
Timer& Timer::operator = (Timer&& rhs)
{
  if (this != &rhs)
  {
    // Free the current Timer object "*this".
    delete timerHandle;
    // Move the value of "rhs" to "this".
    timerHandle = rhs.timerHandle;
    // Initialize "rhs" with nullptr.
    rhs.timerHandle = nullptr;
  }

  return *this;
}

// Method that sets the start time of a Timer object
void Timer::setStartTime()
{
  timerHandle->running = true;
  timerHandle->startTime = std::chrono::steady_clock::now();
}

// Method that sets the stop time of a Timer object
void Timer::setStopTime()
{
  timerHandle->stopTime = std::chrono::steady_clock::now();
  timerHandle->running = false;
}

// Method that returns the time elapsed since the last start of the timer object. The timer object can
// have stopped or is still running.
TimeDuration Timer::getElapsedTime()
{
  TimeDuration period;

  if (timerHandle->running)
    period.timeDurationHandle->elapsedTime = std::chrono::steady_clock::now() - timerHandle->startTime;
  else
    period.timeDurationHandle->elapsedTime = timerHandle->stopTime - timerHandle->startTime;

  return period;
}

//+++++++++++++++++++++++++
// Class TimeDurationSeries
//+++++++++++++++++++++++++

// Default constructor that creates and initializes a TimeDurationSeries object.
TimeDurationSeries::TimeDurationSeries()
{
  timeDurationSeriesHandle = new TimeDurationSeriesImplementation;
}

//Copy constructor that copies the TimeDurationSeries object "rhs" to the current TimeDurationSeries object.
TimeDurationSeries::TimeDurationSeries(const TimeDurationSeries& rhs)
{
  timeDurationSeriesHandle = new TimeDurationSeriesImplementation;
  *timeDurationSeriesHandle = *rhs.timeDurationSeriesHandle;
}

// Move constructor that moves the TimeDurationSeries object "rhs" to the current TimeDurationSeries object.
TimeDurationSeries::TimeDurationSeries(TimeDurationSeries&& rhs)
{
  // Move the value of "rhs" to "this".
  timeDurationSeriesHandle = rhs.timeDurationSeriesHandle;
  // Initialize "rhs" with nullptr.
  rhs.timeDurationSeriesHandle = nullptr;
}

// Destructor that frees the needed memory resources of the current TimeDurationSeries object.
TimeDurationSeries::~TimeDurationSeries()
{
  delete timeDurationSeriesHandle;
}

// Copy assignment operator that copies the TimeDurationSeries object "rhs" to the current TimeDurationSeries object.
TimeDurationSeries& TimeDurationSeries::operator = (const TimeDurationSeries& rhs)
{
  if (this != &rhs) *timeDurationSeriesHandle = *rhs.timeDurationSeriesHandle;
  return *this;
}

// Move assignment operator that moves the TimeDurationSeries object "rhs" to the current TimeDurationSeries object.
TimeDurationSeries& TimeDurationSeries::operator = (TimeDurationSeries&& rhs)
{
  if (this != &rhs)
  {
    // Free the current TimeDurationSeries object "*this".
    delete timeDurationSeriesHandle;
    // Move the value of "rhs" to "this".
    timeDurationSeriesHandle = rhs.timeDurationSeriesHandle;
    // Initialize "rhs" with nullptr.
    rhs.timeDurationSeriesHandle = nullptr;
  }

  return *this;
}

// Method that adds a TimeDuration object to a TimeDurationSeries object
void TimeDurationSeries::addDuration(const TimeDuration& rhs)
{
  timeDurationSeriesHandle->series.push_back(rhs.timeDurationHandle->elapsedTime);
  timeDurationSeriesHandle->meanDurationIsUpToDate = false;
  timeDurationSeriesHandle->varianceDurationIsUpToDate = false;
  timeDurationSeriesHandle->standardDeviationIsUpToDate = false;
}

// Method that calculates the mean duration of a TimeDurationSeries object
TimeDuration TimeDurationSeries::calculateMean()
{
  TimeDuration mean;

  if (!timeDurationSeriesHandle->meanDurationIsUpToDate)
  {
    // Sum up all durations of the series.
    mean.timeDurationHandle->elapsedTime = std::accumulate(timeDurationSeriesHandle->series.begin(),
                                                           timeDurationSeriesHandle->series.end(),
                                                           std::chrono::steady_clock::duration::zero());
    // Divide the sum by the number of durations.
    mean.timeDurationHandle->elapsedTime /= timeDurationSeriesHandle->series.size();

    // Mean duration is computed. Set flag "meanDurationIsUpToDate".
    timeDurationSeriesHandle->meanDurationIsUpToDate = true;
    // Store mean duration since it will be needed for variance and standard deviation.
    timeDurationSeriesHandle->meanDuration = mean.timeDurationHandle->elapsedTime;
  }
  else
    mean.timeDurationHandle->elapsedTime = timeDurationSeriesHandle->meanDuration;
  
  return mean;
}

// Method that calculates the variance of a TimeDurationSeries object
TimeDuration TimeDurationSeries::calculateVariance()
{
  TimeDuration variance;

  if (!timeDurationSeriesHandle->varianceDurationIsUpToDate)
  {
    if (!timeDurationSeriesHandle->meanDurationIsUpToDate)
    {
      TimeDuration mean;

      mean = this->calculateMean();
      timeDurationSeriesHandle->meanDurationIsUpToDate = true;
      timeDurationSeriesHandle->meanDuration = mean.timeDurationHandle->elapsedTime;
    }

    // Compute variance v = (sum_1^n (X - mean)^2) / n
    // Step 1: Initialization of the summation process
    std::chrono::steady_clock::duration diff;
    variance.timeDurationHandle->elapsedTime = std::chrono::steady_clock::duration::zero();
    // Step 2: Compute sum_1^n (X - mean)^2. A problem is that the operator '*' is not defined in the
    // class "duration". Therefore, a cast of one argument is needed.
    for (unsigned i = 0; i <= timeDurationSeriesHandle->series.size() - 1; ++i)
    {
      // Compute the difference "diff" between a vector element and the mean. This difference must always be positive.
      if (timeDurationSeriesHandle->series[i] >= timeDurationSeriesHandle->meanDuration)
        diff = timeDurationSeriesHandle->series[i] - timeDurationSeriesHandle->meanDuration;
      else
        diff = timeDurationSeriesHandle->meanDuration - timeDurationSeriesHandle->series[i];
      // Add the square of "diff"
      variance.timeDurationHandle->elapsedTime +=
        diff * std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count();
    }
    // Step 3: Divide by n.
    variance.timeDurationHandle->elapsedTime /= timeDurationSeriesHandle->series.size();

    // Variance duration is computed. Set flag "varianceDurationIsUpToDate".
    timeDurationSeriesHandle->varianceDurationIsUpToDate = true;
    // Store variance duration since it will be needed for standard deviation.
    timeDurationSeriesHandle->varianceDuration = variance.timeDurationHandle->elapsedTime;
  }
  else
    variance.timeDurationHandle->elapsedTime = timeDurationSeriesHandle->varianceDuration;
  
  return variance;
}

// Method that calculates the standard deviation of a TimeDurationSeries object.
TimeDuration TimeDurationSeries::calculateStandardDeviation()
{
  TimeDuration standardDeviation;

  if (!timeDurationSeriesHandle->standardDeviationIsUpToDate)
  {
    if (!timeDurationSeriesHandle->varianceDurationIsUpToDate)
    {
      TimeDuration variance;

      variance = this->calculateVariance();
      timeDurationSeriesHandle->varianceDurationIsUpToDate = true;
      timeDurationSeriesHandle->varianceDuration = variance.timeDurationHandle->elapsedTime;
    }

    // Compute standard deviation.
    std::chrono::nanoseconds one(1);
    double stdv = std::sqrt(std::chrono::duration_cast<std::chrono::nanoseconds>(timeDurationSeriesHandle->varianceDuration).count());
    timeDurationSeriesHandle->standardDeviationDuration = one * int64_t(stdv);

    // Standard deviation duration is computed. Set flag "standardDeviationDurationIsUpToDate".
    timeDurationSeriesHandle->standardDeviationIsUpToDate = true;
  }

  standardDeviation.timeDurationHandle->elapsedTime = timeDurationSeriesHandle->standardDeviationDuration;

  return standardDeviation;
}

// Method that writes a TimeDurationSeries object into a string object as a comma separated sequence of time
// durations for a later use in a CSV file. Each time duration is expressed with respect to the unit "timeUnit".
std::string TimeDurationSeries::writeAsACommaSeparatedList(const BaseTimeUnit& timeUnit)
{
  if (timeDurationSeriesHandle->series.empty())
    return std::string();

  double duration;
  std::ostringstream convert;
  calculateMean();

  for (unsigned i = 0; i <= timeDurationSeriesHandle->series.size(); ++i)
  {
    if (i != 0) convert << ",";

    if (i == timeDurationSeriesHandle->series.size())
    {
      duration = std::chrono::duration_cast<std::chrono::nanoseconds>(timeDurationSeriesHandle->meanDuration).count();
    }
    
    else 
    {
      // Convert the current duration object into a string and add it to "str".
      duration = std::chrono::duration_cast<std::chrono::nanoseconds>(timeDurationSeriesHandle->series[i]).count();
    }
    

    switch(timeUnit)
    {
      case BaseTimeUnit::NANOSECONDS  :                           break;
      case BaseTimeUnit::MICROSECONDS : duration /= 1000.0;       break;
      case BaseTimeUnit::MILLISECONDS : duration /= 1000000.0;    break;
      case BaseTimeUnit::SECONDS      : duration /= 1000000000.0; break;
    }

    convert << std::fixed << duration;
  }
  return convert.str();
}

//++++++++++++++++++++++++++++++
// Class AlgorithmRuntimeManager
//++++++++++++++++++++++++++++++

// Default constructor that creates and initializes an AlgorithmRuntimeManager object. The first parameter indicates
// the number of algorithms that have to be handled. The second parameter is assumed to provide a vector as a sorted
// list of positive integers that indicate the number of elements for which the algorithms are tested. Correctness of
// the input is assumed and not checked. The vector is transformed into a list. Each list element keeps the number
// of elements to which an algorithms is applied as well as a vector that later stores a time duration series for each
// algorithm.
AlgorithmRuntimeManager::AlgorithmRuntimeManager(size_t numberOfAlgorithms,
                                                 const std::vector<size_t>& numberOfElementsVector)
{
  algorithmRuntimeManagerHandle = new AlgorithmRuntimeManagerImplementation;

  AlgorithmRuntimesForTheSameNumberOfElements element;

  // Create a list where each list element keeps the number of elements for which the algorithms are tested and
  // reserves vector space for the time durtion series for each algorithm.
  for(std::vector<size_t>::const_iterator it = numberOfElementsVector.cbegin();
      it != numberOfElementsVector.cend();
      ++it)
  {
    element.numberOfElements = *it;
    element.series.resize(numberOfAlgorithms + 1);
    algorithmRuntimeManagerHandle->runTimes.push_back(element);
  }
}

//Copy constructor that copies the AlgorithmRuntimeManager object "rhs" to the current AlgorithmRuntimeManager object.
AlgorithmRuntimeManager::AlgorithmRuntimeManager(const AlgorithmRuntimeManager& rhs)
{
  algorithmRuntimeManagerHandle = new AlgorithmRuntimeManagerImplementation;
  *algorithmRuntimeManagerHandle = *rhs.algorithmRuntimeManagerHandle;
}

// Move constructor that moves the AlgorithmRuntimeManager object "rhs" to the current AlgorithmRuntimeManager object.
AlgorithmRuntimeManager::AlgorithmRuntimeManager(AlgorithmRuntimeManager&& rhs)
{
  // Move the value of "rhs" to "this".
  algorithmRuntimeManagerHandle = rhs.algorithmRuntimeManagerHandle;
  // Initialize "rhs" with nullptr.
  rhs.algorithmRuntimeManagerHandle = nullptr;
}

// Destructor that frees the needed memory resources of the current AlgorithmRuntimeManager object.
AlgorithmRuntimeManager::~AlgorithmRuntimeManager()
{
  delete algorithmRuntimeManagerHandle;
}

// Copy assignment operator that copies the AlgorithmRuntimeManager object "rhs" to the current
// AlgorithmRuntimeManager object.
AlgorithmRuntimeManager& AlgorithmRuntimeManager::operator = (const AlgorithmRuntimeManager& rhs)
{
  if (this != &rhs) *algorithmRuntimeManagerHandle = *rhs.algorithmRuntimeManagerHandle;
  return *this;
}

// Move assignment operator that moves the AlgorithmRuntimeManager object "rhs" to the current
// AlgorithmRuntimeManager object.
AlgorithmRuntimeManager& AlgorithmRuntimeManager::operator = (AlgorithmRuntimeManager&& rhs)
{
  if (this != &rhs)
  {
    // Free the current AlgorithmRuntimeManager object "*this".
    delete algorithmRuntimeManagerHandle;
    // Move the value of "rhs" to "this".
    algorithmRuntimeManagerHandle = rhs.algorithmRuntimeManagerHandle;
    // Initialize "rhs" with nullptr.
    rhs.algorithmRuntimeManagerHandle = nullptr;
  }

  return *this;
}

// Method that adds a TimeDuration object to an AlgorithmRuntimeManager object with respect to the algorithm number
// "algorithmNumber" and the number of elements "".
void AlgorithmRuntimeManager::addDuration(size_t algorithmNumber, size_t numberOfElements, const TimeDuration& rhs)
{
  // Search list element that handles the time duration series of all algorithms for "numberOfElements" elements. 
  std::list<AlgorithmRuntimesForTheSameNumberOfElements>::iterator it =
      algorithmRuntimeManagerHandle->runTimes.begin();
  while(it != algorithmRuntimeManagerHandle->runTimes.end() && it->numberOfElements != numberOfElements)
    ++it;
  if (it == algorithmRuntimeManagerHandle->runTimes.end())
  {
    std::cout << "ERROR: Time duration sequence for algorithm number " << algorithmNumber << " and ";
    std::cout << numberOfElements << " elements does not exist!" << std::endl;
    return;
  }

  // "it" points to the right list element. Add the duration object "rhs" to the time duration series of algorithm
  // number "algorithmNumber".
  it->series[algorithmNumber].addDuration(rhs);
}

// Method that writes the contents of an AlgorithmRuntimeManager object into a CSV file with name "filename.csv". The
// CSV file starts with the lines in "header". Runtimes are expressed in terms of the unit "timeUnit".
void AlgorithmRuntimeManager::writeToCSVFile(const std::string& fileName,
                                             const std::string& header,
                                             const BaseTimeUnit& timeUnit,
                                             const std::vector<std::string>& ConvexHullAlgorithms)
{
  const std::string ending(".csv");
  std::string newFileName, str, csvStr;

  // Check correctness of file name (coarsely).
  if (fileName.size() > 0)
  {
    newFileName = fileName;
    if (!std::equal(ending.rbegin(), ending.rend(), newFileName.rbegin())) newFileName += ".csv";
  }
  else
  {
    newFileName = "test.csv";
    std::cout << "Incorrect file name! Selected file name is '" << newFileName << "'." << std::endl;
  }

  // Generate the string that contains the run times for all algorithms and has to be written into a CSV file.
  str = header;
  for (std::list<AlgorithmRuntimesForTheSameNumberOfElements>::iterator listIt =
                algorithmRuntimeManagerHandle->runTimes.begin();
       listIt != algorithmRuntimeManagerHandle->runTimes.end();
       ++listIt)
  {
    str += "\nNumber of elements: " + std::to_string(listIt->numberOfElements) + "\n";
    // Traverse the vector of time duration sequences. Begin with vector position 1 that corresponds to algorithm 1.
    // Add each time duration sequence as a comma separated list of time durations.
    for (unsigned int algorithmNumber = 1; algorithmNumber <= listIt->series.size()-1; ++algorithmNumber)
    {
      csvStr = listIt->series[algorithmNumber].writeAsACommaSeparatedList(timeUnit);
      if (!csvStr.empty())
        str += ConvexHullAlgorithms[algorithmNumber] + " (Alg. " +
               std::to_string(algorithmNumber) + ")," + csvStr + "\n";
    }
  }
  
  std::cout << str; // temp
  
  // Write the generated string into the desired file.
  std::ofstream out(newFileName);
  out << str;
  out.close();
}

