#include "global.h"

#include <algorithm>

#include "ostimer.h"
#include "xb_audio.h"
#include "xb_file.h"
#include "osdevopts.h"
#include "osfile.h"

#undef free
#undef malloc
#include "ngl.h"

#include "profiler.h"

static int automatic_emitter_counter=0;

DEFINE_SINGLETON(sound_device)
unsigned int sound_instance::id_counter = 0;

static list<sound_instance *> playback_pending_list;

#define MINIMUM_VOLUME 0.001f

// minimum value for this would be the max number of cd streams (10 right now)
#define SPU_STREAM_BUFFERS 12
#define CRAZY_VOLUME_VALUE -123456789.0f

extern profiler_timer proftimer_adv_sound_gas;
extern profiler_timer proftimer_adv_sound_add_inst;
extern profiler_timer proftimer_adv_sound_play;
extern profiler_timer proftimer_adv_sound_stop;
extern profiler_timer proftimer_adv_sound_cmd;
extern profiler_timer proftimer_adv_sound_is_ready;
extern profiler_timer proftimer_adv_sound_is_playing;
extern profiler_timer proftimer_adv_sound_pause;
extern profiler_timer proftimer_adv_sound_unpause;
extern profiler_counter profcounter_gas_add_inst;
extern profiler_counter profcounter_gas_add_inst_legit;

void set_3d_volume(int inst, int angle, unsigned short vol, short *volume_left, short *volume_right);

/*-------------------------------------------------------------------------------------------------------

  sound_device implementation

-------------------------------------------------------------------------------------------------------*/

/*** constructor ***/
sound_device::sound_device() 
  : listener_position(0)
  , listener_velocity(0)
  , listener_orientation_front(0,0,1)
  , listener_orientation_top(0,1,0)
  , sound_data_list_count(0)
  , used_audio_memory(0)
  , lock_playback(false)
{
/*  if (os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    nglPrintf("Audio disabled.  If you want to re-enable it, go check your GAME.INI\n");
  }
  else
  {
    debug_print("sound_device init");

    stringx debug_dir(nglHostPrefix);
    debug_dir += "c:\\usr\\local\\sce\\iop\\modules\\";
    if (load_iop_module("libsd.irx", debug_dir.c_str()) == false)
    {
	    nglPrintf("Can't load module libsd\n");
	    Exit(0);
    }

    // clear out the command list
    memset(&command_list, 0, GAS_COMMAND_LIST_SIZE);

    debug_dir = nglHostPrefix;
    debug_dir += "c:\\ks\\src\\hwosxb\\gas\\";
    //nglPrintf("debug dir %s\n", debug_dir_str);
    if (load_iop_module("gas.irx", debug_dir.c_str()) == false)
    {
	    nglPrintf("Can't load module gas\n");
	    Exit(0);
    }
    init();
  }
  */
}


/*** destructor ***/
sound_device::~sound_device()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    shutdown();
    debug_print("sound_device shut down");
  }
*/}


/*** init ***/
void sound_device::init()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    debug_print("sound_device::init");

    os_file io;
    listfile_size = -1;
    char temp_filename[FILENAME_LENGTH];

    gas_rpc_init();

    // try to open it on the cd first
    if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_CD))
    {
      strcpy(listfile_name, os_developer_options::inst()->get_string(os_developer_options::STRING_SOUND_LIST).c_str());
      io.open(listfile_name, os_file::FILE_READ);
      if (io.is_open())
      {
        if (io.is_from_cd())
        {
          strcpy(listfile_name, "cdrom:\\");
          strcpy(temp_filename, os_developer_options::inst()->get_string(os_developer_options::STRING_SOUND_LIST).c_str());
          strupr(temp_filename);
          strcat(listfile_name, temp_filename);
          strcat(listfile_name, ";1");
        }
        else
        {
          strcpy(listfile_name, nglHostPrefix);
          strcat(listfile_name, os_developer_options::inst()->get_string(os_developer_options::STRING_SOUND_LIST).c_str());
        }
        listfile_size = io.get_size();
        io.close();
      }
    }
    // we need to find a list file somewhere (and it can't be in the stash, because the iop needs
    // to be able to load it).
#ifdef WEENIEASSERT  // this triggers every time KSPS starts so it's gon
    assert(listfile_size != -1);
#endif
    gas_rpc(GAS_RPC_INIT, listfile_name, SPU_STREAM_BUFFERS , listfile_size, 1, 0);

	partial_init();
  }*/
}


/*** shutdown ***/
void sound_device::shutdown()
{
/*  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    debug_print("sound_device::shutdown");
    clear();
    gas_rpc(GAS_RPC_SHUTDOWN, NULL, 0, 0, 0, 0);
  }*/
}


/*** partial_init ***/
void sound_device::partial_init()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
		dampened_flag = false;
		dampen_value = 80;
    debug_print("sound_device::partial_init");
  }*/
}


/*** partial_shutdown ***/
void sound_device::partial_shutdown()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    debug_print("sound_device::partial_shutdown");
    gas_rpc(GAS_RPC_RESET, listfile_name, listfile_size, 0, 0, 0);
//    clear();
  }*/
}


/*** stop_all_instances ***/
void sound_device::stop_all_instances()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    debug_print("sound_device::stop_all_sounds");
    gas_rpc(GAS_RPC_STOP_ALL, NULL, 0, 0, 0, 0);
  }*/
}


/*** set_master_volume ***/
void sound_device::set_master_volume(rational_t new_volume)
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    assert(new_volume >= 0.0f && new_volume <= 1.0f);
    int xb_volume = (int)(new_volume * SCE_VOLUME_MAX);
    gas_rpc(GAS_RPC_SET_MASTER_VOLUME, NULL, xb_volume, 0, 0, 0);
  }*/
}

void sound_device::set_dampen_value( int new_value )
{/*
	if ( new_value < 1 || new_value > 100 )
		return;

	dampen_value = new_value;*/
}

int sound_device::get_dampen_value( void )
{/*
	return( dampen_value );*/
  return 1;
}

//*** dampen_all_instances ***
void sound_device::dampen_all_instances( void )
{/*
	if ( !os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
	{
		dampened_flag = true;
		gas_rpc(GAS_RPC_DAMPEN_ALL, NULL, get_dampen_value(), 0, 0, 0); 
	}*/
}

//*** undampen_all_instances ***
void sound_device::undampen_all_instances( void )
{/*
	if ( !os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
	{
		dampened_flag = false;
		gas_rpc(GAS_RPC_UNDAMPEN_ALL, NULL, get_dampen_value(), 0, 0, 0);
	}*/
}
  
