
#include <algorithm>    // std::sort
#include <iostream>
#include <memory>
#include <vector>
#include "ConvexHullQuickHull.h"
#include "PointHandler.h"

#include <cassert> // assert macro
#include <cstdlib> // std::size_t
#include <iterator> // std::iterator_traits std::distance . . .
#include <utility> // std::pair std::make_pair . . .


const signed short int LOWER  = 0;
const signed short int UPPER  = 1;

using P = Point;
using S = std::vector<P>;
using I = typename S::iterator;

//**********************************************
// Local methods used by our in place quickhull
//**********************************************
void findHullInPlace(I first, I past, I leftMost, I rightMost, I& itrForNextHullPoint, const signed short int location);
void lowerPartition(I& itrForNextOfLastPointOfFirstGroup, I& itrForFirstPointOfSecondGroup, 
                    const I& leftMostP, const I& rightMostP, const I& furthestP);
void upperPartition(I& itrForNextOfLastPointOfFirstGroup, I& itrForFirstPointOfSecondGroup, 
                    const I& leftMostP, const I& rightMostP, const I& furthestP);
// A function pointer that calls either lowerPartition or upperPartition. 
void (*partition[2])(I& itrForNextOfLastPointOfFirstGroup, I& itrForFirstPointOfSecondGroup, 
                    const I& leftMostP, const I& rightMostP, const I& furthestP) = {lowerPartition, upperPartition};
// Methods that duplicate their local methods. 
I partition_right_left(I first, I last, const I& leftMostP, const I& rightMostP);
I find_furthest(I first, I past, I leftMost, I rightMost);

//************************************************
// Local methods used by their in place quickhull
//************************************************
I find_furthest(I first, I past, I antipole);
std::pair<I, I> find_poles(I first, I past);
void parallel_iter_swap(I st, I nd, I rd, I th);
I partition_left_right(I first, I past, I antipole);
void swap_blocks(I source, I past, I target);
void move_away(I here, I rest, I past); 
I chain(I pole, I past, I antipole);

//**********************************************************
// ConvexHullInplaceQuickHull: In place QuickHull algorithm
//**********************************************************
// It returns an iterator and it points the next of the last convex hull vertex in the PointSequence.  
std::vector<Point>::iterator ConvexHullInPlaceQuickHull(PointSequence& pointSeq)
{ 
  // Check whether the point sequence "pointSeq" fulfills some minimal requirements for computing the convex hull from
  // it. These requirements include that the point sequence contains at least three points and that it is not the case
  // that all points are collinear. If it does not, return the empty point sequence.
  if (! PointSequenceFulfillsMinimalRequirements(pointSeq))
    return pointSeq.begin();

  // Adapt their way to find the leftmost and rightmost points. 
  I first = pointSeq.begin();
  I past = pointSeq.end();
  std::pair<I, I> pair = find_poles(first, past);
  I itrForLeftMostPoint = first;
  I itrForRightMostPoint = std::prev(past);
  // Place the leftmost point at the beginning of the pointSeq and the rightmost point at the end of the pointSeq. 
  parallel_iter_swap(itrForLeftMostPoint, itrForRightMostPoint, std::get<0>(pair) , std::get<1>(pair));
  
  // Because the beginning and the end of the pointSeq have the leftmost and rightmost point respectively, 
  // modify the iterator. 
  first++;
  past = itrForRightMostPoint;
  // Set the iterator that is used to store the convex hull vertices. 
  // It is where the next found hull vertex will be placed. 
  I itrForNextHullPoint = first;
  
  // An iterator of the first point of the second block.
  // Second block is a group of points that place above the middle segment.
  // After partition, it can be found. 
  I itrForFirstPointOfSecondGroup = partition_right_left(first, past, itrForLeftMostPoint, itrForRightMostPoint);
  
  // Find the lower hull vertices recursively. 
  findHullInPlace(first, itrForFirstPointOfSecondGroup, itrForLeftMostPoint, itrForRightMostPoint, 
                  itrForNextHullPoint, LOWER);
  // After finding the lower hull vertices, the rightmost point will be placed to the next. 
  std::iter_swap(itrForRightMostPoint, itrForNextHullPoint);
  itrForRightMostPoint = itrForNextHullPoint;
  // If the iterator for the next hull point is the same as the iterator for the first point of the second block, 
  // the first point of the second group moves to the end, hence, increase the iterator to include the point. 
  if (itrForNextHullPoint == itrForFirstPointOfSecondGroup)
  {
    itrForFirstPointOfSecondGroup++;
    past++;
  }
  itrForNextHullPoint++;
  // Find the upper hull vertices recursively. 
  findHullInPlace(itrForFirstPointOfSecondGroup, past, 
                  itrForRightMostPoint, itrForLeftMostPoint, itrForNextHullPoint, UPPER);

  return itrForNextHullPoint;
}

