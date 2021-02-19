/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// user interaction, undo commands
// derived from various Qucs "MouseAction" etc.

#include <QAction>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QGraphicsSceneEvent>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QRegExpValidator>
#include <QUndoCommand>

#include "misc.h"
#include "mouseaction.h"
#include "node.h"
#include "qucs_app.h"
#include "qucsdoc.h"
#include "qucs_globals.h"
#include "schematic_action.h"
#include "schematic_dialog.h"
#include "schematic_edit.h"
#include "widget.h"
/*--------------------------------------------------------------------------*/
// TODO: why are actions implemented twice?
// one button and one event handler. and random glue all over the place
/*--------------------------------------------------------------------------*/
// inherit from DocAction??
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class ActionInsEqn : public QAction{
public:
	explicit ActionInsEqn(QObject* parent) : QAction(parent) { untested();
		setIcon(QIcon(":/bitmaps/equation.png"));
		setText(tr("Insert Equation"));
		setShortcut(Qt::CTRL+Qt::Key_Less);
		setStatusTip(tr("Inserts an equation"));
		setWhatsThis(tr("Insert Equation\n\nInserts a user defined equation"));
		setCheckable(true);
	}
};
/*--------------------------------------------------------------------------*/
class ActionRotate : public QAction{
public:
	explicit ActionRotate(QObject* parent) : QAction(parent) { untested();
		setIcon(QIcon(":/bitmaps/rotate_ccw.png"));
		setText(tr("Rotate"));
		setShortcut(Qt::CTRL+Qt::Key_R);
		setStatusTip(tr("Rotates the selected component by 90\x00B0"));
		setWhatsThis(tr("Rotate\n\nRotates the section by 90\x00B0 counter-clockwise"));
		setCheckable(true);
	}
};
/*--------------------------------------------------------------------------*/
class ActionMirrorX : public QAction{
public:
	explicit ActionMirrorX(QObject* parent) : QAction(parent) { untested();
  		setIcon(QIcon(":/bitmaps/mirror.png"));
		setText(tr("Mirror Y"));
		setShortcut(Qt::CTRL+Qt::Key_J);
		setStatusTip(tr("Mirror the selected item vertically"));
		setWhatsThis(tr("Mirror Y\n\nMirror the selected item vertically"));
		setCheckable(true);
	}
};
/*--------------------------------------------------------------------------*/
class ActionMirrorY : public QAction{
public:
	explicit ActionMirrorY(QObject* parent) : QAction(parent) { untested();
		setIcon(QIcon(":/bitmaps/mirrory.png"));
		setText(tr("Mirror X"));
		setShortcut(Qt::CTRL+Qt::Key_M);
		setStatusTip(tr("Mirror selected items horizontally"));
		setWhatsThis(tr("Mirror X\n\nMirrors selected items horizontally"));
		setCheckable(true);
	}
};
/*--------------------------------------------------------------------------*/
class ActionInsertPort : public QAction{
public:
	explicit ActionInsertPort(QObject* parent) : QAction(parent) { untested();
		setIcon(QIcon(":/bitmaps/port.png"));
		setText(tr("Insert Port"));
		setStatusTip(tr("Insert a port symbol"));
		setWhatsThis(tr("Insert Port\n\nInsert a port symbol"));
		setCheckable(true);
	}
};
/*--------------------------------------------------------------------------*/
class ActionInsertGround : public QAction{
public:
	explicit ActionInsertGround(QObject* parent) : QAction(parent) { untested();
		setIcon(QIcon(":/bitmaps/ground.png"));
		setText(tr("Insert Ground"));
		setShortcut(Qt::CTRL+Qt::Key_G);
		setStatusTip(tr("Insert a ground symbol"));
		setWhatsThis(tr("Insert Ground\n\nInsert a ground symbol"));
		setCheckable(true);
	}
};
/*--------------------------------------------------------------------------*/
class ActionSelect : public QAction{
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
/*--------------------------------------------------------------------------*/
template<class CMD>
class MouseActionSelCmd : public MouseAction{
public:
	explicit MouseActionSelCmd(QObject* p) : MouseAction(p){}
private:
	Action* clone() const{
		return new MouseActionSelCmd<CMD>(nullptr); // this?
	}
	QAction* createAction(QObject* parent) const override{ untested();
		auto x = new ActionSelect(parent);
		connect(x, &QAction::toggled, this, &MouseAction::slotToggle);
		return x;
	}

private:
	cmd* activate(QObject* sender) override;
	cmd* deactivate() override;
//	cmd* move(QEvent*) override;
	cmd* press(QEvent*) override;
	cmd* release(QEvent*) override;
//	cmd* generic(QEvent*) override;
private:
	QCursor _oldcursor;
};
/*--------------------------------------------------------------------------*/
template<class CMD>
QUndoCommand* MouseActionSelCmd<CMD>::deactivate()
{itested();
	setCursor(_oldcursor);
	return MouseAction::deactivate();
}
/*--------------------------------------------------------------------------*/
template<class CMD>
QUndoCommand* MouseActionSelCmd<CMD>::activate(QObject* sender)
{itested();
	MouseAction::activate(sender); // ...

	_oldcursor = view()->cursor();
	setCursor(Qt::CrossCursor);

	auto s = scene()->selectedItems();
	bool selected = !s.empty();

	if(selected){itested();
		auto cmd = new CMD(scene(), s);
		return cmd;
	}else{ untested();
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
template<class CMD>
QUndoCommand* MouseActionSelCmd<CMD>::press(QEvent* e)
{ untested();
	if(!e){ untested();
		return nullptr;
	}else if(auto i = dynamic_cast<ItemEvent*>(e)){ untested();
		QList<ElementGraphics*> l;
		l.push_back(&i->item());
		return new CMD(scene(), l);
	}else{ untested();
		trace1("delete::scene unknown sender", e->type());
		return nullptr;
	}
} // select::press
/*--------------------------------------------------------------------------*/
template<class CMD>
QUndoCommand* MouseActionSelCmd<CMD>::release(QEvent*)
{ untested();
	incomplete(); // why?
	return nullptr;
} // select::release
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// swapSelection?
class DeleteSelection : public SchematicEdit {
public:
	template<class IT>
	DeleteSelection(SchematicScene* ctx, IT deletelist)
	  : SchematicEdit(ctx) { untested();
		size_t k = 0;

		qDelete(deletelist);

		setText("delete " + QString::number(k) + " items");
	}
}; // DeleteSelection
typedef MouseActionSelCmd<DeleteSelection> MouseActionDelete;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void transformElement(Element* e, qucsSymbolTransform a, pos_t pivot)
{itested();
	trace1("..", a.degrees_int());
	assert(!(a.degrees_int()%90));
	assert(e);
	if(auto* s=dynamic_cast<Symbol*>(e)){ untested();
		int angle = 0;
		int vflip = 0;
		int hflip = 0;
		int mx = 0;
		int my = 0;
		unsigned r = 0;
		try {itested();
			std::string mxs = s->paramValue("$hflip");
			hflip = atoi(mxs.c_str()); // \pm 1
			//assert(hflip == 1); // for now.
			// 1  |-> 0
			// -1 |-> 1
			mx = (1 - hflip) / 2;
		}catch(qucs::ExceptionCantFind const&){ untested();
			incomplete();
		}
		try {itested();
			// this is what legacy used ("mirrorX", vertical flip);
			std::string mys = s->paramValue("$vflip");
			vflip = atoi(mys.c_str());
			// 1  |-> 0
			// -1 |-> 1
			my = (1 - vflip) / 2;
		}catch(qucs::ExceptionCantFind const&){ untested();
			unreachable();
		}
		try {itested();
			std::string rs = s->paramValue("$angle");
			angle = atoi(rs.c_str());
			assert(!(angle%90));
			assert(angle<360);
			r = angle/90;
		}catch(qucs::ExceptionCantFind const&){ untested();
			unreachable();
		}

		trace3("pretransform", hflip, vflip, angle);

		assert(mx==0 || mx==1);
		assert(my==0 || my==1);
		assert(r < 4); // yikes //

		//assert(hflip==1);

		rotate_after_mirror current(int(r*90), bool(mx), bool(my));
		assert(!(current.degrees_int()%90));
		rotate_after_mirror1_t new_mr = a * current;
		assert(!(new_mr.degrees_int()%90));

		vflip = -2 * int(new_mr.mirror()) + 1;

		s->setParameter(std::string("$hflip"), std::string("1"));
		s->setParameter(std::string("$vflip"), std::to_string(vflip));
		s->setParameter(std::string("$angle"), std::to_string(new_mr.degrees_int()));

		trace3("posttransform", hflip, vflip, new_mr.degrees_int());

		auto p = e->center();
		trace1("posttransform setpos0", p);
		int x = getX(p);
		int y = getY(p);

		x -= pivot.first;
		y -= pivot.second;

		pos_t new_xy(x,y);
		new_xy = a.apply(new_xy);

		x = pivot.first + new_xy.first;
		y = pivot.second + new_xy.second;

		// todo: rounding errors.

		p = pos_t(x, y);
		trace1("posttransform setpos1", p);
		e->setPosition(p);
	// prepareGeometryChange(); // needed??
	}else{ untested();
	}
}
/*--------------------------------------------------------------------------*/
template<class T>
class TransformSelection : public SchematicEdit {
public:
	template<class IT>
	TransformSelection(SchematicScene* ctx, IT selection, T const& t)
	: SchematicEdit(ctx){itested();
		size_t k = 0;
		// TODO: bounding box center?
		QRectF bb;
		std::vector<std::pair<ElementGraphics*,Element*>> buf;
		for(auto i : selection){itested();
			++k;
			QGraphicsItem const* g = i;//huh?
			if(auto eg=dynamic_cast<ElementGraphics*>(i)){itested();
				auto br = g->boundingRect();
				bb |= i->mapToScene(br).boundingRect(); // translate(eg->pos());
				Element* e = eg->cloneElement();
				buf.push_back(std::make_pair(eg, e));
				// qSwap(eg, e);
			}else{ untested();
				unreachable(); // really? use prechecked_cast then.
			}
		}

		auto center = bb.center();
		pos_t pivot(getX(center), getY(center));
		trace1("transform", pivot);

		for(auto i : buf){
			transformElement(i.second, t, pivot);
			qSwap(i.first, i.second);
		}

		setText("transform " + QString::number(k) + " items");
	}
}; // TransformSelection
/*--------------------------------------------------------------------------*/
static const rotate_after_mirror1_t ninety_degree_transform(90, false);
class RotateSelectionTransform : public TransformSelection<qucsSymbolTransform>{
	typedef TransformSelection<qucsSymbolTransform> base;
public:
	template<class IT>
	RotateSelectionTransform(SchematicScene* ctx, IT selection)
	  : base(ctx, selection, ninety_degree_transform) {}
};
/*--------------------------------------------------------------------------*/
class MirrorXaxisSelection : public TransformSelection<qucsSymbolTransform>{
	typedef TransformSelection<qucsSymbolTransform> base;
public:
	template<class IT>
	MirrorXaxisSelection(SchematicScene* ctx, IT selection)
	  : base(ctx, selection, transformFlipHorizontally) {}
};
/*--------------------------------------------------------------------------*/
class MirrorYaxisSelection : public TransformSelection<qucsSymbolTransform>{
	typedef TransformSelection<qucsSymbolTransform> base;
public:
	template<class IT>
	MirrorYaxisSelection(SchematicScene* ctx, IT selection)
	  : base(ctx, selection, transformFlipVertically) {}
};
/*--------------------------------------------------------------------------*/
typedef MouseActionSelCmd<DeleteSelection> MouseActionDelete;
typedef MouseActionSelCmd<DeleteSelection> MouseActionActivate; // TODO
typedef MouseActionSelCmd<RotateSelectionTransform> MouseActionRotate;
typedef MouseActionSelCmd<MirrorXaxisSelection> HandleMirrorX;
typedef MouseActionSelCmd<MirrorYaxisSelection> HandleMirrorY;
/*--------------------------------------------------------------------------*/
class MouseActionNewElement : public MouseAction{
public:
	explicit MouseActionNewElement(Element const* proto=nullptr)
		: MouseAction(), _gfx(nullptr), _proto(proto)
  	{}

	Action* clone() const{
		return new MouseActionNewElement(); // this?
	}
	QAction* createAction(QObject* parent) const override{ untested();
		QAction* x = nullptr;
		assert(_proto);
		trace2("createAction", _proto->typeName(), _proto->label());
		if(_proto->typeName() == "GND"){
			x = new ActionInsertGround(parent);
		}else if(_proto->typeName() == "Port"){
			x = new ActionInsertPort(parent);
		}else if(_proto->typeName() == "Eqn"){
			x = new ActionInsEqn(parent);
		}else{
			incomplete();
			x = new ActionInsertPort(parent);
		}
		connect(x, &QAction::toggled, this, &MouseAction::slotToggle);
		return x;
	}
private:
	cmd* activate(QObject* sender) override;
	cmd* deactivate() override;
	cmd* move(QEvent*) override;
	cmd* press(QEvent*) override;
	cmd* enter(QEvent*) override;
	cmd* leave(QEvent*) override;
	cmd* release(QEvent*) override;

private:
	cmd* makeNew(QEvent*);
	cmd* rotate(QEvent*);

private:
	ElementGraphics* _gfx;
	Element const* _proto;
};
/*--------------------------------------------------------------------------*/
class NewElementCommand : public SchematicEdit {
public:
	NewElementCommand(SchematicScene* ctx, ElementGraphics* gfx)
	: SchematicEdit(ctx) { untested();
		assert(gfx->scene());
		assert(gfx->scene() == ctx); // why pass scene then??
		assert(!element(gfx)->owner());
		gfx->hide();
		setText("NewElement" /*element(gfx)->label()*/); // tr?
		trace0("NewElementCommand::NewElementCommand");
		qInsert(gfx);
	}
	~NewElementCommand(){ untested();
		// _gfx owns element(_gfx)
		// ctx owns _gfx
	}
}; // NewElementCommand
/*--------------------------------------------------------------------------*/
#include <component_widget.h> // not here.
QUndoCommand* MouseActionNewElement::activate(QObject* sender)
{
	if(auto s=dynamic_cast<ComponentListWidgetItem*>(sender)){
		_proto = s->proto(); // better clone?
	}else{
	}
	return MouseAction::activate(sender);
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::release(QEvent* ev)
{ untested();
	QUndoCommand* cmd = nullptr;
	auto m = dynamic_cast<QMouseEvent*>(ev);
	if(!m){ untested();
	}else if(m->button() == Qt::LeftButton){ itested();
		cmd = makeNew(ev);
	}else if(m->button() == Qt::RightButton){ untested();
	}
	return cmd;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::makeNew(QEvent* ev)
{ untested();
	// assert(ev->widget=doc->scene()) // or so.
	trace1("RELEASE", ev->type());
	if(ev->type() == QEvent::MouseButtonRelease){ itested();
	}else{ untested();
		unreachable();
	}

	assert(element(_gfx));
	auto elt = element(_gfx);
	assert(elt);

	if(auto se=dynamic_cast<QGraphicsSceneMouseEvent*>(ev)){ untested();
		QPointF pos = se->scenePos();
		QPoint xx = scene()->snapToGrid(pos);
		_gfx->setPos(xx);
	}else{
	}

	assert(scene() == _gfx->scene()); // why both?
	cmd* c = new NewElementCommand(scene(), _gfx);
	// _gfx = nullptr;

	{ untested();
		_gfx = _gfx->clone(); // new ElementGraphics(elt);
		scene()->addItem(_gfx); // does not attach.
		assert(!element(_gfx)->scope());
	}

	if(_gfx->isVisible()){ untested();
		// BUG? should addItem make visible?
	}else{ untested();
		_gfx->show();
	}

	ev->accept();
	return c;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::deactivate()
{ untested();
	// assert(!attached);
	scene()->removeItem(_gfx);
	delete _gfx; // CHECK: who owns _elt?
	_gfx = nullptr;
	incomplete();
	return MouseAction::deactivate();
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::move(QEvent* ev)
{ itested();
	QPointF sp;
	if(auto ee=dynamic_cast<QMouseEvent*>(ev)){ untested();
		unreachable();
		QPointF wp;
		wp = ee->localPos(); // use oldPos?
		sp = mapToScene(wp.toPoint());
	}else if(auto ee=dynamic_cast<QGraphicsSceneMouseEvent*>(ev)){ untested();
		sp = ee->scenePos();

		QPoint xx = scene()->snapToGrid(sp);
		sp = xx;
	}else{ untested();
		unreachable();
	}
	trace2("MouseActionNewElement::move", ev->type(), sp);

	if(_gfx){ itested();
		_gfx->setPos(sp.x(), sp.y());
	}else{ untested();
		unreachable();
	}

	ev->accept();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::enter(QEvent* ev)
{ untested();
	auto ee = prechecked_cast<QEnterEvent*>(ev);
	assert(ee);
	
	auto wp = ee->localPos();

	auto d = dynamic_cast<QGraphicsView const*>(doc());
	assert(d);
	auto sp = d->mapToScene(wp.toPoint());
	trace2("MouseActionNewElement::enter", ev->type(), sp);

	Element* elt;
	if(!_gfx){ untested();
		assert(_proto);
		elt = _proto->clone_instance();
		if(auto sym=dynamic_cast<Symbol const*>(elt)){
			elt->setLabel(sym->typeName());
		}else{
		}
		elt->setPosition(pos_t(sp.x(), sp.y()));
		_gfx = new ElementGraphics(elt); // BUG
	}else{ untested();
		_gfx->setPos(sp.x(), sp.y());
	}

	assert(scene());
	scene()->addItem(_gfx);

	if(_gfx->isVisible()){ untested();
		// BUG?
	}else{ untested();
		_gfx->show(); // ??
	}
	
//	doc().sceneAddItem(_gfx);

	ev->accept();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::leave(QEvent* ev)
{ untested();
	sceneRemoveItem(_gfx);
	ev->accept();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::rotate(QEvent*)
{ untested();
	if(!_gfx){ untested();
		unreachable();
	}else if(dynamic_cast<Symbol*>(element(_gfx))){ untested();
		// always do this?
		_gfx->transform(ninety_degree_transform);
	}else{ untested();
		unreachable();
	}
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::press(QEvent* ev)
{ untested();
	auto a = dynamic_cast<QMouseEvent*>(ev);
	auto m = dynamic_cast<QGraphicsSceneMouseEvent*>(ev);
	QUndoCommand* cmd = nullptr;
	if(a){ untested();
		unreachable();
		// somehow it is a scene event??
	}else if(!m){ untested();
		trace1("MouseActionNewElement::press", ev->type());
		unreachable();
	}else if(m->button() == Qt::LeftButton){ untested();
	}else if(m->button() == Qt::RightButton){ untested();
		cmd = MouseActionNewElement::rotate(ev);
		ev->accept(); // really?
	}else{ untested();
		unreachable();
	}
	return cmd;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#include "action/paste.cpp" // for now.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
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
// it's a "wire" if it has two ports that connect to the same net.
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
inline Symbol* symbol(Element* e)
{
	return dynamic_cast<Symbol*>(e);
}
inline Symbol* symbol(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return symbol(e->operator->());
}
inline Symbol* symbol(ElementGraphics* e)
{
	if(!e) return nullptr;
	return symbol(e->operator->());
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void setParent_(Object* o, QObject* par)
{
	auto oo = dynamic_cast<QObject*>(o); // prechecked?
	assert(oo);
	oo->setParent(par);
}
/*--------------------------------------------------------------------------*/
SchematicActions::SchematicActions(QucsDoc* ctx)
  : MouseActions(ctx)
{itested();
	assert(ctx);
	assert(doc());

	// not entirely clear how to refactor this
	// maybe dispatch mouse actions.
	// or merge into QAction buttons (connect as needed?)

	untested();
	maDelete = new MouseActionDelete(this);
	setParent_(maDelete, this);
	untested();

	Action* cc;

	cc = action_dispatcher.clone("ActionSelect");
	setParent_(cc, this);
	maSelect = dynamic_cast<MouseAction*>(cc);
	assert(maSelect);

	cc = action_dispatcher.clone("AddWire");
	setParent_(cc, this);
	maWire = dynamic_cast<MouseAction*>(cc);
	assert(maWire);

	cc = action_dispatcher.clone("ZoomIn");
	setParent_(cc, this);
	maZoomIn = dynamic_cast<MouseAction*>(cc);
	assert(maZoomIn);

//	maZoomOut = new MouseActionZoomOut(*this); // not a mouseaction.

	//  maMove = new MouseActionMove(*this);
	Element const* gnd = symbol_dispatcher["GND"];
	assert(gnd);
	maInsertGround = new MouseActionNewElement(gnd);
	setParent_(maInsertGround, this);

	Element const* port = symbol_dispatcher["Port"];
	assert(port);
	maInsertPort = new MouseActionNewElement(port);
	setParent_(maInsertPort, this);

	Element const* eqn = symbol_dispatcher["Eqn"];
	assert(eqn);
	maInsertEqn = new MouseActionNewElement(eqn);
	setParent_(maInsertEqn, this);

	maInsertElement = new MouseActionNewElement();
	setParent_(maInsertElement, this);

	maActivate = new MouseActionActivate(this);
	setParent_(maActivate, this);

	maMirrorXaxis = new HandleMirrorX(this);
	setParent_(maMirrorXaxis, this);

	maMirrorYaxis = new HandleMirrorY(this);
	setParent_(maMirrorYaxis, this);

	maRotate = new MouseActionRotate(this);
	setParent_(maRotate, this);

	maEditPaste = new MouseActionPaste();
	setParent_(maEditPaste, this);

} // SchematicActions::SchematicActions
/*--------------------------------------------------------------------------*/
void SchematicActions::setControls(QucsDoc* ctx)
{
	auto p = dynamic_cast<QWidget*>(ctx); // prech?
	assert(p);

 	assert(maSelect);
 	assert(maWire);
 	assert(maMirrorXaxis);
 	assert(maMirrorYaxis);
 	assert(maRotate);
 	assert(maInsertGround);
 	assert(maInsertPort);
 	assert(maInsertEqn);

	auto b = ctx->newToolBar();
	QAction* A;

	A = maSelect->createAction(p);
	b->addAction(A);
	A = maWire->createAction(p);
	b->addAction(A);
	A = maInsertPort->createAction(p);
	b->addAction(A);
	A = maRotate->createAction(p);
	b->addAction(A);
	A = maMirrorYaxis->createAction(p);
	b->addAction(A);
	A = maMirrorXaxis->createAction(p);
	b->addAction(A);
	A = maInsertPort->createAction(p);
	b->addAction(A);
	A = maInsertGround->createAction(p);
	b->addAction(A);
	A = maInsertEqn->createAction(p);
	b->addAction(A);

	// TODO: menu...
}
/*--------------------------------------------------------------------------*/
SchematicActions::~SchematicActions()
{itested();
//	delete maActivate;
//	delete maDelete;
//	delete maInsertGround;
//	delete maInsertPort;
//	delete maMirrorXaxis;
//	delete maMirrorYaxis;
//	delete maRotate;
//	delete maSelect;
//	delete maWire;
//	delete maZoomIn;
}
/*--------------------------------------------------------------------------*/
SchematicScene const* SchematicActions::scene() const
{ untested();
	auto d = dynamic_cast<QGraphicsView const*>(doc());
	assert(d);
	return dynamic_cast<SchematicScene const*>(d->scene());
}
/*--------------------------------------------------------------------------*/
void SchematicActions::updateViewport()
{ untested();
	doc()->updateViewport();	
}
/*--------------------------------------------------------------------------*/
QPoint SchematicActions::snapToGrid(QPointF const&p) const
{
	return scene()->snapToGrid(p);
}
/*--------------------------------------------------------------------------*/
QRegExp Expr_CompProp;
QRegExpValidator Val_CompProp(Expr_CompProp, 0);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
