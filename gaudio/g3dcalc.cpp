#include <cmath>
#include <string.h>
#include <float.h>
#include "gmath.h"
#include "gcontext.h"
#include "gsource.h"

#ifdef HAVE_SQRTF
#define gSqrt(x) ((float)sqrtf((float)(x)))
#else
#define gSqrt(x) ((float)sqrt((double)(x)))
#endif

static inline
float gDotproduct(float* inVector1,float *inVector2)
{
    return inVector1[0]*inVector2[0] + inVector1[1]*inVector2[1] +
           inVector1[2]*inVector2[2];
}

static inline
void gCrossproduct(float* inVector1,float* inVector2,float* outVector)
{
    outVector[0] = inVector1[1]*inVector2[2] - inVector1[2]*inVector2[1];
    outVector[1] = inVector1[2]*inVector2[0] - inVector1[0]*inVector2[2];
    outVector[2] = inVector1[0]*inVector2[1] - inVector1[1]*inVector2[0];
}

static inline
void gNormalize(float* inVector)
{
    float length,inverse_length;

    length = (float)gSqrt(gDotproduct(inVector,inVector));
    if(length != 0)
    {
        inverse_length = 1.0f/length;
        inVector[0] *= inverse_length;
        inVector[1] *= inverse_length;
        inVector[2] *= inverse_length;
    }
}

static inline
void gMatrixVector(float* vector,float matrix[3][3])
{
    float result[3];
    result[0] = vector[0]*matrix[0][0] + vector[1]*matrix[1][0] + vector[2]*matrix[2][0];
    result[1] = vector[0]*matrix[0][1] + vector[1]*matrix[1][1] + vector[2]*matrix[2][1];
    result[2] = vector[0]*matrix[0][2] + vector[1]*matrix[1][2] + vector[2]*matrix[2][2];
    memcpy(vector,result,sizeof(result));
}

void g3dEffectCalc(gsource* source)
{
    //static float DryBuffer[BUFFERSIZE][OUTPUTCHANNELS];
    //static float WetBuffer[BUFFERSIZE][OUTPUTCHANNELS];
    float dry[OUTPUTCHANNELS] = {0.0f,0.0f,0.0f,0.0f};
    float wet[OUTPUTCHANNELS] = {0.0f,0.0f,0.0f,0.0f};
    float pitch[1];

    gCalcSourceParams(source,true,dry,wet,pitch);

    for(int32_t i=0;i<source->frontbuffer.use;i++)
    {
        source->frontbuffer.data[0][i] *= (dry[0] + wet[0]);
        source->frontbuffer.data[1][i] *= (dry[1] + wet[1]);
    }
    /*DryBuffer[j][0] += value*dry[0];
    //Room path final mix buffer and panning (left)
    WetBuffer[j][0] += value*wet[0];
    //First order interpolator (right)
    //value = (ALfloat)((ALshort)(((Data[k*2+1]*((1L<<FRACTIONBITS)-fraction))+(Data[k*2+3]*(fraction)))>>FRACTIONBITS));
    //Direct path final mix buffer and panning (right)
    DryBuffer[j][1] += value*dry[1];
    //Room path final mix buffer and panning (right)
    WetBuffer[j][1] += value*wet[1];*/

    //source->frontbuffer
}

