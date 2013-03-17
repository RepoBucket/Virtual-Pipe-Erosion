__kernel void findFlowVectors(__global const int* dimensions, __global const float4* flux, __global float2* vector)
{
    size_t i = get_global_id(0);
	int2 coords;
	coords.x = i % *dimensions;
	coords.y = i / *dimensions;
	
	float2 output;
	output.x = flux[coords.x - 1 + coords.y * *dimensions].w
			   + flux[coords.x + coords.y * *dimensions].w
			   - flux[coords.x - 1 + coords.y* *dimensions].z
			   - flux[coords.x + coords.y * *dimensions].z;
	output.y = flux[coords.x + (coords.y - 1)* *dimensions].y
			   + flux[coords.x + coords.y * *dimensions].y
			   - flux[coords.x + (coords.y - 1)* *dimensions].x
			   - flux[coords.x + coords.y * *dimensions].x;
			   
    output /= 2.0f;
	
	vector[i] = output;
}