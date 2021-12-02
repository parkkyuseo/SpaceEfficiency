
#include <algorithm>    // std::sort
#include <iomanip>
#include <iostream>
#include <ostream>
#include <random>
#include <string>    // to be deleted later
#include <vector>
#include "Number.h"
#include "PointHandler.h"

//
// Constructors and destructor
//
// Empty constructor: Initializes data members x and y with 0
Point::Point() : x(0.0), y(0.0) {}

// Constructor: Initializes data members x and y with Number values x and y
Point::Point(const Number& x, const Number& y) : x(x), y(y) {} 

// Copy constructor: Copies point p to *this
Point::Point(const Point& p) : x(p.x), y(p.y) {}

// Destructor
Point::~Point() {}

//
// Comparison operators based on the lexicographical order on points
//
bool Point::operator < (const Point& rhs) const
{
  return (x < rhs.x || (x == rhs.x && y < rhs.y));
}

bool Point::operator <= (const Point& rhs) const
{
  return (x < rhs.x || (x == rhs.x && y <= rhs.y));
}

bool Point::operator > (const Point& rhs) const
{
  return (x > rhs.x || (x == rhs.x && y > rhs.y));
}

bool Point::operator == (const Point& rhs) const
{
  return (x == rhs.x && y == rhs.y);
}

bool Point::operator != (const Point& rhs) const
{
  return (x != rhs.x || y != rhs.y);
}

// Assignment operator
Point& Point::operator = (const Point& rhs)
{
  x = rhs.x;
  y = rhs.y;

  return *this;
}

// Output method
std::ostream& operator << (std::ostream& os, const Point& p)
{
  os << std::fixed << std::setprecision(2) << "(" << p.x << ", " << p.y << ")";
  return os;
}

// Method that for three points p, q, and r finds the orientation of the ordered triplet (p, q, r). The method returns
// the value COLLINEAR if p, q, and r are collinear, the value COUNTERCLOCKWISE if p, q, and r are oriented
// counterclockwise, and CLOCKWISE if p, q, and r are oriented clockwise. Another view is that the method checks
// whether r is on (COLLINEAR), left of/above (COUNTERCLOCKWISE), or right of/below (CLOCKWISE) a directed line defined
// by the directed segment from p to q.
Orientation getOrientation(const Point& p, const Point& q, const Point& r)
{
  // Compute the value of the crossproduct.
  Number value = (q.x - p.x) * (r.y - p.y) - (q.y - p.y) * (r.x - p.x);

  if (value == 0)
    return Orientation::COLLINEAR;
  else
    return (value < 0) ? Orientation::CLOCKWISE : Orientation::COUNTERCLOCKWISE;
}
Orientation getOrientationWithArea(const Point& p, const Point& q, const Point& r, Number& area)
{
  // Compute the value of the crossproduct.
  area = (q.x - p.x) * (r.y - p.y) - (q.y - p.y) * (r.x - p.x);

  if (area == 0)
    return Orientation::COLLINEAR;
  else
  {
    if (area < 0)
    {
      area = area * (-1);
      return Orientation::CLOCKWISE;
    }
    else
      return Orientation::COUNTERCLOCKWISE;
    /* return (area < 0) ? Orientation::CLOCKWISE : Orientation::COUNTERCLOCKWISE; */
  }
}

// Method that checks whether the distance of a point p to a point q is smaller (DISTANCETOFIRSTPOINTISSMALLER),
// larger(DISTANCETOSECONDPOINTISSMALLER), or equal (EQUALDISTANCE) than the distance of the point p to the point r.
CompareDistance compareDistance(const Point& p, const Point& q, const Point& r)
{
  Number x1Dist = p.x - q.x;
  Number y1Dist = p.y - q.y;
  Number x2Dist = p.x - r.x;
  Number y2Dist = p.y - r.y;

  Number distanceSquare1 = x1Dist * x1Dist + y1Dist * y1Dist;
  Number distanceSquare2 = x2Dist * x2Dist + y2Dist * y2Dist;

  if (distanceSquare1 < distanceSquare2)
    return CompareDistance::DISTANCETOFIRSTPOINTISSMALLER;
  else if (distanceSquare1 > distanceSquare2)
    return CompareDistance::DISTANCETOSECONDPOINTISSMALLER;
  else
    return CompareDistance::EQUALDISTANCE;
}

