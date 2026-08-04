package com.sentinel.api;

import lombok.RequiredArgsConstructor;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequiredArgsConstructor
@RequestMapping("/api/v1/ota")
public class OTAController {

    private final OTAService service;

    @GetMapping("/check")
    public ResponseEntity<UpdateCheckResponse> checkUpdate(@RequestParam String ver) throws Exception {
        UpdateCheckResponse resp = service.checkForUpdate(ver);
        return new ResponseEntity<>(resp, HttpStatus.OK);
    }


}