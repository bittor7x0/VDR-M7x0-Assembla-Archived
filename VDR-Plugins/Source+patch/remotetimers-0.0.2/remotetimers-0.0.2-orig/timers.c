#include <stdlib.h>
#include "timers.h"
#include "setup.h"
#include "i18n.h"

cRemoteTimers RemoteTimers;

// cRemoteTimer -------------------------------------------------

cRemoteTimer::cRemoteTimer(): id(-1), orig(NULL) {
}

cRemoteTimer::cRemoteTimer(const cEvent* &Event): cTimer(Event), id(-1), orig(NULL) {
}

cRemoteTimer::~cRemoteTimer() {
	free(orig);
}

cRemoteTimer& cRemoteTimer::operator= (const cRemoteTimer &Timer) {
	if (this != &Timer) {
		cTimer::operator=(Timer);
		id = Timer.id;
		orig = Timer.orig ? strdup(Timer.orig) : NULL;
	}
	return *this;
}

bool cRemoteTimer::Parse(const char *s) {
	bool result = false;

	free(orig);
	orig = NULL;
	id = -1;
	if (s) {
		orig = strdup(s);
		id = atoi(s);
		s = strchr(s, ' ');
	}
	if (s && id > 0) {
		result = cTimer::Parse(s);
		// Parse clears the recording flag
		if (atoi(s) & tfRecording)
			SetRecording(true);
	}
	return result;
}

// cRemoteTimers -------------------------------------------------

cRemoteTimers::cRemoteTimers(): refcount(0) {
	plugin = NULL;
	svdrp.handle = -1;
}

cRemoteTimers::~cRemoteTimers() {
	if (svdrp.handle >= 0)
		plugin->Service("SvdrpConnection-v1.0", &svdrp);
}

bool cRemoteTimers::Connect() {
	refcount++;
	if (!plugin)
		plugin = cPluginManager::GetPlugin("svdrpservice");
	if (!plugin)
		esyslog("remotetimers: Plugin svdrpservice not available. Aborting.");
	else if (svdrp.handle < 0) {
		svdrp.serverIp = RemoteTimersSetup.serverIp;
		svdrp.serverPort = RemoteTimersSetup.serverPort;
		svdrp.shared = true;
		plugin->Service("SvdrpConnection-v1.0", &svdrp);
	}
	return svdrp.handle >= 0;
}

void cRemoteTimers::Disconnect() {
	refcount--;
	if (!refcount && svdrp.handle >= 0) {
		plugin->Service("SvdrpConnection-v1.0", &svdrp);
		svdrp.handle = -1;
	}
}

cString cRemoteTimers::GetErrorMessage(eRemoteTimersState state) {
	switch (state) {
		case rtsRefresh:
			return cString(trNOOP("Updated remote timers list"));
		case rtsLocked:
			return cString(trNOOP("Timers are being edited remotely - please try again"));
		case rtsRecording:
			return cString(trNOOP("Timer is recording - please deactivate it first"));
		case rtsRejected:
			return cString(trNOOP("Timer already defined"));
		case rtsUpdated:
			return cString(trNOOP("Timers modified remotely - please check remote timers"));
		case rtsUnexpected:
			return cString(trNOOP("Unexpected error - please check remote timers"));
		case rtsConnError:
			return cString(trNOOP("Lost SVDRP connection - please check remote timers"));
		default:
			return cString();
	}
}

unsigned int cRemoteTimers::GetFlags(const char *TimerString, const char*& Tail) {
	int count;
	unsigned int flags;
	Tail = TimerString;
	if (sscanf(TimerString, " %*u %u :%n", &flags, &count) > 0)
		Tail += count;
	// mask recording flag
	return flags & ~tfRecording;
}

eRemoteTimersState cRemoteTimers::Refresh() {
	SvdrpCommand_v1_0 cmd;
	eRemoteTimersState state;

	cmd.command = "LSTT id\r\n";
	cmd.handle = svdrp.handle;
	plugin->Service("SvdrpCommand-v1.0", &cmd);

	Clear();
	if (cmd.responseCode == 250) {
		for (cLine *line = cmd.reply.First(); line; line = cmd.reply.Next(line)) {
			cRemoteTimer* timer = new cRemoteTimer();
			if (timer->Parse(line->Text()))
				Add(timer);
			else {
				esyslog("remotetimers: Error parsing timer %s", line->Text());
				delete timer;
			}
		}
		state = rtsOk;
	}
	else if (cmd.responseCode == 550)
		state = rtsOk;	//no timers defined
	else if (cmd.responseCode < 100)
		state = rtsConnError;
	else
		state = rtsUnexpected;
	return state;
}

eRemoteTimersState cRemoteTimers::New(cRemoteTimer *Timer) {
	char *s;
	int num;
	unsigned short result;
	eRemoteTimersState state;

	result = CmdNEWT(Timer, num);

	if (result == 250 && num > 0) {
		state = List(num, s);
		if (state == rtsOk && s) {
			if (Timer->Parse(s)) {
				Add(Timer);
				if (num != Count())
					state = rtsRefresh;
			}
			else
				state = rtsUnexpected;
		}
		free(s);
	}
	else if (result < 100)
		state = rtsConnError;
	else if (result == 550)
		state = rtsRejected;
	else
		state = rtsUnexpected;
	return state;
}

