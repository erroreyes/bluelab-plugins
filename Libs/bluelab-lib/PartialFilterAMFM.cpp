#include <stdio.h>

#include <cmath>
#include <algorithm>
using namespace std;

#include <BLUtils.h>
#include <BLUtilsMath.h>

#include <BLDebug.h>

#include <Hungarian.h>

#include "PartialFilterAMFM.h"

// 3 is often good
//#define MAX_ZOMBIE_AGE 3 //2 //5 //2
// 5 avoids changing id when partial crossing
#define MAX_ZOMBIE_AGE 5

// Must keep history size >= 3, for FixPartialsCrossing
#define PARTIALS_HISTORY_SIZE 3 //2


#define EXTRAPOLATE_KALMAN 0 // 1
// Propagate dead and zombies with alpha0 and beta0
// Problem: at partial crossing, alpha0 (for amp) sometimes has big values 
#define EXTRAPOLATE_AMFM 0 //1

#define ASSOC_AMFM 1 //0
#define ASSOC_HUNGARIAN_AMFM 0 //1
#define ASSOC_HUNGARIAN_NERI 0 //1

#define RESCALE_HZ 0 //1

PartialFilterAMFM::PartialFilterAMFM(int bufferSize, BL_FLOAT sampleRate)
{    
    mBufferSize = bufferSize;
    mSampleRate = sampleRate;
}

PartialFilterAMFM::~PartialFilterAMFM() {}

void
PartialFilterAMFM::Reset(int bufferSize, BL_FLOAT sampleRate)
{    
    mBufferSize = bufferSize;
    mSampleRate = sampleRate;
    
    mPartials.clear();
}
           
void
PartialFilterAMFM::FilterPartials(vector<Partial> *partials)
{
#if RESCALE_HZ
    for (int i = 0; i < partials->size(); i++)
    {
        Partial &p = (*partials)[i];
        p.mFreq *= mSampleRate*0.5;
        p.mBeta0 *= mSampleRate*0.5;
    }
#endif
    
    //DBG_PrintPartials(*partials);
    
    mPartials.push_front(*partials);
    
    while(mPartials.size() > PARTIALS_HISTORY_SIZE)
        mPartials.pop_back();
    
    partials->clear();
    
    if (mPartials.empty())
        return;
    
    if (mPartials.size() == 1)
        // Assigne ids to the first series of partials
    {
        for (int j = 0; j < mPartials[0].size(); j++)
        {
            Partial &currentPartial = mPartials[0][j];
            currentPartial.GenNewId();
        }
        
        // Not enough partials to filter, need 2 series
        return;
    }
    
    if (mPartials.size() < 2)
        return;
    
    const vector<Partial> &prevPartials = mPartials[1];
    vector<Partial> &currentPartials = mTmpPartials0;
    currentPartials = mPartials[0];
    
    // Partials that was not associated at the end
    vector<Partial> &remainingCurrentPartials = mTmpPartials1;
    remainingCurrentPartials.resize(0);

    // DEBUG
    //DBG_DumpPartials("prev.txt", prevPartials, mBufferSize);
    //DBG_DumpPartials("cur.txt", currentPartials, mBufferSize);

    //DBG_PrintPartials(prevPartials);
        
#if ASSOC_AMFM
    AssociatePartialsAMFM(prevPartials, &currentPartials,
                          &remainingCurrentPartials);
#endif

#if ASSOC_HUNGARIAN_AMFM
    AssociatePartialsHungarianAMFM(prevPartials, &currentPartials,
                                   &remainingCurrentPartials);
#endif

#if ASSOC_HUNGARIAN_NERI
    AssociatePartialsHungarianNeri(prevPartials, &currentPartials,
                                   &remainingCurrentPartials);
#endif
    
    vector<Partial> &deadZombiePartials = mTmpPartials7;
    ComputeZombieDeadPartials(prevPartials, currentPartials, &deadZombiePartials);
    
    // Add zombie and dead partial
    for (int i = 0; i < deadZombiePartials.size(); i++)
        currentPartials.push_back(deadZombiePartials[i]);

#if 1 //0 // 1
    if (mPartials.size() >= 3)
        FixPartialsCrossing(mPartials[2], mPartials[1], &currentPartials);
#endif
    
    // At the end, there remains the partial that have not been matched
    //
    // Add them at to the history for next time
    //
    for (int i = 0; i < remainingCurrentPartials.size(); i++)
    {
        Partial p = remainingCurrentPartials[i];
        
        p.GenNewId();
        
        currentPartials.push_back(p);
    }
    
    // Then sort the new partials by frequency
    sort(currentPartials.begin(), currentPartials.end(), Partial::FreqLess);
    
    //
    // Update: add the partials to the history
    // (except the dead ones)
    mPartials[0].clear();
    for (int i = 0; i < currentPartials.size(); i++)
    {
        const Partial &currentPartial = currentPartials[i];

#if 0 // ORIGIN
        // TEST: do not skip the dead partials:
        // they will be used for fade out !
        //if (currentPartial.mState != Partial::DEAD)
        mPartials[0].push_back(currentPartial);
#endif
#if 1
        if (currentPartial.mState != Partial::DEAD)
            mPartials[0].push_back(currentPartial);
#endif
    }

    *partials = mPartials[0];

#if RESCALE_HZ
    BL_FLOAT coeff = 1.0/(mSampleRate*0.5);
    for (int i = 0; i < partials->size(); i++)
    {
        Partial &p = (*partials)[i];
        p.mFreq *= coeff;
        p.mBeta0 *= coeff;

        //#if !EXTRAPOLATE_KALMAN
        //p.mPredictedFreq = p.mFreq;
        //#endif
    }
#endif
}

