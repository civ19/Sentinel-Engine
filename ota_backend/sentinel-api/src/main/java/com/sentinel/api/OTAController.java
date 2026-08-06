package com.sentinel.api;

import lombok.RequiredArgsConstructor;
import org.springframework.core.io.Resource;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

@RestController
@RequiredArgsConstructor
@RequestMapping("/api/v1/ota")
public class OTAController {

    private final OTAService service;

    @GetMapping("/check")
    public ResponseEntity<OTAWrapper> checkUpdate(@RequestParam String ver) throws Exception {

        UpdateCheckResponse resp = service.checkForUpdate(ver);
        return new ResponseEntity<>(resp, HttpStatus.OK);
    }

    @GetMapping("/download/{version}")
    public ResponseEntity<Resource> downloadFirmware(@PathVariable String ver) throws Exception {
        OTAWrapper otaData = service.getBinary(ver);
        return ResponseEntity.ok().contentType(MediaType.APPLICATION_OCTET_STREAM)
                .header("X-Sentinel-Hash", otaData.sha_hash()).body(otaData.resource());
    }



}