// Releases all sound_data's and sound_instances.
void sound_device::release_all_sounds()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    // Sound source data.
    for (int i=0; i<sound_data_list_count; ++i)
    { 
      sound_data_list[i].release();
    }
    sound_data_list_count = 0;

    // Storage for sound emitters.
    list<sound_emitter *>::iterator curr = emitter_list.begin();
    while (curr != emitter_list.end())
    {
      sound_emitter *nuke_me = (*curr);
      ++curr;
      delete nuke_me;
    }
    emitter_list.resize(0);

    // storage for sound instances
    list<sound_instance *>::iterator curr_inst = instance_list.begin();
    while (curr_inst != instance_list.end())
    {
      sound_instance *nuke_me = (*curr_inst);
      if (nuke_me->iop_instance_id >= 0)
      {
        proftimer_adv_sound_gas.start();
        proftimer_adv_sound_stop.start();
        gas_rpc (GAS_RPC_STOP_INSTANCE, NULL, nuke_me->iop_instance_id, 0, 0, 0 );
        proftimer_adv_sound_stop.stop();
        proftimer_adv_sound_gas.stop();
      }
      nuke_me->iop_instance_id = -1;
      ++curr_inst;
      delete nuke_me;
    }
    instance_list.resize(0);
    
    // clear auxillary lists
    emitter_purgatory.resize(0);
    instance_purgatory.resize(0);
    instance_death_row.resize(0);
  }*/
}


/*** release_instance ***/
void sound_device::release_instance(sound_instance* si)
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    // mark the instance as a death row instance, and add it to death row
    si->killed = true;
    if (find(instance_death_row.begin(), instance_death_row.end(), si) == instance_death_row.end())
      instance_death_row.push_back(si);
    else
    {
//      nglPrintf("WOAH MOMMA, we almost set ourselves up for double-deletion of the instance kind!\n");
//      assert(0);
    }
      
#ifdef DEBUG_AUDIO
if (si->data)
    nglPrintf("releasing instance %d %s\n", si->id, si->data->get_file_name().c_str());
else
    nglPrintf("releasing instance %d\n", si->id);
#endif
  }*/
}

/*** release_emitter ***/
list<sound_emitter *>::iterator sound_device::release_emitter(sound_emitter* se)
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
#ifdef DEBUG_AUDIO
nglPrintf("releasing emitter %d\n", se->auto_count);
#endif
    // release all of its instances and add it to emitter purgatory
    list<sound_instance *>::iterator curr = se->sound_list.begin();

    while (curr != se->sound_list.end())
    {
      (*curr)->emitter = NULL;
      (*curr)->release();
      ++curr;
    }
    se->used = false;
    se->sound_list.resize(0);
    list<sound_emitter *>::iterator nuke_me = find(emitter_list.begin(), emitter_list.end(), se);
    if (find(emitter_purgatory.begin(), emitter_purgatory.end(), se) == emitter_purgatory.end())
      emitter_purgatory.push_back(se);
    else
    {
//      nglPrintf("WOAH MOMMA, we almost set ourselves up for double-deletion of the instance kind!\n");
//      assert(0);
    }
    return emitter_list.erase(nuke_me);
  }
  else
  {
    return NULL;
  }*/
  return NULL;
}

/*** kill_death_row_instances ***/
void sound_device::kill_death_row_instances()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    // kill any pending instances
    list<sound_instance *>::iterator victim = instance_death_row.begin();

    while (victim != instance_death_row.end())
    {
  #ifdef DEBUG_AUDIO
  nglPrintf("killing instance %d ", (*victim)->id);
  #endif
      // remove this instance from its emitter (if it has one)
      if ((*victim)->emitter)
      {
        list<sound_instance *>::iterator nuke_me = find ((*victim)->emitter->sound_list.begin(),
          (*victim)->emitter->sound_list.end(), (*victim));
        (*victim)->emitter->sound_list.erase(nuke_me);
        (*victim)->emitter = NULL;
      }

      // remove this instance from its data
      if ((*victim)->data)
      {
  #ifdef DEBUG_AUDIO
  nglPrintf(" %s", (*victim)->data->get_file_name().c_str());
  #endif
        list<sound_instance *>::iterator nuke_me = find ((*victim)->data->instance_list.begin(),
          (*victim)->data->instance_list.end(), (*victim));
        (*victim)->data->instance_list.erase(nuke_me);
        (*victim)->data = NULL;
      }
  #ifdef DEBUG_AUDIO
  nglPrintf("\n");
  #endif

      // make it look unused
      (*victim)->id = (unsigned)-1;
      (*victim)->iop_instance_id = -1;

      // push it into the unused instances list
      if (find(instance_purgatory.begin(), instance_purgatory.end(), *victim) == instance_purgatory.end())
        instance_purgatory.push_back(*victim);
      else
      {
//        nglPrintf("WOAH MOMMA, we almost set ourselves up for double-deletion of the instance purgatory kind!\n");
//        assert(0);
      }
      ++victim;
    }
    // clear out the bodies, we're finished
    instance_death_row.resize(0);
  }*/
}


/*** load_sound ***/
// Load from disk.  Note that no file extension should be specified.
sound_id_t sound_device::load_sound( const stringx & file_name )
{/*
  int i;

  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    filespec spec(file_name);
    pstring snd(spec.name);
  
    // Find the first sound in the list that isn't loaded.
    for(i=0; i<sound_data_list_count; ++i)
    {
	    if ( sound_data_list[i].file_name == snd )
		    return i;
    }

    if (sound_data_list_count >= MAX_SOUND_ID-1)
      error( ("Exceeded number of loadable sounds limit: " + file_name).c_str() );

    sound_data_list[sound_data_list_count].load(file_name);                           
    sound_data_list_count++;
    return sound_data_list_count-1;
  }
  else
  {
    return 0;
  }*/

  return 0;
}


/*** set_sound_ranges ***/
void sound_device::set_sound_ranges( sound_id_t id, rational_t min, rational_t max )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    assert( id >= 0 && id <= MAX_SOUND_ID );
	  sound_data_list[id].min_dist = min;
	  sound_data_list[id].max_dist = max;
  }*/
}


/*** get_sound_id ***/
sound_id_t sound_device::get_sound_id( const pstring& name, bool fatal )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    for ( int i = 0; i < sound_data_list_count; ++i )
    {
//nglPrintf("comparing %s to %s\n", sound_data_list[ i ].file_name.c_str(), name.c_str());
      if ( sound_data_list[ i ].file_name == name )
        return i;
    }

	  if ( fatal )
		  debug_print( "Sound not loaded: %s.", name.c_str() );
  }

  // id for 'SILENT' see todo in game.cpp::load_this_level*/
  return 0;
}


/*** play_sound ***/
// Fire-and-forget(tm) playback interface.
void sound_device::play_sound( sound_id_t sound_id, rational_t vol, rational_t pitch )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    assert( sound_id >= 0 && sound_id <= MAX_SOUND_ID );

    // Create a new instance of the sound.
    sound_data * data = &sound_data_list[ sound_id ];
    sound_instance * instance = data->create_instance();
    instance->last_positional_volume = CRAZY_VOLUME_VALUE;
  //	instance->set_volume( vol );
  //	instance->set_pitch( pitch );

    // Start the new instance playing.
    instance->play();
  }*/
}