eRemoteTimersState cRemoteTimers::Delete(cRemoteTimer *Timer) {
	unsigned short result;
	eRemoteTimersState state;
	
	state = Verify(Timer);
	if (state == rtsOk) {
		result = CmdDELT(Timer);
		if (result == 250) {
			Del(Timer);
			// DELT changes timer numbers
			state = rtsRefresh;
		}
		else if (result < 100)
			state = rtsConnError;
		else if (result == 550)
			state = Timer->Recording() ? rtsRecording : rtsLocked;
		else
			state = rtsUnexpected;
	}
	return state;
}

eRemoteTimersState cRemoteTimers::Modify(cRemoteTimer *Timer) {
	unsigned short result;
	eRemoteTimersState state;
	
	state = Verify(Timer);
	if (state == rtsOk) {
		result = CmdMODT(Timer);
		if (result == 250) {
			char *s = NULL;
			state = List(Timer->Id(), s);
			if (state == rtsOk && s) {
				if (!Timer->Parse(s))
					result = rtsUnexpected;
			}
			free(s);
		}
		else if (result < 100)
			state = rtsConnError;
		else if (result == 550)
			state = rtsLocked;
		else
			state = rtsUnexpected;
	}
	return state;
}

eRemoteTimersState cRemoteTimers::Verify(cRemoteTimer *Timer) {
	unsigned short result;
	eRemoteTimersState state;
	
	char *s = NULL;
	result = CmdLSTT(Timer->Id(), s);
	if (result == 250) {
		if (s) {
			const char *orig, *curr;
			// masks the recording flag
			unsigned int origFlags = GetFlags(Timer->Orig(), orig);
			unsigned int currFlags = GetFlags(s, curr);
			// compare flags and the rest
			if (origFlags == currFlags && strcmp(orig, curr) == 0)
				state = rtsOk;
			else
				state = rtsUpdated;
		}
		else
			state = rtsUnexpected;
	}
	else
		state = (result < 100) ? rtsConnError : rtsUpdated;
	free(s);
	return state;
}

eRemoteTimersState cRemoteTimers::List(int Number, char*& TimerString) {
	unsigned short result;
	eRemoteTimersState state;
	
	result = CmdLSTT(Number, TimerString);
	if (result == 250) {
		if (TimerString)
			state = rtsOk;
		else
			state = rtsUnexpected;
	}
	else
		state = (result < 100) ? rtsConnError : rtsUpdated;
	return state;
}

cRemoteTimer *cRemoteTimers::GetTimer(cTimer *Timer)
{
	for (cRemoteTimer *ti = First(); ti; ti = Next(ti)) {
		if (ti->Channel() == Timer->Channel() &&
				(ti->WeekDays() && ti->WeekDays() == Timer->WeekDays() || !ti->WeekDays() && ti->Day() == Timer->Day()) &&
				ti->Start() == Timer->Start() &&
				ti->Stop() == Timer->Stop())
			return ti;
	}
	return NULL;
}

cRemoteTimer *cRemoteTimers::GetMatch(const cEvent *Event, int *Match)
{
	cRemoteTimer *t = NULL;
	int m = tmNone;
	for (cRemoteTimer *ti = First(); ti; ti = Next(ti)) {
		int tm = ti->Matches(Event);
		if (tm > m) {
			t = ti;
			m = tm;
			if (m == tmFull)
				break;
		}
	}
	if (Match)
		*Match = m;
	return t;
}

unsigned short cRemoteTimers::CmdDELT(cRemoteTimer *Timer) {
	SvdrpCommand_v1_0 cmd;

	cmd.command = cString::sprintf("DELT %d\r\n", Timer->Id());
	cmd.handle = svdrp.handle;
	plugin->Service("SvdrpCommand-v1.0", &cmd);
	return cmd.responseCode;
}

unsigned short cRemoteTimers::CmdMODT(cRemoteTimer *Timer) {
	SvdrpCommand_v1_0 cmd;

	cmd.command = cString::sprintf("MODT %d %s", Timer->Id(), *Timer->ToText(true));
	cmd.handle = svdrp.handle;
	plugin->Service("SvdrpCommand-v1.0", &cmd);
	return cmd.responseCode;
}

unsigned short cRemoteTimers::CmdNEWT(cRemoteTimer *Timer, int &Number) {
	SvdrpCommand_v1_0 cmd;

	cmd.command = cString::sprintf("NEWT %s", *Timer->ToText(true));
	cmd.handle = svdrp.handle;
	plugin->Service("SvdrpCommand-v1.0", &cmd);

	Number = (cmd.responseCode == 250) ? atoi(cmd.reply.First()->Text()) : -1;
	return cmd.responseCode;
}

unsigned short cRemoteTimers::CmdLSTT(int Number, char*& s) {
	SvdrpCommand_v1_0 cmd;

	cmd.command = cString::sprintf("LSTT id %d\r\n", Number);
	cmd.handle = svdrp.handle;
	plugin->Service("SvdrpCommand-v1.0", &cmd);

	s = (cmd.responseCode == 250) ? strdup(cmd.reply.First()->Text()) : NULL;
	return cmd.responseCode;
}
