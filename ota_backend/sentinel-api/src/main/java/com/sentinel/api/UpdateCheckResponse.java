package com.sentinel.api;

public record UpdateCheckResponse(boolean updateAvail,
                                  String latestVer,
                                  long fileSize,
                                  String sha_hash) {
}
