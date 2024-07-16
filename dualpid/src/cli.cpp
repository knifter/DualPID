#include "cli.h"

#include <Arduino.h>

#include <tools-log.h>
#include "globals.h"
#include "pidloop.h"
#include "FPID.h"

bool process_line(const char* line);
bool process_tokens(const size_t n, const char* tokens[]);

bool cmd_reboot(size_t len, const char* tokens[]);
bool cmd_setpoint(size_t len, const char* tokens[]);

#define CLI_STREAM_DEVICE	Serial
#define CLI_MAX_TOKENS		8
#define REPLY(msg, ...)      CLI_STREAM_DEVICE.printf("> " msg "\n", ##__VA_ARGS__)

typedef bool (*cli_callback_t)(size_t toklen, const char* tokens[]);

typedef struct
{
	const char* cmd;
	const char* help;
	cli_callback_t cb;
} cli_cmddef_t;

cli_cmddef_t commands[] = {
	{"reboot", "Reboots the device", cmd_reboot},
	{"setpoint", "Read (setpoint 1 ?) or set (setpoint 1 = 5) the setpoint", cmd_setpoint},
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

			return cmd->cb(n-1, &(tokens[1]));
		};
		cmd++;
	};
	REPLY("Unknown command: %s", tokens[0]);
	return false;

};

bool cmd_save(size_t n, const char* params[])
{
	if(n != 0)
	{
		REPLY("<save> requires no parameters.", n);
		return false;
	};

	setman.save();

	REPLY("Settings saved.");
	return true;
};

bool cmd_setpoint(size_t n, const char* params[])
{
	if(n != 2)
	{
		REPLY("<setpoint> requires 2 parameters: \"setpoint <int:channel> <float:value>\"");
		return false;
	};

	int channel = atoi(params[0]);
	double setpoint = atof(params[1]);

	if(channel < 1 || channel > NUMBER_OF_CHANNELS)
	{
		REPLY("Invalid channel: %d, use 1..%d", channel, NUMBER_OF_CHANNELS);
		return false;
	};

	PIDLoop* pidloop = pids[channel - 1];
    InputDriver* indrv = pidloop->input_drv();
    PIDLoop::settings_t& pset = pidloop->_settings;
	float sp_min = indrv->setpoint_min();
	float sp_max = indrv->setpoint_max();
	if(setpoint < sp_min || setpoint > sp_max)
	{
		REPLY("Setpoint out of range: min=%f < %f < max=%f", sp_min, setpoint, sp_max);
		return false;
	};

	pset.fpid.setpoint = setpoint;
	
	return true;
};
