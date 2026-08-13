#include "debug.h"
#include "esp_pm.h"
#include "esp_core_dump.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_console.h"  
#include "linenoise/linenoise.h"
#include "driver/uart.h"   
#include "esp_sleep.h" 
#include <stdio.h>
#include <inttypes.h>
#include "esp_vfs_dev.h"

#include "nvs_store/nvs_store.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "safe_cmd.h"



void check_panic_data(esp_core_dump_summary_t *sum) {
    void* sp = esp_cpu_get_sp();
    
    if(sum == NULL) return;

    if(sum != NULL) {
        //if theres a summary then that'll mean core dump availabvle
        //now this is when I check if it was able to get the summary
        if(esp_core_dump_get_summary(sum) == ESP_OK) {
            printf("--- Found a crash report from the previous session! ---\n");
            printf("Died in task: %s\n", sum->exc_task);
            printf("Exception Cause: %d\n", (int)sum->ex_info.exc_cause);
            printf("Exception PC: 0x%08" PRIx32 "\n", sum->exc_pc); // Added 08 for padding

            printf("Fault Address EXCVADDR: 0x%08" PRIx32 "\n", sum->ex_info.exc_vaddr);
            printf("SP: %p\n", (void *)sp); // Cast to void*
            printf("Return Addr LR (A0): 0x%08" PRIx32 "\n", *(sum->ex_info.exc_a));

            
        } 

        else printf("Clean Sys Boot. No crash data detected.");
        
    }

    
}

void esp_wake_reason(void) {

    printf("\n--- WAKE REASON ---\n");

    uint32_t cause = esp_sleep_get_wakeup_causes();

    if(cause == 0) {
        printf("Esp wake by Std Power-On/RST");
        return; 
    }
    if (cause & BIT(ESP_SLEEP_WAKEUP_TIMER)) {
        printf("Esp wake by Wakeup Timer");
    }
    
    if (cause & BIT(ESP_SLEEP_WAKEUP_EXT0)) {
        printf("Esp wake by GPIO or btn.");
    }

    uint32_t masks = BIT(ESP_SLEEP_WAKEUP_TIMER) | BIT(ESP_SLEEP_WAKEUP_EXT0);
    if (cause & ~masks) {
        printf("Other raw active wake reasons mask: 0x%" PRIx32, cause);
    }
}

void esp_rst_reason(void) {
    esp_reset_reason_t rst = esp_reset_reason();

    printf("\n--- RESET REASON ---\n");

    switch(rst) {
        case ESP_RST_POWERON:
            printf("Esp reset by Manual Cold boot! Normal reboot.\n");
        break;
        case ESP_RST_WDT:
            printf("Esp hard reset by General Watchdog.\n");
        break;
        case ESP_RST_TASK_WDT:
            printf("Esp hard reset by Task Watchdog.\n");
        break;
        case ESP_RST_SW:
            printf("Esp reset by calling esp_restart(). Hint: Could be OTA\n");
        break;

        default:
            printf("Reset reason: %d\n.", rst);
    }
}

void init_console() {
    
    esp_log_level_set("*", ESP_LOG_NONE); //silencing the uart

    
    esp_console_repl_t *repl = NULL; //repl config
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "Sentinel> ";
    repl_config.max_history_len = 10;

    
    esp_cmd_conf(); 

    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT(); //uart configuration
    uart_config.channel = CONFIG_ESP_CONSOLE_UART_NUM; //on uart 0


    esp_err_t ret = esp_console_new_repl_uart(&uart_config, &repl_config, &repl); //allocating memory for repl
    
    if (ret != ESP_OK) {
        printf("Failed to initialize REPL: %s\n", esp_err_to_name(ret));
        return;
    }

    
    printf("\n --- SENTINEL-OS SAFE MODE: CMD INTERFACE ---\n");
    printf("Type 'help' for commands.\n\n");
    esp_console_start_repl(repl); //actually, well, running repl
}

void activate_safe_mode() {

    esp_log_level_set("*", ESP_LOG_NONE); 
    printf("\033[2J\033[H"); 

    printf("--- SENTINEL-OS SAFE MODE: MAIN MENU ---\n\n");

    esp_wake_reason();
    printf("\n");

    esp_rst_reason();
    printf("\n");

    init_console();
}

