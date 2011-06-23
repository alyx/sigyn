/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#pragma once

/* config.h - Sigyn temp. configuration file
 * Fill in the values here to configure sigyn
 * NOTE: This will eventually be replaced with
 * a proper configuration file.
 */

#define SIGYN_NICK "sigyn" // Define the nickname we should use.
#define SIGYN_REALNAME "Sigyn IRC Bot" // Define the real name (The GECOS).

#define UPLINK_SERVER "irc.thinstack.net" // Server address we should connect to.
#define UPLINK_PORT 6667 // Port we should connect to.
//#define UPLINK_PASS "moocow" // Password for the IRC connection. Uncomment this is you need it.

#define LOGFILE "sigyn.log" // File Sigyn should log to. Undef this if you do not want a log file.