// Method that randomly generates "numberOfPoints" points without duplicates and stores them as a point sequence. 
void generateRandomPoints(PointSequence& pointSeq, size_t numberOfPoints)
{
  constexpr size_t NoOfDecimalPlacesMultiplier = 100; // Implies two decimal places.
  constexpr size_t RangeMultiplier = 10;
  size_t upperBound = numberOfPoints * RangeMultiplier,
         generatedIntegerForXCoord, generatedIntegerForYCoord;
  std::vector<bool> visited(upperBound + 1, false); // Stores whether integer number has already been generated.

  // Clear point sequence
  pointSeq.clear();

  // Seed generator
  std::random_device rd;
  // Generator initialized with seed from rd
  std::mt19937_64 generator{rd()};
  // Create distribution
  std::uniform_int_distribution<size_t> integerNumber(0, upperBound - 1);
  std::uniform_int_distribution<size_t> decimalPlaces(0, 99);

  // Each loop produces a unique point "randomPoint" that is stored in the point sequence "pointSeq". It is guaranteed
  // that no duplicates are produced for the point sequence "pointSeq".
  Point randomPoint;
  for(size_t i = 0 ; i < numberOfPoints ; i++ ) 
  {
    // Generate two random numbers for the x-coordinate and the y-coordinate of the point "randomPoint".
    generatedIntegerForXCoord = integerNumber(generator);
    generatedIntegerForYCoord = integerNumber(generator);

    // If the generated number "generatedIntegerForXCoord" has already been visited, increment it until a number is
    // found that has not yet been visited.
    while (visited[generatedIntegerForXCoord])
      generatedIntegerForXCoord = (generatedIntegerForXCoord + 1) % upperBound;

   // Mark "generatedIntegerForXCoord" as visited.
   visited[generatedIntegerForXCoord] = true;

    // If the generated number "generatedIntegerForYCoord" has already been visited, increment it until a number is
    // found that has not yet been visited.
    while (visited[generatedIntegerForYCoord])
      generatedIntegerForYCoord = (generatedIntegerForYCoord + 1) % upperBound;

   // Mark "generatedIntegerForYCoord" as visited.
   visited[generatedIntegerForYCoord] = true;

   // Create the point "randomPoint"
   randomPoint.x = Number(generatedIntegerForXCoord) + Number(decimalPlaces(generator)) / 100;
   randomPoint.y = Number(generatedIntegerForYCoord) + Number(decimalPlaces(generator)) / 100;

   // Add the point "randomPoint" to the point sequence "pointSeq".
   pointSeq.push_back(randomPoint);
  }
}

// Method that randomly generates "numberOfPoints" points without duplicates and stores them as a point sequence. 
// The points are randomly planced in a shape of a Circle. 
void generateCircledPointSequence(PointSequence& points, size_t numberOfPoints)
{
  const double PI = 3.14159265;

  constexpr size_t NoOfDecimalPlacesMultiplier = 100; // Implies two decimal places.
  constexpr size_t RangeMultiplier = 100;
  size_t upperBound = numberOfPoints * RangeMultiplier,
         generatedIntegerForRadius;
  std::vector<bool> visited(upperBound + 1, false);

  std::uniform_int_distribution<size_t> integerNumber(0, upperBound / 2);
  std::uniform_int_distribution<size_t> decimalPlaces(0, 99);

  points.clear();
  std::vector<Number> radius;
  int midX = upperBound/2, midY = upperBound/2;
  // Seed generator
  std::random_device rd;
  // Generator initialized with seed from rd
  std::mt19937_64 generator{rd()};

  // it will determine the thickness of the circle by the radius of generated points. 
  // 
  // ex., if we give 5, points will be located in the radius ( (midX - midX/circleThickness, midX) ) between 400 and 500 when the number of points is 1,000,
  // if we give midX + 1, every point will be located on the boundary of a circle.
  // if we give midX, the thickness of the circle will be 1, which means the difference of the maximum and minimum radius among points is 1. 
  // if we give 1, it will be like points interior circle. Calls it "Disk".
  const int circleThickness = 1; 

  std::vector<Number> angles;

  // the way of Random number generate uing C++11
  std::uniform_real_distribution<Number> dist(0,360);
  // std::uniform_real_distribution<CoordinateType> radiuss(midX - midX/circleThickness, midX);

  // std::default_random_engine generator((unsigned int)time(0));
  // std::uniform_real_distribution<CoordinateType> distribution(1, midX+1);


  // auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
  // auto realRand = std::bind(std::uniform_real_distribution<CoordinateType>(0,360), mt19937(seed));
  // auto realRadius = std::bind(std::uniform_real_distribution<CoordinateType>(midX - midX/circleThickness,midX), mt19937(seed));



  std::mt19937 rngAngle, rngRadius;
  rngAngle.seed(std::random_device{}());
  // rngRadius.seed(std::random_device{}());

  for(int i = 0 ; i < (numberOfPoints); i ++)
  {
    // angles.push_back(realRand());
    // radius.push_back(realRadius());
    // radius.push_back(distribution(generator));
    generatedIntegerForRadius = integerNumber(generator);
    while(visited[generatedIntegerForRadius] == true)
    {
        generatedIntegerForRadius = integerNumber(generator);
    }
    visited[generatedIntegerForRadius] = true;

    angles.push_back(dist(rngAngle));
    radius.push_back(double(generatedIntegerForRadius) + double(decimalPlaces(generator)) / 100);
    // radius.push_back(radiuss(rngRadius));
  }

  for(int i = 0 ; i < angles.size(); i++)
  {
    Point point;

    point.x = (radius[i] * cos(angles.at(i) * (PI/180))) + midX; //convert degree to radian
    point.y = (radius[i] * sin(angles.at(i) * (PI/180))) + midY;
    points.push_back(point);
  }
}

