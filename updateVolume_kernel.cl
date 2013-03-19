
__kernel void updateVolume(global const int* timeStep, global const int* dimensions, __global const float4* flux, __global float2* heightmap) 
{
    unsigned int x = get_global_id(0);
	unsigned int y = get_global_id(1);

	float4 thisFlux = flux[x + y * dimensions[0]];
	
	// netFlux
	heightmap[x + y * dimensions[0]].y += *timeStep * (
		flux[x -1 + y * dimensions[0]].w  // right
		+ flux[x + (y + 1) * dimensions[0]].x // top
		+ flux[x + 1 + y * dimensions[0]].z + // left 
		+ flux[x - 1 + y * dimensions[0]].y // bottom
		- thisFlux.x - thisFlux.y - thisFlux.z - thisFlux.w);
	
}