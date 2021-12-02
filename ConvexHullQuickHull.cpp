
#include <algorithm>    // std::sort
#include <iostream>
#include <vector>
#include "ConvexHullQuickHull.h"
#include "PointHandler.h"

const signed short int UPPER        = 0;
const signed short int LOWER        = 1;

//
// Local methods
//
void findHull(const PointSequence& pointSeq, const Point& p, const Point& q, CCWPointSequence& ccwPointSeq, 
              const signed short int location);


//********************
// ConvexHullQuickHull
//********************

CCWPointSequence ConvexHullQuickHull(const PointSequence& pointSeq)
{
  CCWPointSequence ccwPointSeq; // ccw means counterclockwise

  // Check whether the point sequence "pointSeq" fulfills some minimal requirements for computing the convex hull from
  // it. These requirements include that the point sequence contains at least three points and that it is not the case
  // that all points are collinear. If it does not, return the empty point sequence.
  if (! PointSequenceFulfillsMinimalRequirements(pointSeq))
    return ccwPointSeq;

  // Find the leftmost point with minimum x-coordinate and the rightmost point with maximum x-coordinate of the point
  // sequence "pointSeq".
  Point leftMostPoint = pointSeq[0], rightMostPoint = pointSeq[0];
  size_t numOfElements = pointSeq.size();

  for(size_t i = 1; i < numOfElements; ++i)
    if(pointSeq[i] < leftMostPoint)
      leftMostPoint = pointSeq[i];
    else if (pointSeq[i] > rightMostPoint)
      rightMostPoint = pointSeq[i];

  // Split point sequence "pointSeq" into the two point sequences "pointSeqAbove" and "pointSeqBelow" that contain the
  // points above and below the directed segment (leftMostPoint, rightMostPoint) respectively. Points that are located
  // on the interior of the segment (note that the boundary points "leftMostPoint" and "rightMostPoint" belong to the
  // convex hull and are stored in the point sequence "ccwPointSeq" below) can be ignored since they cannot contribute
  // to the convex hull. Unfortunately, in the worst case, it can happen that the points of the point sequence
  // "pointSeq" are almost all above or almost all below the middle segment. Therefore, two point sequences of the size
  // of the point sequence "pointSeq" are reserved. This avoids reallocations that especially for very high numbers of
  // points lead to program abortions due to memory allocation errors.
  Orientation pointOrientation;
  PointSequence pointSeqAbove, pointSeqBelow;
  pointSeqAbove.reserve(pointSeq.size());
  pointSeqBelow.reserve(pointSeq.size());
  for (size_t i = 0; i < numOfElements; ++i)
  {
    // Check where point "pointSeq[i]" is located with respect to the middle segment from point "leftMostPoint" to
    // point "rightMostPoint".
    pointOrientation = getOrientation(leftMostPoint, rightMostPoint, pointSeq[i]);
    if (pointOrientation == Orientation::COUNTERCLOCKWISE) // Point "pointSeq[i]" is above segment.
      pointSeqAbove.push_back(pointSeq[i]);
    else if (pointOrientation == Orientation::CLOCKWISE) // Point "pointSeq[i]" is below segment.
      pointSeqBelow.push_back(pointSeq[i]);
  }

  // The leftmost point and the rightmost point definitely belong to the convex hull. Use the point sequence
  // "ccwPointSeq" to determine all those points (in arbitrary order) that belong to the lower convex hull including
  // the point "leftMostPoint" and excluding the point "rightMostPoint". Later this point sequence will contain all
  // points of the convex hull in counterclockwise order. Declare and use the point sequence "ccwPointSeqAbove" to
  // determine all those points (in arbitrary order) that belong to the upper convex hull including the point
  // "rightMostPoint" and excluding the point "leftMostPoint". Later this point sequence will be appropriately added to
  // the point sequence "ccwPointSeq" in counterclockwise order.
  ccwPointSeq.push_back(leftMostPoint);

  /* std::cout << "number of points above+below: " << pointSeqBelow.size() + pointSeqAbove.size() << "\n"; */
  /* std::cout << "recursive for below \n"; */

  // Compute the lower convex hull recursively.
  findHull(pointSeqBelow, leftMostPoint, rightMostPoint, ccwPointSeq, LOWER);
  ccwPointSeq.push_back(rightMostPoint);
  /* std::cout << "recursive for above \n"; */
  // Compute the upper convex hull recursively.
  findHull(pointSeqAbove, rightMostPoint, leftMostPoint, ccwPointSeq, UPPER);

  return ccwPointSeq;
}


