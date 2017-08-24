double START;
static Eina_Bool sh_log = EINA_TRUE;
static Eina_Bool sh_log_debug = EINA_TRUE;
static Eina_Bool sh_log_info = EINA_FALSE;//EINA_TRUE;
#define _CR_ "%c[%d;%d;%dm"
#define _DEF_ "%c[%dm"
// TEXT MODE
#define DEF 0x1B, 0
#define CRDEF(COLOR, BACKGROUND) 0x1B, 0, COLOR, BACKGROUND
#define CRBLD(COLOR, BACKGROUND) 0x1B, 1, COLOR, BACKGROUND
// TEXT COLOR
#define CRBLK 30
#define CRRED 31
#define CRGRN 32
#define CRYLW 33
#define CRBLU 34
#define CRMGT 35
#define CRCYN 36
#define CRWHT 37
// BACKGROUND COLOR
#define BGBLK 40
#define BGRED 41
#define BGGRN 42
#define BGYLW 43
#define BGBLU 44
#define BGMGT 45
#define BGCYN 46
#define BGWHT 47

#define CRCRI CRDEF(CRWHT, BGRED)
#define CRDBG CRDEF(CRBLK, BGGRN)
#define CRINF CRDEF(CRWHT, BGBLK)

#define _PRINT(_MSG_, ...) fprintf(stderr, _MSG_, ## __VA_ARGS__)

#define SHPRT_TIME _PRINT(_CR_"[%.4f]"_DEF_"::", CRBLD(CRBLK, BGYLW), ecore_time_get() - START, DEF);
#define SHPRT_LOGO(_LOG_, _LOGCR_) _PRINT(_CR_ _LOG_ _DEF_"::", _LOGCR_, DEF);
#define SHPRT_FUNC _PRINT(_CR_"[%s()"_CR_":%d"_CR_"]"_DEF_"::", CRDEF(CRBLU, BGWHT), __FUNCTION__, CRDEF(CRRED, BGWHT), __LINE__, CRDEF(CRBLU, BGWHT), DEF);
#define SHPRT(_MSG_, _MSGCR_, ...) _PRINT(_CR_ _MSG_, _MSGCR_, ## __VA_ARGS__);
#define SHPRT_ENDL _PRINT(_DEF_"\n", DEF); fflush(stderr);

#define SH_IFLOG if (sh_log)
#define SH_IFDBG if (sh_log && sh_log_debug)
#define SH_IFINF if (sh_log && sh_log_info)

#define SHCRI(MSG, ...)  SH_IFLOG { \
                           SHPRT_TIME \
                           SHPRT_LOGO("[SH-CRITICAL]", CRBLD(CRRED, BGBLK)) \
                           SHPRT_FUNC \
                           SHPRT(MSG, CRCRI, ## __VA_ARGS__) \
                           SHPRT_ENDL}
#define SHDBG(MSG, ...)  SH_IFDBG { \
                           SHPRT_TIME \
                           SHPRT_LOGO("[  SH-DEBUG ]", CRBLD(CRYLW, BGMGT)) \
                           SHPRT_FUNC \
                           SHPRT(MSG, CRDBG, ## __VA_ARGS__) \
                           SHPRT_ENDL}
#define SHINF(MSG, ...)  SH_IFINF { \
                           SHPRT_TIME \
                           SHPRT_LOGO("[  SH-INFO  ]", CRBLD(CRGRN, BGCYN)) \
                           SHPRT_FUNC \
                           SHPRT(MSG, CRINF, ## __VA_ARGS__) \
                           SHPRT_ENDL}

#define SHNAME(_OBJ) efl_class_name_get(efl_class_get(_OBJ))
