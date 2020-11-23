#ifndef _TOPIC_INPUT_RC_H
#define _TOPIC_INPUT_RC_H

#include "stdint.h"

typedef struct _topic_input_rc {
	uint16_t rc_values[18];
	uint8_t input_source;
	uint8_t rc_lost;
}topic_input_rc;

#endif /* _TOPIC_INPUT_RC_H */
