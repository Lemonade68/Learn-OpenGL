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
//	//1.原效果
//    vec3 col = texture(screenTexture, TexCoords).rgb;
//    FragColor = vec4(col, 1.0);

//	//2.反色效果：
//	FragColor = vec4(vec3(1.0 - texture(screenTexture,TexCoords)), 1.0);

//	//3.灰度化：均分 或 加权
//	//均分-------
//	FragColor = texture(screenTexture, TexCoords);
//    float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
//    FragColor = vec4(average, average, average, 1.0);
//	//加权-------
//	FragColor = texture(screenTexture, TexCoords);
//    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
//    FragColor = vec4(average, average, average, 1.0);

	//4.核效果

	vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++) {
        col += sampleTex[i] * kernel_Sharpen[i];		//锐化效果
//        col += sampleTex[i] * kernel_Border[i];		//边缘检测效果
//		col += sampleTex[i] * kernel_Blurring[i];		//模糊效果 

	}

    FragColor = vec4(col, 1.0);

} 