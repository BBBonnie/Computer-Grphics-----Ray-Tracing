// Dawon Ahn, Boning Li
#include "reflective_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Reflective_Shader::Reflective_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    shader=parse->Get_Shader(in);
    in>>reflectivity;
    reflectivity=std::max(0.0,std::min(1.0,reflectivity));
}

vec3 Reflective_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    vec3 direction = ray.direction.normalized();
    Ray refRay;
    refRay.endpoint = intersection_point;
    refRay.direction = (direction - 2 * dot(direction, normal) * normal).normalized();  // reflected ray formula r = i-2(i`n)n

    vec3 refColor = render_world.Cast_Ray(refRay, recursion_depth + 1);
    vec3 objectColor = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);


    vec3 color = refColor * reflectivity + objectColor * (1 - reflectivity);

    return color;
}