void findHullInPlace(I first, I past, I leftMost, I rightMost, I& itrForNextHullPoint, const signed short int location)
{
  size_t sizeOfPoints = std::distance(first, past);
  
  // if the point sequence only has one point, it should be added to the result vector. 
  if (sizeOfPoints == 1)
  {
    std::iter_swap(first, itrForNextHullPoint++);
    return;
  }

  if (sizeOfPoints == 0)
  {
    return;
  }

  // Iterators for the furthest point and the last point in the current block of points. 
  I furthestPoint = find_furthest(first, past, leftMost, rightMost);
  I last = std::prev(past);
  // Move the furthest point to the end. 
  std::iter_swap(furthestPoint, last);
  furthestPoint = last; // to make it understandable. 

  // Initialize an iterator of the next point of the first block.
  // First block is a group of points that place the right to the segment(leftmost, furthest).
  // After partition, it can be found. 
  I itrForNextOfLastPointOfFirstGroup = first;
  // Initialize an iterator of the first point of the second block.
  // Second block is a group of points that place the right to the segment(furthest, rightmost).
  // After partition, it can be found. 
  I itrForFirstPointOfSecondGroup = std::prev(last);
  // After finding the furthest point, partition the current group of points.
  // partition is a function pointer and it calls either lowerPartition or upperPartition 
  // depending on the location variable.  
  partition[location](itrForNextOfLastPointOfFirstGroup, itrForFirstPointOfSecondGroup, 
                      leftMost, rightMost, furthestPoint);
  
  // Recursively find the hull vertices. 
  findHullInPlace(first, itrForNextOfLastPointOfFirstGroup, leftMost, furthestPoint, itrForNextHullPoint, location);
  // After finding the hull vertices, the furthest point will be placed to the next. 
  std::iter_swap(furthestPoint, itrForNextHullPoint);
  furthestPoint = itrForNextHullPoint; if (itrForNextHullPoint == itrForFirstPointOfSecondGroup)
  { 
    itrForFirstPointOfSecondGroup++;
    last++;
  }
  itrForNextHullPoint++;

  // Recursively find the hull vertices for the next group of points. 
  findHullInPlace(itrForFirstPointOfSecondGroup, last, furthestPoint, rightMost, itrForNextHullPoint, location);
}
// Partition the points placed in the lower of middle segment. 
void lowerPartition(I& itrForNextOfLastPointOfFirstGroup, I& itrForFirstPointOfSecondGroup, 
                    const I& leftMostP, const I& rightMostP, const I& furthestP)
{ 
  // Initialize current to the first of the points.
  I current = itrForNextOfLastPointOfFirstGroup;
  while (current != itrForFirstPointOfSecondGroup)
  {
    // without Orienting vector.
    if (getOrientation(*leftMostP, *furthestP, *current) == Orientation::CLOCKWISE)
    {
      std::iter_swap(current, itrForNextOfLastPointOfFirstGroup);
      itrForNextOfLastPointOfFirstGroup++;
      current++;
    }

    else if (getOrientation(*furthestP, *rightMostP, *current) == Orientation::CLOCKWISE)
    {
      std::iter_swap(current, itrForFirstPointOfSecondGroup);
      itrForFirstPointOfSecondGroup--;
    }
    else
      current++;
    
    /* if ((*current).x < (*furthestP).x) // Orienting vector.
     * {
     *   if (getOrientation(*leftMostP, *furthestP, *current) == Orientation::CLOCKWISE)
     *   {
     *     std::iter_swap(current, itrForNextOfLastPointOfFirstGroup);
     *     itrForNextOfLastPointOfFirstGroup++;
     *   }
     *   current++;
     * }
     * else
     * {
     *   if (getOrientation(*furthestP, *rightMostP, *current) == Orientation::CLOCKWISE)
     *   {
     *     std::iter_swap(current, itrForFirstPointOfSecondGroup);
     *     itrForFirstPointOfSecondGroup--;
     *   }
     *   else
     *     current++;
     * } */
  }
  // for the element that is not checked in the loop.
  if ((*current).x < (*furthestP).x)
  {
    if (getOrientation(*leftMostP, *furthestP, *current) == Orientation::CLOCKWISE)
    {
      std::iter_swap(current, itrForNextOfLastPointOfFirstGroup);
      itrForNextOfLastPointOfFirstGroup++;
    }
    itrForFirstPointOfSecondGroup++;
    
  }
  else
  {
    if (getOrientation(*furthestP, *rightMostP, *current) == Orientation::COUNTERCLOCKWISE)
      itrForFirstPointOfSecondGroup++;
  }

}
// Partition the points placed in the upper of middle segment. 
void upperPartition(I& itrForNextOfLastPointOfFirstGroup, I& itrForFirstPointOfSecondGroup, 
                    const I& leftMostP, const I& rightMostP, const I& furthestP)
{
  // Initialize current to the first of the points.
  I current = itrForNextOfLastPointOfFirstGroup;
  while (current != itrForFirstPointOfSecondGroup)
  {
    // without Orienting vector
    if (getOrientation(*leftMostP, *furthestP, *current) == Orientation::CLOCKWISE)
    {
      std::iter_swap(current, itrForNextOfLastPointOfFirstGroup);
      itrForNextOfLastPointOfFirstGroup++;
      current++;
    }
    else if (getOrientation(*furthestP, *rightMostP, *current) == Orientation::CLOCKWISE)
    {
      std::iter_swap(current, itrForFirstPointOfSecondGroup);
      itrForFirstPointOfSecondGroup--;
    }
    else
      current++;
    
    /* // with orienting vector
     * if ((*current).x > (*furthestP).x)
     * {
     *   if (getOrientation(*leftMostP, *furthestP, *current) == Orientation::CLOCKWISE)
     *   {
     *     std::iter_swap(current, itrForNextOfLastPointOfFirstGroup);
     *     itrForNextOfLastPointOfFirstGroup++;
     *   }
     *   current++;
     * }
     * else
     * {
     *   if (getOrientation(*furthestP, *rightMostP, *current) == Orientation::CLOCKWISE)
     *   {
     *     std::iter_swap(current, itrForFirstPointOfSecondGroup);
     *     itrForFirstPointOfSecondGroup--;
     *   }
     *   else
     *     current++;
     * } */
  }
  // for the element that is not checked in the loop.
  if ((*current).x > (*furthestP).x)
  {
    if (getOrientation(*leftMostP, *furthestP, *current) == Orientation::CLOCKWISE)
    {
      std::iter_swap(current, itrForNextOfLastPointOfFirstGroup);
      itrForNextOfLastPointOfFirstGroup++;
    }
    itrForFirstPointOfSecondGroup++;
  }
  else
  {
    if (getOrientation(*furthestP, *rightMostP, *current) == Orientation::COUNTERCLOCKWISE)
      itrForFirstPointOfSecondGroup++;
  }
}