/*** play_3d_sound ***/
sound_emitter *sound_device::play_3d_sound( sound_id_t id, const vector3d &pos, rational_t vol, rational_t pitch, rational_t min, rational_t max )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    sound_emitter *ret = create_emitter(pos);
    ret->automatic = true;
    ret->play_sound(id, vol, pitch, min, max);

    ret->auto_count = ++automatic_emitter_counter;
  //nglPrintf("automatic emitter for sound %s %d\n", sound_data_list[id].get_file_name().c_str(), ret->auto_count);
    return ret;
  }
  else
  {
    return &dummy_emitter;
  }*/

  return &dummy_emitter;
}

// Create a new sound emitter for playing 3D sounds.
sound_emitter * sound_device::create_emitter(const vector3d &pos)
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    sound_emitter *emitter = NULL;
    if (emitter_purgatory.size() > 0)
    {
      // re-use an old one
  #ifdef DEBUG_AUDIO
  nglPrintf("reusing emitter\n");
  #endif
      emitter = emitter_purgatory.front();
      emitter_purgatory.pop_front();
      emitter->initialize();
    }
    else
    {
  #ifdef DEBUG_AUDIO
  nglPrintf("creating emitter\n");
  #endif
      // create a new one
      emitter = new sound_emitter;
    }

    if(emitter != NULL)
    {
      // add it to the emitter list
      emitter_list.push_back(emitter);
      emitter->set_position(pos);
      emitter->waiting_for_action = true;
    }
    else 
      error("Cannot create sound_emitter");

    return(emitter);
  }
  else
  {
    return &dummy_emitter;
  }*/
  return NULL;
}

// Create a new sound instance
sound_instance * sound_device::create_instance()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    sound_instance *instance = NULL;
    if (instance_purgatory.size() > 0)
    {
      // re-use an old one
      instance = instance_purgatory.front();
  #ifdef DEBUG_AUDIO
  nglPrintf("reusing instance %d ", instance->id);
  #endif
      instance_purgatory.pop_front();
      instance->initialize();
  #ifdef DEBUG_AUDIO
  nglPrintf("%d\n", instance->id);
  #endif
    }
    else
    {
      // create a new one
      instance = new sound_instance;
      instance_list.push_back(instance);
  #ifdef DEBUG_AUDIO
  nglPrintf("creating instance %d\n", instance->id);
  #endif
    }

    return(instance);
  }
  else
  {
    return &dummy_instance;
  }*/

  return &dummy_instance;
}

// replacing the integer divide with a shift, since int devides are so slow on the xb.
// same as dividing by 2^EMITTER_UPDATES_SHIFT_BITS
#define EMITTER_UPDATES_SHIFT_BITS 1

bool gimme_the_lowdown = false;

// Allows the emitters to update their owned sounds.
// we are limiting the number of updates per emitter to once every few frames, since
// the xb exhibits audible 'pops' when the volume is updated too often.
void sound_device::frame_advance(const time_value_t &time_inc)
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    list<sound_emitter *>::iterator emitter = emitter_list.begin();

    // debugging code, remove me if I bother you.
    if (gimme_the_lowdown)
    {
      proftimer_adv_sound_gas.start();
      gas_rpc(GAS_RPC_VOICE_STATUS, NULL, 0, 0, 0, 0);
      proftimer_adv_sound_gas.stop();
      gimme_the_lowdown = false;
    }

//    int target_updates_this_frame = emitter_count; // >> EMITTER_UPDATES_SHIFT_BITS;
//    int updates_this_frame = 0;

    if (emitter_list.size() > 0)
    {
      while (emitter != emitter_list.end())
      {
        (*emitter)->update_sounds();
//        updates_this_frame++;

        // prune out dead emitters
        if ((*emitter)->automatic == true && (*emitter)->in_use() == false && (*emitter)->waiting_for_action == false)
        {
    //nglPrintf(" removing emitter %d\n", emitter->auto_count);
          sound_emitter * delete_me = (*emitter);
          emitter = release_emitter(delete_me);
        }
        else
        {
          ++emitter;
        }
      }
    }

    // send the command list, get the status
    proftimer_adv_sound_gas.start();
    proftimer_adv_sound_cmd.start();
    gas_rpc (GAS_RPC_COMMAND_LIST, (char *)command_list, 0, 0, 0, 0 );
    proftimer_adv_sound_cmd.stop();
    proftimer_adv_sound_gas.stop();

    kill_death_row_instances();

    // try to play playback_pending instances
    if (playback_pending_list.size() > 0)
    {
      list<sound_instance *>::iterator it = playback_pending_list.begin();
      while( it != playback_pending_list.end() )
      {
        if ((*it)->is_ready())
        {
          (*it)->play();
  #ifdef DEBUG_AUDIO
  nglPrintf("Delayed playback of %s 0x%x\n", (*it)->data->get_file_name().c_str(), (*it)->iop_instance_id);
  #endif
          it = playback_pending_list.erase(it);
        }
        if (it != playback_pending_list.end())
          ++it;
      }
    }
  }*/
}


/*** create_sound ***/
// Create an instance of a sound for playback, etc.  Don't forget to release() it when you're done.
sound_instance * sound_device::create_sound( sound_id_t sound_id )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    assert( sound_id >= 0 && sound_id <= MAX_SOUND_ID );
    assert( sound_id < sound_data_list_count );
    return sound_data_list[ sound_id ].create_instance();
  }
  else
  {
    return &dummy_instance;
  }*/
  return NULL;
}


/*** clear ***/
void sound_device::clear()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    release_all_sounds();
    if ( os_developer_options::inst()->is_flagged(os_developer_options::FLAG_REPORT_LOAD_SOUND) )
      debug_print( "sound_device::clear()" );
  }*/
}

void sound_device::set_listener_position( const vector3d & p )
{ /*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    listener_position = p; 
  //  nglPrintf("Listener position (%g, %g, %g)\n", p.x, p.y, p.z);
  }*/
}

void sound_device::set_listener_velocity( const vector3d & v )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    listener_velocity = v; 
  }*/
}

void sound_device::set_listener_orientation( const vector3d & front, const vector3d & top )
{ /*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    listener_orientation_front = front; listener_orientation_top = top; 
  //  nglPrintf("Listener front/up (%g, %g, %g)/(%g, %g, %g)\n", front.x, front.y, front.z,
  //    top.x, top.y, top.z);
  }*/
}


/*-------------------------------------------------------------------------------------------------------

  sound_data implementation

-------------------------------------------------------------------------------------------------------*/

/*** constructor ***/
sound_data::sound_data()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    loaded = false;

    iop_source_id = -1;

    min_dist = 0.0f;
    max_dist = 0.0f;

    file_name = "Not loaded.";
  }*/
}


/*** destructor ***/
sound_data::~sound_data() 
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    // If still loaded, warn the user.  
    if ( loaded )
    {
      debug_print("Sound object not released: "+stringx(file_name.c_str())+sendl);
      release();
    }
  }*/
}


