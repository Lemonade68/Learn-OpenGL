#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;  

vec2 offsets[9] = vec2[](	//偏移量数组
    vec2(-offset,  offset), // 左上
    vec2( 0.0f,    offset), // 正上
    vec2( offset,  offset), // 右上
    vec2(-offset,  0.0f),   // 左
    vec2( 0.0f,    0.0f),   // 中
    vec2( offset,  0.0f),   // 右
    vec2(-offset, -offset), // 左下
    vec2( 0.0f,   -offset), // 正下
    vec2( offset, -offset)  // 右下
);

float kernel_Sharpen[9] = float[](	//核1 —— 这里是锐化核，进行颜色值的锐化（原理再看看）
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);

float kernel_Blurring[9] = float[](	//核2 —— 模糊核（相加权值为1，与四周的混合）
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

float kernel_Border[9] = float[](	//核3 —— 边缘检测核(加起来为0，相同的会消掉，而边缘两侧差别大，不会消掉)
	-1, -1, -1,
    -1,  8, -1,
    -1, -1, -1
);

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(col, 1.0);

} 