// Their in-place quickhull algorithm
std::vector<Point>::iterator TheirConvexHullInPlaceQuickHull(PointSequence& pointSeq)
{
  // Check whether the point sequence "pointSeq" fulfills some minimal requirements for computing the convex hull from
  // it. These requirements include that the point sequence contains at least three points and that it is not the case
  // that all points are collinear. If it does not, return the empty point sequence.
  if (! PointSequenceFulfillsMinimalRequirements(pointSeq))
    return pointSeq.begin();

  I first = pointSeq.begin();
  I past = pointSeq.end();

  // Find the leftmost point with minimum x-coordinate and the rightmost point with maximum x-coordinate of the point
  // sequence "pointSeq".
  std::pair<I, I> pair = find_poles(first, past);
  I west = first;
  I east = std::prev(past);
  parallel_iter_swap(west, east, std::get<0>(pair) , std::get<1>(pair));
  if (*west == *east) {
    return std::next(first);
  }
  I middle = partition_left_right(west, east, east); 
  std::size_t m = std::distance(middle, past);
  I eliminated = chain(first, middle, east);
  std::iter_swap(middle, east);
  std::iter_swap(eliminated, middle); // east at its final place
  east = eliminated;
  ++middle;
  ++eliminated;
  move_away(eliminated, middle, past);
  I border = east;
  std::advance(border, m);
  eliminated = chain(east, border, west); // downunder

  return eliminated;
}
std::pair<I, I> find_poles(I first, I past) 
{
  using P = typename std::iterator_traits<I>::value_type;
  auto pair = std::minmax_element(first, past,
    [ ](P const& a, P const& b) -> bool {
      return (a.x < b.x) or (a.x == b.x and a.y < b.y);
    });
  return pair;
}
void parallel_iter_swap(I st, I nd, I rd, I th) 
{
  /* assert(st != nd and rd != th); */
  std::iter_swap(st, rd);
  if (th == st) {
    std::iter_swap(nd, rd);
  }
  else {
    std::iter_swap(nd, th);
  }
}
I partition_left_right(I first, I past, I antipole) 
{
  /* assert(first != past); */
  using P = typename std::iterator_traits<I>::value_type;
  I pole = first;
  I middle = std::partition(std::next(pole), past,
    [&](P const& q) -> bool {
      return Orientation::COUNTERCLOCKWISE != getOrientation(*pole, q, *antipole);  // not left_turn(*pole, q, *antipole);
    });
  return middle;
}
I partition_right_left(I first, I last, const I& leftMostP, const I& rightMostP) 
{
  /* assert(first != last); */
  using P = typename std::iterator_traits<I>::value_type;
  I middle = std::partition(first, last,
    [&](P const& q) -> bool {
      return Orientation::CLOCKWISE != getOrientation(*leftMostP, q, *rightMostP);  // not left_turn(*pole, q, *antipole);
    });
  return middle;
}

