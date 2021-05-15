/***************************************************************************
    copyright            : (C) 2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "schematic_action.h"
#include "io_trace.h"
#include "qucsdoc.h"
#include "doc_actions.h"
#include "platform.h"
#include <QAction>
#include <QMouseEvent>
#include <QGraphicsView>
#include <QGraphicsSceneEvent> // probably bug
#include "component_widget.h" // bug
#include <QStringLiteral> //bug
#include <QMimeData> //bug

namespace qucs {

QUndoCommand* MouseAction::handle(QEvent* e)
{itested();

  { untested();
    // pass to other places unless accepted somewhere else.
    // assert(is_ignored) instead!
    e->ignore();
  }
  assert(e);
  auto* m = prechecked_cast<QMouseEvent*>(e);
//  auto* s = prechecked_cast<QGraphicsSceneEvent*>(e);
  auto a = ComponentWidget::itemMimeType();

  if(dynamic_cast<QDragLeaveEvent*>(e)){ untested();
    unreachable();
    return leave(m);
  }else if(auto de = dynamic_cast<QDragEnterEvent*>(e)){ untested();
    trace1("dragenter", de->mimeData()->formats()[0]);
    unreachable();

    if (de->mimeData()->hasFormat(a)){ untested();
      // got a pointer. possibly from ComponentWidget
      e->accept();
    // }else if (de->mimeData()->hasFormat(someThingElse)){ untested();
    //   unreachable();
    //   e->accept();
    } else{ untested();
      e->ignore();
    }

    return enter(m);

    return nullptr;
  }else if(auto de = dynamic_cast<QDragMoveEvent*>(e)){ untested();
    trace1("dragmove", de->mimeData()->formats()[0]);
  }else if(auto de = dynamic_cast<QDropEvent*>(e)){ untested();
    trace1("dragdrop", de->mimeData()->formats()[0]);
  }else{ untested();
  }

  // why??
  switch(e->type()){
//  case QEvent::MouseLeave:
//    assert(m);
//    return leave(m);
  case QEvent::Leave:
    return leave(e);
  case QEvent::Enter:
    return enter(e);
  case QEvent::GraphicsSceneMouseDoubleClick:
	 untested();
	 return dblclk(e);
  case QEvent::GraphicsSceneMouseMove:
    // getting here when moving elements.
  case QEvent::MouseMove:
    return move(e);
  case QEvent::GraphicsSceneMouseRelease:itested();
    return nullptr;
    //fallthrough
  case QEvent::MouseButtonRelease:itested();
    assert(m);
    return release(m);
  case QEvent::GrabMouse:itested();
    return nullptr;
//    return grab(s);
  case QEvent::DragEnter: untested();
    unreachable(); // proper type check above
    return nullptr;
  case QEvent::MouseButtonPress: untested();
    // fallthrough
  case QEvent::GraphicsSceneMousePress:itested();
    return press(e);
  default:
    trace1("mouseaction miss", e->type());
    e->ignore();
    return nullptr;
  }
}
// SchematicMouseAction::doc?
Doc const* MouseAction::doc() const
{itested();
  auto cc = const_cast<MouseAction*>(this);
  return cc->doc();
}


SchematicScene* MouseAction::scene()
{itested();
	auto cc = dynamic_cast<QGraphicsView*>(doc());
	
	if(!cc){ untested();
	}else if(auto s = dynamic_cast<SchematicScene*>(cc->scene())){itested();
		return s;
	}else{ untested();
	}
	return nullptr;
}

QGraphicsView* MouseAction::view()
{itested();
  auto cc = dynamic_cast<QGraphicsView*>(doc());
  if(cc){ untested();
	  return cc;
  }else{ untested();
	  return nullptr;
  }
}

Doc* MouseAction::doc()
{itested();
  Doc* c = ctx()->doc();
  return c;
#if 0
  auto cc = dynamic_cast<SchematicDoc*>(c);
  assert(cc);
  return *cc;
#endif
}

#include <QGraphicsView>
QPointF MouseAction::mapToScene(QPoint const& p) const
{ untested();
	if(auto s=dynamic_cast<QGraphicsView const*>(doc())){ untested();
		return s->mapToScene(p);
	}else{ untested();
		assert(false); // for now.
	}
}
/* -------------------------------------------------------------------------------- */
void MouseAction::setCursor(QCursor const& c)
{ itested();
   view()->setCursor(c);
}
/* -------------------------------------------------------------------------------- */
void MouseAction::updateViewport()
{itested();
	assert(ctx());
	ctx()->updateViewport(); // use a signal?
}


QUndoCommand* MouseAction::activate(QObject* sender)
{itested();
  _sender = dynamic_cast<QAction*>(sender);
  return nullptr;
}

QUndoCommand* MouseAction::deactivate()
{itested();
  return nullptr;
}

void MouseAction::uncheck()
{ untested();
  if(_sender){itested();
    _sender->blockSignals(true); // do not call toggle slot
    _sender->setChecked(false);       // set last toolbar button off
    _sender->blockSignals(false);
  }else{itested();
  }

  deactivate(); // BUG?
}
/*--------------------------------------------------------------------------*/
SchematicScene const* MouseAction::scene() const
{ untested();
	auto s = dynamic_cast<QGraphicsView const*>(doc());
	if(!s){ untested();
	}else if(auto ss=dynamic_cast<SchematicScene const*>(s->scene())){ untested();
		return ss;
	}else{ untested();
	}
	return nullptr;
}
/*--------------------------------------------------------------------------*/
// isPlace?
bool MouseAction::isNode(pos_t const& p) const
{ untested();
	assert(scene());
	return scene()->isNode(p);
	return false; // doc().isNode(p);
}
/*--------------------------------------------------------------------------*/
bool MouseAction::isConductor(pos_t const&p) const
{itested();
	assert(scene());
	return scene()->isConductor(p);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
MouseActions* MouseAction::ctx() const
{ untested();
	auto q = dynamic_cast<QObject const*>(this);
	assert(q);
	auto p = dynamic_cast<MouseActions*>(q->parent());
	assert(p);
	return p;
}
/*--------------------------------------------------------------------------*/
QList<ElementGraphics*> MouseAction::selectedItems()
{ untested();
   // BUG. copies.
  unreachable();
#if 0
  return doc()->selectedItems();
#else
	auto v = dynamic_cast<QGraphicsView*>(doc());
	if(v){ untested();
	}else if(auto s=dynamic_cast<SchematicScene*>(v->scene())){ untested();
		return s->selectedItems();
	}else{ untested();
		unreachable();
	}
	return QList<ElementGraphics*>();
#endif
}
/*--------------------------------------------------------------------------*/
void MouseAction::sceneAddItem(ElementGraphics* x)
{ untested();
#if 0
  doc()->sceneAddItem(x);
#else
  auto v = dynamic_cast<QGraphicsView*>(doc());

  if(v){ untested();
    incomplete();
    assert(v->scene());
    v->scene()->addItem(x);
    QGraphicsItem* g = x;
    g->show();
  }else{ untested();
  }
#endif
}
/*--------------------------------------------------------------------------*/
void MouseAction::sceneRemoveItem(ElementGraphics* x)
{ untested();
	auto v = dynamic_cast<QGraphicsView*>(doc());
	if(v){ untested();
		assert(v->scene());
		v->scene()->removeItem(x);
	}else{ untested();
	}
}
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/