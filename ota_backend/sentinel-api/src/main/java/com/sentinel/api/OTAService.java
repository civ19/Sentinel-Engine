package com.sentinel.api;

import jakarta.annotation.Resource;
import org.apache.logging.log4j.message.Message;

import java.io.File;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.security.MessageDigest;
import java.util.HexFormat;

import static org.springframework.security.config.http.MatcherType.path;

public class OTAService {

    private final String BIN_DIR = ".../firmware_binaries/";

    public Resource getBinary(String ver) {
        //get path based on ver
        Path path = Paths.get(BIN_DIR + "sentinel_v" + ver + ".bin"); //path
        File bin_f = path.toFile(); //file obj from path. actual binary

        if(!bin_f.exists()) throw new FileNotFoundException("Firmware binary not found.");
    }

    public String calc_sha256(Path path) throws Exception {
        MessageDigest digest = MessageDigest.getInstance("SHA-256");

        //opening an IS, and feeding it to sha
        try(InputStream is = Files.newInputStream(path)) {
            byte[] buf = new byte[8192]; //8kb per chunk
            int bytes_read;
            while((bytes_read = is.read(buf)) != -1) {
                digest.update(buf, 0, bytes_read); //feeding to sha!
            }
        }
        return HexFormat.of().formatHex(digest.digest());

    }



}