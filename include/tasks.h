/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QUCS_SIM_TASK_H
#define QUCS_SIM_TASK_H

// command "elements" in a document. typically simulation commands.
//
#include <list>

namespace qucs {
class TaskElement; // Task??

class CmdEltList /*HACK, for now */ : public std::list<TaskElement*> {
};
}
#endif
