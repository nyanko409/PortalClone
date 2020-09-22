
struct mat4x4
{
	float a1, a2, a3, a4;
	float b1, b2, b3, b4;
	float c1, c2, c3, c4;
	float d1, d2, d3, d4;
};

StructuredBuffer<mat4x4> Buffer0 : register(t0);
RWStructuredBuffer<mat4x4> BufferOut : register(u0);

[numthreads(1, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
	BufferOut[0] = Buffer0[0];
}




/*
struct BufType
{
	int i;
	float f;
};

StructuredBuffer<BufType> Buffer0 : register(t0);
StructuredBuffer<BufType> Buffer1 : register(t1);
RWStructuredBuffer<BufType> BufferOut : register(u0);

[numthreads(1, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
	BufferOut[DTid.x].i = Buffer0[DTid.x].i + Buffer1[DTid.x].i;
	BufferOut[DTid.x].f = Buffer0[DTid.x].f + Buffer1[DTid.x].f;
}
*/
