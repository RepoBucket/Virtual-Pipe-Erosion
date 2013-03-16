
__kernel void updateVolume(global const int* timeStep, global const int* dimensions, __global const float4* flux, __global float2* heightmap) 
{
    size_t i = get_global_id(0);
	int2 coords;
	coords.x = i % *dimensions;
	coords.y = i / *dimensions;
	float4 thisFlux = flux[i];
	
	// netFlux
	heightmap[i].y += *timeStep * (
		flux[coords.x -1 + coords.y * *dimensions].w  // right
		+ flux[coords.x + (coords.y + 1) * *dimensions].x // top
		+ flux[coords.x + 1 + coords.y * *dimensions].z + // left 
		+ flux[coords.x - 1 + coords.y * *dimensions].y // bottom
		- thisFlux.x - thisFlux.y - thisFlux.z - thisFlux.w);
	
}