#include "vectorTransform.h"
#include <math.h>

static float dot(Vector a, Vector b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static Vector cross(Vector a, Vector b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

static float norm(Vector v)
{
    return sqrtf(dot(v, v));
}

static Vector normalize(Vector v)
{
    float n = norm(v);

    if (n < 1e-6f)
    {
        return {0.0f, 0.0f, 0.0f};
    }

    return {
        v.x / n,
        v.y / n,
        v.z / n};
}

Vector getTransformedVector(Vector calibration, Vector data)
{
    Vector from = normalize(calibration);
    Vector to = {0.0f, 0.0f, 1.0f}; // +Z represents down / gravity

    float fromNorm = norm(from);
    if (fromNorm < 1e-6f)
    {
        return data;
    }

    Vector axis = cross(from, to);
    float s = norm(axis);
    float c = dot(from, to);

    if (s < 1e-6f)
    {
        if (c > 0.0f)
        {
            return data;
        }

        return {
            -data.x,
            data.y,
            -data.z};
    }

    axis = normalize(axis);

    Vector axisCrossData = cross(axis, data);
    float axisDotData = dot(axis, data);

    Vector result;

    result.x =
        data.x * c +
        axisCrossData.x * s +
        axis.x * axisDotData * (1.0f - c);

    result.y =
        data.y * c +
        axisCrossData.y * s +
        axis.y * axisDotData * (1.0f - c);

    result.z =
        data.z * c +
        axisCrossData.z * s +
        axis.z * axisDotData * (1.0f - c);

    return result;
}