void
PartialFilterAMFM::
AssociatePartialsAMFM(const vector<Partial> &prevPartials,
                      vector<Partial> *currentPartials,
                      vector<Partial> *remainingCurrentPartials)
{
    // Quick optimization (avoid long freezing)
    // (later, will use hungarian)
#define MAX_NUM_ITER 5
    
    // Sort current partials and prev partials by increasing frequency
    sort(currentPartials->begin(), currentPartials->end(), Partial::FreqLess);
    
    vector<Partial> &prevPartials0 = mTmpPartials5;
    prevPartials0 = prevPartials;
    sort(prevPartials0.begin(), prevPartials0.end(), Partial::FreqLess);
    
    // Associated partials
    bool stopFlag = true;
    int numIters = 0;
    do {
        stopFlag = true;

        numIters++;
        
        for (int i = 0; i < prevPartials0.size(); i++)
        {
            const Partial &prevPartial = prevPartials0[i];
            
            // Check if the link is already done
            if (((int)prevPartial.mId != -1) &&
                (FindPartialById(*currentPartials, (int)prevPartial.mId) != -1))
                // Already linked
                continue;
            
            for (int j = 0; j < currentPartials->size(); j++)
            {
                Partial &currentPartial = (*currentPartials)[j];
                
                if (currentPartial.mId == prevPartial.mId)
                    continue;
                
                BL_FLOAT LA = ComputeLA(prevPartial, currentPartial);
                BL_FLOAT LF = ComputeLF(prevPartial, currentPartial);

                bool bigJump = CheckBigJump(prevPartial, currentPartial);
                
                // As is the paper
                if ((LA > 0.5) && (LF > 0.5) &&
                    // Avoid big jumps
                    !bigJump)
                     
                    // Associate!
                {
                    // Current partial already has an id
                    bool mustFight0 = (currentPartial.mId != -1);

                    int fight1Idx = FindPartialById(*currentPartials,
                                                    (int)prevPartial.mId);
                    // Prev partial already has some association with the current id
                    bool mustFight1 = (fight1Idx != -1);
                        
                    if (!mustFight0 && !mustFight1)
                    {
                        currentPartial.mId = prevPartial.mId;
                        currentPartial.mAge = prevPartial.mAge;
                        
#if EXTRAPOLATE_KALMAN
                        currentPartial.mKf = prevPartial.mKf; //
#endif
                        
                        stopFlag = false;
                        
                        continue;
                    }
                        
                    // Fight!
                    //
                    
                    // Find the previous link for case 0
                    int otherPrevIdx =
                        FindPartialById(prevPartials0, (int)currentPartial.mId);
                    // Find prev partial
                    Partial prevPartialFight =
                        mustFight0 ? prevPartials0[otherPrevIdx] : prevPartial;
                    // Find current partial
                    Partial currentPartialFight =
                        mustFight0 ? currentPartial : (*currentPartials)[fight1Idx];

                    // Compute scores
                    BL_FLOAT otherLA =
                        ComputeLA(prevPartialFight, currentPartialFight);
                    BL_FLOAT otherLF =
                        ComputeLF(prevPartialFight, currentPartialFight);
                    
                    // Joint likelihood
                    BL_FLOAT j0 = LA*LF;
                    BL_FLOAT j1 = otherLA*otherLF;
                    if (j0 > j1)
                        // Current partial won
                    {
                        currentPartial.mId = prevPartial.mId;
                        currentPartial.mAge = prevPartial.mAge;
                        
#if EXTRAPOLATE_KALMAN
                        currentPartial.mKf = prevPartial.mKf; //
#endif

                        // Disconnect for case 1
                        if (mustFight1)
                            (*currentPartials)[fight1Idx].mId = -1;
                        stopFlag = false;
                    }
                    else
                        // Other partial won
                    {
                        // Just keep it like it is!
                    }
                }
            }
        }

        // Quick optimization
        if (numIters > MAX_NUM_ITER)
            break;
        
    } while (!stopFlag);
    
    // Update partials
    vector<Partial> &newPartials = mTmpPartials6;
    newPartials.resize(0);
    
    for (int j = 0; j < currentPartials->size(); j++)
    {
        Partial &currentPartial = (*currentPartials)[j];

        if (currentPartial.mId != -1)
        {
            currentPartial.mState = Partial::ALIVE;
            currentPartial.mWasAlive = true;
    
            // Increment age
            currentPartial.mAge = currentPartial.mAge + 1;

#if 0 // No need
            
#if EXTRAPOLATE_KALMAN
            ExtrapolatePartialKalman(&currentPartial);
#endif
            
#if EXTRAPOLATE_AMFM
            ExtrapolatePartialAMFM(&currentPartial);
#endif

#endif
            
            newPartials.push_back(currentPartial);
        }
    }
    
    // Add the remaining partials
    remainingCurrentPartials->clear();
    for (int i = 0; i < currentPartials->size(); i++)
    {
        const Partial &p = (*currentPartials)[i];
        if (p.mId == -1)
            remainingCurrentPartials->push_back(p);
    }
    
    // Update current partials
    *currentPartials = newPartials;
}

