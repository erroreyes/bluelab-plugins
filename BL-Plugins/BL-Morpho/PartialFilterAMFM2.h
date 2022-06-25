#ifndef PARTIAL_FILTER_AMFM2_H
#define PARTIAL_FILTER_AMFM2_H

#include <vector>
#include <deque>
using namespace std;

#include <PartialFilter2.h>

// Method using alhpa0 and dlta0 from AM FM paper
// https://www.researchgate.net/publication/235219224_Improved_partial_tracking_technique_for_sinusoidal_modeling_of_speech_and_audio
// Also use zombied as suggested
class PartialFilterAMFM2 : public PartialFilter2
{
 public:
    PartialFilterAMFM2(int bufferSize, BL_FLOAT sampleRate);
    virtual ~PartialFilterAMFM2();

    void Reset(int bufferSize, BL_FLOAT sampleRate);
        
    void FilterPartials(vector<Partial2> *partials);

    void SetNeriDelta(BL_FLOAT delta);
    
 protected:
    // Method based on alpha0 and beta0 (not optimized)
    void AssociatePartialsAMFMSimple(const vector<Partial2> &prevPartials,
                                     vector<Partial2> *currentPartials,
                                     vector<Partial2> *remainingCurrentPartials);

    // Method based on alpha0 and beta0 (optimized)
    void AssociatePartialsAMFM(const vector<Partial2> &prevPartials,
                               vector<Partial2> *currentPartials,
                               vector<Partial2> *remainingCurrentPartials);
    long FindNearestFreqId(const vector<Partial2> &partials,
                           BL_FLOAT freq, int index);
        
    // Compute score using Neri
    void AssociatePartialsNeri(const vector<Partial2> &prevPartials,
                               vector<Partial2> *currentPartials,
                               vector<Partial2> *remainingCurrentPartials);
    
    void AssociatePartialsHungarianAMFM(const vector<Partial2> &prevPartials,
                                        vector<Partial2> *currentPartials,
                                        vector<Partial2> *remainingCurrentPartials);

    // Compute score like in the paper
    void AssociatePartialsHungarianNeri(const vector<Partial2> &prevPartials,
                                        vector<Partial2> *currentPartials,
                                        vector<Partial2> *remainingCurrentPartials);
    
    void ComputeZombieDeadPartials(const vector<Partial2> &prevPartials,
                                   const vector<Partial2> &currentPartials,
                                   vector<Partial2> *zombieDeadPartials);

    void FixPartialsCrossing(const vector<Partial2> &partials0,
                             const vector<Partial2> &partials1,
                             vector<Partial2> *partials2);
        
    int FindPartialById(const vector<Partial2> &partials, int idx);
    // Optimized
    int FindPartialByIdSorted(const vector<Partial2> &partials,
                              const Partial2 &refPartial);
    
    // For AMFM
    BL_FLOAT ComputeLA(const Partial2 &prevPartial, const Partial2 &currentPartial);
    BL_FLOAT ComputeLF(const Partial2 &prevPartial, const Partial2 &currentPartial);

    // for hungarian Neri
    void ComputeCostNeri(const Partial2 &prevPartial,
                         const Partial2 &currentPartial,
                         BL_FLOAT delta, BL_FLOAT zetaF, BL_FLOAT zetaA,
                         BL_FLOAT *A, BL_FLOAT *B);
        
    void ExtrapolatePartialAMFM(Partial2 *p);
    void ExtrapolatePartialKalman(Partial2 *p);

    bool CheckDiscardBigJump(const Partial2 &prevPartial,
                             const Partial2 &currentPartial);
    bool CheckDiscardOppositeDirection(const Partial2 &prevPartial,
                                       const Partial2 &currentPartial);
    
    //
    deque<vector<Partial2> > mPartials;

    int mBufferSize;
    BL_FLOAT mSampleRate;

    BL_FLOAT mNeriDelta;
    
 private:
    vector<Partial2> mTmpPartials0;
    vector<Partial2> mTmpPartials1;
    vector<Partial2> mTmpPartials2;
    vector<Partial2> mTmpPartials3;
    vector<Partial2> mTmpPartials4;
    vector<Partial2> mTmpPartials5;
    vector<Partial2> mTmpPartials6;
    vector<Partial2> mTmpPartials7;
};

#endif
