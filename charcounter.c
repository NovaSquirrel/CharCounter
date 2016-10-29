/*
 * CharCounter
 *
 * Copyright (C) 2015 NovaSquirrel
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define PNAME "Character counter"
#define PDESC "Displays a character count as you type"
#define PVERSION "0.01"

#include "hexchat-plugin.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
static hexchat_plugin *ph;

void hexchat_plugin_get_info(char **name, char **desc, char **version, void **reserved) {
  *name = PNAME;
  *desc = PDESC;
  *version = PVERSION;
}

int hexchat_plugin_deinit() {
  hexchat_print(ph, "Character counter unloaded");
  return 1;
}

// Updates the displayed character count
static void Update() {
  // Empty tab? Don't display character count
  if(!strlen(hexchat_get_info(ph, "host")))
    return;

  int Len = strlen(hexchat_get_info(ph, "inputbox"));
  hexchat_context *Context = hexchat_get_context(ph);

  // HexChat doesn't seem to provide a great way to get the current server tab
  // but it does give each connected server a unique integer ID, so we can find
  // the server tab that has the same ID.
  int Id;
  hexchat_get_prefs(ph, "id", NULL, &Id);

  // Request a list of all tabs open in HexChat
  hexchat_list *list = hexchat_list_get(ph, "channels");
  if(list) {
    while(hexchat_list_next(ph, list)) {
      // Tab type 1 is a server tab
      if(hexchat_list_int(ph, list, "type")==1 && hexchat_list_int(ph, list, "id")==Id) { // server
        // Switch to the tab's context so we can do a command on it
        if(hexchat_set_context(ph,(hexchat_context *)hexchat_list_str(ph, list, "context"))) {
          // If the user typed any characters, display the character count, otherwise change it back
          if(Len)
            hexchat_commandf(ph, "settab %-3i %s", Len, hexchat_get_info(ph, "channel"));
          else
            hexchat_commandf(ph, "settab %s", hexchat_get_info(ph, "channel"));
        }
      }
    }
    hexchat_list_free(ph, list);
  }
  // Restore the context now that we're done
  hexchat_set_context(ph, Context);
}

// Run the updater every second
static int timer_cb(void *userdata) {
  Update();
  return 1;
}

// Run the updater on every key press or some other events
static int event_cb(char *word[], void *userdata) {
  Update();
  return HEXCHAT_EAT_NONE;
}

int hexchat_plugin_init(hexchat_plugin *plugin_handle,
                      char **plugin_name,
                      char **plugin_desc,
                      char **plugin_version,
                      char *arg) {
  ph = plugin_handle;
  *plugin_name = PNAME;
  *plugin_desc = PDESC;
  *plugin_version = PVERSION;
  hexchat_print(ph, "Character counter loaded");
  hexchat_hook_timer(ph, 1000, timer_cb, NULL);
  hexchat_hook_print(ph, "Key Press",    HEXCHAT_PRI_NORM, event_cb, NULL); 
  hexchat_hook_print(ph, "Your Message", HEXCHAT_PRI_NORM, event_cb, NULL); 
  hexchat_hook_print(ph, "Your Action",  HEXCHAT_PRI_NORM, event_cb, NULL); 
  hexchat_hook_print(ph, "Focus Tab",    HEXCHAT_PRI_NORM, event_cb, NULL); 
  return 1;
}