void
PartialFilterAMFM::
AssociatePartialsHungarianAMFM(const vector<Partial> &prevPartials,
                               vector<Partial> *currentPartials,
                               vector<Partial> *remainingCurrentPartials)
{    
    // Init cost matrix (MxN)
    vector<vector<BL_FLOAT> > costMatrix;
    costMatrix.resize(prevPartials.size());
    for (int i = 0; i < costMatrix.size(); i++)
        costMatrix[i].resize(currentPartials->size());
    
    // Fill the cost matrix
    for (int i = 0; i < costMatrix.size(); i++)
    {
        for (int j = 0; j < costMatrix[i].size(); j++)
        {
            BL_FLOAT LA = ComputeLA(prevPartials[i], (*currentPartials)[j]);
            BL_FLOAT LF = ComputeLF(prevPartials[i], (*currentPartials)[j]);

#if 0 // Check discard?
            if ((LA < 0.5) || (LF < 0.5))
                // Discard
                costMatrix[i][j] = BL_INF;
            else
#endif
                costMatrix[i][j] = 1.0 - LA*LF;
        }
    }

    // Solve
    HungarianAlgorithm HungAlgo;
	vector<int> assignment;
	BL_FLOAT cost = HungAlgo.Solve(costMatrix, assignment);
    
    for (int i = 0; i < assignment.size(); i++)
    {
        int a = assignment[i];
       
        // If num prev > num current, there will be some unassigned partials
        // (int this case, assignment is -1)
        if ((a != -1) && 
            (prevPartials[i].mId != -1))
            (*currentPartials)[a].mId = prevPartials[i].mId;
    }

    vector<Partial> newPartials;
    
    // Add the remaining partials
    remainingCurrentPartials->clear();
    for (int i = 0; i < currentPartials->size(); i++)
    {
        Partial &p = (*currentPartials)[i];
        if (p.mId != -1)
        {
            p.mState = Partial::ALIVE;
            p.mWasAlive = true;
    
            // Increment age
            p.mAge = p.mAge + 1;
            
            newPartials.push_back(p);
        }
        else          
            remainingCurrentPartials->push_back(p);
    }

    *currentPartials = newPartials;
}

