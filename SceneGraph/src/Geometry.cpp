#include "Geometry.h"

Geometry::Geometry(geometryType geomType) :
    type_(geomType)
{
}

Geometry::~Geometry()
{
    vertices_.clear();
    normals_.clear();
    colors_.clear();
    indices_.clear();
}

Intersection Geometry::intersect(const glm::mat4 &T, Ray ray_world)
{
    // The input ray here is in WORLD-space. It may not be normalized!

    // TODO: normalize ray_world.
	ray_world.dir=glm::normalize(ray_world.dir);

    // Transform the ray into OBJECT-LOCAL-space, for intersection calculation.
    Ray ray_local;  // TODO: COMPUTE THIS
    // TODO: transform the ray by the inverse transformation.
	glm::vec4 position,direction;
	position=glm::inverse(T)*glm::vec4(ray_world.orig,1);
	direction=glm::inverse(T)*glm::vec4(ray_world.dir,0);
	ray_local.dir=glm::vec3(direction);
	ray_local.orig=glm::vec3(position);
    //     (Remember that position = vec4(vec3, 1) while direction = vec4(vec3, 0).)

    // Compute the intersection in LOCAL-space.
    Intersection isx = intersectImpl(ray_local);

    if (isx.t != -1) {
        // Transform the local-space intersection BACK into world-space.
        //     (Note that, as long as you didn't re-normalize the ray direction
        //     earlier, `t` doesn't need to change.)
        const glm::vec3 normal_local = isx.normal;
		//cout<<normal_local.x<<endl;
        glm::vec3 normal_world;  // TODO: COMPUTE THIS
        // TODO: inverse-transpose-transform the normal to get it back from local-space to world-space.
        //     (If you were transforming a position, you would just use the unmodified transform T.)
        //     http://www.arcsynthesis.org/gltut/Illumination/Tut09%20Normal%20Transformation.html
		normal_world=glm::vec3(T*glm::vec4(normal_local,0));
        //isx.normal = normal_world;
		//if(normal_local.x==1){ cout<<normal_world.x<<","<<normal_world.y<<","<<normal_world.z<<endl;
		//getchar();}
		 // TODO: You might want to do this here: make sure here that your
        // normal is pointing the right way (toward, not away from, the ray
        // origin). Instead of doing this inside intersectImpl, you can do so
        // here by just negating normal_world depending on the glm::sign of
        //     glm::dot(normal_world, ray_world.dir).
		//if(normal_world.z>0.5) cout<<"!"<<endl;
		if(glm::dot(normal_world,ray_world.dir)/(glm::length(normal_world)*glm::length(ray_world.dir))>0){
			normal_world=-normal_world;
		}
		isx.normal = normal_world;
	}
    // The final output intersection data is in WORLD-space.
    return isx;
}