// Given the points "p" and "q" that are known to belong to the convex hull and the point sequence "pointSeq" whose
// points are known to be located on the right side of the directed segment from "p" to "q", this method recursively
// determines all points that belong to the convex hull of the points in the point sequence "pointSeq". These convex
// hull points are stored in the point sequence "ccwPointSeq" in arbitrary (and not counterclockwise) order.
void findHull(const PointSequence& pointSeq, const Point& p, const Point& q, CCWPointSequence& ccwPointSeq, const signed short int location)
{
  // If the point sequence "pointSeq" is empty, nothing has to be done.
  if (pointSeq.size() == 0)
    return;

  if (pointSeq.size() == 1)
  {
    ccwPointSeq.push_back(pointSeq[0]);
    return;
  }
  // Find the point "furthestPoint" of the point sequence "pointSeq" that has the largest (minimal) distance from the
  // segment defined by the points "p" and "q". This point belongs definitely to the convex hull and is stored in the
  // point sequence "ccwPointSeq".
  Number squaredDistanceFromPointToSegment, maxSquaredDistance = 0;
  size_t index, numOfElements = pointSeq.size();
  for(size_t i = 0; i < numOfElements; ++i)
  {
    squaredDistanceFromPointToSegment = computeSquaredDistanceFromPointToSegment(pointSeq[i], p, q);
    if (maxSquaredDistance < squaredDistanceFromPointToSegment)
    {
      maxSquaredDistance = squaredDistanceFromPointToSegment;
      index = i;
    }
    else if (maxSquaredDistance == squaredDistanceFromPointToSegment && pointSeq[i].x < pointSeq[index].x)
      // If there are more than two points with the same largest distance to the segment, we must ensure that none of
      // the interior collinear points are selected for the convex hull. We achieve this by selecting the point with
      // the smallest x-coordinate; it will definitely belong to the convex hull. The point with the largest
      // x-coordinate of all collinear points will be taken in the next recursive step of "findHull". 
      index = i;
  }
  Point furthestPoint = pointSeq[index];

  // The point sequence "pointSeq" is split into the point sequence "pointseq1" of points that are located right of the
  // directed segment defined by the points "p" and "furthestPoint", and into the point sequence "pointseq2" of points
  // that are located right of the directed segment defined by the points "furthestPoint" and "q". The points of the
  // point sequence "pointSeq" that are located in the triangle defined by the points "p", "furthestPoint", and "q" are
  // ignored since they cannot be part of the convex hull.
  PointSequence pointSeq1,pointSeq2;
  if (location == LOWER)
  {
    for(size_t i = 0; i < numOfElements; ++i)
    if (pointSeq[i].x < furthestPoint.x)
    {
      if (getOrientation(p, furthestPoint, pointSeq[i]) == Orientation::CLOCKWISE)
        pointSeq1.push_back(pointSeq[i]);
    }
    else
      if (getOrientation(furthestPoint, q, pointSeq[i]) == Orientation::CLOCKWISE)
        pointSeq2.push_back(pointSeq[i]);
    }
  else
  {
    for(size_t i = 0; i < numOfElements; ++i)
    if (pointSeq[i].x > furthestPoint.x)
    {
      if (getOrientation(p, furthestPoint, pointSeq[i]) == Orientation::CLOCKWISE)
        pointSeq1.push_back(pointSeq[i]);
    }
    else
      if (getOrientation(furthestPoint, q, pointSeq[i]) == Orientation::CLOCKWISE)
        pointSeq2.push_back(pointSeq[i]);
  }
  /* std::cout << "number of points left+right: " << pointSeq1.size() + pointSeq2.size() << "\n"; */

  /* std::cout << "recursive for left\n"; */

  // Recursively determine the convex hull points in the point sequence "pointSeq1" that are located on the right side
  // of the the segment between point "p" and point "furthestPoint". 
  findHull(pointSeq1, p, furthestPoint, ccwPointSeq, location);
  ccwPointSeq.push_back(furthestPoint);
  // Recursively determine the convex hull points in the point sequence "pointSeq2" that are located on the right side
  // of the the segment between point "furthestPoint" and point "q".
  /* std::cout << "recursive for right\n"; */
  findHull(pointSeq2, furthestPoint, q, ccwPointSeq, location);
}