// Compute score like in the paper
void
PartialFilterAMFM::
AssociatePartialsHungarianNeri(const vector<Partial> &prevPartials,
                               vector<Partial> *currentPartials,
                               vector<Partial> *remainingCurrentPartials)
{
    // Parameters
    /*const*/ BL_FLOAT delta = 0.2;
    /*const*/ BL_FLOAT zetaF = 50.0; // in Hz
    /*const*/ BL_FLOAT zetaA = 15; // in dB

#if !RESCALE_HZ
    zetaF *= 1.0/(mSampleRate*0.5);
#endif
    
    // These can't be <= 0
    if (zetaF < 1e-1)
        zetaF = 1e-1;
    if (zetaA < 1e-1)
        zetaA = 1e-1;
 
    //Convert to log from dB
    zetaA = zetaA/20*log(10);

    
    // Init cost matrix (MxN)
    vector<vector<BL_FLOAT> > costMatrix;
    costMatrix.resize(prevPartials.size());
    for (int i = 0; i < costMatrix.size(); i++)
        costMatrix[i].resize(currentPartials->size());
    
    // Fill the cost matrix
    for (int i = 0; i < costMatrix.size(); i++)
    {
        for (int j = 0; j < costMatrix[i].size(); j++)
        {
            BL_FLOAT A;
            BL_FLOAT B;
            ComputeScoreNeri(prevPartials[i], (*currentPartials)[j],
                             delta, zetaF, zetaA,
                             &A, &B);

            costMatrix[i][j] = (A < B) ? A : B;
        }
    }

    // Solve
    HungarianAlgorithm HungAlgo;
	vector<int> assignment;
	BL_FLOAT cost = HungAlgo.Solve(costMatrix, assignment);
    
    for (int i = 0; i < assignment.size(); i++)
    {
        int a = assignment[i];
       
        // If num prev > num current, there will be some unassigned partials
        // (int this case, assignment is -1)
        if ((a != -1) && 
            (prevPartials[i].mId != -1))
            (*currentPartials)[a].mId = prevPartials[i].mId;
    }

    vector<Partial> newPartials;
    
    // Add the remaining partials
    remainingCurrentPartials->clear();
    for (int i = 0; i < currentPartials->size(); i++)
    {
        Partial &p = (*currentPartials)[i];
        if (p.mId != -1)
        {
            p.mState = Partial::ALIVE;
            p.mWasAlive = true;
    
            // Increment age
            p.mAge = p.mAge + 1;
            
            newPartials.push_back(p);
        }
        else          
            remainingCurrentPartials->push_back(p);
    }

    *currentPartials = newPartials;
}

void
PartialFilterAMFM::ComputeZombieDeadPartials(const vector<Partial> &prevPartials,
                                             const vector<Partial> &currentPartials,
                                             vector<Partial> *zombieDeadPartials)
{
    zombieDeadPartials->clear();
    
    // Add the new zombie and dead partials
    for (int i = 0; i < prevPartials.size(); i++)
    {
        const Partial &prevPartial = prevPartials[i];

        bool found = false;
        for (int j = 0; j < currentPartials.size(); j++)
        {
            const Partial &currentPartial = currentPartials[j];
            
            if (currentPartial.mId == prevPartial.mId)
            {
                found = true;
                
                break;
            }
        }

        if (!found)
        {            
            if (prevPartial.mState == Partial::ALIVE)
            {
                // We set zombie for 1 frame only
                Partial newPartial = prevPartial;
                newPartial.mState = Partial::ZOMBIE;
                newPartial.mZombieAge = 0;

#if 1 // Good, extrapolate zombies
                
#if EXTRAPOLATE_KALMAN
                ExtrapolatePartialKalman(&newPartial);
#endif

#if EXTRAPOLATE_AMFM
                ExtrapolatePartialAMFM(&newPartial);
#endif

#endif
                if (newPartial.mZombieAge < MAX_ZOMBIE_AGE)
                    zombieDeadPartials->push_back(newPartial);
            }
            else if (prevPartial.mState == Partial::ZOMBIE)
            {
                Partial newPartial = prevPartial;
                newPartial.mZombieAge++;
                
                if (newPartial.mZombieAge >= MAX_ZOMBIE_AGE)
                {
                    newPartial.mState = Partial::DEAD;
                }
                else
                {
#if 1
                
#if EXTRAPOLATE_KALMAN
                    ExtrapolatePartialKalman(&newPartial);
#endif

#if EXTRAPOLATE_AMFM
                    ExtrapolatePartialAMFM(&newPartial);
#endif
                
#endif
                }
                
                zombieDeadPartials->push_back(newPartial);
            }
            
            // If DEAD, do not add, forget it
        }
    }
}

