idf_component_register(
    SRCS "main.c"
         "pwm/reboot.c"
         "wdt/wdt.c"
         "abstractions/abstractions.c"
    INCLUDE_DIRS "."
    REQUIRES 
        esp_pm
        esp_timer
        app_update
        esp_system
        espcoredump
        freertos
        
)