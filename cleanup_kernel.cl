__kernel void cleanUp(__global float2* heightmap)
{
  size_t i = get_global_id(0);
  heightmap[i].y = max(0.0001f, heightmap[i].y);
}