// Simple fix for partial crossing error
void
PartialFilterAMFM::FixPartialsCrossing(const vector<Partial> &partials0,
                                       const vector<Partial> &partials1,
                                       vector<Partial> *partials2)
{    
    // Tmp optimization
#define MIN_PARTIAL_AGE 5

#if RESCALE_HZ
#define MAX_SWAP_FREQ 100.0
#else
#define MAX_SWAP_FREQ 100.0/(mSampleRate*0.5)
#endif
    
    const vector<Partial> partials2Copy = *partials2;
        
    Partial p0[3];
    Partial p1[3];
    
    for (int i = 0; i < partials2Copy.size(); i++)
    {
        p0[2] = partials2Copy[i];
        if (p0[2].mId == -1)
            continue;

        // Tmp optim
        if (p0[2].mAge < MIN_PARTIAL_AGE)
            continue;
        
        int idx01 = FindPartialById(partials1, p0[2].mId);
        if (idx01 == -1)
            continue;
        p0[1] = partials1[idx01];

        int idx00 = FindPartialById(partials0, p0[2].mId);
        if (idx00 == -1)
            continue;
        p0[0] = partials0[idx00];

        //
        for (int j = i + 1; j < partials2Copy.size(); j++)
        {
            p1[2] = partials2Copy[j];
            if (p1[2].mId == -1)
                continue;

            // Try to avoid very messay results
            if (std::fabs(p1[2].mFreq - p0[2].mFreq) > MAX_SWAP_FREQ)
                continue;
            
            int idx11 = FindPartialById(partials1, p1[2].mId);
            if (idx11 == -1)
                continue;
            p1[1] = partials1[idx11];
            
            int idx10 = FindPartialById(partials0, p1[2].mId);
            if (idx10 == -1)
                continue;
            p1[0] = partials0[idx10];

            // Extrapolated values
            BL_FLOAT extraP0 = p0[1].mFreq + (p0[1].mFreq - p0[0].mFreq);
            BL_FLOAT extraP1 = p1[1].mFreq + (p1[1].mFreq - p1[0].mFreq);

            // Check if extrapolated points intersect
            BL_FLOAT extraSeg0[2][2] = { { p0[1].mFreq, 0.0 }, { extraP0, 1.0 } };
            BL_FLOAT extraSeg1[2][2] = { { p1[1].mFreq, 0.0 }, { extraP1, 1.0 } };
            bool extraIntersect = BLUtilsMath::SegSegIntersect2(extraSeg0, extraSeg1);

            // Check if real points intersect
            BL_FLOAT seg0[2][2] = { { p0[1].mFreq, 0.0 }, { p0[2].mFreq, 1.0 } };
            BL_FLOAT seg1[2][2] = { { p1[1].mFreq, 0.0 }, { p1[2].mFreq, 1.0 } };
            bool intersect = BLUtilsMath::SegSegIntersect2(seg0, seg1);
            
            if (intersect != extraIntersect)
            {   
                int tmpId = p0[2].mId;
                p0[2].mId = p1[2].mId;
                p1[2].mId = tmpId;
                
                (*partials2)[i].mId = p0[2].mId;
                (*partials2)[j].mId = p1[2].mId;

                // ??
                break;
            }
        }
    }
}