/*** load ***/
void sound_data::load( const stringx & _file_name )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    assert(loaded == false);

    stringx __file_name(_file_name);
    __file_name.to_upper();
    filespec spec(__file_name);

    iop_source_id = gas_rpc (GAS_RPC_ADD_SOURCE, (char *)spec.name.c_str(), 0, 0, 0, 0 );

    if (iop_source_id < 0 )
    {
      debug_print("Warning.  Could not load sound source %s\n", __file_name.c_str());
      loaded = false;
    }
    else
    {
      loaded = true;
      file_name = spec.name.c_str();
  //nglPrintf("new filename %s, should be %s\n", file_name.c_str(), _file_name.c_str());
      min_dist = 10.0f;
      max_dist = 30.0f;
    }
  }*/
}


/*** release ***/
void sound_data::release()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    // release all remaining sound_instance's. 
    list<sound_instance *>::iterator curr = instance_list.begin();
    while (curr != instance_list.end())
    {
      // release the instance (which removes it from its data's instance list).
      (*curr)->data = NULL;
      (*curr)->release();
      ++curr;
    }
    instance_list.resize(0);

//    gas_rpc (GAS_RPC_REMOVE_SOURCE, NULL, iop_source_id, 0, 0, 0 );
    loaded = false;
  }*/
}


/*** create_instance ***/
sound_instance * sound_data::create_instance()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    sound_instance *instance = sound_device::inst()->create_instance();

    // add it to the instance list
    instance_list.push_back(instance);

    // set its data up
    instance->data = this;
    instance->min_dist = min_dist;
    instance->max_dist = max_dist;

    return instance;
  }
  else
  {
    return &sound_device::inst()->dummy_instance;
  }*/
  return NULL;
}


/*** delete_instance ***/
void sound_data::delete_instance( sound_instance * s_instance )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    s_instance->release();
  }*/
}


/*-------------------------------------------------------------------------------------------------------

  sound_instance implementation

-------------------------------------------------------------------------------------------------------*/

/*** constructor ***/
sound_instance::sound_instance()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    initialize();
  }*/
}

void sound_instance::initialize()
{/*
  data = NULL;
  emitter = NULL;    
  volume = 1.0f;
  positional_volume = 1.0f;
  last_positional_volume = -1.0f;
  volume_left = 0;
  volume_right = 0;
  pitch = 1.0f;
  min_dist = 0.0f;
  max_dist = 0.0f;
  iop_instance_id = -1;
  is_paused = false;
  loop = false;
  playback_begun = false;
  killed = false;
  user_stopped = false;
  stereo = false;
  pan_left = 0.5f;
  pan_right = 0.5f;
  angle = 0.0f;         // straight ahead, then angles go cw, viewed from above
  last_angle = 0.0f;

  id = ++id_counter;

	dampen_count = 0;

  // Make sure id's are always not zero
  if(id == 0)
  {
    id = ++id_counter;
    assert(0);
  }*/
}


/*** destructor ***/
sound_instance::~sound_instance()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
//  release();
//  assert(!emitter);
//  assert(!data);
  }*/
}


/*** release ***/
void sound_instance::release()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    if (iop_instance_id >= 0)
    {
      proftimer_adv_sound_gas.start();
      proftimer_adv_sound_stop.start();
      gas_rpc (GAS_RPC_STOP_INSTANCE, NULL, iop_instance_id, 0, 0, 0 );
      proftimer_adv_sound_stop.stop();
      proftimer_adv_sound_gas.stop();
    }
    iop_instance_id = -1;
    sound_device::inst()->release_instance(this);
  }*/
}

//#include "script_object.h"
//#include "wds.h"

// return true if we have an active instance after this fn.
bool sound_instance::check_internal_instance()
{/*
  // if we have an in-active iop instance, but the volumes are higher, pre-load it
//#pragma todo("Look into why we have so many SILENT sounds attempting to add themselves (GT--5/4/01)")
  if (data->iop_source_id > 0 && iop_instance_id < 0 && volume >= MINIMUM_VOLUME && 
    (positional_volume >= MINIMUM_VOLUME || loop==false) && user_stopped == false)
  {
    if (data)
    {
      proftimer_adv_sound_gas.start();
      proftimer_adv_sound_add_inst.start();
      ADD_PROF_COUNT(profcounter_gas_add_inst, 1);
      iop_instance_id = gas_rpc (GAS_RPC_ADD_INSTANCE, NULL, data->iop_source_id, 0, 0, 0 );
			if ( sound_device::inst()->dampened_flag )
				gas_rpc(GAS_RPC_DAMPEN_INSTANCE, NULL, sound_device::inst()->get_dampen_value(), 0, 0, 0);
      if (iop_instance_id != -1)
      {
        ADD_PROF_COUNT(profcounter_gas_add_inst_legit, 1);
      }
      else
        nglPrintf("Illegit %d %s\n", data->iop_source_id, data->file_name.c_str());
      proftimer_adv_sound_add_inst.stop();
      proftimer_adv_sound_gas.stop();
      if ((iop_instance_id & GAS_INSTANCE_STEREO_FLAG_BIT) != 0)
      {
        // flag the sound as a stereo sound, and clear the flag bit from the id
        stereo = true;
        iop_instance_id &= ~GAS_INSTANCE_STEREO_FLAG_BIT;
      }
#ifdef DEBUG_AUDIO
      nglPrintf("Preloading instance 0x%x, %s\n", iop_instance_id, data->file_name.c_str());
#endif
    }
  } 

  return (iop_instance_id >= 0);*/
  return true;
}


/*** play ***/
void sound_instance::play( bool looping )  
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    if (killed == false)
    {
      user_stopped = false;
      loop = looping;
      if (check_internal_instance())
      {
        if (is_ready() == false)
        {
          // queue it up
          playback_pending_list.push_back(this);
        }
        else
        {
          last_positional_volume = CRAZY_VOLUME_VALUE; 
          set_volume( volume );
          set_pitch( pitch );
          if (is_paused)
          {
      proftimer_adv_sound_gas.start();
      proftimer_adv_sound_cmd.start();
            gas_rpc (GAS_RPC_COMMAND_LIST, (char *)command_list, 0, 0, 0, 0 );
      proftimer_adv_sound_cmd.stop();
      proftimer_adv_sound_unpause.start();
            gas_rpc (GAS_RPC_UNPAUSE_INSTANCE, NULL, iop_instance_id, 0, 0, 0 );
      proftimer_adv_sound_unpause.stop();
      proftimer_adv_sound_gas.stop();

            is_paused = false;
          }
          else
          {
      proftimer_adv_sound_gas.start();
      proftimer_adv_sound_play.start();
            gas_rpc (GAS_RPC_PLAY_INSTANCE, NULL, iop_instance_id, volume_left, volume_right, 0 );
      proftimer_adv_sound_play.stop();
      proftimer_adv_sound_gas.stop();
          }
          playback_begun = true;
  #ifdef DEBUG_AUDIO
  nglPrintf("playing instance %s 0x%x\n", data->get_file_name().c_str(), iop_instance_id);
  #endif
        }
      }
      else
      {
      }
    }
  }*/
}


