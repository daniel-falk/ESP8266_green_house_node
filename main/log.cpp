#include "log.h" 

static char log_fatal[10];

void info(char *msg) {
  Serial.print(log_fatal);
  Serial.println(msg);
}

void err(char *msg) {
  Serial.print(log_fatal);
  Serial.print("ERROR: ");
  Serial.println(msg);
}

void fatal(char *msg) {
  Serial.print(log_fatal);
  snprintf(log_fatal, 10, "FATAL -- ");
  Serial.print("FATAL ERROR: ");
  Serial.println(msg);
}
