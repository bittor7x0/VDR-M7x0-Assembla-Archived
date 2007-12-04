#include <set>
#include "menu_favorites.h"
#include "menu_whatson.h"
#include "menu_commands.h"

using std::set;

extern const char* ButtonBlue[3];
extern int exitToMainMenu;

cMenuFavorites::cMenuFavorites()
   : cMenuSearchResults(cTemplFile::GetTemplateByName("MenuFavorites"))
{
   BuildList();
}

bool cMenuFavorites::BuildList()
{
   Clear();
   eventObjects.Clear();

   cSearchResults* pCompleteSearchResults = NULL;
   cMutexLock SearchExtsLock(&SearchExts);
   cSearchExt *SearchExt = SearchExts.First();
   int timespan = EPGSearchConfig.FavoritesMenuTimespan*60;

   while (SearchExt) 
   {
      if (SearchExt->useInFavorites)
         pCompleteSearchResults = SearchExt->Run(modeBlue == showNoPayTV?1:0, false, timespan, pCompleteSearchResults, true);
      SearchExt = SearchExts.Next(SearchExt);
   }

   if (pCompleteSearchResults)
   {
      set<const cEvent*> foundEvents;
      pCompleteSearchResults->SortBy(CompareEventTime);
	
      for (cSearchResult* pResultObj = pCompleteSearchResults->First(); 
           pResultObj; 
           pResultObj = pCompleteSearchResults->Next(pResultObj))
      {
         if (foundEvents.find(pResultObj->event) == foundEvents.end())
         {
            foundEvents.insert(pResultObj->event);
            Add(new cMenuSearchResultsItem(pResultObj->event, modeYellow == showEpisode, false, menuTemplate));
            eventObjects.Add(pResultObj->event);
        }
      }	
      delete pCompleteSearchResults;
   }
   SetHelpKeys();  
   char* szTitle = NULL;
   asprintf(&szTitle, "%s: %d %s", tr("Favorites"), Count(), tr("Search results"));
   SetTitle(szTitle);
   free(szTitle);
   Display();
    
   return true;
}

eOSState cMenuFavorites::OnGreen()
{
   eOSState state = osUnknown;
   if(!HasSubMenu()) 	     
   {
      toggleKeys = 0;
      cMenuWhatsOnSearch::currentShowMode = cMenuWhatsOnSearch::GetNextMode();
      return osUnknown;
   }
   return state;
}

eOSState cMenuFavorites::OnYellow()
{
   eOSState state = osUnknown;
   if(!HasSubMenu()) 	     
   {
      cMenuSearchResultsItem *item = (cMenuSearchResultsItem *)Get(Current());
      if (item && item->event)
      {
         cChannel *channel = Channels.GetByChannelID(item->event->ChannelID(), true, true);
         cMenuWhatsOnSearch::scheduleChannel = channel;
         cMenuWhatsOnSearch::currentShowMode = showNow;
      }
      toggleKeys = 0;
      return osBack;
   }
   return state;
}

eOSState cMenuFavorites::ProcessKey(eKeys Key)
{
   exitToMainMenu = 0;
   if (!HasSubMenu() && Key == kBack)
   {
      exitToMainMenu = 1;
      return osBack;
   }

   eOSState state = cMenuSearchResults::ProcessKey(Key);
   if (state == osUnknown) 
   {
      switch (Key) {
         case kRecord:
         case kRed:
            state = OnRed();
            break;
         case k0:
            if(!HasSubMenu()) 
            {
               toggleKeys = 1 - toggleKeys;
               SetHelpKeys();
            }
            state = osContinue;
            break;
         case k1...k9: 
            state = HasSubMenu()?osContinue:Commands(Key);
            break;
         case kBlue:
            return EPGSearchConfig.useOkForSwitch?ShowSummary():Switch();
            break;
         case kOk:
            if(HasSubMenu()) 
            {
               state = cOsdMenu::ProcessKey(Key);
               break;
            }
            if (Count())
               state = EPGSearchConfig.useOkForSwitch?Switch():ShowSummary();
            else
               state = osBack;
            break;
         default:      
            break;
      }
   }
   return state;
}

void cMenuFavorites::SetHelpKeys()
{
   cMenuSearchResultsItem *item = (cMenuSearchResultsItem *)Get(Current());
   bool hasTimer = item && item->Selectable() && item->timerMatch == tmFull;

   showMode nextShowMode = cMenuWhatsOnSearch::GetNextMode();    
   cShowMode* mode = cMenuWhatsOnSearch::GetShowMode(nextShowMode);
   char* szButtonGreen = NULL;
   if (mode) 
      szButtonGreen = mode->GetDescription();

   if (toggleKeys==0)
      SetHelp((EPGSearchConfig.redkeymode==0?(hasTimer?tr("Button$Timer"):tr("Button$Record")):tr("Button$Commands")), szButtonGreen,tr("Button$Schedule"), EPGSearchConfig.useOkForSwitch?tr("Button$Info"):tr("Button$Switch"));
   else
      SetHelp((EPGSearchConfig.redkeymode==1?(hasTimer?tr("Button$Timer"):tr("Button$Record")):tr("Button$Commands")), szButtonGreen,tr("Button$Schedule"), EPGSearchConfig.useOkForSwitch?tr("Button$Info"):tr("Button$Switch"));
}

