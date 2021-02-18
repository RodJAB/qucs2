/***************************************************************************
    copyright            : (C) 2016, 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QUCS_PLACE_H
#define QUCS_PLACE_H
/*--------------------------------------------------------------------------*/
#include "symbol.h"
#include "qucs_globals.h"
#include "exception.h"
#include "geometry.h"
#include "node.h"
#include "place.h" // for now.
#ifdef DO_TRACE
#include "viewpainter.h"
#endif
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
Place d0;
Dispatcher<Command>::INSTALL p(&symbol_dispatcher, "place", &d0);
/*--------------------------------------------------------------------------*/
} // namespace
/*--------------------------------------------------------------------------*/
void Place::paint(ViewPainter* p) const
{
	trace3("placepaint", label(), position(), node_degree());
#ifdef DO_TRACE
	if(node_degree()==1){
		p->setPen(QPen(Qt::red, 1));
	}else if(node_degree()==3){
		p->setPen(QPen(Qt::white, 1));
	}else if(node_degree()==2){
		p->setPen(QPen(Qt::green, 1));
	}else if(node_degree()==0){
		// BUG. a place without purpose...
		//  (where does it come from??)
		p->setPen(QPen(Qt::red, 3));
	}else{
		p->setPen(QPen(Qt::green, 2));
	}
	p->drawEllipse(-1, -1, 1, 1);
#else
	(void) p;
#endif
}
/*--------------------------------------------------------------------------*/
rect_t Place::bounding_rect() const
{
  return rect_t(pos_t(-1,-1), pos_t(1,1));
}
/*--------------------------------------------------------------------------*/
unsigned Place::node_degree() const
{
	if(_port.value()){
		assert(_port.value()->numPorts());
		return _port.value()->numPorts()-1;
	}else{
		unreachable();
		return 0;
	}
}
/*--------------------------------------------------------------------------*/
bool Place::showLabel() const
{
#ifdef DO_TRACE
  	return true;
#endif
  	return false;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
