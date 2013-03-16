
__kernel void stepFlux(__global const int* dimensions, __global const float2* heightmap, __global const float* timeStep, __global float4* fluxmap) 
{
    // 0: fluxTop, 1: fluxBottom, 2: fluxLeft, 3: fluxRight
    float4 flux;
	float2 buffer;
	float scalingK;
	
    // Get the index of the current element to be processed
    size_t i = get_global_id(0);
	int2 coords;
	// Get the coords.
	coords.y = floor ((float)(i / *dimensions));
	coords.x = i % *dimensions;
	
	// Calc flux first.
	// First have the same part.
	flux.xyzw = 9.8f * 1 * *timeStep;
	buffer = heightmap[coords.x + coords.y * *dimensions] - heightmap[coords.x + (coords.y -1 ) * *dimensions];
	flux.x *= buffer.x + buffer.y;
	
	buffer = heightmap[coords.x + coords.y * *dimensions] - heightmap[coords.x -1 + coords.y * *dimensions];
	flux.y *= buffer.x + buffer.y;
	
	buffer = heightmap[coords.x + coords.y * *dimensions] - heightmap[coords.x + 1 +coords.y * *dimensions];
	flux.z *= buffer.x + buffer.y;
	
	buffer = heightmap[coords.x + coords.y * *dimensions] - heightmap[coords.x + (coords.y + 1) * *dimensions];
	flux.w *= buffer.x + buffer.y;
    
	// Find the max of each component.
	flux = max (flux, 0);

    // waterHeight * (cellArea = 1) / netFlux / timeStep
    scalingK = min(1.0f, heightmap[coords.x + coords.y * *dimensions].y / (flux.x + flux.y + flux.z + flux.w) / *timeStep);
	

	flux *= scalingK;

	fluxmap[coords.x + coords.y * *dimensions] = flux;
}