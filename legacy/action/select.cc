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

#ifndef QUCS_SELECT_H
#define QUCS_SELECT_H
#include "mouseaction.h"
#include "schematic_dialog.h"
#include "schematic_edit.h"
#include "qucs_globals.h"
#include "qucsdoc.h"
#include "node.h"

#include <QAction>
#include <QGraphicsView>

namespace{
/*--------------------------------------------------------------------------*/
using qucs::Element;
using qucs::SchematicEdit;
using qucs::SchematicDialog;
using qucs::Action;
using qucs::MouseAction;
using qucs::MouseActions;
/*--------------------------------------------------------------------------*/
static Symbol* symbol(Element* e)
{
	return dynamic_cast<Symbol*>(e);
}
/*--------------------------------------------------------------------------*/
static Symbol* symbol(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return symbol(e->operator->());
}
/*--------------------------------------------------------------------------*/
static Symbol* symbol(ElementGraphics* e)
{
	if(!e) return nullptr;
	return symbol(e->operator->());
}
/*--------------------------------------------------------------------------*/
static bool isWire(Symbol const* e)
{
	assert(e);
	if(e->numPorts()!=2){
		return false;
	}else{
	}

	assert(e->portValue(0));
	assert(e->portValue(1));

	return e->portValue(0)->net() == e->portValue(1)->net();
}
/*--------------------------------------------------------------------------*/
static void selectWireLine(ElementGraphics *g)
{
	Symbol* s = symbol(g);
	assert(isWire(s));
	auto scn = g->scene();
	assert(scn);
	
	for(unsigned i=0; i<s->numPorts(); ++i){
		auto pos = makeQPointF(s->nodePosition(i));
		auto items = scn->items(pos);
		if(items.size()==2){

			for(auto ii : items){
				Symbol* si = symbol(ii);
				if(!si){
				}else if(g->isSelected()==ii->isSelected()){
				}else if(si == s){
				}else if(isWire(si)){
					ii->setSelected(g->isSelected());
					selectWireLine(ii);
				}else{
				}
			}
		}else{ untested();
		}
	}
}
/*--------------------------------------------------------------------------*/
static QPoint getDelta(ElementGraphics* e)
{itested();
	auto p = e->pos().toPoint();
	assert(element(e));
	auto p1_ = element(e)->center();
	auto p1 = QPoint(getX(p1_), getY(p1_));
	return p - p1;
}
/*--------------------------------------------------------------------------*/
class MoveSelection : public SchematicEdit {
public:
	template<class IT>
	MoveSelection(QPoint delta, SchematicScene* ctx, IT selection)
	: SchematicEdit(ctx) { itested();
		trace1("MoveSelection", delta);
		size_t k = 0;
		for(auto i : selection){itested();
			if(auto eg=dynamic_cast<ElementGraphics*>(i)){itested();
				++k;
				Element* elt = eg->cloneElement();
				auto ec = elt->center();

				trace1("MoveSelection", eg->isSelected());
		//		eg->hide(); // no longer needed (and wrong!!)
				eg->setPos(QPoint(getX(ec), getY(ec)));
		//		eg->show_(); // no longer needed (and wrong!!)
				assert(elt);
				int dx = getX(delta);
				int dy = getY(delta);

				pos_t c = elt->center() + pos_t(dx, dy);
				elt->setCenter(c);

				qSwap(eg, elt); // gaah.
			}else{ untested();
				unreachable(); // really? use prechecked_cast then.
			}
		}
		setText("move " + QString::number(k) + " items by "
				+ QString::number(delta.x()) + ", "
				+ QString::number(delta.y())) ;
	}
}; // MoveSelection
/*--------------------------------------------------------------------------*/
class ActionSelect : public QAction {
public:
	explicit ActionSelect(QObject* parent) : QAction(parent) { untested();
		setIcon(QIcon(":/bitmaps/pointer.png"));
		setText(tr("Select"));
		setStatusTip(tr("Activate select mode"));
		setWhatsThis(tr("Select\n\nActivates select mode"));
		setCheckable(true);
	}
};
/*--------------------------------------------------------------------------*/
class MouseActionSelect : public MouseAction{
public:
#if 0
	explicit MouseActionSelect() {}
#else
	explicit MouseActionSelect(MouseActions* ctx=nullptr)
		: MouseAction() {
//			assert(scene());
		}
#endif
private:
	Action* clone() const{ untested();
		return new MouseActionSelect(); // this?
	}
	QAction* createAction(QObject* parent) const override{ untested();
		auto x = new ActionSelect(parent);
		connect(x, &QAction::toggled, this, &MouseAction::slotToggle);
		return x;
	}

private: // MouseAction
//	cmd* activate(QAction* sender) override;
	cmd* move(QEvent*) override;
	cmd* press(QEvent*) override;
	cmd* release(QEvent*) override;
	//	cmd* release2(QMouseEvent*); // what is this?
	// cmd* enter(QEvent*) override;
	cmd* dblclk(QEvent*) override;

private:
	void showSchematicWidget(Widget*, ElementGraphics*);
	cmd* release_left(QEvent*);

private: // more decoupling
	//ElementMouseAction focusElement;
	bool isMoveEqual; //?
}a0; // MouseActionSelect
static Dispatcher<Widget>::INSTALL p1(&action_dispatcher, "ActionSelect", &a0);
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionSelect::dblclk(QEvent* evt)
{ untested();
	incomplete();
	//  Doc->releaseKeyboard();  // allow keyboard inputs again
	//  QucsMain->editText->setHidden(true);
	//  editElement(Doc, Event);
	Element* elt = nullptr;
	ElementGraphics* gfx = nullptr;
	//
	if(auto i = dynamic_cast<qucs::ItemEvent*>(evt)){ untested();
		// QList<ElementGraphics*> l;
		gfx = &i->item();
		elt = element(gfx);
		// l.push_back(&i->item());
	}else{ untested();
	}

	// BUG: don't ask elt.
	if(!elt){ untested();
	}else if(auto ew = elt->schematicWidget(nullptr)){ untested();
		trace0("got editElement");
		assert(gfx);
		showSchematicWidget(ew, gfx);
	}else{ untested();
		trace0("no editElement");
		incomplete(); // memory leak
	}

	return nullptr;
}
/*--------------------------------------------------------------------------*/
// not sure I like this.
void MouseActionSelect::showSchematicWidget(Widget* ew, ElementGraphics* gfx)
{
	if(auto eew=dynamic_cast<SchematicDialog*>(ew)){ untested();
		assert(gfx);
		eew->attach(gfx);
	}else{
	}

	if(auto eew=dynamic_cast<QDialog*>(ew)){ untested();
		eew->setParent(view(), Qt::Dialog);
		if(eew->exec() != 1){ untested();
			// done=true;   // dialog is WDestructiveClose
		}else{ untested();
			incomplete();
		}
		//delete eew; // crash. why?
	}else{
	}
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionSelect::move(QEvent *)
{itested();
	// obsolete?
	if(isMoveEqual) {itested();
		// square?
	}else{itested();
	}

	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionSelect::press(QEvent*)
{itested();

	incomplete();
	return nullptr;
#if 0
	QMouseEvent* e;
	SchematicDoc* Doc = &doc();
	assert(Doc);
	QPointF pos = Doc->mapToScene(e->pos());
	trace2("sel::press", pos, e->pos());
	setPos1(pos);
	float fX=pos.x();
	float fY=pos.y();

	bool Ctrl = e->modifiers().testFlag(Qt::ControlModifier);

	int No=0;

	// memorise first click position
	//MAx1 = int(fX);
	//MAy1 = int(fY);

	focusElement = ctx().selectElement(e->pos(), Ctrl, &No);
	isMoveEqual = false;   // moving not neccessarily square

	incomplete(); //this does not add up.
#if 0
	if(!focusElement){ untested();
	}else if(focusElement->Type == isDiagramHScroll){ untested();
		// BUG: move to selectElement? what is MAy1?!
		MAy1 = MAx1;
	}else{ untested();
	}
#endif

	if(!focusElement){ untested();
		qDebug() << "MPressSelect miss" << e->pos() << pos;
	}else if(focusElement->Type == isPaintingResize){ untested();
		incomplete(); // delegate. how?
#if 0
		focusElement->Type = isPainting;
		QucsMain->MouseReleaseAction = &MouseActions::MReleaseResizePainting;
		QucsMain->MouseMoveAction = &MouseActions::MMoveResizePainting;
		QucsMain->MousePressAction = 0;
		QucsMain->MouseDoubleClickAction = 0;
		Doc->grabKeyboard();  // no keyboard inputs during move actions
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		return nullptr;
	}else if(focusElement->Type == isDiagramResize){ untested();
		incomplete();

		if(((Diagram*)focusElement)->name().left(4) != "Rect")
			if(((Diagram*)focusElement)->name().at(0) != 'T')
				if(((Diagram*)focusElement)->name() != "Curve")
					/* if(((Diagram*)focusElement)->name() != "Waveac")
						if(((Diagram*)focusElement)->name() != "Phasor")*/
					isMoveEqual = true;  // diagram must be square

		focusElement->Type = isDiagram;
		MAx1 = focusElement->cx_();
		MAx2 = focusElement->x2_();

		focusElement->someDiagramStateCallback()

			// old:
			// if(diagram(focusElement)->State & 1) { untested();
			//   MAx1 += MAx2;
			//   MAx2 *= -1;
			// }

			MAy1 =  focusElement->cy_();
		MAy2 = -focusElement->y2_();
		if(((Diagram*)focusElement)->State & 2) { untested();
			MAy1 += MAy2;
			MAy2 *= -1;
		}

		 // diagram_action?
		QucsMain->MouseReleaseAction = &MouseActions::MReleaseResizeDiagram;
		QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
		QucsMain->MousePressAction = 0;
		QucsMain->MouseDoubleClickAction = 0;
		Doc->grabKeyboard(); // no keyboard inputs during move actions
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		return nullptr;

	}else if(focusElement->Type == isDiagramHScroll
			|| focusElement->Type == isDiagramVScroll){ untested();
		incomplete();

		focusElement->Type = isDiagram; // reset happens here. FIXME.

		auto d=diagram(focusElement); // is this necessary?!
		assert(d);
		No = d->scroll(MAy1);

		switch(No) {
		case 1:
			Doc->setChanged(true, true, 'm'); // 'm' = only the first time
			break;
		case 2:  // move scroll bar with mouse cursor
			QucsMain->MouseMoveAction = &MouseActions::MMoveScrollBar;
			QucsMain->MousePressAction = 0;
			QucsMain->MouseDoubleClickAction = 0;
			Doc->grabKeyboard();  // no keyboard inputs during move actions

			// Remember inital scroll bar position.
			MAx2 = int(d->xAxis_limit_min());
			// Update matching wire label highlighting
			Doc->highlightWireLabels ();
			return;
		}
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		Doc->viewport()->update();
		drawn = false;
		return;

	}else if(focusElement->Type == isComponentText){ untested();
		incomplete();

		focusElement->Type &= (~isComponentText) | isComponent;

		MAx3 = No;
		QucsMain->slotApplyCompText();
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		return;

	}else if(auto n=node(focusElement)){ untested();
		(void)n;
		if (QucsSettings.NodeWiring) { untested();
			incomplete();

			MAx1 = 0;   // paint wire corner first up, then left/right
			MAx3 = focusElement->cx_();  // works even if node is not on grid
			MAy3 = focusElement->cy_();
			QucsMain->MouseMoveAction = &MouseActions::MMoveWire2;
			QucsMain->MousePressAction = &MouseActions::MPressWire2;
			QucsMain->MouseReleaseAction = 0; // if function is called from elsewhere
			QucsMain->MouseDoubleClickAction = 0;

			formerAction = QucsMain->select; // to restore action afterwards
			QucsMain->activeAction = QucsMain->insWire;

			QucsMain->select->blockSignals(true);
			QucsMain->select->setChecked(false);
			QucsMain->select->blockSignals(false);

			QucsMain->insWire->blockSignals(true);
			QucsMain->insWire->setChecked(true);
			QucsMain->insWire->blockSignals(false);
			// Update matching wire label highlighting
			Doc->highlightWireLabels ();
			return;
		}else{ untested();
		}
#endif
	}else{ untested();
		// default case
		// unreachable?
	}

	QucsMain->MousePressAction = 0;
	QucsMain->MouseDoubleClickAction = 0;
	Doc->grabKeyboard();  // no keyboard inputs during move actions
	Doc->viewport()->update();
	//setDrawn(false);

	// Update matching wire label highlighting
	assert(Doc);
	// Doc->highlightWireLabels ();
//	e->ignore(); // handle in QGraphicsView?
	return nullptr;
#endif
} // select::press
/*--------------------------------------------------------------------------*/
// was MouseActions::MReleaseSelect(SchematicDoc *Doc, QMouseEvent *Event)
QUndoCommand* MouseActionSelect::release(QEvent *ev)
{itested();
	QUndoCommand* cmd = nullptr;
	auto m = dynamic_cast<QMouseEvent*>(ev);
	if(!m){ untested();
	}else if(m->button() == Qt::LeftButton){itested();
		cmd = release_left(ev);
	}else if(m->button() == Qt::RightButton){ untested();
	}
	return cmd;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionSelect::release_left(QEvent *e)
{itested();
    auto m = prechecked_cast<QMouseEvent*>(e);
	 if(!m){
		 unreachable();
		 return nullptr;
	 }else{
	 }
    bool ctrl = m->modifiers().testFlag(Qt::ControlModifier);

	if(!ctrl) {itested();
		incomplete();
	}else{itested();
	}

	cmd* c = nullptr;

	assert(scene());
	auto s = scene()->selectedItems();
	if(s.isEmpty()){ untested();
	}else{itested();

		auto delta = getDelta(s.first());
#ifndef NDEBUG
		for(auto i : s){
			trace2("check delta", delta, getDelta(i));
			assert(delta == getDelta(i));
		}
#endif
		int fX = int(delta.x());
		int fY = int(delta.y());

		if(fX || fY){itested();
			trace1("possible move", delta);
			c = new MoveSelection(delta, scene(), s);
		}else{itested();
		}
	}

	if(c){
	}else if(s.size()!=1){
	}else if(!symbol(s.front())){
	}else if(m->button() == Qt::LeftButton){itested();
			// if it's a wire, select the whole thing?
			// (what is a wire?)
		if(isWire(symbol(s.front()))) { untested();
			incomplete();
#if 1
			selectWireLine(s.front());
#endif
		}else{itested();
		}
	}else{itested();
	}

	view()->releaseKeyboard();
//	Doc->highlightWireLabels ();
	updateViewport();
	// drawn = false;
	return c;
} // select::release
/*--------------------------------------------------------------------------*/
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
