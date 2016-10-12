//
//  instant_ipc.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/22/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__instant_ipc__
#define __home3D__instant_ipc__

#include <stdio.h>

/*********************** BK INSTANT MEDIA - INTERFACE   *********************************
 This holds the shared memory connections to bkInstant.  ie. Video and audio buffers.
 
 ****************************************************************************************/
const int MAX_SAMPLE_RATE = 44100;
const int MAX_AUDIO_CHANNELS = 2;
const int MAX_AUDIO_SECONDS  = 5;
const int MAX_AUDIO_BUFFERS  = 3;
const int MAX_VIDEO_FRAME_SIZE = 1900 * 1280 * 4;       // in bytes.
const int MAX_AUDIO_FRAME_SIZE = MAX_SAMPLE_RATE * MAX_AUDIO_CHANNELS * MAX_AUDIO_SECONDS;       // in bytes.

struct st_audio_buffer
{
    unsigned char     audio_buffer[MAX_AUDIO_FRAME_SIZE];
    unsigned long     bytes_available;
    char              num_channels;
};

struct st_video_buffer
{
    unsigned char     video_buffer[MAX_VIDEO_FRAME_SIZE];
    unsigned int     width;
    unsigned int     height;
    char             frame_valid;
};

#define MAX_FRAMES 4

struct bk_instant_ipc_memory_map
{
    long int StatusCounter;
    char	 ConnectionStatus[64];
    
    long int AcknowledgeCounter;    // move object
    long int CommandCounter;        // move object
    
    
    struct st_video_buffer     video_frame [MAX_FRAMES];
    struct st_audio_buffer     audio_buffer[MAX_AUDIO_BUFFERS];
    
    long int ResponseCounter;
    char	 Response        [128];
};


#endif /* defined(__home3D__instant_ipc__) */