/*** stop ***/
void sound_instance::stop()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    if (killed == false)
    {
      if (is_playing() && (!loop))
      {
  #ifdef DEBUG_AUDIO
  nglPrintf("attempting to STOP iop instance 0x%x, %s\n", iop_instance_id, data->file_name.c_str());
  #endif
      proftimer_adv_sound_gas.start();
      proftimer_adv_sound_pause.start();
        gas_rpc (GAS_RPC_PAUSE_INSTANCE, NULL, iop_instance_id, 0, 0, 0 );
      proftimer_adv_sound_pause.stop();
      proftimer_adv_sound_gas.stop();

        is_paused = true;
      }
      else
      {
        if (iop_instance_id >= 0)
        {
  #ifdef DEBUG_AUDIO
  nglPrintf("attempting to KILL iop instance 0x%x, %s\n", iop_instance_id, data->file_name.c_str());
  #endif
      proftimer_adv_sound_gas.start();
      proftimer_adv_sound_stop.start();
          gas_rpc (GAS_RPC_STOP_INSTANCE, NULL, iop_instance_id, 0, 0, 0 );
      proftimer_adv_sound_stop.stop();
      proftimer_adv_sound_gas.stop();
        }
        iop_instance_id = -1;
      }
      user_stopped = true;
    }
  }*/
}


/*** rewind ***/
void sound_instance::rewind() 
{ /*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    if (killed == false)
    {
      if (iop_instance_id >= 0)
      {
      proftimer_adv_sound_gas.start();
      proftimer_adv_sound_stop.start();
        gas_rpc (GAS_RPC_STOP_INSTANCE, NULL, iop_instance_id, 0, 0, 0 );
      proftimer_adv_sound_stop.stop();
      proftimer_adv_sound_gas.stop();

        iop_instance_id = -1;
        check_internal_instance();
      }
    }
  }*/
}


/*** is_playing ***/
// returns false if a sound has stopped either by itself or by stop() or if it hasn't been started.
bool sound_instance::is_playing()
{/*
  bool ret;
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    if (killed)
      return false;
    if (is_paused)
      return false;
    if ( (iop_instance_id >= 0) && playback_begun )
    {
      proftimer_adv_sound_gas.start();
      proftimer_adv_sound_is_playing.start();
      ret = (bool)gas_rpc (GAS_RPC_STATUS_IS_PLAYING, NULL, iop_instance_id, 0, 0, 0 );
      proftimer_adv_sound_is_playing.stop();
      proftimer_adv_sound_gas.stop();
      return ret;
  //    return (g_iop_status.instances[iop_instance_id & GAS_INSTANCE_ID_MASK].is_playing);
    }

    // check if it is pending
    if (playback_pending_list.size() > 0)
    {
      list<sound_instance *>::iterator it = playback_pending_list.begin();
      while( it != playback_pending_list.end() )
      {
        if ((*it) == this)
        {
          return true;
        }
        ++it;
      }
    }
  }
  return false;*/
  return true;
}

// get the buffers preloaded so we can start playback instantly, and a query fn for the preload
void sound_instance::preload()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    if (killed == false)
    {
      // since they want a preload, we will force the minimum volumes 
      // to be loud enough to trigger a preload
      if (volume <= MINIMUM_VOLUME)
        volume = MINIMUM_VOLUME * 2.0f;
      if (positional_volume <= MINIMUM_VOLUME)
        positional_volume = MINIMUM_VOLUME * 2.0f;

      check_internal_instance();
    }
  }*/
}

bool sound_instance::is_ready()
{/*
  bool ret;
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    if (killed)
      return false;
//  return (g_iop_status.instances[iop_instance_id & GAS_INSTANCE_ID_MASK].is_ready);
    proftimer_adv_sound_gas.start();
    proftimer_adv_sound_is_ready.start();
    ret = (bool)gas_rpc (GAS_RPC_STATUS_IS_READY, NULL, iop_instance_id, 0, 0, 0 );
    proftimer_adv_sound_is_ready.stop();
    proftimer_adv_sound_gas.stop();
    return ret;
  }
  else
  {
    return true;
  }*/

  return false;
}

/*** set_pos ***/
// Position control
void sound_instance::set_pos( int new_pos )      // skip to a specific sample number, or 0.
{/*
  debug_print("Warning, sound_instance::set_pos does nothing on the XB");
  */
}


/*** get_scaled_pos ***/
rational_t sound_instance::get_scaled_pos()      // returns a number between 0 and 1, scaled to the length of the sound.
{/*
  // Get the current position divided by the number of samples in the file.
  debug_print("Warning, sound_instance::get_scaled_pos does nothing on the XB");*/
  return 0;
}


/*** get_pos ***/
int sound_instance::get_pos()                    // returns the current sample number.
{
  //debug_print("Warning, sound_instance::get_pos does nothing on the XB");
  return 0;
}


/*** set_volume ***/
void sound_instance::set_volume( rational_t _volume )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    if (killed == false)
    {
      if (iop_instance_id >= 0)
      {
        if (volume == _volume && positional_volume == last_positional_volume && angle == last_angle)
          return;

        volume = _volume;
        last_positional_volume = positional_volume;
        last_angle = angle;

        rational_t scaled_volume = volume * positional_volume;
        assert(scaled_volume <= 1.0f && scaled_volume >= 0.0f);

        if (stereo)
        {
          // simple stereo case
          volume_left = (short)((scaled_volume * pan_left) * SCE_VOLUME_MAX);
          volume_right = (short)((scaled_volume * pan_right) * SCE_VOLUME_MAX);

          gas_volume(iop_instance_id, volume_left, volume_right);
  //        gas_rpc (GAS_RPC_INSTANCE_VOLUME, NULL, iop_instance_id, vol_left, vol_right, 0 );
        }
        else
        {
          // 3d case
          int vol = (int)(scaled_volume * SCE_VOLUME_MAX);
          int angle_in_degrees = (int)((angle * 180.0f) / M_PI);

          assert(angle >= 0.0f);
          set_3d_volume (iop_instance_id, angle_in_degrees, vol, &volume_left, &volume_right);
        }
      }
    }
  }*/
}


/*** set_pitch ***/
void sound_instance::set_pitch( rational_t _pitch )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
  //  debug_print("Warning, sound_instance::set_volume does nothing on the XB, yet");
    if (killed == false)
    {
      // pitch of 1000 is normal pitch (since iop doesn't have floats)
      if (pitch == _pitch)
        return;
      assert(_pitch >= 0.0f);
      if (check_internal_instance())
      {
        pitch = _pitch;
        int iop_pitch = (int)((_pitch) * 1000);
    proftimer_adv_sound_gas.start();
        gas_rpc (GAS_RPC_INSTANCE_PITCH, NULL, iop_instance_id, iop_pitch, 0, 0 );
    proftimer_adv_sound_gas.stop();
      }
    }
  }*/
}


/*** set_ranges ***/
void sound_instance::set_ranges( rational_t min, rational_t max)
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    if (killed == false)
    {
      min_dist = min;
      max_dist = max;

      if(min_dist < 0.0f)
        min_dist = 0.0f;

      if(max_dist < min_dist)
      {
        assert(max_dist < min_dist);
        max_dist = min_dist;
      }
    }
  }*/
}