int
PartialFilterAMFM::FindPartialById(const vector<Partial> &partials, int idx)
{
    long numPartials = partials.size();
    for (int i = 0; i < numPartials; i++)
    {
        const Partial &partial = partials[i];
        
        if (partial.mId == idx)
            return i;
    }
    
    return -1;
}

// Compute amplitude likelihood
// (increase when the penality decrease)
BL_FLOAT
PartialFilterAMFM::ComputeLA(const Partial &prevPartial,
                             const Partial &currentPartial)
{
    // Use general polygon
    BL_FLOAT x[4] = { 0.0, 1.0, 1.0, 0.0 };
    BL_FLOAT y[4] = { prevPartial.mAmp,
                      prevPartial.mAmp + prevPartial.mAlpha0,
                      currentPartial.mAmp,
                      currentPartial.mAmp - currentPartial.mAlpha0 };
                        
    BL_FLOAT area = BLUtilsMath::PolygonArea(x, y, 4);
    
    // u
    BL_FLOAT denom = sqrt(currentPartial.mAmp*prevPartial.mAmp);
    BL_FLOAT ua = 0.0;
    if (denom > BL_EPS)
        ua = area/denom;
    
    // Likelihood
    BL_FLOAT LA = 1.0/(1.0 + ua);
    
    return LA;
}

// Compute frequency likelihood
// (increase when the penality decrease)
BL_FLOAT
PartialFilterAMFM::ComputeLF(const Partial &prevPartial,
                             const Partial &currentPartial)
{
    // General polygon
    BL_FLOAT x[4] = { 0.0, 1.0, 1.0, 0.0 };
    BL_FLOAT y[4] = { prevPartial.mFreq,
                      prevPartial.mFreq + prevPartial.mBeta0,
                      currentPartial.mFreq,
                      currentPartial.mFreq - currentPartial.mBeta0 };

    // TEST
    //for (int i = 0; i < 4; i++)
    //    y[i] = log(1.0 + y[i]*mSampleRate*0.5);
    
    BL_FLOAT area = BLUtilsMath::PolygonArea(x, y, 4);
    
    // u
    BL_FLOAT denom = sqrt(currentPartial.mFreq*prevPartial.mFreq);
    //BL_FLOAT denom = sqrt(log(1.0 + currentPartial.mFreq*mSampleRate*0.5)*
    //                      log(1.0 + prevPartial.mFreq*mSampleRate*0.5));
    
    BL_FLOAT uf = 0.0;
    if (denom > BL_EPS)
        uf = area/denom;
    
    // Likelihood
    BL_FLOAT LF = 1.0/(1.0 + uf);
        
    return LF;
}

void
PartialFilterAMFM::ComputeScoreNeri(const Partial &prevPartial,
                                    const Partial &currentPartial,
                                    BL_FLOAT delta, BL_FLOAT zetaF, BL_FLOAT zetaA,
                                    BL_FLOAT *A, BL_FLOAT *B)
{
    BL_FLOAT deltaF =
        (currentPartial.mFreq - currentPartial.mBeta0*0.5) -
        (prevPartial.mFreq + prevPartial.mBeta0*0.5);
    BL_FLOAT deltaA = (currentPartial.mAmp - currentPartial.mAlpha0*0.5) -
        (prevPartial.mAmp + prevPartial.mAlpha0*0.5);
    
    // Like in paper
    //BL_FLOAT denom = (2.0*log(delta - 2.0) - 2.0*log(delta - 1.0));
    //BL_FLOAT sigmaF2 = zetaF*zetaF/denom;
    //BL_FLOAT sigmaA2 = zetaA*zetaA/denom;

    // Like in github
    BL_FLOAT coeff = log((delta - 1.0)/(delta - 2.0));
    BL_FLOAT sigmaF2 = -zetaF*zetaF*coeff;
    BL_FLOAT sigmaA2 = -zetaA*zetaA*coeff;

    // Like in paper
    //*A = 1.0 - exp(-deltaF*deltaF/(2.0*sigmaF2) - deltaA*deltaA/(2.0*sigmaA2));

    // Like in github
    *A = 1.0 - exp(-deltaF*deltaF/sigmaF2 - deltaA*deltaA/sigmaA2);
     
    *B = 1.0 - (1.0 - delta)*(*A);
}   

