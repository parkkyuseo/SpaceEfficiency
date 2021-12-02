#ifndef POINTHANDLER_H
#define POINTHANDLER_H


#include <ostream>
#include <string>    // to be deleted later
#include <vector>
#include "Number.h"

class Point
{
  public:
  Number x;
  Number y;

  // Empty constructor: Initializes data members x and y with 0
  Point();

  // Constructor: Initializes data members x and y with Number values x and y
  Point(const Number& x, const Number& y);

  // Copy constructor: Copies point p to *this
  Point(const Point& p);

  // Destructor
  ~Point();

  // Comparison operators based on the lexicographical order on points
  bool operator <  (const Point& rhs) const;
  bool operator <= (const Point& rhs) const;
  bool operator >  (const Point& rhs) const;
  bool operator == (const Point& rhs) const;
  bool operator != (const Point& rhs) const;
        
  // Assignment operator
  Point& operator = (const Point& rhs);

  // Output method
  friend std::ostream& operator << (std::ostream& os, const Point& p);
};

// Type that represents the input of a convex hull algorithm as an arbitrary (unordered or ordered) sequence of points.
using PointSequence = std::vector<Point>;

// Type that represents the output of a convex hull algorithm (i.e., a convex hull) as a counterclockwise sequence of
// points.
using CCWPointSequence = std::vector<Point>;

// Type that represents an index sequence for a given point sequence. Such a sequence of indexes enables one to keep a
// different order of the points for a given point sequence without reordering the point sequnce itself. This avoids
// the copying a given point sequence and requires less space.
using IndexSequence = std::vector<size_t>;

// Type that indicates the orientation of an oriented triplet (p, q, r) of points.
enum class Orientation {CLOCKWISE, COLLINEAR, COUNTERCLOCKWISE};

// Type that represents the result of comparing the distances of a point to two other points.
enum class CompareDistance {DISTANCETOFIRSTPOINTISSMALLER, DISTANCETOSECONDPOINTISSMALLER, EQUALDISTANCE};

// Method that for three points p, q, and r finds the orientation of the ordered triplet (p, q, r). The method returns
// the value COLLINEAR if p, q, and r are collinear, the value COUNTERCLOCKWISE if p, q, and r are oriented
// counterclockwise, and CLOCKWISE if p, q, and r are oriented clockwise. Another view is that the method checks
// whether r is on (COLLINEAR), left of/above (COUNTERCLOCKWISE), or right of/below (CLOCKWISE) a directed line defined
// by the directed segment from p to q.
Orientation getOrientation(const Point& p, const Point& q, const Point& r);
Orientation getOrientationWithArea(const Point& p, const Point& q, const Point& r, Number& area);

// Method that checks whether the distance of a point p to a point q is smaller (DISTANCETOFIRSTPOINTISSMALLER),
// larger(DISTANCETOSECONDPOINTISSMALLER), or equal (EQUALDISTANCE) than the distance of the point p to the point r.
CompareDistance compareDistance(const Point& p, const Point& q, const Point& r);

// Method that randomly generates "numberOfPoints" points without duplicates and stores them as a point sequence. 
void generateRandomPoints(PointSequence& pointSeq, size_t numberOfPoints);

// Method that randomly generates "numberOfPoints" points without duplicates and stores them as a point sequence. 
// The points are randomly planced in a shape of a Circle. 
void generateCircledPointSequence(PointSequence& points, size_t numberOfPoints);

// Method that returns the (minmal) squared distance of a point "r" from a segment whose end points are given by the
// points "p" and "q".
Number computeSquaredDistanceFromPointToSegment(const Point& r, const Point& p, const Point& q);

// Method that checks whether a point sequence contains duplicates.
bool PointSequenceContainsDuplicates(const PointSequence& pointSeq);

// Method that checks whether a point sequence fulfills some minimal requirements for computing the convex hull from
// it. These requirements include that the point sequence contains at least three points and that it is not the case
// that all points are collinear.
bool PointSequenceFulfillsMinimalRequirements(const PointSequence& pointSeq);

// HELP FUNCTIONS (to be removed later)

void printPointSequence(const std::string& str1, const std::string& str2, const std::vector<Point>& pointSeq);

#endif // POINTHANDLER_H
