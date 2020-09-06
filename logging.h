/*
 * VICERA by h34ting4ppliance
 *
 * logging.h
 *
 * Logging bindings
 */
#ifndef LOGGING_H
#define LOGGING_H

void logging_log(const char* comp, const char* msg);
void logging_warn(const char* comp, const char* msg);
void logging_error(const char* comp, const char* msg);

#endif
