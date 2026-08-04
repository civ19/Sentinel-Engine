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

    public OTAWrapper getBinary(String ver) throws Exception {
        //get path based on ver
        Path path = Path.of(BIN_DIR + "sentinel_v" + ver + ".bin"); //path
        File bin_f = path.toFile(); //file obj from path. actual binary

        if(!bin_f.exists()) throw new FileNotFoundException("Firmware binary not found.");

        String hash = calc_sha256(path);

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

    public UpdateCheckResponse checkForUpdate(String ver) throws Exception {
        Path binPath = Path.of(BIN_DIR);

        try(Stream<Path> stream = Files.list(binPath)) {
            Path latestFilePath = stream.filter(Files::isRegularFile)
                    .filter(path -> path.toString().endsWith(".bin"))
                    .max(java.util.Comparator.comparingLong(path -> path.toFile().lastModified()))
                    .orElseThrow(() -> new FileNotFoundException("Firmware binaries not found."));

            String fileName = latestFilePath.getFileName().toString();
            String latestVer = fileName.replace("sentinel_v", "").replace(".bin", ".");
            boolean updateAvail = !ver.equals(latestVer);
            long fileSize = Files.size(latestFilePath);
            String sha_hash = calc_sha256(latestFilePath);

            return new UpdateCheckResponse(updateAvail, latestVer, fileSize, sha_hash);
        }


    }





}