#if defined(NEW_CORE)
    #include <IWatchdog.h>
#else
    #include <libmaple/iwdg.h>


    /* Watchdog setting example
    *
    * |  time  | prescaler | reload |
    * | ------ | --------- | ------ |
    * |   8s   |    256    |  1250  |
    * |   4s   |    256    |  625   |
    * |   2s   |    128    |  625   |
    * |   1s   |    64     |  625   |
    * |  500ms |    32     |  625   |
    * |  250ms |    16     |  625   |
    * |  100ms |    32     |  125   |
    * |   1ms  |     8     |   5    |
    *
    */

    // reload = ((40000 / 1000) * 100) / 32;  (40kHz watchdog clock /divided by 1000ms *100 miliseconds desired timeout / 32 prescaler)

    #define WDT_PRESCALER IWDG_PRE_256  // IWDG_PRE_4, IWDG_PRE_8, IWDG_PRE_16, IWDG_PRE_32, IWDG_PRE_64, IWDG_PRE_128, IWDG_PRE_256
    #define WDT_RELOAD 4062
#endif

void wdtSetup() {
#if defined(NEW_CORE)
    IWatchdog.begin(IWDG_TIMEOUT_MAX);
#else
    iwdg_init(WDT_PRESCALER, WDT_RELOAD);  // init WDT timer
#endif
}

void wdtFeed() {
#if defined(NEW_CORE)
    IWatchdog.reload();
#else
    iwdg_feed();
#endif
}
