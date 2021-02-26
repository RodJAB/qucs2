/***************************************************************************
Copyright (C) 2006 by Michael Margraf <michael.margraf@alumni.tu-berlin.de>
              2014 by Guilherme Brondani Torri <guitorri@gmail.com>
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

// text document

#ifndef QUCS_TEXTDOC_H
#define QUCS_TEXTDOC_H

#include "qucs_app.h"
#include "qucs_globals.h"
#include "textdoc.h"
#include "syntax.h"
#include "misc.h"
#include "qucsdoc.h"
#include "searchdialog.h" // BUG

#include <QAction>
#include <QMessageBox>
#include <QTextStream>
#include <QPlainTextEdit>
#include <QFont>

class SyntaxHighlighter;
class QString;

namespace {

// device type flags
#define DEV_BJT      0x0001
#define DEV_MOS      0x0002
#define DEV_MASK_DEV 0x00FF
#define DEV_DIG      0x0100
#define DEV_ANA      0x0200
#define DEV_ALL      0x0300
#define DEV_MASK_TYP 0xFF00
#define DEV_DEF      0x0200 // default value

class TextDoc : public QPlainTextEdit, public QucsDoc {
	Q_OBJECT
public:
	TextDoc();
	~TextDoc ();
	void setParent(QWidget* w) override;
	void  setName (const QString&) override;
	Widget* clone() const override{ untested();
		QucsDoc* wd = new TextDoc();
		return wd;
	}

  bool  load ();
  int   save ();
  float zoomBy (float);
  void  showNoZoom ();
  void becomeCurrent();
  void cleanup(){ incomplete(); }
  bool  loadSimulationTime (QString&);
  void  commentSelected ();
  void  insertSkeleton ();
  void  setLanguage (int);
  void  setLanguage (const QString&);
  QString getModuleName (void);


  QFont TextFont;

  bool simulation;   // simulation or module
  QString Library;   // library this document belongs to
  QString Libraries; // libraries to be linked with
  QString ShortDesc; // icon description
  QString LongDesc;  // component description
  QString Icon;      // icon file
  bool recreate;     // recreate output file
  int devtype;       // device type

  bool SetChanged;
  int language;

  bool loadSettings (void);
  bool saveSettings (void);
  void refreshLanguage(void);

  QMenu* createStandardContextMenu();

signals:
  void signalCursorPosChanged(int, int);
  void signalFileChanged(bool);
  void signalUndoState(bool);
  void signalRedoState(bool);

public slots:
  void search(const QString &str, bool CaseSensitive, bool wordOnly, bool backward);
  void replace(const QString &str, const QString &str2, bool needConfirmed,
               bool CaseSensitive, bool wordOnly, bool backward);
  void slotCursorPosChanged ();
  void slotSetChanged ();

private:
  SyntaxHighlighter * syntaxHighlight;

private: // actions. here?
  void actionCopy(QAction*) override{
	  copy();
  }
  void actionCut(QAction*) override{
	  cut();
  }
  void actionSelect(QAction*) override;
  void actionEditPaste(QAction*) override;
  void actionZoomIn(QAction*) override;

private: // QucsDoc
  void slotEditActivate(QAction*) override;
  void slotEditDelete(QAction*) override;
  void slotEditUndo(QAction*) override{
    viewport()->setFocus();
	 QPlainTextEdit::undo();
  }
  void slotEditRedo(QAction*) override{
	  viewport()->setFocus();
	  QPlainTextEdit::redo();
  }

  void actionInsertEntity(QAction*) override;
  void actionSelectAll(QAction*) override{
	  viewport()->setFocus();
	  //->selectAll(true);
	  selectAll();
  }
  void actionChangeProps(QAction*) override;


private slots:
  void highlightCurrentLine();
  bool baseSearch(const QString &, bool, bool, bool);

private: // TODO
  float Scale;
};

inline void TextDoc::actionInsertEntity(QAction*)
{
  viewport()->setFocus ();
  //TODO Doc->clearParagraphBackground (Doc->tmpPosX);
  insertSkeleton ();

  //int x, y;
  //Doc->getCursorPosition (&x, &y);
  //x = Doc->textCursor().blockNumber();
  //y = Doc->textCursor().columnNumber();
  slotCursorPosChanged();
}

TextDoc::TextDoc()
   : QPlainTextEdit(),
     QucsDoc()
{
}

void TextDoc::setParent(QWidget* parent)
{
	assert(!parentWidget());
	QPlainTextEdit::setParent(parent);
	incomplete();
  TextFont = QFont("Courier New");
//  TextFont.setPointSize(QucsSettings.font.pointSize()-1);
  TextFont.setStyleHint(QFont::Courier);
  TextFont.setFixedPitch(true);
  document()->setDefaultFont(TextFont);

  simulation = true;
  Library = "";
  Libraries = "";
  SetChanged = false;
  devtype = DEV_DEF;

  tmpPosX = tmpPosY = 1;  // set to 1 to trigger line highlighting
  Scale = (float)TextFont.pointSize();
  setLanguage ("ascii"); // initial_name);

  viewport()->setFocus();

  setWordWrapMode(QTextOption::NoWrap);
//  misc::setWidgetBackgroundColor(viewport(), QucsSettings.BGColor);
  connect(this, SIGNAL(textChanged()), SLOT(slotSetChanged()));
  connect(this, SIGNAL(cursorPositionChanged()),
          SLOT(slotCursorPosChanged()));

  if (parent) {itested();
    connect(this, SIGNAL(signalCursorPosChanged(int, int)),
        parent, SLOT(printCursorPosition(int, int)));
    connect(this, SIGNAL(signalUndoState(bool)),
        parent, SLOT(slotUpdateUndo(bool)));
    connect(this, SIGNAL(signalRedoState(bool)),
        parent, SLOT(slotUpdateRedo(bool)));
    connect(this, SIGNAL(signalFileChanged(bool)),
        parent, SLOT(slotFileChanged(bool)));
  }else{ untested();
  }

  syntaxHighlight = new SyntaxHighlighter(this);
  syntaxHighlight->setLanguage(language);
  syntaxHighlight->setDocument(document());

  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
  highlightCurrentLine();
}

/*!
 * \brief TextDoc::~TextDoc Text document destructor
 */
