struct VS_INPUT {
    float3 position : POSITION;  // Vertex position
    float4 color : COLOR;        // Vertex color
};

struct PS_INPUT {
    float4 position : SV_POSITION; // Screen-space position
    float4 color : COLOR;          // Passed color
};

PS_INPUT main(VS_INPUT input) {
    PS_INPUT output;
    output.position = float4(input.position, 1.0f); // Convert to 4D vector
    output.color = input.color; // Pass color to pixel shader
    return output;
}
