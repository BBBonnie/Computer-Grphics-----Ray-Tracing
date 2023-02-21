// Dawon Ahn & Boning Li

#include "mesh.h"
#include <fstream>
#include <limits>
#include <string>
#include <algorithm>

// Consider a triangle to intersect a ray if the ray intersects the plane of the
// triangle with barycentric weights in [-weight_tolerance, 1+weight_tolerance]
static const double weight_tolerance = 1e-4;

Mesh::Mesh(const Parse *parse, std::istream &in)
{
    std::string file;
    in >> name >> file;
    Read_Obj(file.c_str());
}

// Read in a mesh from an obj file.  Populates the bounding box and registers
// one part per triangle (by setting number_parts).
void Mesh::Read_Obj(const char *file)
{
    std::ifstream fin(file);
    if (!fin)
    {
        exit(EXIT_FAILURE);
    }
    std::string line;
    ivec3 e, t;
    vec3 v;
    vec2 u;
    while (fin)
    {
        getline(fin, line);

        if (sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3)
        {
            vertices.push_back(v);
        }

        if (sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3)
        {
            for (int i = 0; i < 3; i++)
                e[i]--;
            triangles.push_back(e);
        }

        if (sscanf(line.c_str(), "vt %lg %lg", &u[0], &u[1]) == 2)
        {
            uvs.push_back(u);
        }

        if (sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &e[0], &t[0], &e[1], &t[1], &e[2], &t[2]) == 6)
        {
            for (int i = 0; i < 3; i++)
                e[i]--;
            triangles.push_back(e);
            for (int i = 0; i < 3; i++)
                t[i]--;
            triangle_texture_index.push_back(t);
        }
    }
    num_parts = triangles.size();
}

// Check for an intersection against the ray.  See the base class for details.
Hit Mesh::Intersection(const Ray &ray, int part) const
{
    Hit hit, h;
    double d = std::numeric_limits<double>::max();
    if (part > 0)
    {
        hit = Intersect_Triangle(ray, part);
    }
    else
    {
        for (long unsigned int i = 0; i < triangles.size(); ++i)
        {
            h = Intersect_Triangle(ray, i);
            if (d > h.dist && h.dist >= 0 && h.triangle != -1)
            { // if min dist > hit dist, update hit and min dist
                hit = h;
                d = h.dist;
            }
        }
    }

    // std::cout << "dist: " << hit.dist << " triangle: " << hit.triangle << " uv: " << hit.uv << std::endl;
    return hit;
}

// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const Ray &ray, const Hit &hit) const
{
    assert(hit.triangle >= 0);

    vec3 a = vertices[triangles[hit.triangle][1]] - vertices[triangles[hit.triangle][0]];
    vec3 b = vertices[triangles[hit.triangle][2]] - vertices[triangles[hit.triangle][0]];

    return vec3(cross(a, b).normalized());
}

// This is a helper routine whose purpose is to simplify the implementation
// of the Intersection routine.  It should test for an intersection between
// the ray and the triangle with index tri.  If an intersection exists,
// record the distance and return true.  Otherwise, return false.
// This intersection should be computed by determining the intersection of
// the ray and the plane of the triangle.  From this, determine (1) where
// along the ray the intersection point occurs (dist) and (2) the barycentric
// coordinates within the triangle where the intersection occurs.  The
// triangle intersects the ray if dist>small_t and the barycentric weights are
// larger than -weight_tolerance.  The use of small_t avoid the self-shadowing
// bug, and the use of weight_tolerance prevents rays from passing in between
// two triangles.
Hit Mesh::Intersect_Triangle(const Ray &ray, int tri) const
{
    Hit hit;
    hit.dist = std::numeric_limits<double>::max();

    ivec3 triang = triangles[tri];
    vec3 a = vertices[triang[0]];
    vec3 b = vertices[triang[1]];
    vec3 c = vertices[triang[2]];

    vec3 dir = ray.direction.normalized();

    vec3 norm = cross(b-a, c-a).normalized(); // normal
    hit.dist = -dot(norm, ray.endpoint - a) / dot(norm, dir);
    vec3 p = ray.Point(hit.dist); // intersection point

    if (hit.dist > small_t)
    {
        double area = dot(cross(b - a, c - a), norm);
        double area1 = -dot(cross(p - b, c - b), norm);
        double area2 = -dot(cross(p - c, a - c), norm);
        double area3 = -dot(cross(p - a, b - a), norm);

        if (area1 < 0 || area2 < 0 || area3 < 0)
            return hit;

        double alpha = area1 / area;
        double beta = area2 / area;
        double gamma = area3 / area;

        // std::cout << "Weights: (" << alpha << ", " << beta << "," << gamma <<")"<< std::endl;
        if (alpha > -weight_tolerance && beta > -weight_tolerance && gamma > -weight_tolerance){
            hit.triangle = tri;

            ivec3 texture = triangle_texture_index[tri];
            vec2 uv = (alpha * uvs[texture[0]]) + (beta * uvs[texture[1]]) + (gamma * uvs[texture[2]]);
            hit.uv = uv;
        }
    }

    return hit;
}

std::pair<Box, bool> Mesh::Bounding_Box(int part) const
{
    if (part < 0)
    {
        Box box;
        box.Make_Empty();
        for (const auto &v : vertices)
            box.Include_Point(v);
        return {box, false};
    }

    ivec3 e = triangles[part];
    vec3 A = vertices[e[0]];
    Box b = {A, A};
    b.Include_Point(vertices[e[1]]);
    b.Include_Point(vertices[e[2]]);
    return {b, false};
}
