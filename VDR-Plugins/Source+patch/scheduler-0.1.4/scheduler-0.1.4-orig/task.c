/*
Copyright (C) 2007 Christian Wieninger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

The author can be reached at cwieninger@gmx.de

The project's page is at http://winni.vdr-developer.org/taskman
*/

#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include "task.h"
#include "log.h"
#include "scheduler.h"
#include "taskmanager_thread.h"

using namespace std;

cTasks Tasks;
cTasks TempTasks;

// -- cTask -----------------------------------------------------------------
cTask::cTask(void)
:taskLog(this)
{
  id = -1;
  strcpy(name, "");
  strcpy(cmd, "");
  strcpy(minute, "*");
  strcpy(hour, "*");
  strcpy(dayofmonth, "*");
  strcpy(month, "*");
  strcpy(dayofweek, "*");
  enabled = 1;
  wakeup = 1;
  announceMode = 0;
  lastStart = 0;
  logging = 1;
  killAfterMins = 0;
}

cTask::cTask(const char* Name, const char* Cmd, int AnnounceMode)
:taskLog(this)
{
  id = -1;
  strcpy(name, Name);
  strcpy(cmd, Cmd);
  strcpy(minute, "*");
  strcpy(hour, "*");
  strcpy(dayofmonth, "*");
  strcpy(month, "*");
  strcpy(dayofweek, "*");
  enabled = 1;
  wakeup = 0;
  announceMode = AnnounceMode;
  lastStart = 0;
  logging = 0;
  killAfterMins = 0;
}

bool cTask::Parse(const char *s)
{
  char *line;
  char *pos;
  char *pos_next;
  int parameter = 1;
  int valuelen;
#define MAXVALUELEN (10 * MaxFileName)
  
  char value[MAXVALUELEN];
  
  pos = line = strdup(s);
  pos_next = pos + strlen(pos);
  if (*pos_next == '\n') *pos_next = 0;
  while (*pos) {
    while (*pos == ' ') pos++;
    if (*pos) {
      if (*pos != ':') {
        pos_next = strchr(pos, ':');
        if (!pos_next)
          pos_next = pos + strlen(pos);
        valuelen = pos_next - pos + 1;
        if (valuelen > MAXVALUELEN) 
	  valuelen = MAXVALUELEN;
        strn0cpy(value, pos, valuelen);
        pos = pos_next;
        switch (parameter) {
	case 1:  id = atoi(value);
	  break;
	case 2:  strcpy(name,value);
	  break;
	case 3:  enabled = atoi(value);
	  break;
	case 4:  strcpy(cmd,value);
	  break;
	case 5:  strcpy(minute,value);
	  break;
	case 6:  strcpy(hour,value);
	  break;
	case 7:  strcpy(dayofmonth,value);
	  break;
	case 8:  strcpy(month,value);
	  break;
	case 9:  strcpy(dayofweek,value);
	  break;
	case 10:  wakeup = atoi(value);
	  break;
	case 11: announceMode = atoi(value);
	  break;
	case 12: logging = atoi(value);
	  break;
	case 13: killAfterMins = atoi(value);
	  break;
	default:
	  break;
        } //switch
      }
      parameter++;
    }
    if (*pos) pos++;
  } //while
  
  free(line);
  return (parameter >= 11) ? true : false;
}

const char *cTask::ToText(void)
{
    char* buffer = NULL;
    asprintf(&buffer, "%ld:%s:%d:%s:%s:%s:%s:%s:%s:%d:%d:%d:%d", 
	     id,
	     name, 
	     enabled,
	     cmd,
	     minute,
	     hour,
	     dayofmonth,
	     month,
	     dayofweek,
	     wakeup,
	     announceMode,
	     logging,
	     killAfterMins);
    return buffer;
}

bool cTask::Save(FILE *f)
{
  return fprintf(f, "%s\n", ToText()) > 0;
}

bool cTask::MatchesTime(time_t t)
{
  // normalize to full minutes
  struct tm tm_r; 
  struct tm *time_start = localtime_r(&t, &tm_r);
  time_start->tm_sec = 0; 

  // read specs
  cValueSpec minuteValues(minute, 0, 59);
  cValueSpec hourValues(hour, 0, 23);
  cValueSpec dayofmonthValues(dayofmonth, 1, 31);
  cValueSpec dayofweekValues(dayofweek, 0, 6);
  cValueSpec monthValues(month, 1, 12);

  return minuteValues.Matches(time_start->tm_min) &&
    hourValues.Matches(time_start->tm_hour) &&
    dayofmonthValues.Matches(time_start->tm_mday) &&
    dayofweekValues.Matches(time_start->tm_wday) &&
    monthValues.Matches(time_start->tm_mon+1);
}

