#ifndef TIMEMEASUREMENT_H
#define TIMEMEASUREMENT_H


#include <string>
#include <vector>

enum class BaseTimeUnit {NANOSECONDS, MICROSECONDS, MILLISECONDS, SECONDS};
enum class ExtendedTimeUnit {MIN_SEC};

//+++++++++++++++++++
// Class TimeDuration
//+++++++++++++++++++

class TimeDuration
{
  friend class Timer;
  friend class TimeDurationSeries;

  public:
    // Default constructor that creates and initializes a TimeDuration object.
    TimeDuration();

    //Copy constructor that copies the TimeDuration object "rhs" to the current TimeDuration object.
    TimeDuration(const TimeDuration& rhs);

    // Move constructor that moves the TimeDuration object "rhs" to the current TimeDuration object.
    TimeDuration(TimeDuration&& rhs);

    // Destructor that frees the needed memory resources of the current TimeDuration object.
    ~TimeDuration();

    // Copy assignment operator that copies the TimeDuration object "rhs" to the current TimeDuration object.
    TimeDuration& operator = (const TimeDuration& rhs);

    // Move assignment operator that moves the TimeDuration object "rhs" to the current TimeDuration object.
    TimeDuration& operator = (TimeDuration&& rhs);

    // Method that converts a TimeDurationObject into a double with respect to a given base time unit
    double convertTo(const BaseTimeUnit& unit);

    // Method that converts a TimeDurationObject into a string with respect to a given base time unit
    std::string convertToString(const BaseTimeUnit& unit);

    // Method that converts a TimeDurationObject into a string with respect to a given extended time unit
    std::string convertToString(const ExtendedTimeUnit& unit);

  private:
    // Forward declaration of a struct for the hidden implementation of TimeDuration objects.
    struct TimeDurationImplementation;
    // Forward declaration of an opaque pointer.
    TimeDurationImplementation* timeDurationHandle;
};


//++++++++++++
// Class Timer
//++++++++++++

class Timer
{
  public:
    // Default constructor that creates and initializes a Timer object.
    Timer();

    //Copy constructor that copies the Timer object "rhs" to the current Timer object.
    Timer(const Timer& rhs);

    // Move constructor that moves the Timer object "rhs" to the current Timer object.
    Timer(Timer&& rhs);

    // Destructor that frees the needed memory resources of the current Timer object.
    ~Timer();

    // Copy assignment operator that copies the Timer object "rhs" to the current Timer object.
    Timer& operator = (const Timer& rhs);

    // Move assignment operator that moves the Timer object "rhs" to the current Timer object.
    Timer& operator = (Timer&& rhs);

    // Method that sets the start time of a Timer object
    void setStartTime();

    // Method that sets the stop time of a Timer object
    void setStopTime();

    // Method that returns the time elapsed since the last start of the timer object. The timer
    // object can have stopped or is still running.
    TimeDuration getElapsedTime();

  private:
    // Forward declaration of a struct for the hidden implementation of Timer objects.
    struct TimerImplementation;
    // Forward declaration of an opaque pointer.
    TimerImplementation* timerHandle;
};

//+++++++++++++++++++++++++
// Class TimeDurationSeries
//+++++++++++++++++++++++++

class TimeDurationSeries
{
  public:
    // Default constructor that creates and initializes a TimeDurationSeries object.
    TimeDurationSeries();

    // Copy constructor that copies the TimeDurationSeries object "rhs" to the current TimeDurationSeries object.
    TimeDurationSeries(const TimeDurationSeries& rhs);

    // Move constructor that moves the TimeDurationSeries object "rhs" to the current TimeDurationSeries object.
    TimeDurationSeries(TimeDurationSeries&& rhs);

    // Destructor that frees the needed memory resources of the current TimeDurationSeries object.
    ~TimeDurationSeries();

    // Copy assignment operator that copies the TimeDurationSeries object "rhs" to the current TimeDurationSeries
    // object.
    TimeDurationSeries& operator = (const TimeDurationSeries& rhs);

