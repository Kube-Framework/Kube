#version 450

// #extension GL_EXT_debug_printf : enable
#extension GL_GOOGLE_include_directive : enable

#include "FilledQuad.glsl"

// Inputs
layout(location = 0) in vec4 fragColor;
layout(location = 1) in flat vec2 fragCenter;
layout(location = 2) in flat vec2 fragHalfSize;
layout(location = 3) in flat vec4 fragRadius;
layout(location = 4) in vec2 fragUV;
layout(location = 5) in vec4 fragBorderColor;
layout(location = 6) in flat float fragBorderWidth;
layout(location = 7) in flat uint fragSpriteIndex;
layout(location = 8) in flat float fragEdgeSoftness;
layout(location = 9) in flat vec2 fragRotationOrigin;
layout(location = 10) in flat vec2 fragRotationCosSin;

// Outputs
layout(location = 0) out vec4 outColor;

float nearestRadius(const vec2 point, const vec2 center, const vec4 radius4)
{
    // Select nearest radius
    const float isTop = float(point.y < center.y);
    const float isLeft = float(point.x < center.x);
    const vec2 radius2 = (isTop * radius4.xy) + ((1.0 - isTop) * radius4.zw);
    return (isLeft * radius2.x) + ((1.0 - isLeft) * radius2.y);
}

// Box signed distance field supporting 4 independent radius
float roundedBoxSdf(const vec2 point, const vec2 center, const vec2 halfSize, const float radius)
{
    // Compute SDF
    const vec2 componentWiseEdgeDistance = abs(point - center) - (halfSize - 0.5) + radius;
    const float outsideDistance = length(max(componentWiseEdgeDistance, 0.0));
    const float insideDistance = min(max(componentWiseEdgeDistance.x, componentWiseEdgeDistance.y), 0.0);
    return (outsideDistance + insideDistance) - radius;
}

void main(void)
{
    // Fill by color
    if (fragSpriteIndex == NullSpriteIndex) {
        outColor = fragColor;
    // Fill by texture
    } else {
        vec4 textureColor = texture(sprites[nonuniformEXT(fragSpriteIndex)], fragUV);
        // Raw texture
        if (fragColor.a == 0.0) {
            outColor = textureColor;
        // Recolored texture
        } else {
            outColor = textureColor * fragColor;
        }
    }

    // Only compute SDF if border or radius is required
    if (fragBorderWidth != 0.0 || fragRadius != vec4(0.0)) {
        const vec2 inversedPoint = applyRotation(getInversedRotationMatrix(fragRotationCosSin), fragRotationOrigin, gl_FragCoord.xy);
        const float radius = nearestRadius(inversedPoint, fragCenter, fragRadius);
        const float dist = roundedBoxSdf(inversedPoint, fragCenter, fragHalfSize, radius);

        // Smooth the border by antialiasing
        const float smoothedBorderAlpha = float(fragBorderWidth != 0.0) * smoothstep(-(fragBorderWidth + fragEdgeSoftness), -fragBorderWidth, dist);
        outColor = fragBorderColor * smoothedBorderAlpha + outColor * (1.0 - smoothedBorderAlpha);

        // Smooth the outer bound by antialiasing
        const float smoothedAlpha = smoothstep(max(min(fragEdgeSoftness, radius), Epsilon), 0.0 , dist);
        outColor.a *= smoothedAlpha;
    }
}
