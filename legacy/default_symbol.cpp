/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// default symbol. unshared legacy version.

#include "components/component.h" // BUG
#include "qucs_globals.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
static const std::string prefix = ":SYMBOL_";
/*--------------------------------------------------------------------------*/
class DefaultSymbol : public Component /*Painting*/ {
public:
	explicit DefaultSymbol() : Component(), _numports(0) {}
	// explicit DefaultSymbol(CommonComponent* s);
	~DefaultSymbol() {};

private:
	explicit DefaultSymbol(DefaultSymbol const&x)
	    : Component(x), _numports(x._numports){
	}
	std::string dev_type()const {
		return prefix + std::to_string(_numports);
	}
public:
	Element* clone() const override{
		return new DefaultSymbol(*this);
	}

protected:
  void init();

private: // Symbol
  // bool portExists(unsigned) const override;
  // std::string const portName(unsigned) const override;

  void set_param_by_name(std::string const& name, std::string const& value) override{
	  if(name == "$numports"){
		  _numports = atoi(value.c_str());
		  assert(_numports);
		  init();
	  }else{
		  return Component::set_param_by_name(name, value);
	  }
  }

  index_t paramCount() const override{
	  return 0;
  }
//  std::string paramName(unsigned i) const override;
//  std::string paramValue(unsigned i) const override;
//  std::string paramValue(std::string const& x) const override;

private:
  int _numports;
}d0; // DefaultSymbol
/*--------------------------------------------------------------------------*/
#if 1 // bug/feature.
// this (legacy subcircuit symbol) does not seem to scale right.
// or maybe the idealbalun2 schematic is broken?
static int HS = 30;
void DefaultSymbol::init()
{
	int No = _numports;
	// sort of default symbol if there is no symbol section?
	int h = HS*((No-1)/2) + 15;
	Lines.append(new Line(-15, -h, 15, -h,QPen(Qt::darkBlue,2)));
	Lines.append(new Line( 15, -h, 15,  h,QPen(Qt::darkBlue,2)));
	Lines.append(new Line(-15,  h, 15,  h,QPen(Qt::darkBlue,2)));
	Lines.append(new Line(-15, -h,-15,  h,QPen(Qt::darkBlue,2)));
	Texts.append(new Text(-10, -6,"sub"));

	int i=0, y = 15-h;
	while(i<No) {
		i++;
		Lines.append(new Line(-30,  y,-15,  y,QPen(Qt::darkBlue,2)));
		Ports.append(new ComponentPort(-30,  y));
		Texts.append(new Text(-25,y-14,QString::number(i)));

		if(i == No){
			break;
		}else{
		}
		i++;
		Lines.append(new Line( 15,  y, 30,  y,QPen(Qt::darkBlue,2)));
		Ports.append(new ComponentPort( 30,  y));
		Texts.append(new Text( 19,y-14,QString::number(i)));
		y += 2*HS;
	}

	x1 = -30; y1 = -h-2;
	x2 =  30; y2 =  h+2;
	tx = x1+4;
	ty = y2+4;
}
#endif
/*--------------------------------------------------------------------------*/
class DefaultSymbols{
	typedef Dispatcher<Symbol>::INSTALL disp_t;
public:
	DefaultSymbols(){
		for(unsigned i=1; i<10; i++){
			auto s = d0.clone();
			s->set_param_by_name("$numports", std::to_string(i));
			s->set_label(prefix + std::to_string(i));
			trace1("default symbol",  s->label());
			auto d = new disp_t(&symbol_dispatcher, s->label(), s);
			_sstash.push_back(s);
			_stash.push_back(d);
		}
	}
	~DefaultSymbols(){
		for(auto& i : _stash){
			delete i;
			i = nullptr;
		}
		for(auto i : _sstash){
			delete i;
			i = nullptr;
		}
	}
private:
	std::vector<Element*> _sstash;
	std::vector<disp_t*> _stash;
}p;
/*--------------------------------------------------------------------------*/
}//namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