// Method that returns the (minmal) squared distance of a point "r" from a segment whose end points are given by the
// points "p" and "q".
Number computeSquaredDistanceFromPointToSegment(const Point& r, const Point& p, const Point& q)
{
  Number diffX = q.x - p.x;
  Number diffY = q.y - p.y;

  // Check if the points "p" and "q" coincide. If so, return the squared Euclidean distance between the points "p" and
  // "r".
  if ((diffX == 0) && (diffY == 0))
  {
    diffX = r.x - p.x;
    diffY = r.y - p.y;
    return diffX * diffX + diffY * diffY;
  }

  Number t = ((r.x - p.x) * diffX + (r.y - p.y) * diffY) / (diffX * diffX + diffY * diffY);

  if (t < 0)
  {
    // Point "r" is nearest to point "p".
    diffX = r.x - p.x;
    diffY = r.y - p.y;
  }
  else if (t > 1)
  {
    // Point "r" is nearest to point "q".
    diffX = r.x - q.x;
    diffY = r.y - q.y;
  }
  else
  {
    // Point "r" is nearest to a point of the interior of the segment between point "p" and point "q", ie., the
    // line through "p" and perpendicular to the segment intersects the segment.
    diffX = r.x - (p.x + t * diffX);
    diffY = r.y - (p.y + t * diffY);
  }

  // Return squared distance.
  return diffX * diffX + diffY * diffY;
}

// Method that checks whether a point sequence contains duplicates.
bool PointSequenceContainsDuplicates(const PointSequence& pointSeq)
{
  bool duplicateFound = false;
  size_t index = 1, numberOfPoints = pointSeq.size();
  PointSequence pointSeqCopy(pointSeq);

  // Sort all points in the point sequence "pointSeqCopy" in order to enable the identifation of duplicates.
  std::sort(pointSeqCopy.begin(), pointSeqCopy.end());

  while (index < numberOfPoints && !duplicateFound)
  {
    if (pointSeqCopy[index] != pointSeqCopy[index-1])
      ++index;
    else
    {
      duplicateFound = true;
      std::cout << "pointSeqCopy[" << index-1 << "] = " << pointSeqCopy[index-1] << ", "
                << "pointSeqCopy[" << index   << "] = " << pointSeqCopy[index] << std::endl;
    }
  }

  return duplicateFound;
}

// Method that checks whether a point sequence fulfills some minimal requirements for computing the convex hull from
// it. These requirements include that the point sequence contains at least three points and that it is not the case
// that all points are collinear.
bool PointSequenceFulfillsMinimalRequirements(const PointSequence& pointSeq)
{
  // Check if there are at least three points.
  if (pointSeq.size() < 3)
    // There is no convex hull.
    return false;

  // Check the extremely rare case that all points are collinear. Then there is no convex hull. Return "false".
  bool allPointsAreCollinear = true; // Assumption
  size_t i = 2;
  while (allPointsAreCollinear && i < pointSeq.size())
  {
    allPointsAreCollinear = (getOrientation(pointSeq[0], pointSeq[1], pointSeq[i]) == Orientation::COLLINEAR);
    ++i;
  }
  if (allPointsAreCollinear)
    // There is no convex hull.
    return false;

  return true;
}

// HELP FUNCTIONS (to be removed later)

void printPointSequence(const std::string& str1, const std::string& str2, const std::vector<Point>& pointSeq)
{
  size_t noOfElements = pointSeq.size();
  const size_t limit = 60;
  
  std::cout << str1 << " " << noOfElements << " " << str2 << " (";
  
  if (noOfElements > limit)
    std::cout << "<Too many points to be displayed! The limit is " << limit << " points.>";
  else if (noOfElements > 0)
  {
    if (noOfElements > 1)
    {
      for (size_t i = 0; i <= noOfElements - 2; ++i)
        std::cout << pointSeq[i] << ", ";
      std::cout << pointSeq[noOfElements - 1];
    }
    else
      std::cout << pointSeq[noOfElements - 1];
  }

  std::cout << ")." << std::endl << std::endl;
}

