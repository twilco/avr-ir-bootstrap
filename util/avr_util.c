#include <stdbool.h>
#include <avr/io.h>

bool icp_listening_for_rising() {
    if(TCCR1B & (1 << ICES1)) {
        return true;
    }
    return false;
}

bool icp_listening_for_falling() {
    return !icp_listening_for_rising();
}

void icp_listen_for_rising() {
    TCCR1B |= (1 << ICES1);
}

void icp_listen_for_falling() {
    TCCR1B ^= (1 << ICES1);
}