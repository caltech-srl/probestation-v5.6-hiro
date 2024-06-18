
#define HISTO 0
#define MAP   1
#define TEMP  2
#define SURF  3
#define WAVE  4
#define CNT   5
#define LEAK  6
#define EXIT  -1

#define TMPDIR "./"

#ifdef HEFT
  #define HIST_GNU ".hist.gnu\" \n"
  #define MAP_GNU  ".map-heft.gnu\" \n"
  #define LEAK_GNU ".leak-heft.gnu\" \n"
  #ifdef CYGWIN
    #define EXE_PH_HIST "./ph_hist-heft.exe "
  #endif
  #ifdef LINUX
    #define EXE_PH_HIST "./ph_hist-heft "
  #endif
  #ifdef OSX
    #define EXE_PH_HIST "./ph_hist-heft "
  #endif
#endif
#ifdef DB
  #define HIST_GNU ".hist.gnu\" \n"
  #define MAP_GNU  ".map-db.gnu\" \n"
  #define LEAK_GNU ".leak-db.gnu\" \n"
  #ifdef CYGWIN
    #define EXE_PH_HIST "./ph_hist-db.exe "
  #endif
  #ifdef LINUX
    #define EXE_PH_HIST "./ph_hist-db "
  #endif
  #ifdef OSX
    #define EXE_PH_HIST "./ph_hist-db "
  #endif
#endif


