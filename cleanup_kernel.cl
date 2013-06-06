__kernel void cleanUp(__global float2* heightmap, __global const int* dimensions)
{
  unsigned int x = get_global_id(0);
  unsigned int y = get_global_id(1);	
  
  heightmap[x + y * dimensions[0]].y = max(0.0001f, heightmap[x + y * dimensions[0]].y);
}