    // Move assignment operator that moves the TimeDurationSeries object "rhs" to the current TimeDurationSeries
    // object.
    TimeDurationSeries& operator = (TimeDurationSeries&& rhs);

    // Method that adds a TimeDuration object to a TimeDurationSeries object
    void addDuration(const TimeDuration& rhs);

    // Method that calculates the mean duration of a TimeDurationSeries object
    TimeDuration calculateMean();

    // Method that calculates the variance of a TimeDurationSeries object
    TimeDuration calculateVariance();

    // Method that calculates the standard deviation of a TimeDurationSeries object
    TimeDuration calculateStandardDeviation();

    // Method that writes a TimeDurationSeries object into a string object as a comma separated sequence of time
    // durations for a later use in a CSV file. Each time duration is expressed with respect to the unit "timeUnit".
    std::string writeAsACommaSeparatedList(const BaseTimeUnit& timeUnit);

  private:
    // Forward declaration of a struct for the hidden implementation of a TimeDurationSeries object.
    struct TimeDurationSeriesImplementation;
    // Forward declaration of an opaque pointer.
    TimeDurationSeriesImplementation* timeDurationSeriesHandle;
};

//++++++++++++++++++++++++++++++
// Class AlgorithmRuntimeManager
//++++++++++++++++++++++++++++++

class AlgorithmRuntimeManager
{
  public:
    // Default constructor that creates and initializes an AlgorithmRuntimeManager object. The first parameter
    // indicates the number of algorithms that have to be handled. The second parameter is assumed to provide a sorted
    // list of positive integers that indicate the number of elements for which the algorithms are tested. Correctness
    // of the input is assumed and not checked.
    AlgorithmRuntimeManager(size_t numberOfAlgorithms, const std::vector<size_t>& numberOfElementsVector);

    // Copy constructor that copies the AlgorithmRuntimeManager object "rhs" to the current AlgorithmRuntimeManager
    // object.
    AlgorithmRuntimeManager(const AlgorithmRuntimeManager& rhs);

    // Move constructor that moves the AlgorithmRuntimeManager object "rhs" to the current AlgorithmRuntimeManager
    // object.
    AlgorithmRuntimeManager(AlgorithmRuntimeManager&& rhs);

    // Destructor that frees the needed memory resources of the current AlgorithmRuntimeManager object.
    ~AlgorithmRuntimeManager();

    // Copy assignment operator that copies the AlgorithmRuntimeManager object "rhs" to the current
    // AlgorithmRuntimeManager object.
    AlgorithmRuntimeManager& operator = (const AlgorithmRuntimeManager& rhs);

    // Move assignment operator that moves the AlgorithmRuntimeManager object "rhs" to the current
    // AlgorithmRuntimeManager object.
    AlgorithmRuntimeManager& operator = (AlgorithmRuntimeManager&& rhs);

    // Method that adds a TimeDuration object to an AlgorithmRuntimeManager object with respect to the algorithm number
    // "algorithmNumber" and the number of elements "".
    void addDuration(size_t algorithmNumber, size_t numberOfElements, const TimeDuration& rhs);

    // Method that writes the contents of an AlgorithmRuntimeManager object into a CSV file with name "filename.csv".
    // The CSV file starts with the lines in "header". Runtimes are expressed in terms of the unit "timeUnit".
    void writeToCSVFile(const std::string& fileName, const std::string& header, const BaseTimeUnit& timeUnit,
                        const std::vector<std::string>& ConvexHullAlgorithms);

  private:
    // Forward declaration of a struct for the hidden implementation of an AlgorithmRuntimeManager object.
    struct AlgorithmRuntimeManagerImplementation;
    // Forward declaration of an opaque pointer.
    AlgorithmRuntimeManagerImplementation* algorithmRuntimeManagerHandle;
};

#endif //TIMEMEASUREMENT_H