time_t cTask::NextExecution(time_t start)
{
  if (start == 0)
    start = time(NULL);

  struct tm tm_r; // normalize to full minutes
  struct tm *time_start = localtime_r(&start, &tm_r);
  time_start->tm_sec = 0; 
  start = mktime(time_start);

  int startYear = time_start->tm_year;

  // read specs
  cValueSpec minuteValues(minute, 0, 59);
  cValueSpec hourValues(hour, 0, 23);
  cValueSpec dayofmonthValues(dayofmonth, 1, 31);
  cValueSpec dayofweekValues(dayofweek, 0, 6);
  cValueSpec monthValues(month, 1, 12);

  do
    {
      if (minuteValues.Matches(time_start->tm_min) &&
	  hourValues.Matches(time_start->tm_hour) &&
	  dayofmonthValues.Matches(time_start->tm_mday) &&
	  dayofweekValues.Matches(time_start->tm_wday) &&
	  monthValues.Matches(time_start->tm_mon+1))
	break;
      else
	{
	  start += 60;
	  time_start = localtime_r(&start, &tm_r);
	  if (time_start->tm_year - startYear > 4) return 0; // calc max 4 years
	}
    }
  while(true);

  return start;
}

bool cTask::Run()
{
  return cTaskManagerThread::StartTask(this);
}

bool cTask::Cancel()
{
  return cTaskManagerThread::CancelTask(this);
}

bool cTask::Active() 
{ 
  return cTaskManagerThread::TaskActive(this); 
}

void cTask::SetCurrentOutput(string Result)
{
  cMutexLock TaskLock(&this->taskLog);
  taskLog.currentOutput = Result;
}

string cTask::CurrentOutput()
{
  cMutexLock TaskLock(&this->taskLog);
  return Active()?taskLog.currentOutput:"";
}

// -- cTasks -----------------------------------------------------------------
cString cTasks::Active()
{
  cMutexLock TasksLock(&Tasks);
  cTask *Task = Tasks.First();
  while (Task) {	
    if (Task->Active())
      return cString::sprintf(tr("'%s' still runs!"), Task->Name());
    Task = Tasks.Next(Task);
  }
  cMutexLock TempTasksLock(&TempTasks);
  Task = TempTasks.First();
  while (Task) {	
    if (Task->Active())
      return cString::sprintf(tr("'%s' still runs!"), Task->Name());
    Task = TempTasks.Next(Task);
  }
  return NULL;
}

time_t cTasks::NextWakeupTime()
{
  time_t NextWakeupTime = 0;
  cTask *NextWakeupTask = NULL;

  cMutexLock TasksLock(&Tasks);
  cTask *Task = Tasks.First();
  while (Task) {	
    if (Task->Enabled() && Task->Wakeup()) {
      time_t testTime = Task->NextExecution(time(NULL)+60);
      if (!NextWakeupTime) 
	{
	  NextWakeupTime = testTime;
	  NextWakeupTask = Task;
	}
      else if (NextWakeupTime > testTime)
	{	
	  NextWakeupTime = testTime;
	  NextWakeupTask = Task;
	}
    }
    Task = Tasks.Next(Task);
  }

  if (NextWakeupTask)
    LogFile.Log(2,"next wakeup for '%s' at %s - %s", 
		NextWakeupTask->Name(), 
		*DateString(NextWakeupTime), 
		*TimeString(NextWakeupTime));
  
  return NextWakeupTime;
}

cTask* cTasks::TaskFromId(long Id)
{
  cMutexLock TasksLock(&Tasks);
  cTask *Task = Tasks.First();
  while (Task) {	
    if (Task->Id() == Id)
      return Task;
    Task = Tasks.Next(Task);
  }

  cMutexLock TempTasksLock(&TempTasks);
  Task = TempTasks.First();
  while (Task) {	
    if (Task->Id() == Id)
      return Task;
    Task = TempTasks.Next(Task);
  }
  return NULL;
}

cTask* cTasks::TaskFromName(const char* Name)
{
  cMutexLock TasksLock(&Tasks);
  cTask *Task = Tasks.First();
  while (Task) {	
    if (strcasecmp(Name, Task->Name()) == 0)
      return Task;
    Task = Tasks.Next(Task);
  }

  cMutexLock TempTasksLock(&TempTasks);
  Task = TempTasks.First();
  while (Task) {	
    if (strcasecmp(Name, Task->Name()) == 0)
      return Task;
    Task = TempTasks.Next(Task);
  }
  return NULL;
}

long cTasks::GetNewId()
{
  long newId = 0;

  cMutexLock TasksLock(&Tasks);
  cTask *Task = Tasks.First();
  while (Task) {
    newId = std::max(newId, Task->Id());
    Task = Tasks.Next(Task);
  }

  cMutexLock TempTasksLock(&TempTasks);
  Task = TempTasks.First();
  while (Task) {
    newId = std::max(newId, Task->Id());
    Task = TempTasks.Next(Task);
  }
  return newId+1;
}

cTask* cTasks::AddTemp(const char* Cmd)
{
  cMutexLock TempTasksLock(&TempTasks);
  cTask* tempTask = new cTask(Cmd, Cmd);
  TempTasks.Add(tempTask);
  tempTask->SetId(GetNewId());
  return tempTask;
}

