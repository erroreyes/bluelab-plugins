//
//  GraphFreqAxis2.h
//  BL-InfrasonicViewer
//
//  Created by applematuer on 11/22/19.
//
//

#ifndef __BL_InfrasonicViewer__GraphFreqAxis2__
#define __BL_InfrasonicViewer__GraphFreqAxis2__

// GraphFreqAxis2: from GraphFreqAxis
// For GraphControl12
//
class GraphAxis2;
class GraphFreqAxis2
{
public:
    GraphFreqAxis2();
    
    virtual ~GraphFreqAxis2();
    
    void Init(GraphAxis2 *graphAxis,
              int bufferSize, BL_FLOAT sampleRate,
              int graphWidth);
    
    void Reset(int bufferSize, BL_FLOAT sampleRate);
    
protected:
    void Update();
    
    //
    GraphAxis2 *mGraphAxis;
    
    int mBufferSize;
    BL_FLOAT mSampleRate;
};

#endif /* defined(__BL_InfrasonicViewer__GraphFreqAxis2__) */
