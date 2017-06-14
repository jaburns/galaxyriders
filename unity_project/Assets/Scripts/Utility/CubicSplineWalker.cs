using UnityEngine;
using System;

public class CubicSplineWalker
{
    readonly public float Length;

    struct DistTimePair {
        public float TimeValue;
        public float Distance;
    }

    readonly Vector3 _anchor0;
    readonly Vector3 _control0;
    readonly Vector3 _control1;
    readonly Vector3 _anchor1;
    readonly DistTimePair[] _distTable;

    int _lastDistanceIndex;
    float _lastDistanceValue;

    public CubicSplineWalker(Vector3 anchor0, Vector3 control0, Vector3 control1, Vector3 anchor1, int segments)
    {
        this._anchor0  = anchor0;
        this._control0 = control0;
        this._control1 = control1;
        this._anchor1  = anchor1;

        if (segments < 1) segments = 1;

        _distTable = new DistTimePair[segments];

        var prevPt = _anchor0;
        var runningTotal = 0f;

        for (int i = 0; i < segments; ++i) {
            var t = (float)(i+1) / segments;
            var thisPt = Evaluate(t);
            var thisDist = (thisPt - prevPt).magnitude;

            _distTable[i] = new DistTimePair {
                TimeValue = t,
                Distance = thisDist + runningTotal
            };

            prevPt = thisPt;
            runningTotal += thisDist;
        }

        Length = runningTotal;

        _lastDistanceIndex = 0;
        _lastDistanceValue = 0f;
    }

    public Vector3 Evaluate(float t)
    {
        var u = 1 - t;
        return u*u*u*_anchor0 + 3*u*u*t*_control0 + 3*u*t*t*_control1 + t*t*t*_anchor1;
    }

    public Vector3 EvaluateByDistance(float s)
    {
        return Evaluate(getTimeFromDistance(s));
    }

    int getDistanceIndex(float s)
    {
        int index = _lastDistanceIndex;
        if (s > _lastDistanceValue) {
            while (s > _distTable[index].Distance) index++;
        } else {
            while (index > 0 && s < _distTable[index-1].Distance) index--;
        }
        _lastDistanceValue = s;
        return index;
    }

    float getTimeFromDistance(float s)
    {
        if (s < 0) return 0;
        if (s > Length) return Length;

        int index = getDistanceIndex(s);

        var thisDist = _distTable[index].Distance;
        var thisTime = _distTable[index].TimeValue;
        var prevDist = index > 0 ? _distTable[index-1].Distance : 0;
        var prevTime = index > 0 ? _distTable[index-1].TimeValue : 0;

        var relativeDist = (s - prevDist) / (thisDist - prevDist);
        return prevTime + relativeDist*(thisTime - prevTime);
    }
}
