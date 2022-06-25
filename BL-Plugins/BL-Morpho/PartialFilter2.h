#ifndef PARTIAL_FILTER2_H
#define PARTIAL_FILTER2_H

#include <vector>
#include <deque>
using namespace std;

#include <Partial2.h>

class PartialFilter2
{
 public:
    virtual void Reset(int bufferSize, BL_FLOAT smapleRate) = 0;
        
    virtual void FilterPartials(vector<Partial2> *partials) = 0;
};

#endif
