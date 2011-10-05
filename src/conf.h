/** @file conf.h
    @brief Defines needed Prototypes to make use of config routines.

  Provied necessars Macros and Structure, to provide Configurtion Definitions and Presets.
*/

#ifndef _CONF_H_
#define _CONF_H_

#define CONF_PRESET_INT(x) .value.i=x
#define CONF_PRESET_STRING(x) .value.s=x
#define CONF_PRESET_FLAG(x) .values.i=x
#define CONF_PRESET_FLOAT(x) .value.f=x
#define CONF_PRESET_DOUBLE(x) .value.d=x
#define CONF_PRESET_NONE(x) .value.s=NULL
#define CONF_ENDTABLE { NULL, CONF_TYPE_INT, {0}, NULL}
#define CONF_NOPRESET 128

typedef struct {
	const char *name;
	enum CONF_TYPE_ID {CONF_TYPE_STRING,CONF_TYPE_INT,CONF_TYPE_FLAG,CONF_TYPE_FLOAT,CONF_TYPE_DOUBLE} type;
	union{
		char *s;
		int i;
		float f;
		double d;
	}value;
	const char *des;
}config_item_t;

#endif /* src/conf.h */
