
Texture2D terraTex : register(t0);


cbuffer data
{
	float4 WorldViewTrans1;
	float4 WorldViewTrans2;
	float4 WorldViewTrans3;
	float4 WorldViewTrans4;
	float4 camPos;
};

cbuffer perHeightmapData
{
	uint4 textureSizeData;
};

struct Fragment
{  
    float4 Pos : SV_POSITION; 
	float4 norm : NORMAL0;
    float4 color : COLOR0; 
	float4 light : NORMAL1;
};

    Fragment VS( uint vertexId : SV_VertexID, uint tileNumber : SV_InstanceID) 
    { 
		float textureSize = textureSizeData.x;
        Fragment frag; 
		uint2 xy;
		
		xy = uint2(tileNumber%textureSize+vertexId/2, tileNumber/textureSize+vertexId%2);
		float height = terraTex[xy].x;
		
        frag.color = float4(terraTex[xy].yzw, 1.0f);
		
		float4 pos;
		//set 3d position
		pos.x = tileNumber%textureSize+vertexId/2;
		pos.z = tileNumber/textureSize+vertexId%2;
		pos.y = height;
		pos.w = 1.0f;
		frag.light = pos - camPos;
		
		float3 other1, other2;
		other1.x = 1;
		other1.z = 0;
		other2.x = 0;
		other2.z = 1;
		uint2 xy1, xy2;
		
		if(tileNumber%textureSize+vertexId/2+1 < textureSize)
			xy1 = uint2(tileNumber%textureSize+vertexId/2+1, tileNumber/textureSize+vertexId%2);
		else
			xy1 = uint2(tileNumber%textureSize+vertexId/2, tileNumber/textureSize+vertexId%2);
		float height1 = terraTex[xy1].x;
		other1.y = height1-pos.y;
		
		if(tileNumber/textureSize+vertexId%2+1 < textureSize)
			xy2 = uint2(tileNumber%textureSize+vertexId/2, tileNumber/textureSize+vertexId%2+1);
		else
			xy2 = uint2(tileNumber%textureSize+vertexId/2, tileNumber/textureSize+vertexId%2);
		float height2 = terraTex[xy2].x;
		other2.y = height2-pos.y;
		
		float3 crossProduct = cross(other1, other2);
		frag.norm = float4(crossProduct, 0.0f);
		frag.norm = normalize(frag.norm);
		frag.norm.w = 0.0f;
		
		//do matrix transformation
		frag.Pos.x = WorldViewTrans1.x * pos.x + WorldViewTrans1.y * 
			pos.y + WorldViewTrans1.z * pos.z + WorldViewTrans1.w *pos.w;
		frag.Pos.y = WorldViewTrans2.x * pos.x + WorldViewTrans2.y * 
			pos.y + WorldViewTrans2.z * pos.z + WorldViewTrans2.w *pos.w;
		frag.Pos.z = WorldViewTrans3.x * pos.x + WorldViewTrans3.y * 
			pos.y + WorldViewTrans3.z * pos.z + WorldViewTrans3.w *pos.w;
		frag.Pos.w = WorldViewTrans4.x * pos.x + WorldViewTrans4.y * 
			pos.y + WorldViewTrans4.z * pos.z + WorldViewTrans4.w *pos.w;
		frag.color = saturate(frag.color);
        return frag; 
    } 