void sound_instance::dampen( void )
{
/*
	if ( !os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
	{
		if ( iop_instance_id >= 0 )
		{
			gas_rpc( GAS_RPC_DAMPEN_INSTANCE, NULL, iop_instance_id, sound_device::inst()->get_dampen_value(), 0, 0 );
		}
	}*/
}

void sound_instance::dampen_guard( void )
{/*
	if ( !os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
	{
		if ( iop_instance_id >= 0 )
		{
			gas_rpc( GAS_RPC_DAMPEN_GUARD, NULL, iop_instance_id, sound_device::inst()->get_dampen_value(), 0, 0);
		}
	}*/
}

void sound_instance::undampen( void )
{/*
	if ( !os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
	{
		if ( iop_instance_id >= 0 )
		{
			gas_rpc( GAS_RPC_UNDAMPEN_INSTANCE, NULL, iop_instance_id, sound_device::inst()->get_dampen_value(), 0, 0 );
		}
	}*/
}
 
/*-------------------------------------------------------------------------------------------------------

  sound_emitter implementation

-------------------------------------------------------------------------------------------------------*/
/*** constructor ***/
sound_emitter::sound_emitter()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    initialize();
  }*/
}

sound_emitter::~sound_emitter()
{
}


void sound_emitter::initialize()
{/*
  sounds_playing = 0;
	position = ZEROVEC;
	velocity = ZEROVEC;
  used = false;
  waiting_for_action = true;
  automatic = false;
auto_count = -1;*/
}

const pstring SILENT("SILENT");

/*** release ***/
void sound_emitter::release()
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    used = false;
    waiting_for_action = false;

    sound_device::inst()->release_emitter(this);
  }*/
}

#define ONE_WITH_ERROR           0.999f
#define NEGATIVE_ONE_WITH_ERROR -0.999f

/*** update_sounds ***/
void sound_emitter::update_sounds()
{/*
  sounds_playing = 0;

  vector3d to_sound = position - sound_device::inst()->listener_position;
  vector3d up_vect = sound_device::inst()->listener_orientation_top;
  vector3d fwd_vect = sound_device::inst()->listener_orientation_front;
  vector3d new_up;
  
  rational_t dist = to_sound.length(); // use the un-normalized to_sound vector!
  if(dist <= 0.0001f)
    dist = 0.0001f;
  to_sound *= (1.0f / dist);

  rational_t cos_theta = dot(to_sound, fwd_vect);
  rational_t angle;

	rational_t max_dist, min_dist;
	rational_t volume;
  
  // compute 3d positioning stuff
  if (cos_theta >= ONE_WITH_ERROR)
  {
    // directly in front of us
    angle = 0.0f;
  }
  else if (cos_theta <= NEGATIVE_ONE_WITH_ERROR)
  {
    // directly behind us
    angle = M_PI;
  }
  else
  {
    // compute the 3d angle (0 is front, cw around (ie PI/2 is right 3PI/2 is left))
    new_up = cross(to_sound, fwd_vect);

    rational_t cos_phi = dot(up_vect, new_up);

    angle = fast_acos(cos_theta);
    if (cos_phi >= 0.0f)
    {
      // invert the angle, we are upside-down
      angle = -angle;
    }
//nglPrintf("Angle (cos %g) %g/%g", cos_theta, angle, angle * 180.0f / M_PI);

    if (angle < 0.0f)
      angle += (M_PI*2.0f);
  }
//nglPrintf("\t%g\n", ((angle * 180.0f) / M_PI));

  if (sound_list.empty() && waiting_for_action == false)
  {
    used = false;
    return;
  }

  list<sound_instance *>::iterator it = sound_list.begin();
  for ( ; it != sound_list.end(); ++it )
  {
    if ((*it)->killed == true)
      continue;

	  min_dist = (*it)->min_dist;
	  max_dist = (*it)->max_dist;
    (*it)->set_angle(angle);

  	if ( dist < min_dist )
  		volume = 1.0f;
  	else
  	if ( dist > max_dist )
  		volume = 0.0f;
  	else
		{
			if ( __fabs( max_dist - min_dist ) > 0.001f )
  			volume = ( 1.0f - ( ( dist - min_dist ) * ( 1.0f / ( max_dist - min_dist ) ) ) );
			else
				volume = 1.0f;
		}

    if (volume >= MINIMUM_VOLUME)
    {
      if ((*it)->check_internal_instance())
      {
        // has an iop instance, check if it is playing
        if ((*it)->is_playing() == false)
        {
          if ((*it)->loop)
          {
            // set the new volume 
            (*it)->positional_volume = volume;
            (*it)->set_volume( (*it)->volume );

            // restart it
#ifdef DEBUG_AUDIO
            nglPrintf("RESTARTING instance 0x%x, %s, to %g/%g loop %d\n", (*it)->iop_instance_id, (*it)->data->file_name.c_str(), volume, (*it)->volume, (*it)->loop);
#endif
            (*it)->play((*it)->loop);
          }
          else if ((*it)->playback_begun)
          {
#ifdef DEBUG_AUDIO
            nglPrintf("RELEASING(1) instance 0x%x, %s loop %d\n", (*it)->iop_instance_id, (*it)->data->file_name.c_str(), (*it)->loop);
#endif
            // release it, since the playback is finished
            (*it)->release();
          }
        }
        else
        {
          // just set the new volume
          (*it)->positional_volume = volume;
          (*it)->set_volume( (*it)->volume );
#ifdef DEBUG_AUDIO
if ((*it)->data->file_name != SILENT)
  nglPrintf("UPDATING(1) instance 0x%x, %s, to %g/%g playing %d loop %d\n", (*it)->iop_instance_id, (*it)->data->file_name.c_str(), volume, (*it)->volume, (*it)->is_playing(), (*it)->loop);
#endif
        }
      }
      else
      {
        // just set the volume to the new amount
        (*it)->positional_volume = volume;
        (*it)->set_volume( (*it)->volume );
#ifdef DEBUG_AUDIO
if ((*it)->data->file_name != SILENT)
  nglPrintf("UPDATING(2) instance 0x%x, %s, to %g/%g playing %d loop %d\n", (*it)->iop_instance_id, (*it)->data->file_name.c_str(), volume, (*it)->volume, (*it)->is_playing(), (*it)->loop);
#endif
      }
    }
    else
    {
      if ((*it)->iop_instance_id >= 0)
      {
        if ((*it)->loop)
        {
          // too quiet, shut it down (For now)
          (*it)->positional_volume = volume;
          (*it)->set_volume( (*it)->volume );
#ifdef DEBUG_AUDIO
          nglPrintf("STOPPING instance 0x%x, %s, to %g/%g playing %d loop %d\n", (*it)->iop_instance_id, (*it)->data->file_name.c_str(), volume, (*it)->volume, (*it)->is_playing(), (*it)->loop);
#endif
      proftimer_adv_sound_gas.start();
      proftimer_adv_sound_stop.start();
          gas_rpc (GAS_RPC_STOP_INSTANCE, NULL, (*it)->iop_instance_id, 0, 0, 0 );
      proftimer_adv_sound_stop.stop();
      proftimer_adv_sound_gas.stop();

          (*it)->iop_instance_id = -1;
        }
        else
        {
          if ((*it)->is_playing() == true)
          {
            // just set the volume to ultra-quiet
            (*it)->positional_volume = volume;
            (*it)->set_volume( (*it)->volume );
#ifdef DEBUG_AUDIO
if ((*it)->data->file_name != SILENT)
  nglPrintf("UPDATING(3) instance 0x%x, %s, to %g/%g playing %d loop %d\n", (*it)->iop_instance_id, (*it)->data->file_name.c_str(), volume, (*it)->volume, (*it)->is_playing(), (*it)->loop);
#endif
          }
          else if ((*it)->playback_begun)
          {
#ifdef DEBUG_AUDIO
            nglPrintf("RELEASING(2) instance 0x%x, %s loop %d\n", (*it)->iop_instance_id, (*it)->data->file_name.c_str(), (*it)->loop);
#endif
            (*it)->release();
          }
        }
      }
      else
      {
        // set the volume
        (*it)->positional_volume = volume;
        (*it)->set_volume( (*it)->volume );
#ifdef DEBUG_AUDIO
if ((*it)->data->file_name != SILENT)
  nglPrintf("UPDATING(4) instance 0x%x, %s, to %g/%g playing %d loop %d\n", (*it)->iop_instance_id, (*it)->data->file_name.c_str(), volume, (*it)->volume, (*it)->is_playing(), (*it)->loop);
#endif
      }
    }
	}*/
}

