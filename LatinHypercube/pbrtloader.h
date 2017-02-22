#ifndef PBRTLOADER_H
#define PBRTLOADER_H

#include <string>
#include <vector>
#include <math.h>

struct vec3
{
    float x;
    float y;
    float z;

    vec3() :
        x(0),
        y(0),
        z(0)
    {
    }

    vec3(const float &f) :
        x(f),
        y(f),
        z(f)
    {
    }

    vec3(const float &a, const float &b, const float &c) :
        x(a),
        y(b),
        z(c)
    {
    }

    vec3 operator+(vec3 p) const
    {
        return vec3(x + p.x, y + p.y, z + p.z);
    }

    vec3 operator-(vec3 p) const
    {
        return vec3(x - p.x, y - p.y, z - p.z);
    }

    vec3 operator*(vec3 p) const
    {
        return vec3(x * p.x, y * p.y, z * p.z);
    }

    vec3 min(const vec3 &p) const
    {
        vec3 out;
        out.x = std::min(x, p.x);
        out.y = std::min(y, p.y);
        out.z = std::min(z, p.z);

        return out;
    }

    vec3 max(const vec3 &p) const
    {
        vec3 out;
        out.x = std::max(x, p.x);
        out.y = std::max(y, p.y);
        out.z = std::max(z, p.z);

        return out;
    }
};

struct AABB
{
    vec3 V[2];

    AABB()
    {
        V[0] = vec3(INFINITY);
        V[1] = vec3(-INFINITY);
    }

    void clipPoint(const vec3 &P)
    {
        V[0] = P.min(V[0]);
        V[1] = P.max(V[1]);
    }

    void clipBox(const AABB &box)
    {
        clipPoint(box.V[0]);
        clipPoint(box.V[1]);
    }

    bool isPointInside(const vec3 &P) const
    {
        return (P.x > V[0].x && P.y > V[0].y && P.z > V[0].z && P.x < V[1].x && P.y < V[1].y && P.z < V[1].z);
    }

    vec3 getCenter() const
    {
        vec3 diff = V[1] - V[0];
        return diff * 0.5f + V[0];
    }
};


class PBRTLoader
{
public:
    PBRTLoader();

    AABB extractAABBFromFile(const std::string &pbrt_path, AABB &box);

private:
    std::string extractNextParameter(std::string &line, std::string &valuesString, bool *values, int *pos);
    std::string extractValues(std::string &line);
};

#endif // PBRTLOADER_H
