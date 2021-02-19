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
#include "schematic_dialog.h"
#include "qucsdoc.h"
/*--------------------------------------------------------------------------*/
SchematicDialog::SchematicDialog()
    : Widget()
{
}
/*--------------------------------------------------------------------------*/
SchematicDialog::SchematicDialog(SchematicDialog const& d)
    : Widget(d)
{
}
/*--------------------------------------------------------------------------*/
void SchematicDialog::execute(QUndoCommand* c)
{
	assert(doc());
	doc()->executeCommand(c);
}
/*--------------------------------------------------------------------------*/
QucsDoc* SchematicDialog::doc()
{ untested();
	auto t = dynamic_cast<QWidget*>(this); // prech?
	assert(t);
	auto p = dynamic_cast<QucsDoc*>(t->parentWidget()); // prech?
	assert(p);
	return p;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
