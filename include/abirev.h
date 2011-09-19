/* Sigyn - A reasonably sane IRC bot.
 * Copyright (c) Alexandria Wolcott <alyx@malkier.net>
 * Released under the BSD license.
 */

#ifndef __SIGYN_ABIREV_H
#define __SIGYN_ABIREV_H

/*
 * Increment this in case of changes to the module ABI (i.e. changes
 * that require modules to be recompiled).
 * When incrementing, if the first two digits do not agree with the
 * current major.minor version (e.g. 3.3-devel), change the first two
 * digits and set the rest to 0 (e.g. 330000). Otherwise, increment
 * the lower digits.
 */
#define SIGYN_ABIREV 100000

#endif /* __SIGYN_ABIREV_H */
