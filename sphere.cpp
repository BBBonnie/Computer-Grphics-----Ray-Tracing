#include "sphere.h"
#include "ray.h"

Sphere::Sphere(const Parse* parse, std::istream& in)
{
    in>>name>>center>>radius;
}

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    //TODO;
    Hit result;
    vec3 w = ray.endpoint - center; // w = e-c
    double a = dot(ray.direction, ray.direction);
    double b = 2 * dot(ray.direction, w);
    //double c = w.magnitude_squared() - radius*radius;
    double c = dot(w, w) - radius*radius;

    double delta =b*b - 4*a*c;
    if(delta > 0){
        double d1 = (-b + sqrt(delta)) / 2*a;
        double d2 = (-b - sqrt(delta)) / 2*a;    //math +-
        if(d1 >= small_t && d2 >= small_t){
            result.dist = std::min(d1, d2);

        }else if(d1 >= small_t && d2 < small_t){
            result.dist = d1;
        }else if(d1 < small_t && d2 >= small_t){
            result.dist = d2;
        }


    }


    return result;
}

vec3 Sphere::Normal(const Ray& ray, const Hit& hit) const
{
    vec3 normal;
    //TODO; // compute the normal direction
    normal = (ray.Point(hit.dist) - center).normalized();
    return normal;
}

std::pair<Box,bool> Sphere::Bounding_Box(int part) const
{
    return {{center-radius,center+radius},false};
}
