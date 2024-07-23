#include "cli.h"

#include <Arduino.h>

#include <tools-log.h>
#include "globals.h"
#include "pidloop.h"
#include "FPID.h"

bool process_line(const char* line);
bool process_tokens(const size_t n, const char* tokens[]);

bool cmd_save(size_t len, const char* tokens[]);
bool cmd_pidx_setpoint(size_t len, const char* tokens[]);

#define CLI_STREAM_DEVICE	Serial
#define CLI_MAX_TOKENS		8
#define REPLY(msg, ...)     CLI_STREAM_DEVICE.printf("> " msg "\n", ##__VA_ARGS__)

typedef bool (*cli_callback_t)(size_t toklen, const char* tokens[]);

typedef struct
{
	const char* cmd;
	const char* help;
	cli_callback_t cb;
} cli_cmddef_t;

cli_cmddef_t commands[] = {
	{"save", "Save settings to NVS", cmd_save},
	{"pid1.setpoint", "Set SP", cmd_pidx_setpoint},
	{"pid2.setpoint", "Set SP", cmd_pidx_setpoint},
	{0, 0, 0},
};

void cli_begin()
{

};

void cli_loop()
{
	static String curline;
	while(true)
	{
		int in = CLI_STREAM_DEVICE.read();
		if(in < 0)
			return;

		char c = (char) in;
		if(c == '\n')
		{
			process_line(curline.c_str());
			curline.clear();
			continue;
		};
		if(c < 0x20 || c > 0x7E) // '<space>' - '~'
		{
			REPLY("Invalid character <0x%x> at %d", c, curline.length());
			
			// Reset
			curline.clear();

			continue;
		};
		
		curline += c;
	};
	return;
};

// void reply(const char* fmt, ...)
// {
// 	va_list args;
// 	va_start (args, fmt);
// 	Serial.printf()
// 	vfprintf (, format, args);
// 	va_end (args);
// };

bool process_line(const char* line_in)
{
	// copy to modifyable string
	size_t ll = strlen(line_in);
	char line[ll];
	strcpy(line, line_in);
	
    // Tokenizer
    char* p = line;
	int32_t n = 0;
    const char* tokens[CLI_MAX_TOKENS];
    while (1) 
	{
        // Find start of token.
        while (*p && isspace((unsigned char)*p))
            p++;
        if (*p == '\0') // EOL?
            break;

		// Mark token begin
		tokens[n++] = p;

		// Max tokens reached?
        if (n >= CLI_MAX_TOKENS) 
		{
			REPLY("Too many tokens.");
			return false;
		};

		// 0-terminate token str
		while (*p && !isspace((unsigned char)*p))
                p++;

		// if this is the original 0, we can stop: end of line
        if(*p == '\0') 
			break;

		// add \0
		*p++ = '\0';
    };

    // If there are no tokens, nothing to do.
    if (n == 0)
	{
		REPLY("No command!"); // print prompt
		return false;
	};

	return process_tokens(n, tokens);
};

bool process_tokens(const size_t n, const char* tokens[])
{
	cli_cmddef_t* cmd = commands;
	while(cmd->cmd)
	{
		if(strcasecmp(cmd->cmd, tokens[0]) == 0)
		{
			// DBG("Run cmd '%s'", cmd->cmd);
			// for(int i = 0; i < n-1; i++)
			// 	DBG("param[%d] = '%s'", i, tokens[i+1]);

			return cmd->cb(n, tokens);
		};
		cmd++;
	};
	REPLY("Unknown command: %s", tokens[0]);
	return false;
};

bool cmd_save(size_t n, const char* tokens[])
{
	if(n > 1)
	{
		REPLY("<save> requires no parameters.");
		return false;
	};

	setman.save();

	REPLY("Settings saved.");
	return true;
};

bool cmd_pidx_setpoint(size_t n, const char* tokens[])
{
	// Determine channel nummer from command
	uint32_t channel;
	if(1 != sscanf(tokens[0], "pid%u.", &channel))
	{
		REPLY("Could not determine channel from '%s'", tokens[0]);
		return false;
	};
	if(channel < 1 || channel > NUMBER_OF_CHANNELS)
	{
		REPLY("Invalid channel: %d, use 1..%d", channel, NUMBER_OF_CHANNELS);
		return false;
	};
	PIDLoop* pidloop = pids[channel - 1];
	InputDriver* indrv = pidloop->input_drv();
	PIDLoop::settings_t& pset = pidloop->_settings;

	// const char* varname = tokens[0] + 5; // everything after "pidx."

	// See if it is more than a 'read'
	if(n == 1)
	{
		REPLY("%f", pset.fpid.setpoint);
		return true;		
	};

	if(n != 3)
	{
		REPLY("Unknown syntax: no parameters or <name> = <value>");
		return false;
	};
	if(strcmp(tokens[1], "=") != 0)
	{
		REPLY("Unknown parameter '%s', expected '='.", tokens[1]);
		return false;
	};

	double value = atof(tokens[2]);

	float min = indrv->setpoint_min();
	float max = indrv->setpoint_max();
	if(value < min || value > max)
	{
		REPLY("%s out of range: min..max=%f .. %f (%f given)", tokens[0], min, max, value);
		return false;
	};

	pset.fpid.setpoint = value;
	REPLY("%f", pset.fpid.setpoint);
	return true;
};
