package com.sentinel.api;

import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/api/v1/ota")
public class OTAController {

    private final String BIN_DIR = "C:/Summer_2026/Embedded/ESP32/Projects/Sentinel-OS/firmware_repo/Sentinel-OS.bin";


}