bool cTasks::IsTemp(cTask* Task)
{
  cMutexLock TempTasksLock(&TempTasks);
  cTask* TempTask = TempTasks.First();
  while (TempTask) {
    if (TempTask == Task) return true;
    TempTask = TempTasks.Next(TempTask);
  }
  return false;
}

vector< string > StringSplit( string const& text, char delimiter )
{
  vector< string > result;
  string::size_type last = 0, pos;
  while ( ( pos = text.find( delimiter, last ) ) != string::npos ) {
    result.push_back( text.substr( last, pos - last ) );
    last = pos + 1;
  }
  if ( last < text.length() )
    result.push_back( text.substr( last ) );
  return result;
}


// -- cValueSpec -----------------------------------------------------------------
cValueSpec::cValueSpec(string value, int range_from, int range_to)
{
  if (value.empty())
    for(int value = range_from; value <= range_to; value++)
      values.insert(value);
  else
    {
      vector<string> parts = StringSplit(value, ',');
      vector< string >::const_iterator part = parts.begin();
      for ( int i = 0; part != parts.end(); ++i, ++part )  
	{
	  set<int> partvalues;
	  vector<string> rangestep = StringSplit(*part, '/');	 
	  vector<string> range = StringSplit(rangestep[0], '-');
	  if (range.size() == 2)
	    for(int value = atoi(range[0].c_str()); value <= atoi(range[1].c_str()); value++)
	      partvalues.insert(std::min(range_to, std::max(range_from,value)));
	  else if (range.size() == 1 && range[0] != "*")
	    partvalues.insert(atoi(range[0].c_str()));
	  else if (range.size() == 1 && range[0] == "*")
	    for(int value = range_from; value <= range_to; value++)
	      partvalues.insert(value);
	  if (rangestep.size() > 1)
	    {
	      int step = atoi(rangestep[1].c_str());
	      int y=0;
	      for(set<int>::iterator it = partvalues.begin(); it != partvalues.end(); ++it, y++)
		if (y%step == 0)
		  values.insert(*it);
	    }
	  else
	    values.insert(partvalues.begin(), partvalues.end());
	}
    }
}
 
bool cValueSpec::Matches(int value)
{
  return (values.find(value) != values.end());
}

// -- cTaskLog -----------------------------------------------------------------
#define LOG_MARKER string("#scheduler-log: ")

cTaskLog::cTaskLog(cTask* Task)
: task(Task)
{
}

string cTaskLog::Filename(string Name)
{
  if (!task) return "";
  if (Name.empty())
    Name = task->Name();
  string logfileName = Name + string(".log");
  return string(AddDirectory(logDirectory.c_str(), logfileName.c_str()));
}

string cTaskLog::Read(time_t logTime, bool skipFirstLine)
{
  if (!task) return "";
  cMutexLock TaskLock(task);

  if (!logTime) // clear only on full read
    logTimes.clear();

  if (!task->logging) return "";

  ifstream logfile;
  logfile.open(Filename().c_str());
  if (!logfile) return "";
  
  string logcontents = "";
  string line;
  bool matcheslogTime = false;
  while (getline(logfile,line))
    {
      if (line.find(LOG_MARKER) == 0)
	{
	  string startTime = line.substr(LOG_MARKER.size());
	  if (!startTime.empty())
	    {
	      time_t Start = atol(startTime.c_str());
	      if (!logTime) logTimes.push_back(Start); // update only on full read
	      matcheslogTime = (Start == logTime);
	    }
	}
      if (!logTime || matcheslogTime)
	logcontents += line + "\n";
    }
  if (skipFirstLine)
    logcontents = logcontents.substr(logcontents.find("\n")+1);
  return logcontents;
}

bool cTaskLog::Insert(string NewLog)
{
  if (!task) return "";
  cMutexLock TaskLock(task);
  
  time_t lastStart = task->LastStart();
  time_t now = time(NULL);

  Read();
  ostringstream logstream;
  logTimes.insert(logTimes.begin(), lastStart);
  logstream << LOG_MARKER << lastStart << endl;
  logstream << tr("Start") << ": " << *DateString(lastStart) << " " << *TimeString(lastStart) << endl;
  logstream << tr("End") << ": " << *DateString(now) << " " << *TimeString(now) << endl;
  logstream << tr("Output") << ":" << endl << NewLog << endl;

  for(uint i=1; i<=std::min((uint)task->logging, (uint) logTimes.size())-1; i++)
    logstream << Read(logTimes[i]);

  ofstream logfile;
  logfile.open(Filename().c_str());
  if (!logfile) return false;
  logfile << logstream.str();
  logfile.close();

  Read();
  return true;
}

bool cTaskLog::Rename(string strOldName)
{
  cMutexLock TaskLock(task);
  return (rename(Filename(strOldName).c_str(), Filename().c_str()) == 0);
}