TextDoc::~TextDoc()
{
  // closing the doc may trigger a signalFileChanged which in turn will run QucsApp::slotFileChanged(), which
  // if text document Tab is removed first, DocumentTab->currentIndex() will point to the previous tab and
  // setSaveIcon() (called by slotFileChanged() ) will wrongly change that tab icon

//  disconnect(this, SIGNAL(signalFileChanged(bool)), App, SLOT(slotFileChanged(bool)));
  delete syntaxHighlight; // what?
}

/*!
 * \brief TextDoc::setLanguage(const QString&)
 * \param FileName Text document file name
 * Extract the file name suffix and assing a language_type to it.
 */
void TextDoc::setLanguage (const QString& FileName)
{
  QFileInfo Info (FileName);
  QString ext = Info.suffix();
  if (ext == "vhd" || ext == "vhdl")
    setLanguage (LANG_VHDL);
  else if (ext == "v")
    setLanguage (LANG_VERILOG);
  else if (ext == "va")
    setLanguage (LANG_VERILOGA);
  else if (ext == "m" || ext == "oct")
    setLanguage (LANG_OCTAVE);
  else
    setLanguage (LANG_NONE);
}

/*!
 * \brief TextDoc::setLanguage(int)
 * \param lang is a language_type
 * Assing value to text document object language variable
 */
void TextDoc::setLanguage (int lang)
{
  language = lang;
}

/*!
 * \brief TextDoc::saveSettings saves the text document settings .cfg
 * \return true/false if settings file opened with success
 */
