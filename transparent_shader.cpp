// Dawon Ahn, Boning Li
#include "transparent_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Transparent_Shader::
Transparent_Shader(const Parse* parse,std::istream& in)
{
    in>>name>>index_of_refraction>>opacity;
    shader=parse->Get_Shader(in);
    assert(index_of_refraction>=1.0);
}

// Use opacity to determine the contribution of this->shader and the Schlick
// approximation to compute the reflectivity.  This routine shades transparent
// objects such as glass.  Note that the incoming and outgoing indices of
// refraction depend on whether the ray is entering the object or leaving it.
// You may assume that the object is surrounded by air with index of refraction
// 1.
vec3 Transparent_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    // std::cout<< "index_of_refraction: " << index_of_refraction << std::endl;

    vec3 direction = ray.direction.normalized();
    double ref = std::pow(1 - index_of_refraction, 2) / std::pow(1 + index_of_refraction, 2); // r0 = (n1-n2 / n1+n2)^2
    double ref2 = std::pow(index_of_refraction, 2);

    Ray refRay;
    refRay.endpoint = intersection_point;
    refRay.direction = (direction - 2 * dot(direction, normal) * normal).normalized();  // reflected ray formula r = i-2(i`n)n
    // std::cout << "Ref.: "  << refRay.endpoint << refRay.direction <<std::endl;

    Ray transRay;
    transRay.endpoint = intersection_point;

    double reflectance;
    if(dot(direction, normal) < 0){
        double tmp = 1 - 1/ref2 * (1 - std::pow(dot(normal, direction), 2)) ;
        double tmp2 =  dot(normal, direction)/index_of_refraction + std::sqrt(tmp); 
        transRay.direction = direction/index_of_refraction - tmp2 * normal;
        if (tmp <=0)
            reflectance = 1; 
        else
            reflectance = ref + (1 - ref) * std::pow(1 + dot(normal, direction), 5); 
        // std::cout << "Trans 0: "  << transRay.endpoint << transRay.direction <<std::endl;
    }else{
        double tmp = 1 - ref2 * (1 - std::pow(dot(-normal, direction), 2));
        double tmp2 =  index_of_refraction * dot(normal, direction) - std::sqrt(tmp); 
        transRay.direction = index_of_refraction * direction - tmp2 * normal;
        if (tmp <= 0)
            reflectance = 1; 
        else
            reflectance = ref + (1 - ref) * std::pow(1 - abs(dot(normal, direction)), 5); 
        // std::cout << "Trans: "  << transRay.endpoint << transRay.direction <<std::endl;
    }
    double transmittance = 1 - reflectance;


    vec3 refColor = render_world.Cast_Ray(refRay, recursion_depth + 1);
    vec3 tranColor = render_world.Cast_Ray(transRay, recursion_depth + 1);
    vec3 objectColor = shader->Shade_Surface(render_world, ray, hit, intersection_point, normal, recursion_depth);



    vec3 color = opacity * objectColor + (1-opacity) * (reflectance*refColor + transmittance * tranColor);  //formula in lecture
    // std::cout << "Color: " << color <<  "Reflectance " << reflectance << " Transmittance " << transmittance << std::endl;
    // std::cout << "Opacity: " << opacity << " objectColor: " << objectColor << " refColor" << refColor << " transColor" << tranColor << std::endl;

    return color;
}