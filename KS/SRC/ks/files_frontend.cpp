// front end files

// With precompiled headers enabled, all text up to and including 
// the following line are ignored by the compiler (dc 01/18/02)
#include "global.h"


#include "globaltextenum.cpp"
#include "boarddata.cpp"
#include "FEPanel.cpp"
#include "FEAnim.cpp"
#include "FEMenu.cpp"
#include "tutorialmanager.cpp"
#include "igolearn_new_trickmanager.cpp"
#include "igoiconmanager.cpp"
#include "igohintmanager.cpp"
#include "igo_widget.cpp"
#include "igo_widget_simple.cpp"
#include "igo_widget_meterchallenge.cpp"
#include "igo_widget_analogclock.cpp"
#include "igo_widget_skillchallenge.cpp"
#include "igo_widget_specialmeter.cpp"
//#include "igo_widget_breakindicator.cpp"
#include "igo_widget_waveindicator.cpp"
#include "igo_widget_balance.cpp"
#include "igo_widget_splitter.cpp"
#include "igo_widget_fanmeter.cpp"
#include "igo_widget_camera.cpp"
#include "igo_widget_objectalert.cpp"
#include "igo_widget_photo.cpp"
#include "igo_widget_replay.cpp"
#include "igo_widget_splitmeter.cpp"
#include "igo_widget_splitscore.cpp"
#include "igo_widget_splitclock.cpp"
#include "igo_widget_timeattack.cpp"
#include "igo_widget_iconcount.cpp"
#include "igo_widget_iconradar.cpp"
#include "igo_widget_grid.cpp"
#include "IGOFrontEnd.cpp"
#include "MainFrontEnd.cpp"
#include "SurferFrontEnd.cpp"
#include "Map.cpp"
#include "BeachFrontEnd.cpp"
#include "TrickBookFrontEnd.cpp"
#include "FrontEndManager.cpp"

/*#ifdef TARGET_XBOX
#include "HWOSXB\xb_SaveLoadFrontEnd.cpp"
#else*/
#include "SaveLoadFrontEnd.cpp"
//#endif

#include "GameData.cpp"
#ifdef TARGET_GC
#include "GCMCDetectFrontEnd.cpp"
#endif
#include "MCDetectFrontEnd.cpp"
#include "FrontEndMenus.cpp"
#include "BoardFrontEnd.cpp"
#include "StatsFrontEnd.cpp"
#include "ExtrasFrontEnd.cpp"
#include "MultiFrontEnd.cpp"
#include "GraphicalMenuSystem.cpp"
#include "FEEntityManager.cpp"
#include "PlaylistMenu.cpp"
#include "HighScoreFrontEnd.cpp"
#include "PhotoFrontEnd.cpp"
#include "TutorialFrontEnd.cpp"
#include "CheatFrontEnd.cpp"
#include "AccompFrontEnd.cpp"
#include "PAL60FrontEnd.cpp"
#include "LogbookFrontEnd.cpp"