bool TextDoc::saveSettings (void)
{
  QFile file (docName() + ".cfg");
  if (!file.open (QIODevice::WriteOnly))
    return false;

  QTextStream stream (&file);
  stream << "Textfile settings file, Qucs " PACKAGE_VERSION "\n"
	 << "Simulation=" << simulation << "\n"
	 << "Duration=" << SimTime << "\n"
	 << "Module=" << (!simulation) << "\n"
	 << "Library=" << Library << "\n"
	 << "Libraries=" << Libraries << "\n"
	 << "ShortDesc=" << ShortDesc << "\n"
	 << "LongDesc=" << LongDesc << "\n"
	 << "Icon=" << Icon << "\n"
	 << "Recreate=" << recreate << "\n"
	 << "DeviceType=" << devtype << "\n";

  file.close ();
  SetChanged = false;
  return true;
}

/*!
 * \brief TextDoc::loadSettings loads the text document settings
 * \return true/false if settings file opened with success
 */
bool TextDoc::loadSettings (void)
{
  QFile file (docName() + ".cfg");
  if (!file.open (QIODevice::ReadOnly))
    return false;

  QTextStream stream (&file);
  QString Line, Setting;

  bool ok;
  while (!stream.atEnd ()) {
    Line = stream.readLine ();
    Setting = Line.section ('=', 0, 0);
    Line = Line.section ('=', 1).trimmed ();
    if (Setting == "Simulation") {
      simulation = Line.toInt (&ok);
    } else if (Setting == "Duration") {
      SimTime = Line;
    } else if (Setting == "Module") {
    } else if (Setting == "Library") {
      Library = Line;
    } else if (Setting == "Libraries") {
      Libraries = Line;
    } else if (Setting == "ShortDesc") {
      ShortDesc = Line;
    } else if (Setting == "LongDesc") {
      LongDesc = Line;
    } else if (Setting == "Icon") {
      Icon = Line;
    } else if (Setting == "Recreate") {
      recreate = Line.toInt (&ok);
    } else if (Setting == "DeviceType") {
      devtype = Line.toInt (&ok);
    }
  }

  file.close ();
  return true;
}

/*!
 * \brief TextDoc::setName sets the text file name on its tab
 * \param Name_ text file name to be set
 */
void TextDoc::setName (const QString& Name_)
{
	incomplete();
  // setFileName(Name_);
  // setLanguage (docName());

  // QFileInfo Info (docName());

  // DataSet = Info.completeBaseName() + ".dat";
  // DataDisplay = Info.completeBaseName() + ".dpl";
  // if(Info.suffix() == "m" || Info.suffix() == "oct")
  //   SimTime = "1";
}

