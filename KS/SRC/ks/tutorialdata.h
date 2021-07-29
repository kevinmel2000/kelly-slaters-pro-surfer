/*  THIS FILE IS AUTOMATICALLY GENERATED BY EXPORT FROM THE EXCEL FILE:  tutorialdata.XLS
    ALL CHANGES SHOULD BE MADE IN EXCEL AND RE_EXPORTED.  DO NOT HAND EDIT.
    JONATHAN WEBSTER, Treyarch, 2/06/02
*/

#ifndef TUTORIALDATA_H
#define TUTORIALDATA_H

#include "types.h"
#include "VOEngine.h"
#include "globaltextenum.h"
#include "trickdata.h"
#include "stringx.h"
#define BOARD_ANIM_NULL -1
#define SURFER_ANIM_NULL -1


enum WAVE_INDICATOR_TYPE
{
    WAVE_IND_NONE,
    WAVE_IND_BLANK,
    WAVE_IND_FACE,
    WAVE_IND_LIP,
    WAVE_IND_TUBE,
};


enum FLAGTYPE
{
    NO_FLAG,
    AIR_AND_FACE_FLAG,
    SPECIAL_ACTIVE_FLAG,
    SPECIAL_TRICK_FLAG,
    PERFECT_TRICK_FLAG,
    ONLY_TUBE_UPDATE_TIME_FLAG,
    BEGIN_IN_TUBE_FLAG,
};


struct TutorialStep
{
    int text_id;
    char vo_name_string[40];
    int hint_text_id;
    int advancement_state;
    int advancement_state2;
    int advancement_super_state;
    int advancement_super_state2;
    int advancement_trick;
    int advancement_trick2;
    int advancement_trick3;
    int advancement_trick4;
    int advancement_gap;
    int flags;
    int wave_indicator;
    float advancement_time;
    uint8 advancement_button;
    uint8 advancement_button2;
};

extern TutorialStep GTutorialStep[];
extern int Tutorial_Step_Num;

#endif /* #ifndef tutorialdata_H */