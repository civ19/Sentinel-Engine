#include "cmd_task.h"
#include "esp_console.h"  
#include "linenoise/linenoise.h"
#include "driver/uart.h" 

#include "abstractions/abstractions.h"

void diagnostic_console_task(void* pv) {
    printf("\n --- SENTINEL-OS SAFE MODE ---\n");
    printf("Please type 'help' to see available commands.\n\n");

    for(;;) {
        
        char *line = linenoise("Sentinel> ");

        if(line == NULL) continue;

        int ret_cmd;

        esp_err_t ret = esp_console_run(line, &ret_cmd);

        if(ret == ESP_ERR_NOT_FOUND) printf("Unrecognized command. Type 'help' for more options.");
        else if (ret == ESP_ERR_INVALID_ARG) printf("Empty command line.\n");

        linenoiseFree(line);
    }
}