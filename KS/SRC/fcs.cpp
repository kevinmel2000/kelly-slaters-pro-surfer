////////////////////////////////////////////////////////////////////////////////
/*
  game.cpp

  oh my god, the legendary game.cpp has regrown from the slime!

*/
////////////////////////////////////////////////////////////////////////////////
#include "global.h"

#include "fcs.h"
//!#include "character.h"
#include "osdevopts.h"
#include "hwmath.h"
#include "game.h"
//!#include "attrib.h"
//!//!#include "limb.h"


////////////////////////////////////////////////////////////////////////////////
// force_control_system
////////////////////////////////////////////////////////////////////////////////

force_control_system::force_control_system() : motion_object()
{
  locked = false;
  reset_locked = false;
}