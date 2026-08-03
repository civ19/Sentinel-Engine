package com.sentinel.api;

import jakarta.annotation.Resource;

import java.io.File;
import java.nio.file.Path;
import java.nio.file.Paths;

public class OTAService {

    private final String BIN_DIR = ".../firmware_binaries/";

    public Resource getBinary(String ver) {
        //get path based on ver
        Path path = Paths.get(BIN_DIR + "sentinel_v" + ver + ".bin"); //path
        File bin_f = path.toFile(); //file obj from path. actual binary


    }

}