// Extrapolate the partial with alpha0 and beta0
void
PartialFilterAMFM::ExtrapolatePartialAMFM(Partial *p)
{
    // Amp is normalized
    p->mAmp += p->mAlpha0;
    if (p->mAmp < 0.0)
        p->mAmp = 0.0;
    if (p->mAmp > 1.0)
        p->mAmp = 1.0;

    // Freq is real freq
    p->mFreq += p->mBeta0;
    
#if RESCALE_HZ
    if (p->mFreq < 0.0)
        p->mFreq = 0.0;
    if (p->mFreq > mSampleRate*0.5)
        p->mFreq = mSampleRate*0.5;
#else
    if (p->mFreq < 0.0)
        p->mFreq = 0.0;
    if (p->mFreq > 1.0)
        p->mFreq = 1.0;
#endif
}

void
PartialFilterAMFM::ExtrapolatePartialKalman(Partial *p)
{
#if RESCALE_HZ
    p->mFreq /= mSampleRate*0.5;
#endif
    
    p->mFreq = p->mKf.updateEstimate(p->mFreq);

#if RESCALE_HZ
    p->mFreq *= mSampleRate*0.5;
#endif
}

void
PartialFilterAMFM::DBG_PrintPartials(const vector<Partial> &partials)
{
    fprintf(stderr, "-------------------\n");
    for (int i = 0; i < partials.size(); i++)
    {
        const Partial &p = partials[i];
        
        fprintf(stderr, "id: %ld state: %d amp: %g freq: %g\n",
                p.mId, p.mState, p.mAmp, p.mFreq);
    }
}

void
PartialFilterAMFM::DBG_DumpPartials(const char *fileName,
                                    const vector<Partial> &partials, int size)
{
#define MIN_AMP_DB -120.0
    
    WDL_TypedBuf<BL_FLOAT> data;
    data.Resize(size);
    BLUtils::FillAllZero(&data);
    
    for (int i = 0; i < partials.size(); i++)
    {
        const Partial &p = partials[i];
        
        int idx = p.mFreq*size;
        if (idx >= size)
            continue;

        BL_FLOAT amp = (p.mAmp - MIN_AMP_DB)/(0.0 - MIN_AMP_DB);
        data.Get()[idx] = amp; //p.mAmp;
    }

    BLDebug::DumpData(fileName, data);
}

bool
PartialFilterAMFM::CheckBigJump(const Partial &prevPartial,
                                const Partial &currentPartial)
{
#define BIG_JUMP_COEFF 16.0 //4.0 //16.0 //4.0 //2.0

    BL_FLOAT oneBinEps = 1.0/mBufferSize;
#if RESCALE_HZ
    oneBinEps *= mSampleRate*0.5;
#endif

    // Check if partials are very close
    // (in this case, it sould keep the same id, even if beta0 is very small)
    if (std::fabs(prevPartial.mFreq - currentPartial.mFreq) <
        oneBinEps*BIG_JUMP_COEFF)
        return false;
        
    // Extrapoled frequency, from prev partial
    BL_FLOAT extraFreq0 = prevPartial.mFreq + prevPartial.mBeta0;
    bool flag0 = (currentPartial.mFreq > extraFreq0 +
                  BIG_JUMP_COEFF*(extraFreq0 - prevPartial.mFreq));


    bool flag1 = (currentPartial.mFreq < extraFreq0 -
                  BIG_JUMP_COEFF*(extraFreq0 - prevPartial.mFreq));

    // Extrapolated, from current partial
    BL_FLOAT extraFreq1 = currentPartial.mFreq - currentPartial.mBeta0;
    bool flag2 = (prevPartial.mFreq > extraFreq1 +
                  BIG_JUMP_COEFF*(extraFreq1 - currentPartial.mFreq));

    bool flag3 = (prevPartial.mFreq < extraFreq1 -
                  BIG_JUMP_COEFF*(extraFreq1 - currentPartial.mFreq));

    // Use flags "&&" to mix cases
    // e.g to give a chance to a case where prev beta0 is almost 0,
    // but current beta0 has a significant value.
    if (flag0 && flag3)
        return true;
    
    if (flag1 && flag2)
        return true;
    
    return false;
}
