
__kernel void stepFlux(__global const int* dimensions, __global const float* timeStep, __global const float2* heightmap, __global float4* fluxmap) 
{
    // 0: fluxTop, 1: fluxBottom, 2: fluxLeft, 3: fluxRight
    float4 flux;
	float2 buffer;
	float scalingK;
	
    // Get the index of the current element to be processed
    unsigned int x = get_global_id(0);
	unsigned int y = get_global_id(1);
	
	// Calc flux first.
	// First have the same part.
	flux.xyzw = 9.8f * 1 * *timeStep;
	buffer = heightmap[x + y * dimensions[0]] - heightmap[x + (y -1) * dimensions[0]];
	flux.x *= buffer.x + buffer.y;
	
	buffer = heightmap[x + y * dimensions[0]] - heightmap[x -1 + y * dimensions[0]];
	flux.y *= buffer.x + buffer.y;
	
	buffer = heightmap[x + y * dimensions[0]] - heightmap[x + 1 +y * dimensions[0]];
	flux.z *= buffer.x + buffer.y;
	
	buffer = heightmap[x + y * dimensions[0]] - heightmap[x + (y + 1) * dimensions[0]];
	flux.w *= buffer.x + buffer.y;
    
	// Find the max of each component.
	flux = max (flux, 0);

    // waterHeight * (cellArea = 1) / netFlux / timeStep
    scalingK = min(1.0f, heightmap[x + y * dimensions[0]].y / (flux.x + flux.y + flux.z + flux.w) / *timeStep);
	

	flux *= scalingK;

	fluxmap[x + y * dimensions[0]] = flux;
}