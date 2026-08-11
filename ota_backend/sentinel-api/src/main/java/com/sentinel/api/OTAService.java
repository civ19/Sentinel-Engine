package com.sentinel.api;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.core.io.Resource;
import org.apache.logging.log4j.message.Message;
import org.springframework.core.io.FileSystemResource;
import org.springframework.stereotype.Service;

import java.io.File;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.security.MessageDigest;
import java.util.HexFormat;
import java.util.stream.Stream;

import static org.springframework.security.config.http.MatcherType.path;

@Service
public class OTAService {

    @Value("${sentinel.firmware.bin-dir}")
    private String BIN_DIR;

    public OTAWrapper getBinaryIfUpdated(String curr_ver) throws Exception {
        //we have to check for updates and scan for newest bin file first
        Path l_path = getLatestFirmwarePath();

        //get path based on ver
        String fileName = l_path.getFileName().toString();

        String baseName = fileName.substring(0, fileName.lastIndexOf('.'));

        String l_ver = baseName.substring(baseName.lastIndexOf('v') + 2); //starts at sentinel_v

        System.out.println("DEBUG: Server thinks latest version is: [" + l_ver + "]");
        System.out.println("DEBUG: ESP32 says it is running: [" + curr_ver + "]");

        if(curr_ver.trim().equals(l_ver.trim())) {
            return null;
        }

        File bin_f = l_path.toFile(); //file obj
        String hash = calc_sha256(l_path);

        Resource resource = new FileSystemResource(bin_f);
        return new OTAWrapper(resource, hash);
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

    private Path getLatestFirmwarePath() throws Exception {
        Path binPath = Path.of(BIN_DIR);

        try(Stream<Path> stream = Files.list(binPath)) {
            return stream.filter(Files::isRegularFile)
                    .filter(path -> path.toString().endsWith(".bin"))
                    .max(java.util.Comparator.comparingLong(path -> path.toFile().lastModified()))
                    .orElseThrow(() -> new FileNotFoundException("Firmware binaries not found."));

        }

    }





}