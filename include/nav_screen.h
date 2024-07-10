#pragma once

void set_activity(const char *txt);

void set_heading_magnetic(const char *txt);

void set_target_heading(const char *txt);

void set_sog(const char *txt);

void set_stw(const char *txt);

void set_pilot_state(const char *txt);

void set_media_states(const char *txt);

void set_subscription_states(const char *txt);

void start_nav_screen();

void init_nav_screen(char *_log_prefix);