#if 0 // TODO: copied from SchDoc
void SchematicDoc::showEvent(QShowEvent*e)
{untested();
	{ // merge?
	becomeCurrent();
	QucsDoc::showEvent(e);
	}

	QGraphicsView::showEvent(e);
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::hideEvent(QHideEvent*e)
{untested();
	QucsDoc::hideEvent(e);
	QGraphicsView::hideEvent(e);
}
#endif
// set as current
void TextDoc::becomeCurrent ()
{
	incomplete();
#if 0
  slotCursorPosChanged();
  viewport()->setFocus ();

  // does this grey/ungrey the undo/redo buttons?
  emit signalUndoState(document()->isUndoAvailable());
  emit signalRedoState(document()->isRedoAvailable());

  // update appropriate menu entries
  App->symEdit->setText (tr("Edit Text Symbol"));
  App->symEdit->setStatusTip (tr("Edits the symbol for this text document"));
  App->symEdit->setWhatsThis (
	tr("Edit Text Symbol\n\nEdits the symbol for this text document"));

  if (language == LANG_VHDL) {
    App->insEntity->setText (tr("VHDL entity"));
    App->insEntity->setStatusTip (tr("Inserts skeleton of VHDL entity"));
    App->insEntity->setWhatsThis (
	tr("VHDL entity\n\nInserts the skeleton of a VHDL entity"));
  }
  else if (language == LANG_VERILOG || language == LANG_VERILOGA) {
    App->insEntity->setText (tr("Verilog module"));
    App->insEntity->setStatusTip (tr("Inserts skeleton of Verilog module"));
    App->insEntity->setWhatsThis (
	tr("Verilog module\n\nInserts the skeleton of a Verilog module"));
    App->buildModule->setEnabled(true);
  }
  else if (language == LANG_OCTAVE) {
    App->insEntity->setText (tr("Octave function"));
    App->insEntity->setStatusTip (tr("Inserts skeleton of Octave function"));
    App->insEntity->setWhatsThis (
	tr("Octave function\n\nInserts the skeleton of a Octave function"));
  }
  App->simulate->setEnabled (true);
  App->editActivate->setEnabled (true);
#endif
}

bool TextDoc::baseSearch(const QString &str, bool CaseSensitive, bool wordOnly, bool backward)
{
  QTextDocument::FindFlags flag = 0;
  bool finded;

  if (CaseSensitive) {
    flag = QTextDocument::FindCaseSensitively;
  }
  if (backward) {
    flag = flag | QTextDocument::FindBackward;
  }
  if (wordOnly) {
    flag = flag | QTextDocument::FindWholeWords;
  }

  finded = find(str, flag);
  if (!finded) {
    if (!backward) {
      moveCursor(QTextCursor::Start);
    } else {
      moveCursor(QTextCursor::End);
    }
    finded = find(str, flag);
    if (!finded) {
      QMessageBox::information(this,
          tr("Find..."), tr("Cannot find target: %1").arg(str),
          QMessageBox::Ok | QMessageBox::Default | QMessageBox::Escape);
    }
  }
  return finded;
}

// implement search function
// if target not find, auto revert to head, find again
// if still not find, pop out message
void TextDoc::search(const QString &str, bool CaseSensitive, bool wordOnly, bool backward)
{
  baseSearch(str, CaseSensitive, wordOnly, backward);
}

// implement replace function
void TextDoc::replace(const QString &str, const QString &str2, bool needConfirmed,
                      bool CaseSensitive, bool wordOnly, bool backward)
{
  bool finded = baseSearch(str, CaseSensitive, wordOnly, backward);
  int i;

  if(finded) {
    i = QMessageBox::Yes;
    if (needConfirmed) {
      i = QMessageBox::information(this,
          tr("Replace..."), tr("Replace occurrence ?"),
          QMessageBox::Yes | QMessageBox::Default,
          QMessageBox::No | QMessageBox::Escape);
    }
    if(i == QMessageBox::Yes) {
      insertPlainText(str2);
    }
  }
}


/*!
 * \brief TextDoc::slotCursorPosChanged update status bar with line:column
 */
void TextDoc::slotCursorPosChanged()
{
  QTextCursor pos = textCursor();
  int x = pos.blockNumber();
  int y = pos.columnNumber();
  emit signalCursorPosChanged(x+1, y+1);
  tmpPosX = x;
  tmpPosY = y;
}

/*!
 * \brief TextDoc::slotSetChanged togles tab icon to indicate unsaved changes
 */
void TextDoc::slotSetChanged()
{
  if((document()->isModified() && !DocChanged) || SetChanged) {
    DocChanged = true;
  }else if((!document()->isModified() && DocChanged)) {
    DocChanged = false;
  }else{
  }
  emit signalFileChanged(DocChanged);
  emit signalUndoState(document()->isUndoAvailable());
  emit signalRedoState(document()->isRedoAvailable());
}

/*!
 * \brief TextDoc::createStandardContextMenu creates the standard context menu
 * \param pos
 * \return
 *
 *  \todo \fixme is this working?
 */
QMenu *TextDoc::createStandardContextMenu()
{
  QMenu *popup = QPlainTextEdit::createStandardContextMenu();

   if (language != LANG_OCTAVE) {
		incomplete();
		// ((QWidget *)popup)->addAction(App->fileSettings);
   }else{
	}
   return popup;
}

// load a text document
// return true/false if the document was opened with success (throw??)
bool TextDoc::load ()
{
  QFile file (docName());
  if (!file.open (QIODevice::ReadOnly))
    return false;
  setLanguage (docName());

  QTextStream stream (&file);
  insertPlainText(stream.readAll());
  document()->setModified(false);
  slotSetChanged ();
  file.close ();
  lastSaved = QDateTime::currentDateTime ();
  loadSettings ();
  SimOpenDpl = simulation ? true : false;
  refreshLanguage();
  return true;
}


/*!
 * \brief TextDoc::save saves the current document and it settings
 * \return true/false if the document was opened with success
 */
int TextDoc::save ()
{
  saveSettings ();

  QFile file (docName());
  if (!file.open (QIODevice::WriteOnly)){
	  // throw
    return -1;
  }else{
  }
  setLanguage (docName());

  QTextStream stream (&file);
  stream << toPlainText();
  document()->setModified (false);
  slotSetChanged ();
  file.close ();

  QFileInfo Info (docName());
  lastSaved = Info.lastModified ();

  /// clear highlighted lines on save \see MessageDock::slotCursor()
  QList<QTextEdit::ExtraSelection> extraSelections;
  this->setExtraSelections(extraSelections);
  refreshLanguage();

  return 0;
}

/*!
 * \brief TextDoc::zoomBy increases/decreases the text font size.
 * \param s font size scaling factor
 * \return (required) final scale
 *
 * \fixme is the return value being saved on the saveSettings() ?
 */
float TextDoc::zoomBy(float s)
{
  if(s == 2.0) {
      QFont f = document()->defaultFont();
      f.setPointSize(f.pointSize()*2);
      document()->setDefaultFont(f);
  }else{
      QFont f = document()->defaultFont();
      f.setPointSize(f.pointSize()*s);
      document()->setDefaultFont(f);
  }
  return Scale;
}

/*!
 * \brief TextDoc::showNoZoom resets the font scaling
 */
void TextDoc::showNoZoom()
{
  TextFont = QFont("Courier New");
  TextFont.setPointSize(1); // QucsSettings.font.pointSize()-1);  // fontSize?
  TextFont.setStyleHint(QFont::Courier);
  TextFont.setFixedPitch(true);
  document()->setDefaultFont(TextFont);
}

/*!
 * \brief TextDoc::loadSimulationTime set SimTime member variable
 * \param Time string  with simulation time
 * \return true if SimTime is set
 */
bool TextDoc::loadSimulationTime(QString& Time)
{
  if(!SimTime.isEmpty()) {
    Time = SimTime;
    return true;
  }
  return false;
}

/*!
 * \brief TextDoc::commentSelected toggles the comment of selected text
 * See also QucsApp::slotEditActivate
 */
void TextDoc::commentSelected ()
{
  QTextCursor cursor = this->textCursor();

  if(!cursor.hasSelection())
      return; // No selection available

  // get range of selection
  int start = cursor.selectionStart();
  int end = cursor.selectionEnd();

  cursor.setPosition(start);
  int firstLine = cursor.blockNumber();
  cursor.setPosition(end, QTextCursor::KeepAnchor);
  int lastLine = cursor.blockNumber();

  // use comment string indicator depending on language
  QString co;

  switch (language) {
  case LANG_VHDL:
    co = "--";
    break;
  case LANG_VERILOG:
  case LANG_VERILOGA:
    co = "//";
    break;
  case LANG_OCTAVE:
    co = "%";
    break;
  default:
    co = "";
    break;
  }

  QStringList newlines;
  for (int i=firstLine; i<=lastLine; i++) {
      QString line = document()->findBlockByLineNumber(i).text();
      if (line.startsWith(co)){
          // uncomment
          line.remove(0,co.length());
          newlines << line;
      }
      else {
          // comment
          line = line.insert(0, co);
          newlines << line;
      }
  }
  insertPlainText(newlines.join("\n"));
}

/*!
 * \brief TextDoc::insertSkeleton adds a basic skeleton for type of text file
 */
void TextDoc::insertSkeleton ()
{
  if (language == LANG_VHDL)
    appendPlainText("entity  is\n  port ( : in bit);\nend;\n"
	    "architecture  of  is\n  signal : bit;\nbegin\n\nend;\n\n");
  else if (language == LANG_VERILOG)
    appendPlainText ("module  ( );\ninput ;\noutput ;\nbegin\n\nend\n"
	    "endmodule\n\n");
  else if (language == LANG_OCTAVE)
    appendPlainText ("function  =  ( )\n"
	    "endfunction\n\n");
}

/*!
 * \brief TextDoc::getModuleName parse the module name ou of the text file contents
 * \return the module name
 */
QString TextDoc::getModuleName (void)
{
	incomplete();
	return "incompletemodulename";
#if 0 // BUG. what is language?
  switch (language) {
  case LANG_VHDL:
    {
      VHDL_File_Info VInfo (toPlainText());
      return VInfo.EntityName;
    }
  case LANG_VERILOG:
    {
      Verilog_File_Info VInfo (toPlainText());
      return VInfo.ModuleName;
    }
  case LANG_VERILOGA:
    {
      VerilogA_File_Info VInfo (toPlainText());
      return VInfo.ModuleName;
    }
  case LANG_OCTAVE:
    {
      QFileInfo Info (docName());
      return Info.completeBaseName();
    }
  default:
    return "";
  }
#endif
}

/*!
 * \brief TextDoc::highlightCurrentLine mark the current line
 */
void TextDoc::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::blue).lighter(195);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void TextDoc::refreshLanguage()
{
    this->setLanguage(docName());
    syntaxHighlight->setLanguage(language);
    syntaxHighlight->setDocument(document());
}

