#ifndef __EPGSEARCHRCFILE_H
#define __EPGSEARCHRCFILE_H

#include <vdr/config.h>

#define MAXSTRINGLEN 256

class cRCLine : public cListObject {
private:
  char *name;
  char *value;
public:
  cRCLine(void);
  cRCLine(const char *Name, const char *Value);
  virtual ~cRCLine();
  const char *Name(void) { return name; }
  const char *Value(void) { return value; }
  bool Parse(char *s);
};


class cRCFile : public cConfig<cRCLine> 
{
    bool Parse(const char *Name, const char *Value);
public:
    cRCFile();
    bool Load(const char *FileName);
    char Search[MAXSTRINGLEN];
    int SearchMode;
    int ChannelNr;
    int UseTitle;
    int UseSubtitle;
    int UseDescr;
};

#endif