void swap_blocks(I source, I past, I target) 
{
  if (source == target or source == past) {
    return;
  }
  using P = typename std::iterator_traits<I>::value_type;
  I hole = target;
  P p = *target;
  I const last = std::prev(past);
  while (true) {
    *hole = *source;
    ++hole;
    if (source == last) {
      break;
    }
    *source = *hole;
    ++source;
  }
  *source = p;
}

void move_away(I here, I rest, I past) 
{
  if (here == rest or rest == past) {
    return;
  }
  if (std::distance(here, rest) < std::distance(rest, past)) {
    I target = past;
    std::advance(target, -std::distance(here, rest));
    swap_blocks(here, rest, target);
  }
  else {
    swap_blocks(rest, past, here);
  }
}
I find_furthest(I first, I past, I leftMost, I rightMost) 
{
  I furthest = first;
  Number squaredDistanceFromPointToSegment, maxSquaredDistance = 0;

  for (I i = first; i != past; ++i) 
  {
    squaredDistanceFromPointToSegment = computeSquaredDistanceFromPointToSegment(*i, *leftMost, *rightMost);
    if (maxSquaredDistance < squaredDistanceFromPointToSegment)
    {
      maxSquaredDistance = squaredDistanceFromPointToSegment;
      furthest = i;
    }
    else if (maxSquaredDistance == squaredDistanceFromPointToSegment && (*i).x < (*furthest).x)
      // If there are more than two points with the same largest distance to the segment, we must ensure that none of
      // the interior collinear points are selected for the convex hull. We achieve this by selecting the point with
      // the smallest x-coordinate; it will definitely belong to the convex hull. The point with the largest
      // x-coordinate of all collinear points will be taken in the next recursive step of "findHull". 
      furthest = i;
  }
  return furthest;
}
I find_furthest(I first, I past, I antipole) 
{
  I pole = first;
  I answer = pole;
  Number squaredDistanceFromPointToSegment, maxSquaredDistance = 0;

  for (I i = std::next(first); i != past; ++i) 
  {
    squaredDistanceFromPointToSegment = computeSquaredDistanceFromPointToSegment(*i, *pole, *antipole);
    if (maxSquaredDistance < squaredDistanceFromPointToSegment)
    {
      maxSquaredDistance = squaredDistanceFromPointToSegment;
      answer = i;
    }
    else if (maxSquaredDistance == squaredDistanceFromPointToSegment && (*i).x < (*answer).x)
      // If there are more than two points with the same largest distance to the segment, we must ensure that none of
      // the interior collinear points are selected for the convex hull. We achieve this by selecting the point with
      // the smallest x-coordinate; it will definitely belong to the convex hull. The point with the largest
      // x-coordinate of all collinear points will be taken in the next recursive step of "findHull". 
      answer = i;
  }
  return answer;
}
I chain(I pole, I past, I antipole)
{
  std::size_t n = std::distance(pole, past);
  if (n == 1) {
    return past;
  }
  if (n == 2) {
    if (getOrientation(*std::next(pole) , *pole, *antipole) == Orientation::COLLINEAR) {
      return std::next(pole);
    }
    else {
      return past;
    }
  }
  I pivot = find_furthest(pole, past, antipole);
  if (getOrientation(*pivot, *pole, *antipole) == Orientation::COLLINEAR) {
    return std::next(pole);
  }
  I last = std::prev(past);
  std::iter_swap(pivot, last); // pivot at the end
  I mid = partition_left_right(pole, last, last);
  I eliminated = chain(pole, mid, last);
  std::iter_swap(mid, last);
  std::iter_swap(eliminated, mid); // pivot at its final place
  pivot = eliminated;
  std::size_t m = std::distance(mid, past);
  ++mid;
  ++eliminated;
  move_away(eliminated, mid, past);
  I border = pivot;
  std::advance(border, m);
  I interior = partition_left_right(pivot, border, antipole);
  eliminated = chain(pivot, interior, antipole);
  return eliminated;
}
