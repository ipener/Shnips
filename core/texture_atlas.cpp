// Copyright © 2018 Igor Pener. All rights reserved.

#include "assets.hpp"

namespace assets {
    const std::unordered_map<wchar_t, tex_coords> texture_atlas {
        {L'!', tex_coords({{0.000000f, 0.000000f}, {0.025635f, 0.125000f}}, {{0.000000f, 0.000000f}, {0.027344f, 0.125000f}}, {{0.000000f, 0.000000f}, {0.030029f, 0.125000f}})},
        {L'%', tex_coords({{0.025635f, 0.000000f}, {0.070557f, 0.125000f}}, {{0.027344f, 0.000000f}, {0.074707f, 0.125000f}}, {{0.030029f, 0.000000f}, {0.079834f, 0.125000f}})},
        {L'&', tex_coords({{0.096191f, 0.000000f}, {0.070557f, 0.125000f}}, {{0.102051f, 0.000000f}, {0.070312f, 0.125000f}}, {{0.109863f, 0.000000f}, {0.071533f, 0.125000f}})},
        {L'\'', tex_coords({{0.166748f, 0.000000f}, {0.012695f, 0.125000f}}, {{0.172363f, 0.000000f}, {0.019043f, 0.125000f}}, {{0.181396f, 0.000000f}, {0.022705f, 0.125000f}})},
        {L'+', tex_coords({{0.179443f, 0.000000f}, {0.060303f, 0.125000f}}, {{0.191406f, 0.000000f}, {0.062500f, 0.125000f}}, {{0.204102f, 0.000000f}, {0.065186f, 0.125000f}})},
        {L'-', tex_coords({{0.239746f, 0.000000f}, {0.044434f, 0.125000f}}, {{0.253906f, 0.000000f}, {0.044922f, 0.125000f}}, {{0.269287f, 0.000000f}, {0.045654f, 0.125000f}})},
        {L',', tex_coords({{0.284180f, 0.000000f}, {0.025146f, 0.125000f}}, {{0.298828f, 0.000000f}, {0.026123f, 0.125000f}}, {{0.314941f, 0.000000f}, {0.027588f, 0.125000f}})},
        {L'.', tex_coords({{0.309326f, 0.000000f}, {0.025146f, 0.125000f}}, {{0.324951f, 0.000000f}, {0.026123f, 0.125000f}}, {{0.342529f, 0.000000f}, {0.027588f, 0.125000f}})},
        {L'/', tex_coords({{0.334473f, 0.000000f}, {0.025635f, 0.125000f}}, {{0.351074f, 0.000000f}, {0.028809f, 0.125000f}}, {{0.370117f, 0.000000f}, {0.031982f, 0.125000f}})},
        {L'?', tex_coords({{0.360107f, 0.000000f}, {0.046631f, 0.125000f}}, {{0.379883f, 0.000000f}, {0.049805f, 0.125000f}}, {{0.402100f, 0.000000f}, {0.052979f, 0.125000f}})},
        {L'—', tex_coords({{0.406738f, 0.000000f}, {0.104736f, 0.125000f}}, {{0.429688f, 0.000000f}, {0.104736f, 0.125000f}}, {{0.455078f, 0.000000f}, {0.104736f, 0.125000f}})},
        {L'×', tex_coords({{0.511475f, 0.000000f}, {0.060303f, 0.125000f}}, {{0.534424f, 0.000000f}, {0.062500f, 0.125000f}}, {{0.559814f, 0.000000f}, {0.065186f, 0.125000f}})},
        {L'$', tex_coords({{0.571777f, 0.000000f}, {0.060303f, 0.125000f}}, {{0.596924f, 0.000000f}, {0.062500f, 0.125000f}}, {{0.625000f, 0.000000f}, {0.065186f, 0.125000f}})},
        {L'£', tex_coords({{0.632080f, 0.000000f}, {0.060303f, 0.125000f}}, {{0.659424f, 0.000000f}, {0.062500f, 0.125000f}}, {{0.690186f, 0.000000f}, {0.065186f, 0.125000f}})},
        {L'€', tex_coords({{0.692383f, 0.000000f}, {0.060303f, 0.125000f}}, {{0.721924f, 0.000000f}, {0.062500f, 0.125000f}}, {{0.755371f, 0.000000f}, {0.065186f, 0.125000f}})},
        {L'0', tex_coords({{0.752686f, 0.000000f}, {0.060547f, 0.125000f}}, {{0.784424f, 0.000000f}, {0.062500f, 0.125000f}}, {{0.820557f, 0.000000f}, {0.065430f, 0.125000f}})},
        {L'1', tex_coords({{0.813232f, 0.000000f}, {0.039795f, 0.125000f}}, {{0.846924f, 0.000000f}, {0.044189f, 0.125000f}}, {{0.885986f, 0.000000f}, {0.047852f, 0.125000f}})},
        {L'2', tex_coords({{0.853027f, 0.000000f}, {0.058350f, 0.125000f}}, {{0.891113f, 0.000000f}, {0.059082f, 0.125000f}}, {{0.933838f, 0.000000f}, {0.060791f, 0.125000f}})},
        {L'3', tex_coords({{0.911377f, 0.000000f}, {0.057617f, 0.125000f}}, {{0.000000f, 0.125000f}, {0.060547f, 0.125000f}}, {{0.000000f, 0.125000f}, {0.063477f, 0.125000f}})},
        {L'4', tex_coords({{0.000000f, 0.125000f}, {0.059570f, 0.125000f}}, {{0.060547f, 0.125000f}, {0.062256f, 0.125000f}}, {{0.063477f, 0.125000f}, {0.065186f, 0.125000f}})},
        {L'5', tex_coords({{0.059570f, 0.125000f}, {0.059570f, 0.125000f}}, {{0.122803f, 0.125000f}, {0.060791f, 0.125000f}}, {{0.128662f, 0.125000f}, {0.062988f, 0.125000f}})},
        {L'6', tex_coords({{0.119141f, 0.125000f}, {0.059814f, 0.125000f}}, {{0.183594f, 0.125000f}, {0.061768f, 0.125000f}}, {{0.191650f, 0.125000f}, {0.064453f, 0.125000f}})},
        {L'7', tex_coords({{0.178955f, 0.125000f}, {0.054932f, 0.125000f}}, {{0.245361f, 0.125000f}, {0.056641f, 0.125000f}}, {{0.256104f, 0.125000f}, {0.058350f, 0.125000f}})},
        {L'8', tex_coords({{0.233887f, 0.125000f}, {0.060303f, 0.125000f}}, {{0.302002f, 0.125000f}, {0.062012f, 0.125000f}}, {{0.314453f, 0.125000f}, {0.064941f, 0.125000f}})},
        {L'9', tex_coords({{0.294189f, 0.125000f}, {0.059814f, 0.125000f}}, {{0.364014f, 0.125000f}, {0.061768f, 0.125000f}}, {{0.379395f, 0.125000f}, {0.064453f, 0.125000f}})},
        {L'A', tex_coords({{0.354004f, 0.125000f}, {0.061523f, 0.125000f}}, {{0.425781f, 0.125000f}, {0.064697f, 0.125000f}}, {{0.443848f, 0.125000f}, {0.068359f, 0.125000f}})},
        {L'B', tex_coords({{0.415527f, 0.125000f}, {0.058350f, 0.125000f}}, {{0.490479f, 0.125000f}, {0.062256f, 0.125000f}}, {{0.512207f, 0.125000f}, {0.065430f, 0.125000f}})},
        {L'C', tex_coords({{0.473877f, 0.125000f}, {0.066650f, 0.125000f}}, {{0.552734f, 0.125000f}, {0.070312f, 0.125000f}}, {{0.577637f, 0.125000f}, {0.072998f, 0.125000f}})},
        {L'D', tex_coords({{0.540527f, 0.125000f}, {0.067139f, 0.125000f}}, {{0.623047f, 0.125000f}, {0.070068f, 0.125000f}}, {{0.650635f, 0.125000f}, {0.072266f, 0.125000f}})},
        {L'E', tex_coords({{0.607666f, 0.125000f}, {0.053711f, 0.125000f}}, {{0.693115f, 0.125000f}, {0.056641f, 0.125000f}}, {{0.722900f, 0.125000f}, {0.058838f, 0.125000f}})},
        {L'F', tex_coords({{0.661377f, 0.125000f}, {0.052246f, 0.125000f}}, {{0.749756f, 0.125000f}, {0.054443f, 0.125000f}}, {{0.781738f, 0.125000f}, {0.056152f, 0.125000f}})},
        {L'G', tex_coords({{0.713623f, 0.125000f}, {0.068359f, 0.125000f}}, {{0.804199f, 0.125000f}, {0.072266f, 0.125000f}}, {{0.837891f, 0.125000f}, {0.074951f, 0.125000f}})},
        {L'H', tex_coords({{0.781982f, 0.125000f}, {0.066406f, 0.125000f}}, {{0.876465f, 0.125000f}, {0.071045f, 0.125000f}}, {{0.912842f, 0.125000f}, {0.074463f, 0.125000f}})},
        {L'I', tex_coords({{0.848389f, 0.125000f}, {0.015381f, 0.125000f}}, {{0.947510f, 0.125000f}, {0.020996f, 0.125000f}}, {{0.000000f, 0.250000f}, {0.025146f, 0.125000f}})},
        {L'J', tex_coords({{0.863770f, 0.125000f}, {0.047852f, 0.125000f}}, {{0.000000f, 0.250000f}, {0.050537f, 0.125000f}}, {{0.025146f, 0.250000f}, {0.054443f, 0.125000f}})},
        {L'K', tex_coords({{0.911621f, 0.125000f}, {0.056641f, 0.125000f}}, {{0.050537f, 0.250000f}, {0.062012f, 0.125000f}}, {{0.079590f, 0.250000f}, {0.066162f, 0.125000f}})},
        {L'L', tex_coords({{0.000000f, 0.250000f}, {0.051758f, 0.125000f}}, {{0.112549f, 0.250000f}, {0.053955f, 0.125000f}}, {{0.145752f, 0.250000f}, {0.055664f, 0.125000f}})},
        {L'M', tex_coords({{0.051758f, 0.250000f}, {0.080811f, 0.125000f}}, {{0.166504f, 0.250000f}, {0.084961f, 0.125000f}}, {{0.201416f, 0.250000f}, {0.088379f, 0.125000f}})},
        {L'N', tex_coords({{0.132568f, 0.250000f}, {0.067383f, 0.125000f}}, {{0.251465f, 0.250000f}, {0.071289f, 0.125000f}}, {{0.289795f, 0.250000f}, {0.073975f, 0.125000f}})},
        {L'O', tex_coords({{0.199951f, 0.250000f}, {0.071533f, 0.125000f}}, {{0.322754f, 0.250000f}, {0.074951f, 0.125000f}}, {{0.363770f, 0.250000f}, {0.077393f, 0.125000f}})},
        {L'P', tex_coords({{0.271484f, 0.250000f}, {0.055664f, 0.125000f}}, {{0.397705f, 0.250000f}, {0.060059f, 0.125000f}}, {{0.441162f, 0.250000f}, {0.063477f, 0.125000f}})},
        {L'Q', tex_coords({{0.327148f, 0.250000f}, {0.071533f, 0.125000f}}, {{0.457764f, 0.250000f}, {0.074951f, 0.125000f}}, {{0.504639f, 0.250000f}, {0.077393f, 0.125000f}})},
        {L'R', tex_coords({{0.398682f, 0.250000f}, {0.057617f, 0.125000f}}, {{0.532715f, 0.250000f}, {0.061523f, 0.125000f}}, {{0.582031f, 0.250000f}, {0.064697f, 0.125000f}})},
        {L'S', tex_coords({{0.456299f, 0.250000f}, {0.057129f, 0.125000f}}, {{0.594238f, 0.250000f}, {0.060547f, 0.125000f}}, {{0.646729f, 0.250000f}, {0.063721f, 0.125000f}})},
        {L'T', tex_coords({{0.513428f, 0.250000f}, {0.057129f, 0.125000f}}, {{0.654785f, 0.250000f}, {0.060303f, 0.125000f}}, {{0.710449f, 0.250000f}, {0.062500f, 0.125000f}})},
        {L'U', tex_coords({{0.570557f, 0.250000f}, {0.066895f, 0.125000f}}, {{0.715088f, 0.250000f}, {0.071045f, 0.125000f}}, {{0.772949f, 0.250000f}, {0.073486f, 0.125000f}})},
        {L'V', tex_coords({{0.637451f, 0.250000f}, {0.061523f, 0.125000f}}, {{0.786133f, 0.250000f}, {0.064697f, 0.125000f}}, {{0.846436f, 0.250000f}, {0.067871f, 0.125000f}})},
        {L'W', tex_coords({{0.698975f, 0.250000f}, {0.091064f, 0.125000f}}, {{0.850830f, 0.250000f}, {0.094971f, 0.125000f}}, {{0.000000f, 0.375000f}, {0.098389f, 0.125000f}})},
        {L'X', tex_coords({{0.790039f, 0.250000f}, {0.061035f, 0.125000f}}, {{0.000000f, 0.375000f}, {0.064697f, 0.125000f}}, {{0.098389f, 0.375000f}, {0.068604f, 0.125000f}})},
        {L'Y', tex_coords({{0.851074f, 0.250000f}, {0.058838f, 0.125000f}}, {{0.064697f, 0.375000f}, {0.062500f, 0.125000f}}, {{0.166992f, 0.375000f}, {0.066162f, 0.125000f}})},
        {L'Z', tex_coords({{0.909912f, 0.250000f}, {0.058838f, 0.125000f}}, {{0.127197f, 0.375000f}, {0.062744f, 0.125000f}}, {{0.233154f, 0.375000f}, {0.065186f, 0.125000f}})},
        {L'Ä', tex_coords({{0.000000f, 0.375000f}, {0.061523f, 0.125000f}}, {{0.189941f, 0.375000f}, {0.064697f, 0.125000f}}, {{0.298340f, 0.375000f}, {0.068359f, 0.125000f}})},
        {L'Ö', tex_coords({{0.061523f, 0.375000f}, {0.071533f, 0.125000f}}, {{0.254639f, 0.375000f}, {0.074951f, 0.125000f}}, {{0.366699f, 0.375000f}, {0.077393f, 0.125000f}})},
        {L'Ü', tex_coords({{0.133057f, 0.375000f}, {0.066895f, 0.125000f}}, {{0.329590f, 0.375000f}, {0.071045f, 0.125000f}}, {{0.444092f, 0.375000f}, {0.073486f, 0.125000f}})},
        {L'ß', tex_coords({{0.199951f, 0.375000f}, {0.053711f, 0.125000f}}, {{0.400635f, 0.375000f}, {0.060059f, 0.125000f}}, {{0.517578f, 0.375000f}, {0.065674f, 0.125000f}})},
        {L'a', tex_coords({{0.354004f, 0.125000f}, {0.061523f, 0.125000f}}, {{0.425781f, 0.125000f}, {0.064697f, 0.125000f}}, {{0.443848f, 0.125000f}, {0.068359f, 0.125000f}})},
        {L'b', tex_coords({{0.415527f, 0.125000f}, {0.058350f, 0.125000f}}, {{0.490479f, 0.125000f}, {0.062256f, 0.125000f}}, {{0.512207f, 0.125000f}, {0.065430f, 0.125000f}})},
        {L'c', tex_coords({{0.473877f, 0.125000f}, {0.066650f, 0.125000f}}, {{0.552734f, 0.125000f}, {0.070312f, 0.125000f}}, {{0.577637f, 0.125000f}, {0.072998f, 0.125000f}})},
        {L'd', tex_coords({{0.540527f, 0.125000f}, {0.067139f, 0.125000f}}, {{0.623047f, 0.125000f}, {0.070068f, 0.125000f}}, {{0.650635f, 0.125000f}, {0.072266f, 0.125000f}})},
        {L'e', tex_coords({{0.607666f, 0.125000f}, {0.053711f, 0.125000f}}, {{0.693115f, 0.125000f}, {0.056641f, 0.125000f}}, {{0.722900f, 0.125000f}, {0.058838f, 0.125000f}})},
        {L'f', tex_coords({{0.661377f, 0.125000f}, {0.052246f, 0.125000f}}, {{0.749756f, 0.125000f}, {0.054443f, 0.125000f}}, {{0.781738f, 0.125000f}, {0.056152f, 0.125000f}})},
        {L'g', tex_coords({{0.713623f, 0.125000f}, {0.068359f, 0.125000f}}, {{0.804199f, 0.125000f}, {0.072266f, 0.125000f}}, {{0.837891f, 0.125000f}, {0.074951f, 0.125000f}})},
        {L'h', tex_coords({{0.781982f, 0.125000f}, {0.066406f, 0.125000f}}, {{0.876465f, 0.125000f}, {0.071045f, 0.125000f}}, {{0.912842f, 0.125000f}, {0.074463f, 0.125000f}})},
        {L'i', tex_coords({{0.848389f, 0.125000f}, {0.015381f, 0.125000f}}, {{0.947510f, 0.125000f}, {0.020996f, 0.125000f}}, {{0.000000f, 0.250000f}, {0.025146f, 0.125000f}})},
        {L'j', tex_coords({{0.863770f, 0.125000f}, {0.047852f, 0.125000f}}, {{0.000000f, 0.250000f}, {0.050537f, 0.125000f}}, {{0.025146f, 0.250000f}, {0.054443f, 0.125000f}})},
        {L'k', tex_coords({{0.911621f, 0.125000f}, {0.056641f, 0.125000f}}, {{0.050537f, 0.250000f}, {0.062012f, 0.125000f}}, {{0.079590f, 0.250000f}, {0.066162f, 0.125000f}})},
        {L'l', tex_coords({{0.000000f, 0.250000f}, {0.051758f, 0.125000f}}, {{0.112549f, 0.250000f}, {0.053955f, 0.125000f}}, {{0.145752f, 0.250000f}, {0.055664f, 0.125000f}})},
        {L'm', tex_coords({{0.051758f, 0.250000f}, {0.080811f, 0.125000f}}, {{0.166504f, 0.250000f}, {0.084961f, 0.125000f}}, {{0.201416f, 0.250000f}, {0.088379f, 0.125000f}})},
        {L'n', tex_coords({{0.132568f, 0.250000f}, {0.067383f, 0.125000f}}, {{0.251465f, 0.250000f}, {0.071289f, 0.125000f}}, {{0.289795f, 0.250000f}, {0.073975f, 0.125000f}})},
        {L'o', tex_coords({{0.199951f, 0.250000f}, {0.071533f, 0.125000f}}, {{0.322754f, 0.250000f}, {0.074951f, 0.125000f}}, {{0.363770f, 0.250000f}, {0.077393f, 0.125000f}})},
        {L'p', tex_coords({{0.271484f, 0.250000f}, {0.055664f, 0.125000f}}, {{0.397705f, 0.250000f}, {0.060059f, 0.125000f}}, {{0.441162f, 0.250000f}, {0.063477f, 0.125000f}})},
        {L'q', tex_coords({{0.327148f, 0.250000f}, {0.071533f, 0.125000f}}, {{0.457764f, 0.250000f}, {0.074951f, 0.125000f}}, {{0.504639f, 0.250000f}, {0.077393f, 0.125000f}})},
        {L'r', tex_coords({{0.398682f, 0.250000f}, {0.057617f, 0.125000f}}, {{0.532715f, 0.250000f}, {0.061523f, 0.125000f}}, {{0.582031f, 0.250000f}, {0.064697f, 0.125000f}})},
        {L's', tex_coords({{0.456299f, 0.250000f}, {0.057129f, 0.125000f}}, {{0.594238f, 0.250000f}, {0.060547f, 0.125000f}}, {{0.646729f, 0.250000f}, {0.063721f, 0.125000f}})},
        {L't', tex_coords({{0.513428f, 0.250000f}, {0.057129f, 0.125000f}}, {{0.654785f, 0.250000f}, {0.060303f, 0.125000f}}, {{0.710449f, 0.250000f}, {0.062500f, 0.125000f}})},
        {L'u', tex_coords({{0.570557f, 0.250000f}, {0.066895f, 0.125000f}}, {{0.715088f, 0.250000f}, {0.071045f, 0.125000f}}, {{0.772949f, 0.250000f}, {0.073486f, 0.125000f}})},
        {L'v', tex_coords({{0.637451f, 0.250000f}, {0.061523f, 0.125000f}}, {{0.786133f, 0.250000f}, {0.064697f, 0.125000f}}, {{0.846436f, 0.250000f}, {0.067871f, 0.125000f}})},
        {L'w', tex_coords({{0.698975f, 0.250000f}, {0.091064f, 0.125000f}}, {{0.850830f, 0.250000f}, {0.094971f, 0.125000f}}, {{0.000000f, 0.375000f}, {0.098389f, 0.125000f}})},
        {L'x', tex_coords({{0.790039f, 0.250000f}, {0.061035f, 0.125000f}}, {{0.000000f, 0.375000f}, {0.064697f, 0.125000f}}, {{0.098389f, 0.375000f}, {0.068604f, 0.125000f}})},
        {L'y', tex_coords({{0.851074f, 0.250000f}, {0.058838f, 0.125000f}}, {{0.064697f, 0.375000f}, {0.062500f, 0.125000f}}, {{0.166992f, 0.375000f}, {0.066162f, 0.125000f}})},
        {L'z', tex_coords({{0.909912f, 0.250000f}, {0.058838f, 0.125000f}}, {{0.127197f, 0.375000f}, {0.062744f, 0.125000f}}, {{0.233154f, 0.375000f}, {0.065186f, 0.125000f}})},
        {L'ä', tex_coords({{0.000000f, 0.375000f}, {0.061523f, 0.125000f}}, {{0.189941f, 0.375000f}, {0.064697f, 0.125000f}}, {{0.298340f, 0.375000f}, {0.068359f, 0.125000f}})},
        {L'ö', tex_coords({{0.061523f, 0.375000f}, {0.071533f, 0.125000f}}, {{0.254639f, 0.375000f}, {0.074951f, 0.125000f}}, {{0.366699f, 0.375000f}, {0.077393f, 0.125000f}})},
        {L'ü', tex_coords({{0.133057f, 0.375000f}, {0.066895f, 0.125000f}}, {{0.329590f, 0.375000f}, {0.071045f, 0.125000f}}, {{0.444092f, 0.375000f}, {0.073486f, 0.125000f}})},
        {glyphs::shadow,       tex_coords({vec2<float>(872.f, 1409.f) / texture_atlas_size, vec2<float>(390.f, 254.f) / texture_atlas_size}, {}, {})},
        {glyphs::blend_spot,   tex_coords({vec2<float>(1262.f, 1536.f) / texture_atlas_size, vec2<float>(128.f, 128.f) / texture_atlas_size}, {}, {})},
        {glyphs::tick,         tex_coords({vec2<float>(0.f, 1408.f) / texture_atlas_size, vec2<float>(226.f, 256.f) / texture_atlas_size})},
        {glyphs::supercharger, tex_coords({vec2<float>(226.f, 1408.f) / texture_atlas_size, vec2<float>(110.f, 256.f) / texture_atlas_size})},
        {glyphs::heart,        tex_coords({vec2<float>(336.f, 1408.f) / texture_atlas_size, vec2<float>(190.f, 256.f) / texture_atlas_size})},
        {glyphs::half_heart,   tex_coords({vec2<float>(526.f, 1408.f) / texture_atlas_size, vec2<float>(134.f, 256.f) / texture_atlas_size})},
        {glyphs::twitter,      tex_coords({vec2<float>(660.f, 1408.f) / texture_atlas_size, vec2<float>(212.f, 256.f) / texture_atlas_size})},
        {glyphs::buttons[button_type_tick],         tex_coords({{0.000000f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_cross],        tex_coords({{0.062500f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_menu],         tex_coords({{0.125000f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_store],        tex_coords({{0.187500f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_unmute],       tex_coords({{0.250000f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_mute],         tex_coords({{0.312500f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_tutorial],     tex_coords({{0.375000f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_exit],         tex_coords({{0.437500f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_supercharger], tex_coords({{0.500000f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_ad],           tex_coords({{0.562500f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_heart],        tex_coords({{0.625000f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_goals],        tex_coords({{0.687500f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_share],        tex_coords({{0.750000f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_sign_in],      tex_coords({{0.812500f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::buttons[button_type_download],     tex_coords({{0.875000f, 0.812500f}, {0.062500f, 0.062500f}})},
        {glyphs::blank_chip, tex_coords()},
        {glyphs::locked,                        tex_coords({{0.625000f, 0.875000f}, {0.125000f, 0.125000f}}, {}, {})},
        {glyphs::chips[chip_type_default],      tex_coords({{0.000000f, 0.875000f}, {0.125000f, 0.125000f}})},
        {glyphs::chips[chip_type_fast],         tex_coords({{0.125000f, 0.875000f}, {0.125000f, 0.125000f}})},
        {glyphs::chips[chip_type_extra_points], tex_coords({{0.250000f, 0.875000f}, {0.125000f, 0.125000f}})},
        {glyphs::chips[chip_type_swap],         tex_coords({{0.375000f, 0.875000f}, {0.125000f, 0.125000f}})},
        {glyphs::chips[chip_type_extra_chip],   tex_coords({{0.500000f, 0.875000f}, {0.125000f, 0.125000f}})},
        {glyphs::gradients[chip_type_default],      tex_coords({vec2<float>(1390.f + 1.f, 1600.f + 1.f) / texture_atlas_size, vec2<float>(62.f, 62.f) / texture_atlas_size})},
        {glyphs::gradients[chip_type_fast],         tex_coords({vec2<float>(1454.f + 1.f, 1600.f + 1.f) / texture_atlas_size, vec2<float>(62.f, 62.f) / texture_atlas_size})},
        {glyphs::gradients[chip_type_extra_points], tex_coords({vec2<float>(1518.f + 1.f, 1600.f + 1.f) / texture_atlas_size, vec2<float>(62.f, 62.f) / texture_atlas_size})},
        {glyphs::gradients[chip_type_swap],         tex_coords({vec2<float>(1582.f + 1.f, 1600.f + 1.f) / texture_atlas_size, vec2<float>(62.f, 62.f) / texture_atlas_size})},
        {glyphs::gradients[chip_type_extra_chip],   tex_coords({vec2<float>(1646.f + 1.f, 1600.f + 1.f) / texture_atlas_size, vec2<float>(62.f, 62.f) / texture_atlas_size})}
    };
}