void sound_emitter::set_position( const vector3d & p )
{ /*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    position = p; 
    update_sounds();
  }
  */
}


/*** play_sound ***/
unsigned int sound_emitter::play_sound( sound_id_t sound_id, rational_t vol, rational_t pitch, rational_t min, rational_t max )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    // Create a new instance of the sound through the sound_device.
    sound_instance * instance = sound_device::inst()->create_sound( sound_id );
    instance->last_positional_volume = CRAZY_VOLUME_VALUE;
  //	instance->set_volume( vol );
  //	instance->set_pitch( pitch );
  #ifdef DEBUG_AUDIO
  nglPrintf("Emitter play sound %s\n", instance->data->file_name.c_str());
  #endif

    if(min < 0.0f)
      min = instance->data->min_dist;
    if(max < 0.0f)
      max = instance->data->max_dist;

    instance->set_ranges(min, max);
  
    // Link the new instance to this emitter.
    instance->emitter = this;

    // Add the sound to the emitter's list.
    sound_list.push_back(instance);

    used = true;
    waiting_for_action = false;

    update_sounds();	// set the initial positional volume.
    instance->play();

    return(instance->get_id());
  }
  else
    return 0;*/
  return 1;
}

unsigned int sound_emitter::play_sound( const stringx& name, rational_t vol, rational_t pitch, rational_t min, rational_t max )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    filespec sp(name);
    pstring snd( sp.name );
	  return(play_sound( sound_device::inst()->get_sound_id( snd ), vol, pitch, min, max ));
  }
  else
    return 0;
    */
  return 1;
}

unsigned int sound_emitter::play_sound( const pstring& name, rational_t vol, rational_t pitch, rational_t min, rational_t max )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  	return(play_sound( sound_device::inst()->get_sound_id( name ), vol, pitch, min, max ));
  else
    return 0;*/
  return 1;
}


/*** kill_sound ***/
bool sound_emitter::kill_sound(unsigned int id)
{
  /*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    list<sound_instance *>::iterator curr = sound_list.begin();
    while (curr != sound_list.end())
    {
      if ((*curr)->get_id() == id)
      {
        (*curr)->release();
        return(true);
      }
      ++curr;
    }
  }
  return(false);
  */ 
  return true;
}


/*** sound_playing ***/
bool sound_emitter::sound_playing(unsigned int id)
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    list<sound_instance *>::iterator curr = sound_list.begin();
    while (curr != sound_list.end())
    {
      if ((*curr)->get_id() == id)
      {
        return (*curr)->is_playing();
      }
      ++curr;
    }
  }
  return(false);
  */ 
  return true;
}


/*** create_sound ***/
sound_instance* sound_emitter::create_sound( sound_id_t sound_id )
{/*
  if (!os_developer_options::inst()->is_flagged(os_developer_options::FLAG_NO_AUDIO))
  {
    // Create a new instance of the sound.
    sound_instance* instance = sound_device::inst()->create_sound( sound_id );
    used = true;
    waiting_for_action = false;

    // Link the new instance to this emitter.
    instance->emitter = this;

    // Add the sound to the emitter's list.
    sound_list.push_back(instance);

	  update_sounds();	// set the initial positional volume.
    return instance;
  }
  else
    return &sound_device::inst()->dummy_instance;
    */
  return NULL;
}

sound_instance* sound_emitter::create_sound( const stringx& name )
{/*
  filespec sp(name);
  pstring snd( sp.name );
	return create_sound( sound_device::inst()->get_sound_id( snd ) );
  */
  return NULL;
}

sound_instance* sound_emitter::create_sound( const pstring& name )
{
	//return create_sound( sound_device::inst()->get_sound_id( name ) );
  return NULL;
}


