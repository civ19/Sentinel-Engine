package com.sentinel.api;

import org.springframework.core.io.Resource;

public record OTAWrapper(Resource resource, String sha_hash) {
}
