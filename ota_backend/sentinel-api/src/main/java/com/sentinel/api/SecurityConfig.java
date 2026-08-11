package com.sentinel.api;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.HttpMethod;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.config.Customizer;
import org.springframework.security.config.annotation.authentication.configuration.AuthenticationConfiguration;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.EnableWebSecurity;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.security.web.SecurityFilterChain;

@Configuration
@EnableWebSecurity
public class SecurityConfig { //basically says whos allowed to do whgat. literally access control/config for security

    @Bean
    public PasswordEncoder passwordEncoder() {
        return new BCryptPasswordEncoder();
    }
    @Bean //firewall rules
    public SecurityFilterChain securityFilterChain(HttpSecurity http) throws Exception {
        return http.csrf(csrf -> csrf.disable()) //cause stateless
                .authorizeHttpRequests(auth -> auth
                        .requestMatchers(HttpMethod.GET,"/api/ota/**" ).permitAll()
                        .requestMatchers(HttpMethod.POST, "/coffees/**").hasRole("ADMIN")
                        .anyRequest().authenticated())
                .oauth2ResourceServer(oath2 -> oath2.jwt(Customizer.withDefaults())).build(); //let spring handle validation
    }

    public AuthenticationManager authManager(AuthenticationConfiguration conf) throws Exception { //checks against db: how were doing it
        return conf.getAuthenticationManager();
    }
}