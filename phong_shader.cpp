// Dawon Ahn & Bonnie Li
#include "light.h"
#include "parse.h"
#include "object.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"

Phong_Shader::Phong_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    color_ambient=parse->Get_Color(in);
    color_diffuse=parse->Get_Color(in);
    color_specular=parse->Get_Color(in);
    in>>specular_power;
}

vec3 Phong_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    vec3 color;
    //TODO; //determine the color

    //ambient + diffuse + specular = phong reflection (color)
    vec3 ambient = render_world.ambient_intensity * color_ambient->Get_Color(hit.uv);
    // vec3 ambient = color_ambient 
    vec3 diffuse;
    vec3 specular;

    const Color* c = render_world.ambient_color;

    if(c != NULL){
        ambient *= c->Get_Color(hit.uv);  // c can be NULL sometimes

    }


    for(const Light* l : render_world.lights){
        const Light* light = l; //assign light
        vec3 intersec = light->position - intersection_point; //intersect pos
        vec3 n = intersec.normalized(); //direction vec from point to light source

        if(render_world.enable_shadows){ // when there is shadow
            Hit h;
            Shaded_Object shaded_object;
            Ray shade (intersection_point,n);
            std::tie(shaded_object, h) = render_world.Closest_Intersection(shade);   
            if(shaded_object.object != NULL && h.dist < intersec.magnitude()){
                break;
            }
        }

        double norm = dot(n,normal);
        diffuse += color_diffuse->Get_Color({hit.uv}) 
        * light->Emitted_Light(intersec) * std::max(0.0, norm); // diffuse = color_diffuse * (surface normal dot intersec)

        if (norm < 0){
            specular = {0, 0, 0};
        }else{
            vec3 ref = ((2 * norm * normal) - n).normalized(); //reflection = 2(n dot normal)*normal - n ref:https://en.wikipedia.org/wiki/Phong_reflection_model
            double ray_ref = -dot(ref, ray.direction.normalized());
            
            specular += color_specular->Get_Color(hit.uv) * std::pow(std::max(0.0, 
            ray_ref), specular_power) * light->Emitted_Light(intersec);  //ref: https://www.robots.ox.ac.uk/~att/index.html#:~:text=Phong%20gave%20spectral%20reflectivity%20as,generally%20accepted%20phong%20lighting%20equation.
        }

    }


    color = ambient + diffuse + specular;
    return color;
}