void gCalcSourceParams(
gsource* source,
bool mono,
//ALenum isMono,ALenum OutputFormat,
float* dry,
float* wet,
float* pitch)
{
    float ListenerOrientation[6];
    float ListenerPosition[3];
    float ListenerVelocity[3];
    float InnerAngle,OuterAngle,OuterGain,Angle;
    float Distance,DryMix,WetMix;
    float Direction[3],Position[3],Velocity[3],SourceToListener[3];
    float MinVolume,MaxVolume,MinDist,MaxDist,Rolloff;
    float Pitch,ConeVolume,SourceVolume,PanningFB;
    float PanningLR,ListenerGain;
    float U[3],V[3],N[3];
    float DopplerFactor,DopplerVelocity;
    float flSpeedOfSound,flMaxVelocity;
    float flVSS,flVLS;
    int32_t DistanceModel;
    float Matrix[3][3];
    int32_t HeadRelative;
    float flAttenuation;

    //Get context properties
    DopplerFactor   = scontext.device.dopplerfactor;
    DistanceModel   = scontext.device.attenuation;
    DopplerVelocity = scontext.device.dopplervelocity;
    flSpeedOfSound  = scontext.device.soundspeed;

    //Get listener properties
    ListenerGain = scontext.device.listener.gain;
    memcpy(ListenerPosition,scontext.device.listener.pos,sizeof(scontext.device.listener.pos));
    memcpy(ListenerVelocity,scontext.device.listener.vel,sizeof(scontext.device.listener.vel));
    memcpy(&ListenerOrientation[0],&scontext.device.listener.forward,sizeof(scontext.device.listener.forward));
    memcpy(&ListenerOrientation[3],&scontext.device.listener.up,sizeof(scontext.device.listener.up));

#define COPY_IT(to,from)\
    memcpy(to,from,sizeof(from));

    //Get source properties
    Pitch        = source->picth;
    SourceVolume = source->gain;

    COPY_IT(Position,source->pos);
    COPY_IT(Velocity,source->velocity);
    COPY_IT(Direction,source->orientation);

    MinVolume    = source->mingain;
    MaxVolume    = source->maxgain;
    MinDist      = source->refdistance;
    MaxDist      = source->maxdistance;
    Rolloff      = source->rolloff;
    OuterGain    = source->outgain;
    InnerAngle   = source->inangle;
    OuterAngle   = source->outangle;
    //HeadRelative = ALSource->bHeadRelative;

    //Set working variables
    DryMix = 1.0f;
    WetMix = 0.0f;

    //Only apply 3D calculations for mono buffers
    /*if(mono == false)
    {
        //1. Multi-channel buffers always play "normal"
        dry[0] = SourceVolume * 1.0f * ListenerGain;
        dry[1] = SourceVolume * 1.0f * ListenerGain;
        dry[2] = SourceVolume * 1.0f * ListenerGain;
        dry[3] = SourceVolume * 1.0f * ListenerGain;
        wet[0] = SourceVolume * 0.0f * ListenerGain;
        wet[1] = SourceVolume * 0.0f * ListenerGain;
        wet[2] = SourceVolume * 0.0f * ListenerGain;
        wet[3] = SourceVolume * 0.0f * ListenerGain;
        pitch[0] = Pitch;
    }
    else*/
    {
        //1. Translate Listener to origin (convert to head relative)
        //...

        //2. Calculate distance attenuation
        Distance = gSqrt(gDotproduct(Position,Position));

        flAttenuation = 1.0f;
        switch(DistanceModel)
        {
        case AUDIO_ATTENUATION_INVERSE_DISTANCE_CLAMPED:
            Distance = gmax(Distance,MinDist);
            Distance = gmin(Distance,MaxDist);
            if(MaxDist < MinDist)
                break;
            //fall-through
        case AUDIO_ATTENUATION_INVERSE_DISTANCE:
            if(MinDist > 0.0f)
            {
                if((MinDist + (Rolloff * (Distance - MinDist))) > 0.0f)
                    flAttenuation = MinDist / (MinDist + (Rolloff * (Distance - MinDist)));
            }
            break;
        case AUDIO_ATTENUATION_LINEAR_DISTANCE_CLAMPED:
            Distance = gmax(Distance,MinDist);
            Distance = gmin(Distance,MaxDist);
            if(MaxDist < MinDist)
                break;
            //fall-through
        case AUDIO_ATTENUATION_LINEAR_DISTANCE:
            Distance = gmin(Distance,MaxDist);
            if(MaxDist != MinDist)
                flAttenuation = 1.0f - (Rolloff*(Distance-MinDist)/(MaxDist - MinDist));
            break;
        case AUDIO_ATTENUATION_EXPONENT_DISTANCE_CLAMPED:
            Distance = gmax(Distance,MinDist);
            Distance = gmin(Distance,MaxDist);
            if(MaxDist < MinDist)
                break;
            //fall-through
        case AUDIO_ATTENUATION_EXPONENT_DISTANCE:
            if((Distance > 0.0f) && (MinDist > 0.0f))
                flAttenuation = (float)pow(Distance/MinDist,-Rolloff);
            break;
        default:
            flAttenuation = 1.0f;
            break;
        }

        // Source Gain + Attenuation
        DryMix = SourceVolume * flAttenuation;

        // Clamp to Min/Max Gain
        DryMix = gmin(DryMix,MaxVolume);
        DryMix = gmax(DryMix,MinVolume);
        WetMix = gmin(WetMix,MaxVolume);
        WetMix = gmax(WetMix,MinVolume);
        //3. Apply directional soundcones
        SourceToListener[0] = -Position[0];
        SourceToListener[1] = -Position[1];
        SourceToListener[2] = -Position[2];
        gNormalize(Direction);
        gNormalize(SourceToListener);
        Angle = (float)(180.0*acos(gDotproduct(Direction,SourceToListener))/M_PI);
        if(Angle >= InnerAngle && Angle <= OuterAngle)
            ConeVolume = (1.0f+(OuterGain-1.0f)*(Angle-InnerAngle)/(OuterAngle-InnerAngle));
        else if(Angle > OuterAngle)
            ConeVolume = (1.0f+(OuterGain-1.0f)                                           );
        else
            ConeVolume = 1.0f;

        //4. Calculate Velocity
        if(DopplerFactor != 0.0f)
        {
            flVLS = gDotproduct(ListenerVelocity,SourceToListener);
            flVSS = gDotproduct(Velocity,SourceToListener);

            flMaxVelocity = (DopplerVelocity * flSpeedOfSound) / DopplerFactor;

            if(flVSS >= flMaxVelocity)
                flVSS = (flMaxVelocity - 1.0f);
            else if(flVSS <= -flMaxVelocity)
                flVSS = -flMaxVelocity + 1.0f;

            if(flVLS >= flMaxVelocity)
                flVLS = (flMaxVelocity - 1.0f);
            else if(flVLS <= -flMaxVelocity)
                flVLS = -flMaxVelocity + 1.0f;

            pitch[0] = Pitch * ((flSpeedOfSound * DopplerVelocity) - (DopplerFactor * flVLS)) /
                               ((flSpeedOfSound * DopplerVelocity) - (DopplerFactor * flVSS));
        }
        else
            pitch[0] = Pitch;

        //5. Align coordinate system axes
        gCrossproduct(&ListenerOrientation[0],&ListenerOrientation[3],U); // Right-vector
        gNormalize(U);                                // Normalized Right-vector
        memcpy(V,&ListenerOrientation[3],sizeof(V));  // Up-vector
        gNormalize(V);                                // Normalized Up-vector
        memcpy(N,&ListenerOrientation[0],sizeof(N));  // At-vector
        gNormalize(N);                                // Normalized At-vector
        Matrix[0][0] = U[0]; Matrix[0][1] = V[0]; Matrix[0][2] = -N[0];
        Matrix[1][0] = U[1]; Matrix[1][1] = V[1]; Matrix[1][2] = -N[1];
        Matrix[2][0] = U[2]; Matrix[2][1] = V[2]; Matrix[2][2] = -N[2];
        gMatrixVector(Position, Matrix);

        //6. Convert normalized position into left/right front/back pannings
        if(Distance != 0.0f)
        {
            gNormalize(Position);
            PanningLR = 0.5f + 0.5f*Position[0];
            PanningFB = 0.5f + 0.5f*Position[2];
        }
        else
        {
            PanningLR = 0.5f;
            PanningFB = 0.5f;
        }

        dry[0] = ConeVolume * ListenerGain * DryMix * gSqrt(1.0f-PanningLR); //L Direct
        dry[1] = ConeVolume * ListenerGain * DryMix * gSqrt(     PanningLR); //R Direct
        wet[0] =              ListenerGain * WetMix * gSqrt(1.0f-PanningLR); //L Room
        wet[1] =              ListenerGain * WetMix * gSqrt(     PanningLR); //R Room

        //7. Convert pannings into channel volumes
        /*switch(OutputFormat)
        {
            case AL_FORMAT_MONO8:
            case AL_FORMAT_MONO16:
                drysend[0] = ConeVolume * ListenerGain * DryMix * aluSqrt(1.0f); //Direct
                drysend[1] = ConeVolume * ListenerGain * DryMix * aluSqrt(1.0f); //Direct
                wetsend[0] =              ListenerGain * WetMix * aluSqrt(1.0f); //Room
                wetsend[1] =              ListenerGain * WetMix * aluSqrt(1.0f); //Room
                break;
            case AL_FORMAT_STEREO8:
            case AL_FORMAT_STEREO16:
                drysend[0] = ConeVolume * ListenerGain * DryMix * aluSqrt(1.0f-PanningLR); //L Direct
                drysend[1] = ConeVolume * ListenerGain * DryMix * aluSqrt(     PanningLR); //R Direct
                wetsend[0] =              ListenerGain * WetMix * aluSqrt(1.0f-PanningLR); //L Room
                wetsend[1] =              ListenerGain * WetMix * aluSqrt(     PanningLR); //R Room
                break;
            case AL_FORMAT_QUAD8:
            case AL_FORMAT_QUAD16:
                drysend[0] = ConeVolume * ListenerGain * DryMix * aluSqrt((1.0f-PanningLR)*(1.0f-PanningFB)); //FL Direct
                drysend[1] = ConeVolume * ListenerGain * DryMix * aluSqrt((     PanningLR)*(1.0f-PanningFB)); //FR Direct
                drysend[2] = ConeVolume * ListenerGain * DryMix * aluSqrt((1.0f-PanningLR)*(     PanningFB)); //BL Direct
                drysend[3] = ConeVolume * ListenerGain * DryMix * aluSqrt((     PanningLR)*(     PanningFB)); //BR Direct
                wetsend[0] =              ListenerGain * WetMix * aluSqrt((1.0f-PanningLR)*(1.0f-PanningFB)); //FL Room
                wetsend[1] =              ListenerGain * WetMix * aluSqrt((     PanningLR)*(1.0f-PanningFB)); //FR Room
                wetsend[2] =              ListenerGain * WetMix * aluSqrt((1.0f-PanningLR)*(     PanningFB)); //BL Room
                wetsend[3] =              ListenerGain * WetMix * aluSqrt((     PanningLR)*(     PanningFB)); //BR Room
                break;
            default:
                break;
        }*/
    }
}
