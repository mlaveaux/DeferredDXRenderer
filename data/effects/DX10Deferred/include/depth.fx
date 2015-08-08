float2 proj = {0, 0};

float makeLinear(float depth)
{
	return proj.x / (depth - proj.y);
}