float RIGHT_PAN_RATIO_TABLE[360] = {
	0.5000000000f, 0.5087261796f, 0.5174497366f, 0.5261679888f, 0.5348782539f, 0.5435778499f, 
	0.5522642136f, 0.5609346628f, 0.5695865750f, 0.5782172084f, 0.5868241191f, 0.5954045057f, 
	0.6039558649f, 0.6124755144f, 0.6209609509f, 0.6294095516f, 0.6378186941f, 0.6461858749f, 
	0.6545085311f, 0.6627840996f, 0.6710100770f, 0.6791839600f, 0.6873033047f, 0.6953655481f, 
	0.7033683062f, 0.7113091350f, 0.7191855907f, 0.7269952297f, 0.7347357869f, 0.7424048185f, 
	0.7500000000f, 0.7575190663f, 0.7649596334f, 0.7723194957f, 0.7795964479f, 0.7867882252f, 
	0.7938926220f, 0.8009075522f, 0.8078307509f, 0.8146601915f, 0.8213938475f, 0.8280295134f, 
	0.8345652819f, 0.8409991860f, 0.8473291993f, 0.8535534143f, 0.8596699238f, 0.8656768799f, 
	0.8715724349f, 0.8773548007f, 0.8830222487f, 0.8885729909f, 0.8940054178f, 0.8993177414f, 
	0.9045085311f, 0.9095760584f, 0.9145187736f, 0.9193353057f, 0.9240240455f, 0.9285836816f, 
	0.9330127239f, 0.9373098612f, 0.9414738417f, 0.9455032349f, 0.9493970275f, 0.9531539083f, 
	0.9567727447f, 0.9602524638f, 0.9635919333f, 0.9667902589f, 0.9698463082f, 0.9727593064f, 
	0.9755282402f, 0.9781523943f, 0.9806308746f, 0.9829629064f, 0.9851478338f, 0.9871850610f, 
	0.9890738130f, 0.9908136129f, 0.9924038649f, 0.9938441515f, 0.9951340556f, 0.9962731004f, 
	0.9972609282f, 0.9980973601f, 0.9987820387f, 0.9993147850f, 0.9996954203f, 0.9999238253f, 
	1.0000000000f, 0.9944444299f, 0.9888888597f, 0.9833333492f, 0.9777777791f, 0.9722222090f, 
	0.9666666389f, 0.9611111283f, 0.9555555582f, 0.9499999881f, 0.9444444180f, 0.9388889074f, 
	0.9333333373f, 0.9277777672f, 0.9222221971f, 0.9166666865f, 0.9111111164f, 0.9055555463f, 
	0.8999999762f, 0.8944444656f, 0.8888888955f, 0.8833333254f, 0.8777777553f, 0.8722222447f, 
	0.8666666746f, 0.8611111045f, 0.8555555344f, 0.8500000238f, 0.8444444537f, 0.8388888836f, 
	0.8333333135f, 0.8277778029f, 0.8222222328f, 0.8166666627f, 0.8111110926f, 0.8055555820f, 
	0.8000000119f, 0.7944444418f, 0.7888888717f, 0.7833333611f, 0.7777777910f, 0.7722222209f, 
	0.7666666508f, 0.7611111403f, 0.7555555701f, 0.7500000000f, 0.7444444299f, 0.7388888597f, 
	0.7333333492f, 0.7277777791f, 0.7222222090f, 0.7166666389f, 0.7111111283f, 0.7055555582f, 
	0.6999999881f, 0.6944444180f, 0.6888889074f, 0.6833333373f, 0.6777777672f, 0.6722221971f, 
	0.6666666865f, 0.6611111164f, 0.6555555463f, 0.6499999762f, 0.6444444656f, 0.6388888955f, 
	0.6333333254f, 0.6277777553f, 0.6222222447f, 0.6166666746f, 0.6111111045f, 0.6055555344f, 
	0.6000000238f, 0.5944444537f, 0.5888888836f, 0.5833333135f, 0.5777778029f, 0.5722222328f, 
	0.5666666627f, 0.5611110926f, 0.5555555820f, 0.5500000119f, 0.5444444418f, 0.5388888717f, 
	0.5333333611f, 0.5277777910f, 0.5222222209f, 0.5166666508f, 0.5111111403f, 0.5055555701f, 
	0.5000000000f, 0.4944444299f, 0.4888888896f, 0.4833333194f, 0.4777777791f, 0.4722222090f, 
	0.4666666687f, 0.4611110985f, 0.4555555582f, 0.4499999881f, 0.4444444478f, 0.4388888776f, 
	0.4333333373f, 0.4277777672f, 0.4222222269f, 0.4166666567f, 0.4111111164f, 0.4055555463f, 
	0.4000000060f, 0.3944444358f, 0.3888888955f, 0.3833333254f, 0.3777777851f, 0.3722222149f, 
	0.3666666746f, 0.3611111045f, 0.3555555642f, 0.3499999940f, 0.3444444537f, 0.3388888836f, 
	0.3333333433f, 0.3277777731f, 0.3222222328f, 0.3166666627f, 0.3111111224f, 0.3055555522f, 
	0.3000000119f, 0.2944444418f, 0.2888889015f, 0.2833333313f, 0.2777777910f, 0.2722222209f, 
	0.2666666806f, 0.2611111104f, 0.2555555701f, 0.2500000000f, 0.2444444448f, 0.2388888896f, 
	0.2333333343f, 0.2277777791f, 0.2222222239f, 0.2166666687f, 0.2111111134f, 0.2055555582f, 
	0.2000000030f, 0.1944444478f, 0.1888888925f, 0.1833333373f, 0.1777777821f, 0.1722222269f, 
	0.1666666716f, 0.1611111164f, 0.1555555612f, 0.1500000060f, 0.1444444507f, 0.1388888955f, 
	0.1333333403f, 0.1277777851f, 0.1222222224f, 0.1166666672f, 0.1111111119f, 0.1055555567f, 
	0.1000000015f, 0.0944444463f, 0.0888888910f, 0.0833333358f, 0.0777777806f, 0.0722222254f, 
	0.0666666701f, 0.0611111112f, 0.0555555560f, 0.0500000007f, 0.0444444455f, 0.0388888903f, 
	0.0333333351f, 0.0277777780f, 0.0222222228f, 0.0166666675f, 0.0111111114f, 0.0055555557f, 
	0.0000000000f, 0.0000761517f, 0.0003045916f, 0.0006852376f, 0.0012179781f, 0.0019026507f, 
	0.0027390718f, 0.0037269408f, 0.0048659779f, 0.0061558355f, 0.0075961216f, 0.0091864420f, 
	0.0109262262f, 0.0128149856f, 0.0148521438f, 0.0170370825f, 0.0193691980f, 0.0218476560f, 
	0.0244717635f, 0.0272407196f, 0.0301536806f, 0.0332098454f, 0.0364081152f, 0.0397475958f, 
	0.0432272777f, 0.0468460917f, 0.0506030433f, 0.0544967838f, 0.0585262291f, 0.0626901463f, 
	0.0669872761f, 0.0714164227f, 0.0759759992f, 0.0806647390f, 0.0854812115f, 0.0904240832f, 
	0.0954915807f, 0.1006822959f, 0.1059946418f, 0.1114270091f, 0.1169778928f, 0.1226452887f, 
	0.1284276396f, 0.1343231648f, 0.1403300911f, 0.1464467198f, 0.1526709050f, 0.1590008736f, 
	0.1654347032f, 0.1719704568f, 0.1786063164f, 0.1853398830f, 0.1921693087f, 0.1990924925f, 
	0.2061075270f, 0.2132119089f, 0.2204036266f, 0.2276805192f, 0.2350403666f, 0.2424811274f, 
	0.2500001192f, 0.2575952709f, 0.2652642429f, 0.2730047405f, 0.2808145881f, 0.2886909842f, 
	0.2966317534f, 0.3046344519f, 0.3126966953f, 0.3208161891f, 0.3289900422f, 0.3372159898f, 
	0.3454915285f, 0.3538141251f, 0.3621814847f, 0.3705905974f, 0.3790391088f, 0.3875244856f, 
	0.3960441351f, 0.4045956433f, 0.4131760001f, 0.4217828214f, 0.4304134548f, 0.4390655160f, 
	0.4477359056f, 0.4564222097f, 0.4651218057f, 0.4738320112f, 0.4825504422f, 0.4912739396f };


short old_left = 255 / 2;
short old_right = 255 / 2;

// if a sound is less than this, then it 
#define NEGATIVE_THRESHHOLD -128

void set_3d_volume(int inst, int angle, unsigned short vol, short *volume_left, short *volume_right)
{

  STUB( "set_3d_volume" ); 
}
