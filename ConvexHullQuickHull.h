#ifndef CONVEXHULLQUICKHULL_H
#define CONVEXHULLQUICKHULL_H

#include "PointHandler.h"

CCWPointSequence ConvexHullQuickHull(const PointSequence& pointSeq);
CCWPointSequence ConvexHullQuickHullJustification(PointSequence& pointSeqA, PointSequence& pointSeqB, 
                                                  const Point& leftMost, const Point& rightMost);

std::vector<Point>::iterator ConvexHullInPlaceQuickHull(PointSequence& pointSeq);
std::vector<Point>::iterator TheirConvexHullInPlaceQuickHull(PointSequence& pointSeq);
#endif // CONVEXHULLQUICKHULL_H
