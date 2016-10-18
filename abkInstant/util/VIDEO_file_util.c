#include "protocol.h"
#include "devices.h"

/***********************************************************************
All Incoming Video will also be saved to the hard-drive on those 
systems with enough capacity.  This allows the user to call back anything
in its memory banks.  Think star trek logs.  "There were two people outside
your side door yesterday...    Can you replay it?  
What was the topic of it?  How long was it?  A smart robot could even
use the info to answer questions.  On Sept 20th, you said you wanted to
ignore all upgrade notices.
***********************************************************************/

/************************************************
Generate Audio File name based on the timestamp
*************************************************/
void GetNewVideoFileName()
{   
}

/************************************************
Create and Open Audio File for saving
*************************************************/
void OpenVideoFile()
{
}

/************************************************
Append Audio Data
*************************************************/
void AppendVideoData( int mFile )
{
}

/************************************************
Compress Audio (by unknown means right now)  
Probably call Ogvorbis library.  mp3 still under
patents?
*************************************************/
WORD* CompressVideoData( WORD* mAudio )
{
    return 0;
}
/************************************************
Uncompress Audio (by unknown means right now)  
Probably call Ogvorbis library.  mp3 still under
patents?
*************************************************/
WORD* UncompressVideoData( WORD* mAudio )
{
    return 0;
}

/************************************************
Find File Closest Timestamp
Maybe this can be shared with Audio search
*************************************************
void FindFile_by_ClosestTimestamp( FILE* mFile )
{
}*/