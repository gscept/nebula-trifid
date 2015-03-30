//------------------------------------------------------------------------------
//  sequencer/beziergraph.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "sequencer/beziergraph.h"

namespace Sequencer
{
__ImplementClass(Sequencer::BezierGraph, 'BGRA', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
BezierGraph::BezierGraph() :
    preInfinity(Constant),
    postInfinity(Constant)
{
}

//------------------------------------------------------------------------------
/**
*/
BezierGraph::~BezierGraph()
{
}

//------------------------------------------------------------------------------
/**
*/
String
BezierGraph::InfinityToString(Infinity infinity)
{
    switch (infinity)
    {
    case Cycle:
            return "Cycle";
        case CycleWithOffset:
            return "CycleWithOffset";
        case Oscillate:
            return "Oscillate";
        case Linear:
            return "Linear";
        case Constant:
            return "Constant";
    }
    return "Unknown";
}

//------------------------------------------------------------------------------
/**
*/
Infinity
BezierGraph::InfinityFromString(String infinity)
{
    if ("Cycle" == infinity)
    {
        return Cycle;
    }
    else if ("CycleWithOffset" == infinity)
    {
        return CycleWithOffset;
    }
    else if  ("Oscillate" == infinity)
    {    
        return Oscillate;
    }
    else if ("Linear" == infinity)
    {    
        return Linear;
    }
    else
    {
        return Constant;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
BezierGraph::SetPreInfinity(const String& infString)
{
    this->preInfinity = this->InfinityFromString(infString);
}

//------------------------------------------------------------------------------
/**
*/
void
BezierGraph::SetPostInfinity(const String& infString)
{
    this->postInfinity = this->InfinityFromString(infString);
}

//------------------------------------------------------------------------------
/**
    Get the value of the bezier graph at a certain time.
    In other words, get the y value from x value.
*/
float
BezierGraph::EvaluateAtTime(Timing::Time time)
{
    int i;
    float graphStart = curveList[0]->GetStartP().x();
    float graphEnd = curveList[curveList.Size()-1]->GetEndP().x();
    float curveLen = graphEnd-graphStart;
    if ((time>=graphStart) && (time<=graphEnd))
    {
        for (i=0; i<curveList.Size(); i++)
        {
            if ((curveList[i]->GetStartP().x() <= time) && (curveList[i]->GetEndP().x() >= time))
            {
                return curveList[i]->EvaluateAtTime(time);
            }
        }
    }
    else if (time<graphStart)
    {
        // preinfinity
        float gradient;
        float distanceToStart = graphStart - time;
        int numOfFits = (int)(distanceToStart/curveLen);
        Ptr<BezierCurve> lastCurve = curveList[curveList.Size()-1];
        float startEndDiff;
        switch (this->preInfinity)
        {
        case Constant:
            return this->curveList[0]->EvaluateAtTime(this->curveList[0]->GetStartP().x());
            break;
        case Linear:
            gradient = (curveList[0]->GetStartCP().y() - curveList[0]->GetStartP().y())
                / (curveList[0]->GetStartCP().x() - curveList[0]->GetStartP().x());
            return curveList[0]->GetStartP().y() + gradient *
                (time-curveList[0]->GetStartP().x());
        case Cycle:
            return this->EvaluateAtTime((numOfFits+1)*curveLen + time);
            break;
        case CycleWithOffset:
            startEndDiff = lastCurve->GetEndP().y() - curveList[0]->GetStartP().y();
            return this->EvaluateAtTime((numOfFits+1)*curveLen + time)
                - ((numOfFits+1)*startEndDiff);
            break;
        case Oscillate:
            if ((numOfFits%2)==0)
            {
                return this->EvaluateAtTime(graphStart+graphEnd
                    -((numOfFits+1)*curveLen + time));
            }
            else
            {
                return this->EvaluateAtTime((numOfFits+1)*curveLen + time);
            }
            break;
        }
    }
    else
    {
        // postinfinity
        float gradient;
        float distanceToEnd = time - graphEnd;
        int numOfFits = (int)(distanceToEnd/curveLen);
        Ptr<BezierCurve> lastCurve = curveList[curveList.Size()-1];
        float startEndDiff;
        switch (this->postInfinity)
        {
        case Constant:
            return this->curveList[curveList.Size()-1]
            ->EvaluateAtTime(this->curveList[curveList.Size()-1]->GetEndP().x());
            break;
        case Linear:
            gradient = (lastCurve->GetEndP().y() - lastCurve->GetEndCP().y())
                / (lastCurve->GetEndP().x() - lastCurve->GetEndCP().x());
            return lastCurve->GetEndP().y() + gradient *
                (time-lastCurve->GetEndP().x());
        case Cycle:
            return this->EvaluateAtTime(time-(numOfFits+1)*curveLen);
            break;
        case CycleWithOffset:
            startEndDiff = lastCurve->GetEndP().y() - curveList[0]->GetStartP().y();
            return this->EvaluateAtTime(time-(numOfFits+1)*curveLen)
                + ((numOfFits+1)*startEndDiff);
            break;
        case Oscillate:
            if ((numOfFits%2)==0)
            {
                return this->EvaluateAtTime(graphStart+graphEnd
                    -(time-(numOfFits+1)*curveLen));
            }
            else
            {
                return this->EvaluateAtTime(time-(numOfFits+1)*curveLen);
            }
            break;
        }
    }
    return 0.0f;
}

}  // namespace Sequencer