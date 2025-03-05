#if defined(ESPS3_1_69) || defined(ESPS3_1_28)
#define ENABLE_APP_QMI8658C
#define ENABLE_APP_ATTITUDE
#endif

#if defined(ELECROW_C3)
#define ENABLE_RTC
#endif

// #define ENABLE_GAME_RACING
#define ENABLE_GAME_SIMON

#define ENABLE_APP_NAVIGATION
#define ENABLE_APP_CONTACTS

#ifdef __cplusplus
extern "C" {
#endif


void helper_setup(void);
void helper_loop(void);


#ifdef __cplusplus
} /* extern "C" */
#endif