void TextDoc::actionSelect(QAction*)
{
	incomplete(); // select action does not exist in textdoc.
	              // ... not yet?
//	viewport()->setFocus();
////	toggleAction(s);
//	selectAction()->blockSignals(true);
//	selectAction()->setChecked(true);
//	selectAction()->blockSignals(false);
}

void TextDoc::slotEditActivate(QAction*)
{
	commentSelected();
	// toggleAction(s);
	incomplete();

	// App->editActivate->blockSignals (true);
	// App->editActivate->setChecked(false);  // release toolbar button
	// App->editActivate->blockSignals (false);
}

void TextDoc::slotEditDelete(QAction*)
{
	viewport()->setFocus();
	textCursor().deleteChar();

//	toggleAction(s);
}

void TextDoc::actionEditPaste(QAction*)
{
	paste();
//	toggleAction(s);
}

void TextDoc::actionZoomIn(QAction*)
{
	zoomBy(1.5f);

#if 0 // why is magPlos checkable?!
	App->magPlus->blockSignals(true);
	App->magPlus->setChecked(false);
	App->magPlus->blockSignals(false);
#endif
}

void TextDoc::actionChangeProps(QAction*)
{
	viewport()->setFocus();

#if 0
	//App->SearchDia->initSearch(this, textCursor().selectedText(), true);
#else	
	// _searchDia->initSearch(this, textCursor().selectedText(), true);
#endif
}

class Factory : public Widget {
	Widget* clone() const override{ untested();
		TextDoc* wd = new TextDoc();
		return wd;
	}
}F;
static Dispatcher<Widget>::INSTALL p1(&widget_dispatcher, "TextDoc", &F);

} // namespace

#endif