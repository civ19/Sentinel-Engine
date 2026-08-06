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
    public ResponseEntity<Resource> downloadFirmware(@RequestParam("ver") String curr_ver) throws Exception {
        OTAWrapper otaData = service.getBinaryIfUpdated(curr_ver);

        if(otaData == null) return ResponseEntity.status(HttpStatus.NOT_MODIFIED).build(); //304

        return ResponseEntity.ok().contentType(MediaType.APPLICATION_OCTET_STREAM)
                .header("X-Sentinel-Hash", otaData.sha_hash()).body(otaData.resource());
    }



}