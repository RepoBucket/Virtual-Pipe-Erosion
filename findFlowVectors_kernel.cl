__kernel void findFlowVectors(__global const int* dimensions, __global const float4* flux, __global float2* vector)
{
    unsigned int x = get_global_id(0);
	unsigned int y = get_global_id(1);
	
	float2 output;
	output.x = flux[x - 1 + y * dimensions[0]].w
			   + flux[x + y * dimensions[0]].w
			   - flux[x - 1 + y* dimensions[0]].z
			   - flux[x + y * dimensions[0]].z;
	output.y = flux[x + (y - 1)* dimensions[0]].y
			   + flux[x + y * dimensions[0]].y
			   - flux[x + (y - 1)* dimensions[0]].x
			   - flux[x + y * dimensions[0]].x;
			   
    output /= 2.0f;
	
	vector[x + y * dimensions[0]] = output;
}