//=====================
//Crystal Feng & Michelle Chu
//=====================
#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"

extern bool enable_acceleration;

Render_World::~Render_World()
{
    for(auto a:all_objects) delete a;
    for(auto a:all_shaders) delete a;
    for(auto a:all_colors) delete a;
    for(auto a:lights) delete a;
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
std::pair<Shaded_Object,Hit> Render_World::Closest_Intersection(const Ray& ray) const
{
    // TODO;
    //hw 2 ===============
    std::pair<Shaded_Object,Hit> closest_structure;
    double min = std::numeric_limits<double>::max();

    //loop through the objects vector seen in render_world.h
    for(Shaded_Object object: objects){
        // temporarily store the hit of the current object to check the dist
        Hit inter = object.object->Intersection(ray,-1);

        if(inter.dist < min && inter.dist >= small_t){
            min = inter.dist;
            closest_structure.first = object;
            closest_structure.second = inter;
        }
    }

    return closest_structure;

    //hw 2 ===============
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    // TODO; // set up the initial view ray here
    Ray ray;

    //hw 2 ===============
    ray.direction = (camera.World_Position(pixel_index) - camera.position).normalized();
    ray.endpoint = camera.position;

    //=====================

    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth) const
{
    vec3 color;
    // TODO; // determine the color here
    //hw2 ===============
    // see function in line 19
    Debug_Scope scope; 
    // Pixel_Print("cast ray (end: ", ray.endpoint, "  dir: " , ray.direction);
    std::pair<Shaded_Object,Hit>closest = Closest_Intersection(ray);
    Shaded_Object closest_object = closest.first;
    Hit closest_hit = closest.second;
    vec3 point_of_intersection = ray.Point(closest_hit.dist);

    
    if(closest_object.object){
        // Pixel_Print(" recursion:" , recursion_depth); 
        // Pixel_Print("  closest intersectionl; obj: ", closest_object.object->name,"; hit: (dist: ", closest_hit.dist, "; triangle: ", closest_hit.triangle, "; uv: ", closest_hit.uv);
        // Pixel_Print("call Shade_Surface with location ", point_of_intersection, "; normal: ", closest_object.object->Normal(ray, closest.second));
       
        color = closest_object.shader->Shade_Surface(*this, ray, closest_hit, point_of_intersection,closest_object.object->Normal(ray, closest.second),recursion_depth);
    }
    else{
        // Pixel_Print("  closest intersection; none");
        if(!background_shader){
            // Pixel_Print("no background; return black");
            color = {0,0,0};
        }
            
        else{
            color = background_shader->Shade_Surface(*this, ray, closest_hit, {0,0,0}, {0,0,0},recursion_depth);
        }
    }

     //===================
    return color;
}